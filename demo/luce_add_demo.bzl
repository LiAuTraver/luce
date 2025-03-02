"""
This file contains helper functions for creating demo flight for the luce project.
"""

load("@rules_cc//cc:defs.bzl", "cc_binary")

def luce_add_demo(name, srcs):
    cc_binary(
        name = name,
        srcs = srcs,
        copts = [
            # "/Iexternal/gtest/googletest/include",
            # "/Iexternal/gtest/googletest",
            "/std:c++latest",
            "/Isource/auxilia/include",
            "/Idriver",
            "/Iinclude",
            "/Zc:preprocessor",
            "/wd4068",
        ],
        defines = [
            "AC_CPP_DEBUG",
            "SPDLOG_FMT_EXTERNAL",
            "FMT_SHARED",
            "SPDLOG_COMPILED_LIB",
            "_CRT_SECURE_NO_WARNINGS",
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
