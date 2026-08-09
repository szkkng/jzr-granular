# JZR-Granular

[JR-Granular](https://github.com/szkkng/jr-granular) built with Zig using [juzi](https://codeberg.org/kengo/juzi).

## Building

Requirements:

- [Zig](https://ziglang.org/download/) 0.16.0
- [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/?q=build+tools) (Windows only)
- [JUCE dependency packages](https://github.com/juce-framework/JUCE/blob/master/docs/Linux%20Dependencies.md#packages) (Linux only)

```sh
git clone https://github.com/szkkng/jzr-granular.git
cd jzr-granular

# Build VST3, AU, and Standalone and install them under ./zig-out
zig build -Doptimize=ReleaseFast

# Alternatively, use `-p` to install each format to a custom location
zig build vst3 -Doptimize=ReleaseFast -p ~/Library/Audio/Plug-Ins/VST3
zig build au -Doptimize=ReleaseFast -p ~/Library/Audio/Plug-Ins/Components

# Run Standalone in debug mode
zig build run
```

