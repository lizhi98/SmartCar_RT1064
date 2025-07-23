{ pkgs ? import <nixpkgs> {} }:
with pkgs; mkShell {
  buildInputs = [
    python312
  ];

  LD_LIBRARY_PATH = lib.makeLibraryPath [
    stdenv.cc.cc.lib
  ];
}