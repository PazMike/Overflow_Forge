{
  description = "ulfius Web Framework";
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    c-flake.url = "github:judah-sotomayor/c-flake";
  };

  outputs = {self, nixpkgs, c-flake} :
    let
      forAllSystems = function:
        nixpkgs.lib.genAttrs [
          "x86_64-linux"
        ] (system: function nixpkgs.legacyPackages.${system} system);
    in
      {
        devShells = forAllSystems (pkgs: system: {
          default = pkgs.mkShell {
            packages = with pkgs; [
              c-flake.packages.${system}.ulfius
            ];
          };
        });
      };
}
