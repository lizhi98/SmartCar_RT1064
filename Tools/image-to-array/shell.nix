with import <nixpkgs> {};
mkShell {
  LD_LIBRARY_PATH = lib.makeLibraryPath [
    stdenv.cc.cc
    openssl
    fontconfig
  ];
}
