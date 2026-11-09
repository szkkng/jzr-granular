# JZR-Granular

[JR-Granular](https://github.com/szkkng/jr-granular) built with Zig using [juzi](https://github.com/szkkng/juzi).

## Building

Requirements:

- [Zig](https://ziglang.org/download/) 0.17.0
- [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/?q=build+tools) (Windows only)
- [JUCE dependency packages](https://github.com/juce-framework/JUCE/blob/master/docs/Linux%20Dependencies.md#packages) (Linux only)

```sh
git clone https://github.com/szkkng/jzr-granular.git
cd jzr-granular

# Build VST3, AUv2, and Standalone into ./zig-out
zig build --release=fast

# Build and install plugin formats (may require elevated permissions)
zig build copy --release=fast

# Run Standalone in debug mode
zig build run

# Print available steps
zig build -l
```

