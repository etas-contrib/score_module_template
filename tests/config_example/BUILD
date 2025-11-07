# Copyright (c) 2025 Contributors to the Eclipse Foundation
#
# SPDX-License-Identifier: Apache-2.0
# *******************************************************************************

load("@rules_shell//shell:sh_test.bzl", "sh_test")

# FlatBuffer schema files for configuration
filegroup(
    name = "schema_basic",
    srcs = ["basic.fbs"],
)

filegroup(
    name = "schema_basic_evolution",
    srcs = ["basic_evolution.fbs"],
)

filegroup(
    name = "schema_showcase",
    srcs = ["showcase.fbs"],
)

# FlatBuffer binary generation from JSON
genrule(
    name = "generate_basic_test_config",
    srcs = ["etc/basic_test_config.json", ":schema_basic"],
    outs = ["etc/basic_test_config.bin"],
    tools = ["@flatbuffers//:flatc"],
    cmd = "$(location @flatbuffers//:flatc) --binary -o $(@D) $(location :schema_basic) $(SRCS)",
)

genrule(
    name = "generate_basic_test_config_evolution",
    srcs = ["etc/basic_test_config_evolution.json", ":schema_basic_evolution"],
    outs = ["etc/basic_test_config_evolution.bin"],
    tools = ["@flatbuffers//:flatc"],
    cmd = "$(location @flatbuffers//:flatc) --binary -o $(@D) $(location :schema_basic_evolution) $(SRCS)",
)

# Decode the generated binary back to JSON for inspection/testing.
genrule(
    name = "decode_basic_test_config",
    srcs = [":generate_basic_test_config", ":schema_basic"],
    outs = ["etc/decoded/basic_test_config.json"],
    tools = ["@flatbuffers//:flatc"],
    cmd = "$(location @flatbuffers//:flatc) --json --strict-json --defaults-json -o $(@D) $(location :schema_basic) -- $(location :generate_basic_test_config)",
)

# Ensure correct schema evolution from basic to basic_evolution
sh_test(
    name = "verify_correct_schema_evolution",
    srcs = ["verify_correct_schema_evolution.sh"],
    data = [
        "basic.fbs",
        "basic_evolution.fbs",
        "@flatbuffers//:flatc",
    ],
    size = "small",
)

# ===============================================================================
# C++ Section
# ===============================================================================
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

# Generate C++ bindings from FlatBuffer schema
genrule(
    name = "generate_api_basic_cpp",
    srcs = [":schema_basic"],
    outs = ["basic_generated.h"],
    tools = ["@flatbuffers//:flatc"],
    cmd = "$(location @flatbuffers//:flatc) --cpp -o $(@D) $(SRCS)",
)

genrule(
    name = "generate_api_basic_evolution_cpp",
    srcs = [":schema_basic_evolution"],
    outs = ["basic_evolution_generated.h"],
    tools = ["@flatbuffers//:flatc"],
    cmd = "$(location @flatbuffers//:flatc) --cpp -o $(@D) $(SRCS)",
)

genrule(
    name = "generate_api_showcase_cpp",
    srcs = [":schema_showcase", ":schema_basic"],
    # Also generates basic_generated.h again (see above)
    outs = ["showcase_generated.h"],
    tools = ["@flatbuffers//:flatc"],
    cmd = "$(location @flatbuffers//:flatc) --cpp -o $(@D) $(location :schema_showcase)",
)

# C++ library for generated code
cc_library(
    name = "lib_basic_cpp",
    hdrs = [":generate_api_basic_cpp"],
    deps = ["@flatbuffers//:flatbuffers"],
)

