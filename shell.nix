with import <nixpkgs> {};
mkShell {
  buildInputs = [
    bear gmpxx
  ];
}
