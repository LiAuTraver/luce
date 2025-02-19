"""
This file contains helper functions for creating demo flight for the luce project.
"""

load("@rules_cc//cc:defs.bzl", "cc_binary")

def luce_add_demo(name, srcs):
    cc_binary(
        name = name,
        srcs = srcs,
        copts = [
            "/std:c++latest",
            "/Isource/auxilia/include",
            "/Zc:preprocessor",
            "/Iinclude",
            "/MDd",  # Multi-threaded Debug DLL
        ],
        defines = [
            "AC_CPP_DEBUG",
            "SPDLOG_FMT_EXTERNAL",
        ],
        linkstatic = True,
        visibility = ["//git:__pkg__"],
        deps = [
            "//source/auxilia",
            "//include:external_deps",
            "@fmt",
            "@spdlog",
        ],
    )
