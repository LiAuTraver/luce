"""
This file contains helper functions for creating tests for the luce project.
"""

load("@rules_cc//cc:defs.bzl", "cc_test")

def luce_add_test(name, src):
    cc_test(
        name = name,
        srcs = [
            src,
        ],
        copts = [
            "/Iexternal/gtest/googletest/include",
            "/Iexternal/gtest/googletest",
            "/std:c++latest",
            "/Isource/auxilia/include",
            "/Zc:preprocessor",
            "/Iinclude",
        ],
        defines = [
            "AC_CPP_DEBUG",
            "SPDLOG_FMT_EXTERNAL",
        ],
        deps = [
            "//driver",
            "//source/auxilia",
            "@fmt",
            "@spdlog",
            "@googletest//:gtest_main",
        ],
        visibility = ["//.git:__pkg__"],
    )
