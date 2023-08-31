FORBIDDEN_ARGS = [
    "exported_headers",
    "exported_linker_flags",
    "exported_platform_headers",
    "exported_preprocessor_flags",
    "header_dirs",
    "header_namespace",
    "headers",
    "include_dirs",
    "linker_flags",
    "platform_headers",
    "platform_header_dirs",
    "preferred_linkage",
]

def _validate_args(args, name):
    if name == "":
        fail("Target in {}//{} must have a name".format(
            native.repository_name(),
            native.package_name(),
        ))

    for arg in FORBIDDEN_ARGS:
        if arg in args:
            fail("Argument " + arg + " is forbidden in oxx_* macros in target {}//{}:{}".format(
                native.repository_name(),
                native.package_name(),
                name,
            ))

def _oxx_prebuilt_library(
        preferred_linkage,
        name = "",
        public_include_directories = [],
        public_raw_headers = [],
        select_public_raw_headers = None,
        public_preprocessor_flags = [],
        public_linker_flags = [],
        public_deps = [],
        deps = [],
        resources = None,
        third_party = False,
        **kwargs):
    _validate_args(kwargs, name)

    header_name = name + "_headers"

    # cxx_library() tracks header usage better than prebuilt_cxx_library(),
    # so use that through our oxx wrapper.
    oxx_static_library(
        name = header_name,
        public_include_directories = public_include_directories,
        public_raw_headers = public_raw_headers,
        select_public_raw_headers = select_public_raw_headers,
        third_party = third_party,
        # Hack this here cause the resources shouldn't be reexported, philosophically.
        deps = deps,
    )

    native.prebuilt_cxx_library(
        name = name,
        exported_preprocessor_flags = public_preprocessor_flags,
        exported_linker_flags = public_linker_flags,
        exported_deps = public_deps + [":" + header_name],
        preferred_linkage = preferred_linkage,
        **kwargs
    )

def oxx_prebuilt_static_library(**kwargs):
    _oxx_prebuilt_library(
        preferred_linkage = "static",
        **kwargs
    )

def oxx_prebuilt_shared_library(**kwargs):
    _oxx_prebuilt_library(
        preferred_linkage = "shared",
        **kwargs
    )

def _oxx_library(
        preferred_linkage,
        linker_flags,
        name = "",
        srcs = [],
        raw_headers = [],
        select_raw_headers = None,
        public_raw_headers = [],
        select_public_raw_headers = None,
        preprocessor_flags = [],
        public_preprocessor_flags = [],
        compiler_flags = [],
        public_linker_flags = [],
        deps = [],
        public_deps = [],
        resources = None,
        host = False,
        third_party = False,
        labels = [],
        public_include_directories = [],
        **kwargs):
    if (select_raw_headers != None):
        fail("select_raw_headers not suppored")
    if (select_public_raw_headers != None):
        fail("select_public_raw_headers not suppored")

    all_raw_headers = list({element: None for element in raw_headers + public_raw_headers}.keys())

    native.cxx_library(
        name = name,
        srcs = srcs,
        raw_headers = all_raw_headers,
        preprocessor_flags = preprocessor_flags,
        exported_preprocessor_flags = public_preprocessor_flags,
        compiler_flags = compiler_flags,
        exported_linker_flags = public_linker_flags,
        deps = deps + public_deps,
        preferred_linkage = preferred_linkage,
        linker_flags = linker_flags,
        labels = labels,
        public_include_directories = public_include_directories,
        **kwargs
    )

def oxx_static_library(**kwargs):
    _oxx_library(
        preferred_linkage = "static",
        linker_flags = None,
        **kwargs
    )

def oxx_shared_library(
        name,
        linker_flags = [],
        link_style = "static",
        soname = None,
        **kwargs):
    _oxx_library(
        name = name,
        preferred_linkage = "shared",
        linker_flags = linker_flags,
        link_style = link_style,
        soname = soname,
        **kwargs
    )

def oxx_binary(
        name = "",
        srcs = [],
        raw_headers = [],
        preprocessor_flags = [],
        compiler_flags = [],
        linker_flags = [],
        runtime_shared_libraries = [],
        deps = [],
        resources = None,
        third_party = False,
        host = False,
        visibility = [],
        **kwargs):
    _validate_args(kwargs, name)

    native.cxx_binary(
        name = name,
        srcs = srcs,
        preprocessor_flags = preprocessor_flags,
        raw_headers = raw_headers,
        compiler_flags = compiler_flags,
        linker_flags = linker_flags,
        deps = deps + runtime_shared_libraries,
        visibility = visibility,
        **kwargs
    )

def oxx_test(
        name,
        oncall = None,
        srcs = [],
        raw_headers = [],
        preprocessor_flags = [],
        compiler_flags = [],
        linker_flags = [],
        runtime_shared_libraries = [],
        deps = [],
        env = {},
        resources = None,
        run_test_separately = False,
        framework = None,
        **kwargs):
    _validate_args(kwargs, name)

    native.cxx_test(
        name = name,
        srcs = srcs,
        preprocessor_flags = preprocessor_flags,
        raw_headers = raw_headers,
        compiler_flags = compiler_flags,
        linker_flags = linker_flags,
        deps = deps,
        env = env,
        framework = framework,
        run_test_separately = run_test_separately,
        **kwargs
    )
