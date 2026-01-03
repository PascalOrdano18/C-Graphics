{
	description = "C graphics projects with SDL2 and other things dev shell"

	inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

	outputs = { self, nixpkgs };
		let 
		   system = "x86_64-linux";
		   pkgs = import nixpkgs { inherit system; };
		in 
		{
		   devShells.${system}.default = pkgs.mkSHell {
   			packages = with pkgs; [
			    gcc
			    gnumake
			    pkg-config
			    SDL2
			    SDL2.dev
			];
		   };
		};
}
