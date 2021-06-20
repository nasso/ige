# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to
[Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.3.2] - 2021-06-20

### Fixed

- EventChannel Subscription operator= didn't return a value.

## [0.3.1] - 2021-06-20

### Fixed

- Crash when entities are removed by scripts.

## [0.3.0] - 2021-06-20

### Added

- Asynchronous `Task<T>` to load assets without blocking.
- OpenGL back-end releases textures properly now.
- Add a `IGE_DEBUG` CMake option to enable/disable some debug-only assertions.

### Changed

- `AudioClip` is now loaded with `AudioClip::load` or `AudioClip::load_async`
  instead of its constructor.

### Removed

- Removed `AudioClip` constructor taking an audio file path.

### Fixed

- Unnecessary compilation of generated files when re-configuring CMake.

## [0.2.3] - 2021-06-19

### Fixed

- Occasional crash when textures are released.

## [0.2.2] - 2021-06-18

### Added

- Add support for animations targeting multiple skeletons.

## [0.2.1] - 2021-06-18

### Fixed

- Models that aren't playing any animation are reset to bind pose.

## [0.2.0] - 2021-06-17

### Added

- A changelog.
- Basic audio plugin.
- Basic UI plugin.
- `GhostObject` for detecting 3D collisions without any physical response.
- Skeletal animation support.
- Basic diffuse lighting
- Input game controller support
- JSON serialised input bindings.

### Fixed

- Fixed a crash when the window is minimised.

## [0.1.0] - 2021-06-11

### Added

- ECS architecture.
- Basic 3D renderer with support for textured and coloured materials.
- Hard-coded lighting with a directional light and slight ambient light.
- `ScriptPlugin` providing a familiar Unity-like `CppBehaviour` class.
- Basic 3D physics capabilities using
  [bullet3](https://github.com/bulletphysics/bullet3).
- Easy to use input manager.
- glTF model loading support.

[unreleased]: https://github.com/nasso/ige/compare/v0.3.2...HEAD
[0.3.2]: https://github.com/nasso/ige/compare/v0.3.1...v0.3.2
[0.3.1]: https://github.com/nasso/ige/compare/v0.3.0...v0.3.1
[0.3.0]: https://github.com/nasso/ige/compare/v0.2.3...v0.3.0
[0.2.3]: https://github.com/nasso/ige/compare/v0.2.2...v0.2.3
[0.2.2]: https://github.com/nasso/ige/compare/v0.2.1...v0.2.2
[0.2.1]: https://github.com/nasso/ige/compare/v0.2.0...v0.2.1
[0.2.0]: https://github.com/nasso/ige/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/nasso/ige/releases/tag/v0.1.0
