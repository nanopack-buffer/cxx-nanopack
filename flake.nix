{
  description = "C++ support library for NanoPack";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?tag=24.05";
  };

  outputs = { self, nixpkgs, ... }:
    let
      supportedSystems = [ "x86_64-linux" "x86_64-darwin" "aarch64-linux" "aarch64-darwin" ];

      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;

      nixpkgsFor = forAllSystems (system: import nixpkgs { inherit system; });
    in
    {
      packages = forAllSystems (system:
        let
          pkgs = nixpkgsFor.${system};
        in
        rec {
          libnanopack = pkgs.stdenv.mkDerivation {
            pname = "libnanopack";
            version = "0.1.0";
            src = ./.;
            nativeBuildInputs = [
              pkgs.cmake
            ];
          };

          default = libnanopack;
        }
      );

      devShells = forAllSystems (system:
        let
          pkgs = nixpkgsFor.${system};
        in
        {
          default = pkgs.mkShell {
            packages = [
              pkgs.cmake
              pkgs.clang-tools
            ];
          };
        }
      );
    };
}
