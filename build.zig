const std = @import("std");
const juzi = @import("juzi");
const zon = @import("build.zig.zon");

pub fn build(b: *std.Build) void {
    var plugin = juzi.Plugin.init(b, .{
        .juzi = b.dependency("juzi", .{}),
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
        .flags = cxx_flags,
    });
    plugin.root_module.addIncludePath(b.path("export/rnbo"));
    plugin.root_module.addIncludePath(b.path("export/rnbo/common"));
    plugin.root_module.addCSourceFiles(.{
        .root = b.path("export"),
        .files = &.{
            "rnbo_granular.cpp",
            "rnbo/RNBO.cpp",
        },
        .flags = rnbo_cxx_flags,
    });
    plugin.root_module.addCMacro("JUCE_VST3_CAN_REPLACE_VST2", "0");
    plugin.root_module.addCMacro("JUCE_WEB_BROWSER", "0");
    plugin.root_module.addCMacro("JUCE_USE_CURL", "0");
    plugin.addBinaryData(.{
        .files = &.{"res/UbuntuRegular.ttf"},
    });

    plugin.finalize();

    var steps_it = plugin.install_steps.valueIterator();
    while (steps_it.next()) |step| {
        b.getInstallStep().dependOn(step.*);
    }
}

const cxx_flags = &.{
    "--std=c++20",
    "-fvisibility=hidden",
    "-fvisibility-inlines-hidden",
};

const rnbo_cxx_flags = &.{
    "--std=c++20",
    "-fvisibility=hidden",
    "-fvisibility-inlines-hidden",
    // RNBO_Engine.cpp uses std::chrono without including its header.
    "-includechrono",
};
