use anyhow::Result;
use vergen_gitcl::{Emitter, GitclBuilder};

fn capture_git_state() {
    let git_instructions = GitclBuilder::default()
        .all()
        .dirty(true)
        .sha(true)
        .build()
        .expect("Failed to build vergen git instructions.");
    Emitter::default()
        .add_instructions(&git_instructions)
        .expect("Failed to add vergen git instructions.")
        .emit()
        .expect("Failed to emit version information");
}

#[cfg(windows)]
fn set_executable_icon() -> std::io::Result<()> {
    let mut res = winres::WindowsResource::new();
    res.set_icon("Olympus.ico");
    res.compile()?;
    Ok(())
}

#[cfg(not(windows))]
fn set_executable_icon() -> std::io::Result<()> {
    Ok(())
}

fn main() -> Result<()> {
    capture_git_state();
    set_executable_icon()?;
    Ok(())
}
