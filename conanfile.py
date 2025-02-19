import conan


class LuceProject(conan.ConanFile):
    """
    Conan package definition for the Luce project. Bazel ONLY.
    For CMake, use vcpkg instead.
    """

    name = "luce"
    version = "1.0.0"

    url = "not.defined.yet.but.you.can.find.me.at.github.com.slash.liautraver"
    license = "Apache-2.0"
    author = "LiAuTraver"
    description = "A riscv32 emulator written in C++"
    topics = ("riscv32", "emulator", "cpp")
    homepage = "not.defined.yet"

    settings = "os", "compiler", "build_type", "arch"
    generators = "BazelDeps", "BazelToolchain"
    requires = (
        "scnlib/4.0.1",
        "fmt/11.0.2",
        "spdlog/1.15.0",
        "argparse/3.1",
    )
