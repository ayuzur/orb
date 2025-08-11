{
  description = "Orb";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  };

  outputs = {nixpkgs, ...}: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {
      inherit system;
      config.allowUnfreePredicate = true;
    };
  in {
    devShells.x86_64-linux = {
      default = pkgs.mkShell {
        buildInputs = builtins.attrValues {
          inherit
            (pkgs)
            ncurses
            gnumake
            ;
          #;
          inherit
            (pkgs.gcc.cc)
            lib
            ;
        };
      };
    };
  };
}
