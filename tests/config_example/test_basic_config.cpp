#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <gtest/gtest.h>
#include "tests/config_example/basic_generated.h"
#include <cstddef>
#include <cstdlib>

using namespace config::example;

class ConfigTest : public ::testing::TestWithParam<const char*> {
protected:
    const char *test_file = nullptr;
    void *mapped_data = nullptr;
    size_t file_size = 0;

    void SetUp() override {
        // If CONFIG_FILE is set, always use it for the first file
        if (strcmp(GetParam(), "tests/config_example/etc/basic_test_config.bin") == 0) {
            const char* env_file = getenv("CONFIG_FILE");
            if (env_file) {
                test_file = env_file;
            } else {
                test_file = GetParam();
            }
        } else {
            test_file = GetParam();
        }

        int fd = open(test_file, O_RDONLY);
        ASSERT_NE(fd, -1) << "Failed to open test file: " << test_file;

        struct stat sb;
        ASSERT_EQ(fstat(fd, &sb), 0) << "Failed to get file size.";
        file_size = sb.st_size;

        mapped_data = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
        close(fd);
        ASSERT_NE(mapped_data, MAP_FAILED) << "Failed to mmap file.";
    }

    void TearDown() override {
        if (mapped_data) {
            munmap(mapped_data, file_size);
        }
    }
};

INSTANTIATE_TEST_SUITE_P(
    ConfigFiles,
    ConfigTest,
    ::testing::Values(
        "tests/config_example/etc/basic_test_config.bin",
        "tests/config_example/etc/basic_test_config_evolution.bin"
    )
);

TEST_P(ConfigTest, ValidateAppConfig) {
    auto app_config = GetAppConfig(mapped_data);
    ASSERT_NE(app_config, nullptr);
    EXPECT_EQ(app_config->schema_version()->major(), 1);
    EXPECT_GE(app_config->schema_version()->minor(), 0);
    EXPECT_EQ(app_config->schema_version()->patch(), 0);
    EXPECT_EQ(app_config->app_name()->str(), "TestApp");
    EXPECT_EQ(app_config->app_id(), 0); // Implicit default is taken
    EXPECT_FALSE(app_config->debug_enabled()); // Default value is taken
    EXPECT_EQ(app_config->max_connections(), 100); // Default value is taken
    EXPECT_EQ(app_config->timeout_ms(), 5000); // Default value is taken
}

TEST_P(ConfigTest, ValidateAdvancedSettings) {
    auto app_config = GetAppConfig(mapped_data);
    ASSERT_NE(app_config, nullptr);

    auto advanced_settings = app_config->advanced_settings();
    ASSERT_NE(advanced_settings, nullptr);
    EXPECT_EQ(advanced_settings->log_level()->str(), "INFO");
    EXPECT_EQ(advanced_settings->buffer_size_kb(), 2048);
    EXPECT_TRUE(advanced_settings->enable_metrics());
    ASSERT_EQ(advanced_settings->allowed_hosts()->size(), 2);
    EXPECT_EQ(advanced_settings->allowed_hosts()->Get(0)->str(), "host1");
    EXPECT_EQ(advanced_settings->allowed_hosts()->Get(1)->str(), "host2");
}

TEST(ConfigTest, AccessGarbageBuffer) {
    const size_t buffer_size = 1024;
    uint8_t garbage_buffer[buffer_size];

    // Fill with deterministic garbage data (pattern based on index)
    for (size_t i = 0; i < buffer_size; i++) {
        garbage_buffer[i] = static_cast<uint8_t>((i * 0x5A + 0xAA) & 0xFF);
    }

    // Attempt to access as a FlatBuffer without verification
    auto app_config = GetAppConfig(garbage_buffer);
    EXPECT_NE(app_config, nullptr);
    // Warning! Attempting to access fields will cause undefined behavior

    flatbuffers::Verifier verifier(garbage_buffer, buffer_size);
    EXPECT_FALSE(VerifyAppConfigBuffer(verifier));
}
