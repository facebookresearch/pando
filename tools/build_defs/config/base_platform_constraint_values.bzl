base_platform_constraint_values = struct(
    windows_x86_64 = [
        "//os/constraints:windows",
        "//cpu/constraints:x86_64",
        "//toolchain/constraints:msvc",
    ],
    linux_x86_64 = [
        "//os/constraints:linux",
        "//cpu/constraints:x86_64",
        "//toolchain/constraints:gnu",
    ],
    linux_xilinx_aarch64 = [
        "//os/constraints:linux",
        "//cpu/constraints:aarch64",
        "//toolchain/constraints:gnu",
    ],
)
