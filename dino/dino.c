#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <time.h>

#define WIDTH 900
#define HEIGHT 600
#define GROUND_Y 470
#define DINO_WIDTH 40
#define DINO_HEIGHT 44

// Color palette (night neon theme)
#define COLOR_SKY       0xFF1A1A2E
#define COLOR_SKY_MID   0xFF16213E
#define COLOR_SKY_LOW   0xFF0F3460
#define COLOR_GROUND    0xFF2C2C54
#define COLOR_GROUND_LINE 0xFF706FD3
#define COLOR_DINO      0xFF00E676
#define COLOR_DINO_EYE  0xFFFFFFFF
#define COLOR_CACTUS    0xFFE94560
#define COLOR_PTERO     0xFFFF9800
#define COLOR_PTERO_WING 0xFFFFB74D
#define COLOR_CLOUD     0xFF3A3A6A
#define COLOR_STAR      0xFFFFFFFF
#define COLOR_SCORE     0xFF82AAFF
#define COLOR_HISCORE   0xFF616161
#define COLOR_GAMEOVER  0xFFFF5252
#define COLOR_WHITE     0xFFFFFFFF
#define COLOR_OVERLAY   0xCC000000

#define MAX_STARS 40

typedef struct {
    int x, y;
    int size;
    int twinkle_phase;
} Star;

Star stars[MAX_STARS];

#define MAX_OBSTACLES 10
#define MAX_CLOUDS 5

typedef enum { OBS_CACTUS_SMALL, OBS_CACTUS_TALL, OBS_PTERO } ObstacleType;
typedef enum { STATE_PLAYING, STATE_GAMEOVER } GameState;

typedef struct {
    float x, y;
    int width, height;
    int ducking;
    int leg_frame;      // Animation frame for legs
} Dino;

typedef struct {
    float x, y;
    float vx;
    int width, height;
    ObstacleType type;
    int active;
} Obstacle;

typedef struct {
    float x, y;
    float vx;
    int width, height;
} Cloud;

// Global game state
Obstacle obstacles[MAX_OBSTACLES];
Cloud clouds[MAX_CLOUDS];
int score = 0;
int high_score = 0;
float game_speed = 1.0f;
GameState game_state = STATE_PLAYING;

// Pixel font for score (5x7 digits)
const unsigned char DIGITS[10][7] = {
    {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}, // 0
    {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E}, // 1
    {0x0E,0x11,0x01,0x0E,0x10,0x10,0x1F}, // 2
    {0x0E,0x11,0x01,0x0E,0x01,0x11,0x0E}, // 3
    {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}, // 4
    {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E}, // 5
    {0x0E,0x10,0x10,0x1E,0x11,0x11,0x0E}, // 6
    {0x1F,0x01,0x02,0x04,0x08,0x08,0x08}, // 7
    {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}, // 8
    {0x0E,0x11,0x11,0x0F,0x01,0x01,0x0E}, // 9
};

void draw_digit(SDL_Surface* surface, int digit, int x, int y, int scale, Uint32 color) {
    if (digit < 0 || digit > 9) return;
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 5; col++) {
            if (DIGITS[digit][row] & (1 << (4 - col))) {
                SDL_Rect pixel = { x + col * scale, y + row * scale, scale, scale };
                SDL_FillRect(surface, &pixel, color);
            }
        }
    }
}

void draw_number(SDL_Surface* surface, int number, int x, int y, int scale, Uint32 color) {
    char buf[16];
    sprintf(buf, "%d", number);
    int len = strlen(buf);
    for (int i = 0; i < len; i++) {
        draw_digit(surface, buf[i] - '0', x + i * (6 * scale), y, scale, color);
    }
}

