{ pkgs ? import <nixpkgs> {} }:


pkgs.stdenv.mkDerivation {
    name = "platformio";
    buildInputs = [
        pkgs.platformio
        pkgs.avrdude
        pkgs.gcc
        pkgs.gdb
        pkgs.gnumake
        pkgs.unzip
    ];
}
