# Bytescript 1.0

Bytescript is an object-oriented interpreted programming language.
it is general-purpose, embeddable language, made as a side project, it does not offer any cool features, nor is it efficient.

## Contents

- [Installation](#installation)
- [Usage](#usage)
- [Documentation](#documentation)

## Installation

There are two ways of installing bytescript :

1. Releases
    - Download the latest release from the github repo
    - Move the release into `C:\Program Files`
    - Add the folder to your computer's `environnement variables`'s `PATH`

<br>

2. Make
    - Download the source code from the github repo
    - Compile bytescript using `make`
    - Move the `dist` folder into `C:\Program Files`
    - Add the folder to your computer's `environnement variables`'s `PATH`

## Usage

`bytescript.exe mycode.bs` or `bytescript mycode.bs`

## Documentation

- [Syntax](docs/syntax.md)
- [Standard library](docs/stdlib.md)
- [Embedding](docs/embedding.md)
- [BVM](docs/bvm.md)