void draw_dino(SDL_Surface* surface, Dino* dino) {
    int x = (int)dino->x;
    int y = (int)dino->y;

    if (dino->ducking) {
        // Ducking dino (wider, shorter)
        // Body
        SDL_Rect body = { x, y + 24, 50, 20 };
        SDL_FillRect(surface, &body, COLOR_DINO);
        // Head
        SDL_Rect head = { x + 35, y + 14, 20, 16 };
        SDL_FillRect(surface, &head, COLOR_DINO);
        // Eye
        SDL_Rect eye = { x + 48, y + 17, 4, 4 };
        SDL_FillRect(surface, &eye, COLOR_DINO_EYE);
        // Legs (animated)
        if (dino->leg_frame < 5) {
            SDL_Rect leg1 = { x + 10, y + 44, 6, 10 };
            SDL_Rect leg2 = { x + 30, y + 44, 6, 10 };
            SDL_FillRect(surface, &leg1, COLOR_DINO);
            SDL_FillRect(surface, &leg2, COLOR_DINO);
        } else {
            SDL_Rect leg1 = { x + 5, y + 44, 6, 10 };
            SDL_Rect leg2 = { x + 25, y + 44, 6, 10 };
            SDL_FillRect(surface, &leg1, COLOR_DINO);
            SDL_FillRect(surface, &leg2, COLOR_DINO);
        }
    } else {
        // Standing dino
        // Body
        SDL_Rect body = { x + 5, y + 14, 30, 30 };
        SDL_FillRect(surface, &body, COLOR_DINO);
        // Head
        SDL_Rect head = { x + 15, y, 25, 20 };
        SDL_FillRect(surface, &head, COLOR_DINO);
        // Eye
        SDL_Rect eye = { x + 32, y + 4, 4, 4 };
        SDL_FillRect(surface, &eye, COLOR_DINO_EYE);
        // Tail
        SDL_Rect tail = { x, y + 20, 10, 8 };
        SDL_FillRect(surface, &tail, COLOR_DINO);
        // Arms
        SDL_Rect arm = { x + 30, y + 24, 8, 4 };
        SDL_FillRect(surface, &arm, COLOR_DINO);
        // Legs (animated)
        if (dino->leg_frame < 5) {
            SDL_Rect leg1 = { x + 10, y + 44, 8, 16 };
            SDL_Rect leg2 = { x + 24, y + 44, 8, 12 };
            SDL_FillRect(surface, &leg1, COLOR_DINO);
            SDL_FillRect(surface, &leg2, COLOR_DINO);
        } else {
            SDL_Rect leg1 = { x + 10, y + 44, 8, 12 };
            SDL_Rect leg2 = { x + 24, y + 44, 8, 16 };
            SDL_FillRect(surface, &leg1, COLOR_DINO);
            SDL_FillRect(surface, &leg2, COLOR_DINO);
        }
    }
}

void init_obstacles(void) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].active = 0;
    }
}

void init_clouds(void) {
    for (int i = 0; i < MAX_CLOUDS; i++) {
        clouds[i].x = rand() % WIDTH;
        clouds[i].y = 50 + rand() % 150;
        clouds[i].vx = -0.3f - (rand() % 50) / 100.0f;
        clouds[i].width = 60 + rand() % 40;
        clouds[i].height = 20 + rand() % 15;
    }
}

void init_stars(void) {
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].x = rand() % WIDTH;
        stars[i].y = rand() % (GROUND_Y - 30);
        stars[i].size = 1 + rand() % 3;
        stars[i].twinkle_phase = rand() % 20;
    }
}

void spawn_obstacle(void) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacles[i].active) {
            int type_roll = rand() % 100;

            if (type_roll < 40) {
                // Small cactus
                obstacles[i].type = OBS_CACTUS_SMALL;
                obstacles[i].width = 20;
                obstacles[i].height = 35;
                obstacles[i].y = GROUND_Y - obstacles[i].height;
            } else if (type_roll < 75) {
                // Tall cactus
                obstacles[i].type = OBS_CACTUS_TALL;
                obstacles[i].width = 25;
                obstacles[i].height = 50;
                obstacles[i].y = GROUND_Y - obstacles[i].height;
            } else {
                // Pterodactyl (flying)
                obstacles[i].type = OBS_PTERO;
                obstacles[i].width = 40;
                obstacles[i].height = 30;
                // Randomly high or low
                obstacles[i].y = (rand() % 2) ? GROUND_Y - 80 : GROUND_Y - 40;
            }

            obstacles[i].x = WIDTH + rand() % 100;
            obstacles[i].vx = -5.0f * game_speed;
            obstacles[i].active = 1;
            break;
        }
    }
}

