def tool_rule(name, add_sysroot_arg = False):
    # This genrule makes a symlink to a single tool inside the installed yocto
    # SDK. The reason for this is that a genrule can be marked "executable",
    # which allows it to be used as the exe argument in the command_alias tool_rule
    # below.
    bin_name = "aarch64-xilinx-linux-" + name
    native.genrule(
        name = bin_name,
        cmd = "ln -s $(location :yocto_sdk)/sysroots/x86_64-petalinux-linux/usr/bin/aarch64-xilinx-linux/" + bin_name + " $OUT",
        out = bin_name,
        executable = True,
    )

    # We use a command_alias rule to staple a --sysroot argument (pointing to
    # the target sysroot in the installed Yocto SDK) onto the gcc and g++
    # executables. We need to expand a $(location ...) macro to find the path
    # of the installed SDK, and the args argument to command_alias is one of a
    # small handful of contexts where such macros are expanded.

    args = []
    if (add_sysroot_arg):
        args += [
            "--sysroot=$(location :yocto_sdk)/sysroots/aarch64-xilinx-linux",
        ]

    native.command_alias(
        name = name,
        exe = ":" + bin_name,
        args = args,
    )
