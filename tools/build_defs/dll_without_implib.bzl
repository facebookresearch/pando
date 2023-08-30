# Several binary vendor libraries that we use consist of one main DLL (for
# which a .lib is provided to link against) and some number of auxiliary DLLs
# that the main one depends on (for which no .lib is provided). We'd like to
# have a prebuilt_cxx_library for all of these DLLs to capture their
# dependency graph, but prebuilt_cxx_library tries to add the shared_lib to the
# linker command if you don't give it an import_lib. This macro generates an
# appropriately named import library with no exports using lib.exe, then
# passes it to prebuilt_cxx_library to work around this behavior.

def dll_without_implib(name, dll, **kwargs):
    native.genrule(
        name = name + "_implib",
        out = name + ".lib",
        cmd_exe = "echo \"LIBRARY {0}\" > {0}.def && lib.exe /DEF:{0}.def /OUT:%OUT%",
    )

    native.prebuilt_cxx_library(
        name = name,
        import_lib = ":" + name + "_implib",
        shared_lib = dll,
        **kwargs
    )
