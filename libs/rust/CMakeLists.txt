cargo_build(
    NAME rustlib
    TARGET thumbv6m-none-eabi
)

target_include_directories(rustlib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(fake_rust_src
    SOURCES
        rustlib.rs
        rust_header.h
)
