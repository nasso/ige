# ige

[![build + tests](https://github.com/nasso/ige/actions/workflows/ci.yml/badge.svg)](https://github.com/nasso/ige/actions/workflows/ci.yml)

`ige` is a 3D game engine built in C++20 for educational purposes.

## Contributing

Contributions are welcome! Just keep the following rules in mind:

- **The repository uses a rebase workflow and has a "no-merge policy" similar to
  the [one used by the rust-lang repository][rust-nmp].** This helps keeping a
  clean commit history.
- **All the code in this repository must be formatted according to the
  `.clang-format` configuration files provided.** However, `clang-format` will
  in some rare occasions, format the code in a weird way (e.g. with some recent
  C++20 syntax that doesn't have a very good support yet). You can temporarily
  turn it off with `// clang-format off/on` blocks.
- Most importantly, try to **keep the code as consistent as possible** with
  what's already here!

[rust-nmp]: https://rustc-dev-guide.rust-lang.org/git.html#no-merge-policy

## License

`ige` is licensed under the MIT license. See [LICENSE] for the full text.

[license]: LICENSE
