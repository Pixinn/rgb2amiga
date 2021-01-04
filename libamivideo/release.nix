{ nixpkgs ? <nixpkgs>
, systems ? [ "i686-linux" "x86_64-linux" ]
, buildForAmiga ? false
, buildForWindows ? false
, amigaosenvPath ? <amigaosenv>
, libamivideo ? {outPath = ./.; rev = 1234;}
, officialRelease ? false
}:

let
  pkgs = import nixpkgs {};
  
  version = builtins.readFile ./version;
  
  jobs = rec {
    tarball =
      with pkgs;

      releaseTools.sourceTarball {
        name = "libamivideo-tarball";
        src = libamivideo;
        inherit version officialRelease;

        buildInputs = [];
      };
      
    build =
      (pkgs.lib.genAttrs systems (system:
        with import nixpkgs { inherit system; };
        
        releaseTools.nixBuild {
          name = "libamivideo";
          inherit version;
          src = tarball;
          CFLAGS = "-ansi -pedantic -Wall";
        }
      )) //
        (pkgs.lib.optionalAttrs (buildForWindows) { i686-windows =
           pkgs.dotnetenv.buildSolution {
             name = "libamivideo";
             src = ./.;
             baseDir = "src";
             slnFile = "libamivideo.sln";
             postInstall = ''
               mkdir -p $out/include/libamivideo
               cp -v libamivideo/*.h $out/include/libamivideo
             '';
           };
        }) //
      (pkgs.lib.optionalAttrs (buildForAmiga)
        (let
          amigaosenv = import amigaosenvPath {
            inherit (pkgs) stdenv uae procps;
            lndir = pkgs.xorg.lndir;
          };
        in
        {
          m68k-amigaos = amigaosenv.mkDerivation {
            name = "libamivideo-${version}";
            src = "${tarball}/tarballs/libamivideo-${version}pre1234.tar.gz";
      
            buildCommand = ''
              tar xfvz $src
              cd libamivideo-${version}pre1234
              CFLAGS='-noixemul -O3' ./configure --prefix=/OUT --disable-shared
              make
              make check
              make install
            '';
          };
        }));
  };
in
jobs