void update_obstacles(void) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active) {
            obstacles[i].x += obstacles[i].vx;
            if (obstacles[i].x + obstacles[i].width < 0) {
                obstacles[i].active = 0;
                score += 10;
            }
        }
    }
}

void update_clouds(void) {
    for (int i = 0; i < MAX_CLOUDS; i++) {
        clouds[i].x += clouds[i].vx;
        if (clouds[i].x + clouds[i].width < 0) {
            clouds[i].x = WIDTH + rand() % 100;
            clouds[i].y = 50 + rand() % 150;
            clouds[i].width = 60 + rand() % 40;
            clouds[i].height = 20 + rand() % 15;
        }
    }
}

void draw_cloud(SDL_Surface* surface, Cloud* cloud) {
    int x = (int)cloud->x;
    int y = (int)cloud->y;
    int w = cloud->width;
    int h = cloud->height;

    // Draw fluffy cloud shape
    SDL_Rect r1 = { x + w/4, y, w/2, h };
    SDL_Rect r2 = { x, y + h/3, w, h/2 };
    SDL_Rect r3 = { x + w/6, y + h/4, w*2/3, h/2 };
    SDL_FillRect(surface, &r1, COLOR_CLOUD);
    SDL_FillRect(surface, &r2, COLOR_CLOUD);
    SDL_FillRect(surface, &r3, COLOR_CLOUD);
}

void draw_cactus(SDL_Surface* surface, Obstacle* obs) {
    int x = (int)obs->x;
    int y = (int)obs->y;

    // Main trunk
    SDL_Rect trunk = { x + obs->width/3, y, obs->width/3, obs->height };
    SDL_FillRect(surface, &trunk, COLOR_CACTUS);

    if (obs->type == OBS_CACTUS_TALL) {
        // Left arm
        SDL_Rect arm_l = { x, y + obs->height/3, obs->width/3, obs->height/4 };
        SDL_Rect arm_l_up = { x, y + obs->height/6, obs->width/4, obs->height/4 };
        SDL_FillRect(surface, &arm_l, COLOR_CACTUS);
        SDL_FillRect(surface, &arm_l_up, COLOR_CACTUS);

        // Right arm
        SDL_Rect arm_r = { x + obs->width*2/3, y + obs->height/2, obs->width/3, obs->height/4 };
        SDL_Rect arm_r_up = { x + obs->width*3/4, y + obs->height/3, obs->width/4, obs->height/4 };
        SDL_FillRect(surface, &arm_r, COLOR_CACTUS);
        SDL_FillRect(surface, &arm_r_up, COLOR_CACTUS);
    }
}

void draw_pterodactyl(SDL_Surface* surface, Obstacle* obs, int frame) {
    int x = (int)obs->x;
    int y = (int)obs->y;

    // Body
    SDL_Rect body = { x + 10, y + 12, 20, 10 };
    SDL_FillRect(surface, &body, COLOR_PTERO);

    // Head
    SDL_Rect head = { x + 28, y + 10, 12, 8 };
    SDL_FillRect(surface, &head, COLOR_PTERO);

    // Beak
    SDL_Rect beak = { x + 36, y + 14, 8, 3 };
    SDL_FillRect(surface, &beak, COLOR_PTERO);

    // Wings (animated)
    if (frame < 5) {
        SDL_Rect wing = { x, y, 30, 8 };
        SDL_FillRect(surface, &wing, COLOR_PTERO_WING);
    } else {
        SDL_Rect wing = { x, y + 20, 30, 8 };
        SDL_FillRect(surface, &wing, COLOR_PTERO_WING);
    }
}

void draw_obstacles(SDL_Surface* surface, int frame) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active) {
            if (obstacles[i].type == OBS_PTERO) {
                draw_pterodactyl(surface, &obstacles[i], frame);
            } else {
                draw_cactus(surface, &obstacles[i]);
            }
        }
    }
}

