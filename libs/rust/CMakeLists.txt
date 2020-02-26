cargo_build(
    NAME rustlib
    TARGET thumbv6m-none-eabi
)

cargo_bindings(rustlib-interface
    FILENAME ${CMAKE_CURRENT_BINARY_DIR}/rustlib.h
    CRATE rustlib
    NAMESPACE rustlib
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(fake_rust_src
    SOURCES
        rustlib.rs
        rust_header.h
        Cargo.toml
        cbingen.toml
)
