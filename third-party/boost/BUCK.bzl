load(
    "//tools/build_defs:oxx.bzl",
    "oxx_static_library",
)
load("//third-party/boost:boost_asio_headers.bzl", "BOOST_ASIO_RAW_HEADERS_GENERATED")
load("//third-party/boost:boost_chrono_headers.bzl", "BOOST_CHRONO_RAW_HEADERS_GENERATED")
load("//third-party/boost:boost_coroutine_headers.bzl", "BOOST_COROUTINE_RAW_HEADERS_GENERATED")
load("//third-party/boost:boost_context_headers.bzl", "BOOST_CONTEXT_RAW_HEADERS_GENERATED")
load("//third-party/boost:boost_date_time_headers.bzl", "BOOST_DATE_TIME_RAW_HEADERS_GENERATED")
load("//third-party/boost:boost_filesystem_headers.bzl", "BOOST_FILESYSTEM_RAW_HEADERS_GENERATED")
load("//third-party/boost:boost_headers.bzl", "BOOST_RAW_HEADERS_GENERATED")
load("//third-party/boost:boost_iostreams_headers.bzl", "BOOST_IOSTREAMS_RAW_HEADERS_GENERATED")
load("//third-party/boost:boost_log_headers.bzl", "BOOST_LOG_RAW_HEADERS_GENERATED")
load("//third-party/boost:boost_program_options_headers.bzl", "BOOST_PROGRAM_OPTIONS_RAW_HEADERS_GENERATED")
load("//third-party/boost:boost_python_headers.bzl", "BOOST_PYTHON_RAW_HEADERS_GENERATED")
load("//third-party/boost:boost_regex_headers.bzl", "BOOST_REGEX_RAW_HEADERS_GENERATED")
load("//third-party/boost:boost_system_headers.bzl", "BOOST_SYSTEM_RAW_HEADERS_GENERATED")
load("//third-party/boost:boost_timer_headers.bzl", "BOOST_TIMER_RAW_HEADERS_GENERATED")
load("//third-party/boost:boost_serialization_headers.bzl", "BOOST_SERIALIZATION_RAW_HEADERS_GENERATED")
load("//third-party/boost:boost_thread_headers.bzl", "BOOST_THREAD_RAW_HEADERS_GENERATED")

BOOST_SOURCE = "boost"

def define_boost_library_raw_headers(generated, public_filespecs, source_dir = "", source_filespecs = [], exclude = []):
    use_glob = native.read_config("xplat", "boost_use_glob", "")
    if use_glob:
        return native.glob(
            ["{}/boost/{}".format(BOOST_SOURCE, filespec) for filespec in public_filespecs] +
            ["{}/{}".format(source_dir, filespec) for filespec in source_filespecs],
            exclude = exclude,
        )
    else:
        return generated