void draw_clouds(SDL_Surface* surface) {
    for (int i = 0; i < MAX_CLOUDS; i++) {
        draw_cloud(surface, &clouds[i]);
    }
}

void draw_sky_gradient(SDL_Surface* surface) {
    // Three-band gradient for night sky
    int band_h = GROUND_Y / 3;
    SDL_Rect top = { 0, 0, WIDTH, band_h };
    SDL_FillRect(surface, &top, COLOR_SKY);
    SDL_Rect mid = { 0, band_h, WIDTH, band_h };
    SDL_FillRect(surface, &mid, COLOR_SKY_MID);
    SDL_Rect low = { 0, band_h * 2, WIDTH, GROUND_Y - band_h * 2 };
    SDL_FillRect(surface, &low, COLOR_SKY_LOW);
}

void draw_stars(SDL_Surface* surface, int frame) {
    for (int i = 0; i < MAX_STARS; i++) {
        int phase = (frame / 3 + stars[i].twinkle_phase) % 20;
        if (phase < 16) {  // twinkle: briefly dim
            int s = stars[i].size;
            SDL_Rect star = { stars[i].x, stars[i].y, s, s };
            SDL_FillRect(surface, &star, COLOR_STAR);
        }
    }
}

void draw_ground(SDL_Surface* surface) {
    // Main ground
    SDL_Rect ground = { 0, GROUND_Y, WIDTH, HEIGHT - GROUND_Y };
    SDL_FillRect(surface, &ground, COLOR_GROUND);

    // Neon ground line
    SDL_Rect line = { 0, GROUND_Y, WIDTH, 3 };
    SDL_FillRect(surface, &line, COLOR_GROUND_LINE);

    // Dashed texture on ground
    for (int i = 0; i < WIDTH; i += 30) {
        SDL_Rect dash = { i, GROUND_Y + 12, 12, 2 };
        SDL_FillRect(surface, &dash, COLOR_GROUND_LINE);
    }
}

int check_collision(Dino* dino, Obstacle* obs) {
    if (!obs->active) return 0;

    // Dino hitbox (smaller than visual for fairness)
    int dx = (int)dino->x + 8;
    int dy = (int)dino->y + 5;
    int dw = dino->ducking ? 42 : dino->width - 16;
    int dh = dino->ducking ? 35 : dino->height - 10;

    return (dx < obs->x + obs->width &&
            dx + dw > obs->x &&
            dy < obs->y + obs->height &&
            dy + dh > obs->y);
}

int check_all_collisions(Dino* dino) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (check_collision(dino, &obstacles[i])) {
            return 1;
        }
    }
    return 0;
}

void reset_game(Dino* dino) {
    dino->x = 80;
    dino->y = GROUND_Y - DINO_HEIGHT;
    dino->ducking = 0;

    init_obstacles();

    if (score > high_score) {
        high_score = score;
    }
    score = 0;
    game_speed = 1.0f;
    game_state = STATE_PLAYING;
}

void draw_game_over(SDL_Surface* surface) {
    // Dark overlay
    SDL_Rect overlay = { WIDTH/2 - 160, HEIGHT/2 - 70, 320, 140 };
    SDL_FillRect(surface, &overlay, 0xFF0A0A1A);

    // Border glow
    SDL_Rect border_t = { WIDTH/2 - 160, HEIGHT/2 - 70, 320, 3 };
    SDL_Rect border_b = { WIDTH/2 - 160, HEIGHT/2 + 67, 320, 3 };
    SDL_Rect border_l = { WIDTH/2 - 160, HEIGHT/2 - 70, 3, 140 };
    SDL_Rect border_r = { WIDTH/2 + 157, HEIGHT/2 - 70, 3, 140 };
    SDL_FillRect(surface, &border_t, COLOR_GAMEOVER);
    SDL_FillRect(surface, &border_b, COLOR_GAMEOVER);
    SDL_FillRect(surface, &border_l, COLOR_GAMEOVER);
    SDL_FillRect(surface, &border_r, COLOR_GAMEOVER);

    // "GAME OVER" text approximation
    draw_number(surface, 0, WIDTH/2 - 80, HEIGHT/2 - 50, 4, COLOR_GAMEOVER);

    // Score display
    draw_number(surface, score, WIDTH/2 - 50, HEIGHT/2 + 10, 3, COLOR_WHITE);
}

