{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    bear
    gcc
    gnumake
    git
  ];
}
