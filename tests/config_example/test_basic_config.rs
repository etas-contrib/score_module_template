use std::fs::File;
use memmap2::Mmap;
use flatbuffers::root;
use lib_basic_rust::config::example::AppConfig;

// Replace with baselibs binary config loader abstraction
fn load_config_file() -> Mmap {
    let file = File::open("tests/config_example/etc/basic_test_config.bin")
        .expect("Failed to open test file");
    unsafe {
        Mmap::map(&file).expect("Failed to mmap file")
    }
}

#[test]
fn validate_app_config() {
    let buffer = load_config_file();
    let app_config = root::<AppConfig>(&buffer).expect("Failed to parse AppConfig");

    assert_eq!(app_config.schema_version().major(), 1);
    assert_eq!(app_config.schema_version().minor(), 0);
    assert_eq!(app_config.schema_version().patch(), 0);
    assert_eq!(app_config.app_name(), "TestApp");
    assert_eq!(app_config.app_id(), 0); // Implicit default is taken
    assert!(!app_config.debug_enabled()); // Default value is taken
    assert_eq!(app_config.max_connections(), 100); // Default value is taken
    assert_eq!(app_config.timeout_ms(), 5000); // Default value is taken
}

#[test]
fn validate_advanced_settings() {
    let buffer = load_config_file();
    let app_config = root::<AppConfig>(&buffer).expect("Failed to parse AppConfig");
    let advanced_settings = app_config.advanced_settings();

    assert_eq!(advanced_settings.log_level(), Some("INFO"));
    assert_eq!(advanced_settings.buffer_size_kb(), 2048);
    assert!(advanced_settings.enable_metrics());
    let allowed_hosts = advanced_settings.allowed_hosts().expect("Failed to get allowed hosts");
    assert_eq!(allowed_hosts.len(), 2);
    assert_eq!(allowed_hosts.get(0), "host1");
    assert_eq!(allowed_hosts.get(1), "host2");
}

#[test]
fn access_garbage_buffer() {
    const BUFFER_SIZE: usize = 1024;
    let mut garbage_buffer = vec![0u8; BUFFER_SIZE];

    // Fill with deterministic garbage data (pattern based on index)
    for i in 0..BUFFER_SIZE {
        garbage_buffer[i] = ((i * 0x5A + 0xAA) & 0xFF) as u8;
    }

    unsafe {
        let _unchecked_root = flatbuffers::root_unchecked::<AppConfig>(&garbage_buffer);
        // Warning! Attempting to access fields will cause undefined behavior
    }

    let verified_root = root::<AppConfig>(&garbage_buffer);
    assert_eq!(verified_root.is_err(), true);
}