int main(void) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Neon Dino",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    srand(time(NULL));
    init_obstacles();
    init_clouds();
    init_stars();

    const Uint32 SIM_MS = 16;
    Uint32 last = SDL_GetTicks();
    Uint32 acc = 0;

    Dino dino = {
        .x = 80,
        .y = GROUND_Y - DINO_HEIGHT,
        .width = DINO_WIDTH,
        .height = DINO_HEIGHT,
        .ducking = 0,
        .leg_frame = 0
    };

    float vy = 0;
    float gravity = 0.8f;
    float jumpSpeed = -14.0f;

    int spawn_timer = 0;
    int spawn_interval = 60;
    int anim_frame = 0;

    int running = 1;
    SDL_Event event;
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    while (running) {
        Uint32 now = SDL_GetTicks();
        Uint32 dt = now - last;
        last = now;

        if (dt > 250) dt = 250;
        acc += dt;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;

                if (game_state == STATE_GAMEOVER) {
                    if (key == SDLK_SPACE || key == SDLK_r) {
                        reset_game(&dino);
                        vy = 0;
                        spawn_timer = 0;
                        spawn_interval = 60;
                    }
                } else {
                    if ((key == SDLK_SPACE || key == SDLK_UP || key == SDLK_w) &&
                        dino.y >= GROUND_Y - DINO_HEIGHT - 1) {
                        vy = jumpSpeed;
                    }
                }
            }
        }

        while (acc >= SIM_MS) {
            if (game_state == STATE_PLAYING) {
                // Check duck input (hold DOWN)
                dino.ducking = (keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S]);

                // Fast fall when ducking in air
                if (dino.ducking && dino.y < GROUND_Y - DINO_HEIGHT) {
                    vy += gravity * 2;
                }

                // Dino physics
                vy += gravity;
                dino.y += vy;

                int ground_level = GROUND_Y - (dino.ducking ? 34 : DINO_HEIGHT);
                if (dino.y >= ground_level) {
                    dino.y = ground_level;
                    vy = 0;
                }

                // Animation
                anim_frame = (anim_frame + 1) % 10;
                dino.leg_frame = anim_frame;

                // Obstacle spawning
                spawn_timer++;
                if (spawn_timer >= spawn_interval) {
                    spawn_obstacle();
                    spawn_timer = 0;
                    // Random interval for variety
                    spawn_interval = 40 + rand() % 40;
                }

                // Update game speed
                game_speed = 1.0f + score / 500.0f;
                if (game_speed > 2.5f) game_speed = 2.5f;

                // Update obstacle speeds
                for (int i = 0; i < MAX_OBSTACLES; i++) {
                    if (obstacles[i].active) {
                        obstacles[i].vx = -5.0f * game_speed;
                    }
                }

                update_obstacles();
                update_clouds();

                // Passive score increase
                score++;

                // Collision detection
                if (check_all_collisions(&dino)) {
                    game_state = STATE_GAMEOVER;
                    if (score > high_score) {
                        high_score = score;
                    }
                }
            }
            acc -= SIM_MS;
        }

        // Render
        draw_sky_gradient(surface);
        draw_stars(surface, anim_frame);
        draw_clouds(surface);
        draw_ground(surface);
        draw_dino(surface, &dino);
        draw_obstacles(surface, anim_frame);

        // HUD
        draw_number(surface, score, WIDTH - 150, 20, 3, COLOR_SCORE);
        if (high_score > 0) {
            draw_number(surface, high_score, WIDTH - 300, 20, 2, COLOR_HISCORE);
        }

        if (game_state == STATE_GAMEOVER) {
            draw_game_over(surface);
        }

        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
