def get_path(f):
    if (f.find("/") == -1):
        return ""
    else:
        return "/".join(f.split("/")[:-1])

def get_filename(f):
    return f.split("/")[-1]

def _fix_slash(path):
    return path.replace("/", "\\")

def protobuf_rule(
        name,
        protos,
        proto_deps = [],
        namespace = "",
        lite = False,
        # Used in fbsource
        protoc_exe = "$(exe //third-party/protobuf:host_protoc)",
        protobuf_path = "//third-party/protobuf"):
    sources = []
    headers = {}

    prepare_args = []
    prepare_args += ["TMP", "$TMP"]
    prepare_args += ["SRCDIR", "$SRCDIR"]
    prepare_args += ["NAMESPACE", "N=" + namespace]
    prepare_args += ["PROTOS"] + protos

    prepare_args += ["PROTO_DEPS"]
    for dep_namespace, target in proto_deps:
        prepare_args += [
            "N=" + dep_namespace,
            "$(location {}_proto_deps)".format(target),
        ]

    native.filegroup(
        name = name + "_proto_deps",
        srcs = protos,
        visibility = ["PUBLIC"],
    )

    for proto in protos:
        proto_path = get_path(proto)
        proto_file = get_filename(proto)
        proto_name = proto_file if proto_file.find(".") == -1 else ".".join(proto_file.split(".")[:-1])
        full_proto_name = proto_name if (proto_path == "") else proto_path + "/" + proto_name
        protoc_cmd = "{} -I $TMP -I $TMP/{}{} --cpp_out={}$OUT $TMP/{}{}".format(protoc_exe, namespace, proto_path, "lite:" if lite else "", namespace, proto)
        proto_rule_name = proto_file.replace("/", "_")

        native.genrule(
            name = proto_rule_name,
            srcs = protos,
            cmd = " && ".join([
                "$(exe {}:prepare) ".format(protobuf_path) + " ".join(prepare_args),
                protoc_cmd,
            ]),
            out = ".",
        )

        full_cpp = full_proto_name + ".pb.cc"
        full_h = full_proto_name + ".pb.h"
        cpp = proto_name + ".pb.cc"
        h = proto_name + ".pb.h"
        gen_proto_rule_cpp_name = full_cpp.replace("/", "_")
        gen_proto_rule_h_name = full_h.replace("/", "_")

        native.genrule(
            name = gen_proto_rule_h_name,
            cmd = "$(exe {}:cp) $(location :{})/{}{} $OUT".format(protobuf_path, proto_file, namespace, full_h),
            out = h,
        )
        headers[namespace + full_h] = ":{}".format(gen_proto_rule_h_name)

        native.genrule(
            name = gen_proto_rule_cpp_name,
            cmd = "$(exe {}:cp) $(location :{})/{}{} $OUT".format(protobuf_path, proto_file, namespace, full_cpp),
            out = cpp,
        )
        sources += [":{}".format(gen_proto_rule_cpp_name)]
    return (sources, headers)

