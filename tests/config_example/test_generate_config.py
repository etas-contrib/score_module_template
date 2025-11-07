#!/usr/bin/env python3
"""
Python test for FlatBuffer configuration generation.

This test demonstrates the Python side generating the buffer using the
native table API (ObjectAPI). The generated buffer will be read by the C++ test
binary.
"""

import os
import subprocess

import flatbuffers
from config.example.AppConfig import AppConfigT
from config.example.AdvancedSettings import AdvancedSettingsT
from config.example.Version import VersionT

def test_flatbuffer_generation():
    """Test creating and serializing a FlatBuffer configuration using native tables."""
    # Create native Python objects (no builder needed yet)

    # Create configuration using native Python objects
    native_config = AppConfigT()

    # Create version (struct fields are set individually)
    version = VersionT()
    version.major = 1
    version.minor = 0
    version.patch = 0
    native_config.schemaVersion = version

    native_config.appName = "TestApp"
    native_config.appId = 0
    native_config.debugEnabled = False
    native_config.maxConnections = 100
    native_config.timeoutMs = 5000

    # Create advanced settings
    native_config.advancedSettings = AdvancedSettingsT()
    native_config.advancedSettings.logLevel = "INFO"
    native_config.advancedSettings.bufferSizeKb = 2048
    native_config.advancedSettings.enableMetrics = True
    native_config.advancedSettings.allowedHosts = ["host1", "host2"]

    # Pack native object into FlatBuffer
    builder = flatbuffers.Builder(1024)
    config = native_config.Pack(builder)
    builder.Finish(config)

    # Write the binary data to a file
    python_generated_file = "python_generated_config.bin"
    try:
        with open(python_generated_file, "wb") as f:
            f.write(builder.Output())

        # Basic validation - ensure file exists and has reasonable size
        assert os.path.exists(python_generated_file), "Generated file does not exist"
        file_size = os.path.getsize(python_generated_file)
        assert file_size > 100, f"Generated config seems too small: {file_size} bytes"

        # Use hardcoded path for the C++ test binary (bundled in runfiles)
        cpp_test_binary = "tests/config_example/test_basic_config_cpp"

        # Set environment variable and run the C++ test (will use the generated file path)
        env = os.environ.copy()
        env["CONFIG_FILE"] = python_generated_file
        result = subprocess.run(
            [cpp_test_binary],
            capture_output=True,
            text=True,
            timeout=30,
            env=env,
        )
        if result.returncode != 0:
            # Log googletest output for debugging
            if result.stdout:
                print("--- C++ test stdout ---")
                print(result.stdout)
            if result.stderr:
                print("--- C++ test stderr ---")
                print(result.stderr)
            assert False, f"C++ test failed with return code {result.returncode}"

    finally:
        # Clean up generated file
        if os.path.exists(python_generated_file):
            os.remove(python_generated_file)

    return True

if __name__ == "__main__":
    test_flatbuffer_generation()
