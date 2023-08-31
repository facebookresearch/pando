def define_helpers():
    native.python_library(
        name = "protobuf_rule_helper_lib",
        srcs = [
            "protobuf_rule_helper.py",
        ],
        base_module = "",
    )

    native.python_binary(
        name = "protobuf_rule_helper",
        deps = [
            ":protobuf_rule_helper_lib",
        ],
        main_module = "protobuf_rule_helper",
    )

    native.command_alias(
        name = "cp",
        exe = ":protobuf_rule_helper",
        args = ["cp"],
        visibility = [
            "PUBLIC",
        ],
    )

    native.command_alias(
        name = "mkdirs",
        exe = ":protobuf_rule_helper",
        args = ["mkdirs"],
        visibility = [
            "PUBLIC",
        ],
    )

    native.command_alias(
        name = "prepare",
        exe = ":protobuf_rule_helper",
        args = ["prepare"],
        visibility = [
            "PUBLIC",
        ],
    )