# A wrapper method to generate a cxx_library for protobuf messages. A target is created for the generated c++ files
# with the given name, visibility, and deps. The header namespace is used as normal -- the headers of the generated
# protos may be included with the header_namespace path.
def proto_cxx_library(name, protos, proto_deps = [], header_namespace = "", visibility = []):
    # Ensure the namespace has a trailing slash since this is how the protobuf directories
    # are set up.
    if len(header_namespace) != 0:
        if header_namespace[-1] != "/":
            header_namespace = header_namespace + "/"

    (protobuf_srcs, protobuf_exported_headers) = protobuf_rule(name, protos, proto_deps, namespace = header_namespace)

    protobuf_exported_headers_with_header_namespace = \
        {header_namespace + k: protobuf_exported_headers[k] for k in protobuf_exported_headers}
    protobuf_exported_headers_with_header_namespace.update(protobuf_exported_headers)

    # note that if we have the following lib structure
    # libfoo
    # |
    # --proto
    #   |
    #   |
    #   -foo.proto

    # proto_rule(["foo.proto"])   # note that we are not providing any paths
    # the above will create the following sources
    # protobuf_srcs = ["foo.pb.cc"]
    # protobuf_exported_headers = {"foo.pb.h": ":foo.pb.h"}
    # protobuf_exported_headers_with_header_namespace = \
    #  {"foo.pb.h", ":foo.pb.h", "<namespace>foo.pb.h": ":foo.pb.h"}
    # By using this dictionary mapping, we support both
    # #include "foo.pb.h" and
    # #include <<namespace>foo.pb.h>

    deps = []
    for (_, dep) in proto_deps:
        deps.append(dep)

    # Note for this trick to work, we need cxx_library header_namespace = ""
    native.cxx_library(
        name = name,
        srcs = protobuf_srcs,
        header_namespace = "",
        exported_headers = protobuf_exported_headers_with_header_namespace,
        deps = deps,
        exported_deps = [
            "//third-party/protobuf:protobuf",
        ],
        visibility = visibility,
        preferred_linkage = "static",
    )

def _py_protobuf_rule(
        name,
        protos,
        proto_deps = [],
        namespace = "",
        lite = False):
    protobuf_path = "//third-party/protobuf"
    protoc_exe = "$(exe //third-party/protobuf:protoc)"
    pyfiles = []
    prepare_args = []
    prepare_args += ["TMP", "$TMP"]
    prepare_args += ["SRCDIR", "$SRCDIR"]
    prepare_args += ["NAMESPACE", "N=" + namespace]
    prepare_args += ["PROTOS"] + protos

    prepare_args += ["PROTO_DEPS"]
    for dep_namespace, target in proto_deps:
        prepare_args += [
            "N=" + dep_namespace,
            "$(location {}_proto_deps)".format(target),
        ]

    native.filegroup(
        name = name + "_proto_deps",
        srcs = protos,
        visibility = ["PUBLIC"],
    )

    for proto in protos:
        proto_path = get_path(proto)
        proto_file = get_filename(proto)
        proto_name = proto_file if proto_file.find(".") == -1 else ".".join(proto_file.split(".")[:-1])
        full_proto_name = proto_name if (proto_path == "") else proto_path + "/" + proto_name
        protoc_cmd = "{} -I $TMP -I $TMP/{}{} --python_out={}$OUT $TMP/{}{}".format(protoc_exe, namespace, proto_path, "lite:" if lite else "", namespace, proto)
        proto_rule_name = proto_file.replace("/", "_") + "prepare_py"

        native.genrule(
            name = proto_rule_name,
            srcs = protos,
            cmd = " && ".join([
                "$(exe {}:prepare) ".format(protobuf_path) + " ".join(prepare_args),
                protoc_cmd,
            ]),
            out = ".",
        )

        full_py = full_proto_name + "_pb2.py"
        py = proto_name + "_pb2.py"
        gen_proto_rule_name = py.replace("/", "_")
        gen_proto_rule_name = py.replace(".py", "__ext__py")
        native.genrule(
            name = gen_proto_rule_name,
            cmd = "$(exe {}:cp) $(location :{})/{}{} $OUT".format(protobuf_path, proto_rule_name, namespace, full_py),
            out = py,
            visibility = ["PUBLIC"],
        )
        pyfiles += [":{}".format(gen_proto_rule_name)]
    return pyfiles

# A wrapper method to generate a python_library for protobuf messages. A target is created for the generated python files
# with the given name, visibility, and deps.
def proto_py_library(name, protos, base_module = "arvr", proto_deps = [], namespace = "", visibility = []):
    pyfiles = _py_protobuf_rule(name, protos, proto_deps, namespace = namespace)
    deps = []
    for (_, dep) in proto_deps:
        deps.append(dep)

    native.python_library(
        name = name,
        srcs = pyfiles,
        base_module = base_module,
        deps = deps,
        visibility = visibility,
    )
