import conan


class LuceProject(conan.ConanFile):
    """
    Conan package definition for the Luce project. Bazel ONLY.
    >>> conan install . --output-folder=conan_installed
    >>> bazel --bazelrc=conan_installed/conan_bzk.rc build //tools/luce:luce # seems the rc was optional
    """

    name = "luce"
    version = "1.0.0"

    url = "https://conan.io"
    license = "Apache-2.0"
    author = "LiAuTraver"
    description = "A riscv64 emulator written in C++"
    topics = ("riscv64", "emulator", "cpp")
    homepage = "https://conan.io"

    settings = "os", "compiler", "build_type", "arch"
    generators = "BazelDeps", "BazelToolchain"
    requires = (
        "capstone/5.0.1",
        "scnlib/4.0.1",
        "fmt/11.0.2",
        "spdlog/1.15.0",
        "argparse/3.1",
        "magic_enum/0.9.7",
    )
