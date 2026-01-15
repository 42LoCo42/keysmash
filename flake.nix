{
  outputs = { flake-utils, nixpkgs, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        inherit (pkgs.lib.fileset) toSource unions;
        stdenv = pkgs.gccStdenv;

        sound_dir = (pkgs.fetchFromGitHub {
          owner = "skkeeper";
          repo = "linux-clicky";
          rev = "ef5070829adfc7b3906852849d574c80caf4196c";
          hash = "sha256-W+NJZ8ARza1Brh5fLlz7hUYniQDpEGOvZQZpb2KDQRI=";
        }) + /sounds;
      in
      rec {
        packages.default = stdenv.mkDerivation (drv: {
          pname = "keysmash";
          version = "1.0.0";

          src = toSource {
            root = ./.;
            fileset = unions [
              ./meson.build
              ./meson_options.txt
              ./src
            ];
          };

          nativeBuildInputs = with pkgs; [
            meson
            ninja
            pkg-config
          ];

          buildInputs = with pkgs; [
            raylib
          ];

          mesonBuildType = "release";

          mesonFlags = [
            "-Dkeyd=${pkgs.keyd.src}/src"
            "-Dsound_dir=${placeholder "out"}/share/sounds/keysmash"
          ];

          postInstall = ''
            mkdir -p $out/share/sounds
            cp -r ${sound_dir} $out/share/sounds/keysmash
          '';

          meta.mainProgram = drv.pname;
        });

        devShells.default = (pkgs.mkShell.override {
          inherit stdenv;
        }) {
          inputsFrom = [ packages.default ];

          packages = with pkgs; [
            clang-tools
            just
            lice
          ];

          env = {
            KEYD = pkgs.keyd.src + /src;
            SOUND_DIR = sound_dir;
          };
        };
      });
}
