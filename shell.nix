{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    bear
  ];

  shellHook = ''
    alias g++="clang++"
  '';
}