# C++ test demonstrating read of config
cc_test(
    name = "test_basic_config_cpp",
    srcs = ["test_basic_config.cpp"],
    deps = [
        ":lib_basic_cpp",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
    data = [":generate_basic_test_config", ":generate_basic_test_config_evolution"],
    size = "small",
)

# ===============================================================================
# Rust Section
# ===============================================================================
load("@rules_rust//rust:defs.bzl", "rust_test", "rust_library")

# Generate Rust bindings from FlatBuffer schema
genrule(
    name = "generate_api_basic_rust",
    srcs = [":schema_basic"],
    outs = ["basic_generated.rs"],
    tools = ["@flatbuffers//:flatc"],
    cmd = "$(location @flatbuffers//:flatc) --rust -o $(@D) $(SRCS)",
)

genrule(
    name = "generate_api_basic_evolution_rust",
    srcs = [":schema_basic_evolution"],
    outs = ["basic_evolution_generated.rs"],
    tools = ["@flatbuffers//:flatc"],
    cmd = "$(location @flatbuffers//:flatc) --rust -o $(@D) $(SRCS)",
)

genrule(
    name = "generate_api_showcase_rust",
    srcs = [":schema_showcase", ":schema_basic"],
    # Also generates basic_generated.rs again (see above)
    outs = ["showcase_generated.rs"],
    tools = ["@flatbuffers//:flatc"],
    cmd = "$(location @flatbuffers//:flatc) --rust -o $(@D) $(location :schema_showcase)",
)

# Rust library for generated code
rust_library(
    name = "lib_basic_rust",
    srcs = [":generate_api_basic_rust"],
    edition = "2021",
    deps = [
        "@crates//:flatbuffers",
    ],
)

# Rust test demonstrating read of config
rust_test(
    name = "test_basic_config_rust",
    srcs = ["test_basic_config.rs"],
    data = [":generate_basic_test_config"],
    deps = [
        "@crates//:flatbuffers",
        "@crates//:memmap2",
        ":lib_basic_rust",
    ],
    size = "small",
)

# ===============================================================================
# Python Section
# ===============================================================================
load("@rules_python//python:py_test.bzl", "py_test")
load("@rules_python//python:py_library.bzl", "py_library")

# Generate Python bindings from FlatBuffer schema
genrule(
    name = "generate_api_basic_python",
    srcs = [":schema_basic"],
    outs = [
        "config/__init__.py",
        "config/example/__init__.py",
        "config/example/AppConfig.py",
        "config/example/AdvancedSettings.py",
        "config/example/Version.py",
    ],
    tools = ["@flatbuffers//:flatc"],
    cmd = "$(location @flatbuffers//:flatc) --python --gen-object-api -o $(@D) $(SRCS)",
)

genrule(
    name = "generate_api_showcase_python",
    srcs = [":schema_showcase", ":schema_basic"],
    outs = [
        # Also generates basic elements again (see above)
        "showcase/__init__.py",
        "showcase/ShowcaseRoot.py",
        "showcase/enum_types/__init__.py",
        "showcase/enum_types/EnumExample.py",
        "showcase/enum_types/EnumType.py",
        "showcase/include_demo/__init__.py",
        "showcase/include_demo/ImportedSchema.py",
        "showcase/key/__init__.py",
        "showcase/key/KeyExample.py",
        "showcase/key/KeySortedVector.py",
        "showcase/scalar_types/__init__.py",
        "showcase/scalar_types/ScalarTypes.py",
        "showcase/string_types/__init__.py",
        "showcase/string_types/StringTypes.py",
        "showcase/struct_types/__init__.py",
        "showcase/struct_types/StructSimple.py",
        "showcase/tree/__init__.py",
        "showcase/tree/TreeElement.py",
        "showcase/union_types/__init__.py",
        "showcase/union_types/ExampleUnion.py",
        "showcase/union_types/UnionExample.py",
        "showcase/vector_types/__init__.py",
        "showcase/vector_types/VectorTypes.py",
    ],
    tools = ["@flatbuffers//:flatc"],
    cmd = """
        $(location @flatbuffers//:flatc) --python --gen-object-api -o $(@D) $(location :schema_showcase)
        # tree $(@D)
    """,
)

# Python library for generated code
py_library(
    name = "lib_basic_python",
    srcs = [":generate_api_basic_python"],
    imports = ["."],
)

# Python test demonstrating creation of config binary
py_test(
    name = "test_basic_config_python_generated",
    srcs = ["test_generate_config.py"],
    deps = [
        ":lib_basic_python",
        "@module_pip//flatbuffers",
    ],
    main = "test_generate_config.py",
    data = [
        ":generate_basic_test_config",
        ":test_basic_config_cpp",
    ],
    size = "small",
)
