pub fn get_version() -> &'static str {
    concat!(
        env!("CARGO_PKG_VERSION_MAJOR"),
        ".",
        env!("CARGO_PKG_VERSION_MINOR"),
        "-",
        env!("VERGEN_GIT_SHA")
    )
}
