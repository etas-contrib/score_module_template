
# FlatBuffers Configuration Example

This directory demonstrates a module configuration using FlatBuffers. Examples cover schema design, code generation, binary serialization, deserialization, and schema evolution validation.

**Key Bazel Targets Overview:**

| Target | Purpose |
|--------|---------|
| `:generate_api_basic_{cpp,python,rust}` | Generate language bindings |
| `:generate_basic_test_config{_evolution}` | Create config binary from JSON |
| `:decode_basic_test_config` | Decode binary config back to JSON |
| `:test_basic_config_{cpp,rust}` | Read and validate config |
| `:test_basic_config_python_generated` | Build and validate config |
| `:verify_correct_schema_evolution` | Validate schema compatibility |


## 1. Basic Configuration

The `basic.fbs` schema defines a minimal exemplary module  configuration with versioning, nested tables, and various data types.

### Schema Definition

The schema (`basic.fbs`) demonstrates:
- **Semantic versioning** using a `Version` struct for schema evolution tracking
- **Scalar types** with implicit defaults (`app_id: uint32`) and explicit defaults (`timeout_ms: uint32 = 5000`)
- **Required fields** (`app_name: string (required)`) to enforce mandatory configuration
- **Nested tables** (`AdvancedSettings`) for logical grouping
- **Vectors** (`allowed_hosts: [string]`) for collections
- **Root type declaration** (`root_type AppConfig`) defining the entry point

### Code Generation

Generate language bindings using the FlatBuffers compiler (`flatc`):

```bash
# C++ bindings
bazel build //tests/config_example:generate_api_basic_cpp

# Python bindings
bazel build //tests/config_example:generate_api_basic_python

# Rust bindings
bazel build //tests/config_example:generate_api_basic_rust
```

### Binary Config Generation

Configuration binaries are created from JSON files using `flatc`:

```bash
# Generate binary from JSON (via genrule)
bazel build //tests/config_example:generate_basic_test_config
```

The genrule invokes:
```bash
flatc --binary -o <directory> basic.fbs basic_test_config.json
```

**Options:**
- `--binary`: Generate binary file containing a serialized flatbuffer

### Configuration Access Tests

Tests demonstrating read of binary configuration:

```bash
# C++ test
bazel test //tests/config_example:test_basic_config_cpp

# Rust test
bazel test //tests/config_example:test_basic_config_rust

# Python test (builder API demonstration)
bazel test //tests/config_example:test_basic_config_python_generated
```

**C++ Test** (`test_basic_config.cpp`) and **Rust Test** (`test_basic_config.rs`):
- Use `mmap` for zero-copy file access
- Demonstrate usage of generated accessor APIs
- Validates values of schema version, scalars, nested tables, and vectors

**Python Test** (`test_generate_config.py`):
- Demonstrates **native tables API** (`--gen-object-api`) for creating buffers
- Validates generated buffer by invoking C++ test binary

## 2. Schema Evolution

The `basic_evolution.fbs` schema demonstrates forward/backward compatible changes.

### Evolution Techniques

**Adding New Fields:**
```idl
// New fields MUST be added at the end of the table
new_feature_enabled:bool = false;
new_feature:NewFeature;  // Optional nested table
```

**Deprecating Fields:**
```idl
max_connections:uint16 = 100 (deprecated);
```

**Field Identifiers (Advanced):**
Use explicit `id` attributes to allow reordering (not shown in example):
```idl
app_name:string (id: 0, required);
timeout_ms:uint32 (id: 5) = 5000;
```
See [FlatBuffers Attributes](https://flatbuffers.dev/schema/#attributes).

### Validation

Ensure schema evolution maintains compatibility:

```bash
bazel test //tests/config_example:verify_correct_schema_evolution
```

This test uses `flatc --conform` to verify that `basic_evolution.fbs` is a valid evolution of `basic.fbs`. The `--conform` option checks:
- Field order compatibility (new fields at end)
- Type compatibility (no type changes for existing fields)
- Preserved field identifiers
- Removed fields are marked deprecated

## 3. Showcase Schema Features

The `showcase.fbs` demonstrates additional FlatBuffers capabilities:

```bash
# Generate showcase bindings
bazel build //tests/config_example:generate_api_showcase_cpp
bazel build //tests/config_example:generate_api_showcase_python
bazel build //tests/config_example:generate_api_showcase_rust
```

**Demonstrated Features:**
- Scalar types with implicit and explicit defaults
- Enums with custom values
- Vectors of scalars, strings, and objects
- Structs with fixed size and alignment (`force_align`)
- Recursive tables (tree structures)
- Unions (discriminated types)
- Key attributes for sorted vector lookups
- Schema inclusion (`include "basic.fbs"`)

## 4. Out of scope

The following features are not demonstrated as they are beyond the scope of
FlatBuffer usage for configuration files:

- **Verifier API**: Runtime schema validation (see [Verifier documentation](https://flatbuffers.dev/languages/cpp/#access-of-untrusted-buffers))
- **Custom allocators**: Control memory allocation strategies for Buffer generation
- **RPC services**: Define service interfaces for client-server communication
- **Reflection**: Runtime schema introspection
