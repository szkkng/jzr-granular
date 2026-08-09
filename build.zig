const std = @import("std");
const juzi = @import("juzi");
const zon = @import("build.zig.zon");
const zcc = @import("compile_commands");
const builtin = @import("builtin");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{
        .default_target = if (builtin.os.tag == .windows) .{ .abi = .msvc } else .{},
    });
    const optimize = b.standardOptimizeOption(.{});

    var plugin = juzi.Plugin.init(b, .{
        .juzi = b.dependency("juzi", .{}),
        .target = target,
        .optimize = optimize,
        .config = .{
            .product_name = "jzr-granular",
            .plugin_name = "JZR-Granular",
            .version = zon.version,
            .bundle_id = "dev.kengo.JzrGranular",
            .plugin_manufacturer_code = "Kngo",
            .plugin_code = "Jzr0",
            .formats = &.{ .vst3, .au, .standalone },
            .company_website = "https://kengo.dev",
            .company_name = "Kengo",
            .company_email = "hi@kengo.dev",
        },
        .juce_modules = &.{juzi.modules.juce_audio_utils},
        .cxx_standard = .cxx20,
    });
    plugin.root_module.addCSourceFiles(.{
        .root = b.path("src"),
        .files = &.{
            "PluginEditor.cpp",
            "PluginProcessor.cpp",
            "ui/Dial.cpp",
            "ui/EditorContent.cpp",
            "ui/EditorLnf.cpp",
        },
    });
    plugin.root_module.addIncludePath(b.path("export/rnbo"));
    plugin.root_module.addIncludePath(b.path("export/rnbo/common"));
    plugin.root_module.addCSourceFiles(.{
        .root = b.path("export"),
        .files = &.{
            "rnbo_granular.cpp",
            "rnbo/RNBO.cpp",
        },
        .flags = &.{
            // RNBO_Engine.cpp uses std::chrono without including its header.
            "-includechrono",
        },
    });
    plugin.addJuceMacro("JUCE_VST3_CAN_REPLACE_VST2", "0");
    plugin.addJuceMacro("JUCE_WEB_BROWSER", "0");
    plugin.addJuceMacro("JUCE_USE_CURL", "0");
    plugin.addBinaryData(.{
        .files = &.{"res/UbuntuRegular.ttf"},
    });

    const result = plugin.finalize();

    var steps_it = result.install_steps.valueIterator();
    while (steps_it.next()) |step| {
        b.getInstallStep().dependOn(step.*);
    }

    var targets: std.ArrayList(*std.Build.Step.Compile) = .empty;
    var artifacts_it = result.artifacts.valueIterator();
    while (artifacts_it.next()) |artifact| {
        targets.append(b.allocator, artifact.*) catch @panic("OOM");
    }
    const cdb_step = zcc.createStep(b, "cdb", targets.toOwnedSlice(b.allocator) catch @panic("OOM"));

    if (result.binary_data) |bd| {
        cdb_step.dependOn(&bd.step);
    }
}
