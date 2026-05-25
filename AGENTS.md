# AGENTS

Project type: Arduino library.

## Building docs

requires: [doxygen]

steps:

- `cd docs`
- `doxygen` (generates `docs/html`)
- optional: open `docs/html/index.html`

## Format sources

requires: `clang-format` v14.x

steps:

- `clang-format-14 -i --style=file <file>`
- On Windows (powershell does not expand globs):
  `clang-format -i --style=file <file>`

[doxygen]: https://github.com/doxygen/doxygen