def define_targets():
    CXXFLAGS = ([
        "-fexceptions",
        "-std=c++14",
        "-Wall",
        "-Wno-narrowing",
        "-Wno-overloaded-virtual",
        "-Wno-shadow",
        "-Wno-unused-function",
        "-Wno-unused-variable",
        "-Wno-unused-local-typedefs",
    ] + ["-frtti"]) if (native.read_config("cxx", "default_platform", "") == "linux-unreal-clang") else []

    if (native.read_config("project", "pc", "") == "true"):
        BOOST_API_VERSION = ["-DBOOST_USE_WINAPI_VERSION=BOOST_WINAPI_VERSION_VISTA"]
    else:
        BOOST_API_VERSION = ["-DBOOST_USE_WINAPI_VERSION=_WIN32_WINNT"]

    WINCXXFLAGS = [
        "-DBOOST_THREAD_BUILD_LIB",
        "-DBOOST_USE_WINDOWS_H",
    ] + BOOST_API_VERSION

    PREPROCESSOR_WINDOWS_FLAGS = ([
        "-DBOOST_NO_RTTI",
    ] if (native.read_config("cxx", "default_platform", "") == "windows-unreal") else []) + [
        "-DWIN32_LEAN_AND_MEAN",
    ]

    PUBLIC_PREPROCESSOR_FLAGS = [
        "-DBOOST_COROUTINES_NO_DEPRECATION_WARNING",
        "-DBOOST_ALL_NO_LIB",
        # Otherwise boost includes <string_view> and we have not enabled
        "-DBOOST_ASIO_DISABLE_STRING_VIEW",
    ]

    # Load cached header lists by default to improve Buck project parsing speed.

    # boost_asio headers.

    BOOST_ASIO_RAW_HEADERS = define_boost_library_raw_headers(
        generated = BOOST_ASIO_RAW_HEADERS_GENERATED,
        public_filespecs = [
            "asio.h*",
            "asio/**/*.h*",
            "asio/**/*.ipp",
        ],
    )

    # boost_date_time headers.

    DATE_TIME_SOURCE_DIR = BOOST_SOURCE + "/libs/date_time/src/gregorian"

    BOOST_DATE_TIME_RAW_HEADERS = define_boost_library_raw_headers(
        generated = BOOST_DATE_TIME_RAW_HEADERS_GENERATED,
        public_filespecs = [
            "date_time.h*",
            "date_time/**/*.h*",
            "date_time/**/*.ipp",
        ],
        source_dir = DATE_TIME_SOURCE_DIR,
        source_filespecs = [
            "**/*.h*",
        ],
    )

    # boost_thread headers.

    THREAD_SOURCE_DIR = BOOST_SOURCE + "/libs/thread"

    BOOST_THREAD_RAW_HEADERS = define_boost_library_raw_headers(
        generated = BOOST_THREAD_RAW_HEADERS_GENERATED,
        public_filespecs = [
            "thread.h*",
            "thread/**/*.h*",
        ],
        source_dir = THREAD_SOURCE_DIR,
        source_filespecs = [
            "**/*.h*",
            "src/pthread/once_atomic.cpp",
        ],
    )

    # boost_thread headers.

    TIMER_SOURCE_DIR = BOOST_SOURCE + "/libs/timer"

    BOOST_TIMER_RAW_HEADERS = define_boost_library_raw_headers(
        generated = BOOST_TIMER_RAW_HEADERS_GENERATED,
        public_filespecs = [
            "timer.h*",
            "timer/**/*.h*",
        ],
        source_dir = TIMER_SOURCE_DIR,
        source_filespecs = [
            "**/*.h*",
        ],
    )

    # boost_system headers.

    SYSTEM_SOURCE_DIR = BOOST_SOURCE + "/libs/system"

    BOOST_SYSTEM_RAW_HEADERS = define_boost_library_raw_headers(
        generated = BOOST_SYSTEM_RAW_HEADERS_GENERATED,
        public_filespecs = [
            "system/**/*.h*",
        ],
        source_dir = SYSTEM_SOURCE_DIR,
        source_filespecs = [
            "**/*.h*",
        ],
    )

    # boost_filesystem headers.

    FILESYSTEM_SOURCE_DIR = BOOST_SOURCE + "/libs/filesystem"

    BOOST_FILESYSTEM_RAW_HEADERS = define_boost_library_raw_headers(
        generated = BOOST_FILESYSTEM_RAW_HEADERS_GENERATED,
        public_filespecs = [
            "filesystem.h*",
            "filesystem/**/*.h*",
        ],
        source_dir = FILESYSTEM_SOURCE_DIR,
        source_filespecs = [
            "**/*.h*",
        ],
    )

    # boost_regex

    REGEX_SOURCE_DIR = BOOST_SOURCE + "/libs/regex"

    BOOST_REGEX_RAW_HEADERS = define_boost_library_raw_headers(
        generated = BOOST_REGEX_RAW_HEADERS_GENERATED,
        public_filespecs = [
            "*regex*.h*",
            "regex/**/*.h*",
            "algorithm/**/*regex*.h*",
        ],
        source_dir = REGEX_SOURCE_DIR,
        source_filespecs = [
            "**/*.h*",
        ],
    )

    # boost_program_options

    PROGRAM_OPTIONS_SOURCE_DIR = BOOST_SOURCE + "/libs/program_options"

    BOOST_PROGRAM_OPTIONS_RAW_HEADERS = define_boost_library_raw_headers(
        generated = BOOST_PROGRAM_OPTIONS_RAW_HEADERS_GENERATED,
        public_filespecs = [
            "program_options.h*",
            "program_options/**/*.h*",
        ],
        source_dir = PROGRAM_OPTIONS_SOURCE_DIR,
        source_filespecs = [
            "**/*.h*",
        ],
    )

    # iostreams options

    IOSTREAMS_SOURCE_DIR = BOOST_SOURCE + "/libs/iostreams"

    BOOST_IOSTREAMS_RAW_HEADERS = define_boost_library_raw_headers(
        generated = BOOST_IOSTREAMS_RAW_HEADERS_GENERATED,
        public_filespecs = [
            "iostreams/**/*.h*",
        ],
        source_dir = IOSTREAMS_SOURCE_DIR,
        source_filespecs = [
            "**/*.h*",
        ],
    )

    # log options

    LOG_SOURCE_DIR = BOOST_SOURCE + "/libs/log"

    BOOST_LOG_RAW_HEADERS = define_boost_library_raw_headers(
        generated = BOOST_LOG_RAW_HEADERS_GENERATED,
        public_filespecs = [
            "log/**/*.h*",
        ],
        source_dir = LOG_SOURCE_DIR,
        source_filespecs = [
            "**/*.h*",
        ],
    )

    # coroutine options

    COROUTINE_SOURCE_DIR = BOOST_SOURCE + "/libs/coroutine"

    BOOST_COROUTINE_RAW_HEADERS = define_boost_library_raw_headers(
        generated = BOOST_COROUTINE_RAW_HEADERS_GENERATED,
        public_filespecs = [
            "coroutine/**/*.h*",
        ],
        source_dir = COROUTINE_SOURCE_DIR,
        source_filespecs = [
            "**/*.h*",
        ],
    )

    # context options

    CONTEXT_SOURCE_DIR = BOOST_SOURCE + "/libs/context"

    BOOST_CONTEXT_RAW_HEADERS = define_boost_library_raw_headers(
        generated = BOOST_CONTEXT_RAW_HEADERS_GENERATED,
        public_filespecs = [
            "context/**/*.h*",
        ],
        source_dir = CONTEXT_SOURCE_DIR,
        source_filespecs = [
            "**/*.h*",
        ],
    )

    # chrono options

    CHRONO_SOURCE_DIR = BOOST_SOURCE + "/libs/chrono"

    BOOST_CHRONO_RAW_HEADERS = define_boost_library_raw_headers(
        generated = BOOST_CHRONO_RAW_HEADERS_GENERATED,
        public_filespecs = [
            "chrono.h*",
            "chrono/**/*.h*",
        ],
        source_dir = CHRONO_SOURCE_DIR,
        source_filespecs = [
            "**/*.h*",
        ],
    )

    # serialization options

    SERIALIZATION_SOURCE_DIR = BOOST_SOURCE + "/libs/serialization"

    BOOST_SERIALIZATION_RAW_HEADERS = define_boost_library_raw_headers(
        generated = BOOST_SERIALIZATION_RAW_HEADERS_GENERATED,
        # Declare serialization without headers
        # because lots of other libraries depend on serialization headers,
        # but do not need serialization runtime.
        public_filespecs = [
        ],
        source_dir = SERIALIZATION_SOURCE_DIR,
        source_filespecs = [
            "**/*.h*",
            "**/*.ipp",
        ],
    )

    # python options

    PYTHON_SOURCE_DIR = BOOST_SOURCE + "/libs/python"

    BOOST_PYTHON_RAW_HEADERS = define_boost_library_raw_headers(
        generated = BOOST_PYTHON_RAW_HEADERS_GENERATED,
        public_filespecs = [
            "python.h*",
            "python/**/*.h*",
        ],
        source_dir = PYTHON_SOURCE_DIR,
        source_filespecs = [
            "**/*.h*",
        ],
    )

    # boost_headers headers.

    BOOST_HEADERS_SOURCE_DIR = BOOST_SOURCE

    BOOST_HEADERS_RAW_HEADERS = define_boost_library_raw_headers(
        exclude =
            BOOST_ASIO_RAW_HEADERS +
            BOOST_CHRONO_RAW_HEADERS +
            BOOST_CONTEXT_RAW_HEADERS +
            BOOST_COROUTINE_RAW_HEADERS +
            BOOST_DATE_TIME_RAW_HEADERS +
            BOOST_FILESYSTEM_RAW_HEADERS +
            BOOST_IOSTREAMS_RAW_HEADERS +
            BOOST_LOG_RAW_HEADERS +
            BOOST_PROGRAM_OPTIONS_RAW_HEADERS +
            BOOST_PYTHON_RAW_HEADERS +
            BOOST_REGEX_RAW_HEADERS +
            BOOST_SERIALIZATION_RAW_HEADERS +
            BOOST_SYSTEM_RAW_HEADERS +
            BOOST_TIMER_RAW_HEADERS +
            BOOST_THREAD_RAW_HEADERS + [],
        generated = BOOST_RAW_HEADERS_GENERATED,
        public_filespecs = [
            "**/*.h*",
            "**/*.ipp",
            "compatibility/cpp_c_headers/*",
        ],
        source_dir = BOOST_HEADERS_SOURCE_DIR,
        source_filespecs = [
            "libs/**/*.h*",
        ],
    )

    # Build target definitions.

    oxx_static_library(
        name = "boost",
        public_include_directories = [BOOST_HEADERS_SOURCE_DIR],
        public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS,
        public_raw_headers = BOOST_HEADERS_RAW_HEADERS,
        third_party = True,
        visibility = [
            "PUBLIC",
        ],
        public_linker_flags = (
            ["bcrypt.lib"] if native.host_info().os.is_windows else []
        ),
    )

    oxx_static_library(
        name = "boost_thread",
        srcs = [THREAD_SOURCE_DIR + file for file in [
            "/src/future.cpp",
            "/src/tss_null.cpp",
            "/src/win32/thread.cpp",
            "/src/win32/tss_dll.cpp",
            "/src/win32/tss_pe.cpp",
        ]],
        compiler_flags = WINCXXFLAGS,
        force_static = True,
        platform_compiler_flags = [
            (
                "linux-unreal-clang",
                ["-pthread"],
            ),
        ],
        preprocessor_flags = PREPROCESSOR_WINDOWS_FLAGS,
        public_deps = [
            "//third-party/boost:boost",
            "//third-party/boost:boost_chrono",
            "//third-party/boost:boost_date_time",
            "//third-party/boost:boost_system",
        ],
        public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS,
        raw_headers = BOOST_THREAD_RAW_HEADERS,
        third_party = True,
        visibility = ["PUBLIC"],
    )

    oxx_static_library(
        name = "boost_timer",
        srcs = native.glob([TIMER_SOURCE_DIR + filespec for filespec in [
            "/src/**/*.cpp",
        ]]),
        compiler_flags = WINCXXFLAGS,
        force_static = True,
        preprocessor_flags = PREPROCESSOR_WINDOWS_FLAGS,
        public_deps = [
            "//third-party/boost:boost",
            "//third-party/boost:boost_thread",
        ],
        public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS,
        raw_headers = BOOST_TIMER_RAW_HEADERS,
        third_party = True,
        visibility = ["PUBLIC"],
    )

    oxx_static_library(
        name = "boost_system",
        srcs = native.glob([SYSTEM_SOURCE_DIR + filespec for filespec in [
            "/src/**/*.cpp",
        ]]),
        compiler_flags = WINCXXFLAGS,
        force_static = True,
        preprocessor_flags = PREPROCESSOR_WINDOWS_FLAGS,
        public_deps = [
            "//third-party/boost:boost",
        ],
        public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS,
        raw_headers = BOOST_SYSTEM_RAW_HEADERS,
        third_party = True,
        visibility = ["PUBLIC"],
    )

    oxx_static_library(
        name = "boost_filesystem",
        srcs = native.glob([FILESYSTEM_SOURCE_DIR + filespec for filespec in [
            "/src/**/*.cpp",
        ]]),
        compiler_flags = WINCXXFLAGS,
        force_static = True,
        preprocessor_flags = PREPROCESSOR_WINDOWS_FLAGS,
        public_deps = [
            "//third-party/boost:boost",
            "//third-party/boost:boost_system",
        ],
        public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS,
        raw_headers = BOOST_FILESYSTEM_RAW_HEADERS,
        third_party = True,
        visibility = ["PUBLIC"],
    )

    REGEX_DIR = BOOST_SOURCE + "/libs/regex"

    oxx_static_library(
        name = "boost_regex",
        srcs = native.glob([REGEX_SOURCE_DIR + filespec for filespec in [
            "/src/**/*.cpp",
        ]]),
        compiler_flags = WINCXXFLAGS,
        force_static = True,
        preprocessor_flags = PREPROCESSOR_WINDOWS_FLAGS,
        public_deps = [
            "//third-party/boost:boost",
            "//third-party/boost:boost_system",
        ],
        public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS,
        raw_headers = BOOST_REGEX_RAW_HEADERS,
        third_party = True,
        visibility = ["PUBLIC"],
    )

    oxx_static_library(
        name = "boost_program_options",
        srcs = native.glob([PROGRAM_OPTIONS_SOURCE_DIR + filespec for filespec in [
            "/src/**/*.cpp",
        ]]),
        compiler_flags = WINCXXFLAGS,
        force_static = True,
        preprocessor_flags = PREPROCESSOR_WINDOWS_FLAGS,
        public_deps = [
            "//third-party/boost:boost",
        ],
        public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS,
        raw_headers = BOOST_PROGRAM_OPTIONS_RAW_HEADERS,
        third_party = True,
        visibility = ["PUBLIC"],
    )

    oxx_static_library(
        name = "boost_asio",
        compiler_flags = WINCXXFLAGS,
        force_static = True,
        preprocessor_flags = PREPROCESSOR_WINDOWS_FLAGS,
        public_deps = [
            "//third-party/boost:boost",
            "//third-party/boost:boost_date_time",
            "//third-party/boost:boost_regex",
            "//third-party/boost:boost_system",
        ],
        public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS,
        raw_headers = BOOST_ASIO_RAW_HEADERS,
        third_party = True,
        visibility = ["PUBLIC"],
    )

    oxx_static_library(
        name = "boost_date_time",
        srcs = native.glob([DATE_TIME_SOURCE_DIR + filespec for filespec in [
            "/*.cpp",
        ]]),
        compiler_flags = WINCXXFLAGS,
        force_static = True,
        preprocessor_flags = PREPROCESSOR_WINDOWS_FLAGS,
        public_deps = [
            "//third-party/boost:boost",
        ],
        public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS,
        raw_headers = BOOST_DATE_TIME_RAW_HEADERS,
        third_party = True,
        visibility = ["PUBLIC"],
    )

    # Not supported atm, commented out to enable buck determinator
    #
    # TEST_DIR = BOOST_SOURCE + "/libs/test"
    #
    # oxx_static_library(
    #     name = "boost_prg_exec_monitor",
    #     third_party = True,
    #     srcs = [TEST_DIR + file for file in [
    #         "/src/execution_monitor.cpp",
    #         "/src/debug.cpp",
    #         "/src/cpp_main.cpp",
    #     ]],
    #     force_static = True,
    #     compiler_flags = select({
    #         "ovr_config//compiler:msvc": WINCXXFLAGS,
    #         "DEFAULT": CXXFLAGS,
    #     }),
    #     public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS,
    #     visibility = ["PUBLIC"],
    #     public_deps = [
    #         "//third-party/boost:boost",
    #     ],
    # )

    # LeafLabs ovrsource is missing third-party/zlib, so this can't work
    # oxx_static_library(
    #     name = "boost_iostreams",
    #     srcs = [IOSTREAMS_SOURCE_DIR + file for file in [
    #         "/src/bzip2.cpp",
    #         "/src/file_descriptor.cpp",
    #         "/src/gzip.cpp",
    #         "/src/mapped_file.cpp",
    #         "/src/zlib.cpp",
    #     ]],
    #     compiler_flags = WINCXXFLAGS,
    #     force_static = True,
    #     preprocessor_flags = PREPROCESSOR_WINDOWS_FLAGS,
    #     public_deps = [
    #         "//third-party/boost:boost",
    #     ],
    #     public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS + [
    #         "/DBOOST_IOSTREAMS_NO_LIB",
    #     ],
    #     raw_headers = BOOST_IOSTREAMS_RAW_HEADERS,
    #     third_party = True,
    #     visibility = ["PUBLIC"],
    #     deps = [
    #         "//third-party/bzip2:bzlib2",
    #         "//third-party/zlib:zlib",
    #     ],
    # )

    oxx_static_library(
        name = "boost_log",
        srcs = native.glob([LOG_SOURCE_DIR + filespec for filespec in [
            "/src/**/*.cpp",
        ]], exclude = [
            LOG_SOURCE_DIR + "/src/posix/**/*",
            LOG_SOURCE_DIR + "/src/dump_avx2.cpp",
            LOG_SOURCE_DIR + "/src/dump_ssse3.cpp",
            LOG_SOURCE_DIR + "/src/windows/event_log_backend.cpp",
            LOG_SOURCE_DIR + "/src/syslog_backend.cpp",
        ]),
        compiler_flags = WINCXXFLAGS,
        force_static = True,
        include_directories = [
            LOG_SOURCE_DIR + "/src",
        ],
        preprocessor_flags = PREPROCESSOR_WINDOWS_FLAGS,
        public_deps = [
            "//third-party/boost:boost",
        ],
        public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS,
        raw_headers = BOOST_LOG_RAW_HEADERS,
        third_party = True,
        visibility = ["PUBLIC"],
        deps = [
            "//third-party/boost:boost",
            "//third-party/boost:boost_asio",
            "//third-party/boost:boost_date_time",
            "//third-party/boost:boost_filesystem",
            "//third-party/boost:boost_regex",
            "//third-party/boost:boost_thread",
        ],
    )

    oxx_static_library(
        name = "boost_coroutine",
        srcs = [COROUTINE_SOURCE_DIR + file for file in [
            "/src/exceptions.cpp",
            "/src/windows/stack_traits.cpp",
            "/src/detail/coroutine_context.cpp",
        ]],
        compiler_flags = WINCXXFLAGS,
        force_static = True,
        preprocessor_flags = PREPROCESSOR_WINDOWS_FLAGS,
        public_deps = [
            "//third-party/boost:boost",
            "//third-party/boost:boost_context",
            "//third-party/boost:boost_thread",
        ],
        public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS,
        raw_headers = BOOST_COROUTINE_RAW_HEADERS,
        # TODO: should work on Android too
        third_party = True,
        visibility = ["PUBLIC"],
    )

    oxx_static_library(
        name = "boost_context_masm",
        srcs = [CONTEXT_SOURCE_DIR + file for file in [
            "/src/asm/jump_x86_64_ms_pe_masm.asm",
            "/src/asm/make_x86_64_ms_pe_masm.asm",
            "/src/asm/ontop_x86_64_ms_pe_masm.asm",
        ]],
        compiler_flags = [
            "/W3",
            "/Zi",
            "/nologo",
            "-Cp",
            "-Cx",
            "/DBOOST_CONTEXT_EXPORT=",
            "/DBOOST_CONTEXT_SOURCE",
            "/Zp4",
        ],
        third_party = True,
        visibility = ["PUBLIC"],
    )

    oxx_static_library(
        name = "boost_context",
        srcs = [CONTEXT_SOURCE_DIR + file for file in [
            "/src/execution_context.cpp",
            "/src/windows/stack_traits.cpp",
            #"/src/asm/make_x86_64_sysv_macho_gas.S",
            #"/src/asm/make_x86_64_sysv_elf_gas.S",
            #"/src/asm/make_x86_64_ms_pe_gas.asm",
        ]],
        force_static = True,
        preprocessor_flags = PREPROCESSOR_WINDOWS_FLAGS,
        public_deps = [
            "//third-party/boost:boost",
        ],
        public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS,
        raw_headers = BOOST_CONTEXT_RAW_HEADERS,
        third_party = True,
        visibility = ["PUBLIC"],
        deps = [":boost_context_masm"],
    )

    oxx_static_library(
        name = "boost_chrono",
        srcs = [CHRONO_SOURCE_DIR + file for file in [
            "/src/chrono.cpp",
            "/src/process_cpu_clocks.cpp",
            "/src/thread_clock.cpp",
        ]],
        compiler_flags = WINCXXFLAGS,
        force_static = True,
        preprocessor_flags = PREPROCESSOR_WINDOWS_FLAGS,
        public_deps = [
            "//third-party/boost:boost",
            "//third-party/boost:boost_system",
        ],
        public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS,
        raw_headers = BOOST_CHRONO_RAW_HEADERS,
        third_party = True,
        visibility = ["PUBLIC"],
    )

    oxx_static_library(
        name = "boost_serialization",
        srcs = native.glob([SERIALIZATION_SOURCE_DIR + filespec for filespec in [
            "/src/**/*.cpp",
        ]]),
        compiler_flags = WINCXXFLAGS,
        force_static = True,
        preprocessor_flags = PREPROCESSOR_WINDOWS_FLAGS,
        public_deps = [
            "//third-party/boost:boost",
        ],
        public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS,
        raw_headers = BOOST_SERIALIZATION_RAW_HEADERS,
        third_party = True,
        visibility = ["PUBLIC"],
    )

    oxx_static_library(
        name = "boost_python",
        srcs = native.glob(
            [PYTHON_SOURCE_DIR + filespec for filespec in [
                "/src/**/*.cpp",
            ]],
            exclude = [PYTHON_SOURCE_DIR + filespec for filespec in [
                "/src/numpy/*.cpp",
            ]],
        ),
        compiler_flags = WINCXXFLAGS,
        force_static = True,
        preprocessor_flags = PREPROCESSOR_WINDOWS_FLAGS + [
            "-DBOOST_PYTHON_STATIC_LIB=1",
        ],
        public_deps = [
            "//third-party/boost:boost",
            "//third-party/boost:boost_system",
            "//third-party/python/python_3.6:python_3.6",
        ],
        public_preprocessor_flags = PUBLIC_PREPROCESSOR_FLAGS,
        raw_headers = BOOST_PYTHON_RAW_HEADERS,
        third_party = True,
        visibility = ["PUBLIC"],
    )
