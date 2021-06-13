# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to
[Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- A changelog.
- Basic audio plugin.
- Basic UI plugin.
- `GhostObject` for detecting 3D collisions without any physical response.

### Fixed

- Fixed a crash when the window is minimised.

## [v0.1.0] - 2021-06-11

### Added

- ECS architecture.
- Basic 3D renderer with support for textured and coloured materials.
- Hard-coded lightning with a directional light and slight ambient light.
- `ScriptPlugin` providing a familiar Unity-like `CppBehaviour` class.
- Basic 3D physics capabilities using
  [bullet3](https://github.com/bulletphysics/bullet3).
- Easy to use input manager.
- glTF model loading support.

[unreleased]: https://github.com/nasso/ige/compare/v0.1.0...HEAD
[v0.1.0]: https://github.com/nasso/ige/releases/tag/v0.1.0
