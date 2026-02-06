use vergen_gitcl::{Emitter, GitclBuilder};

#[cfg(windows)]
use winres;

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
fn set_executable_icon() {
    let mut res = winres::WindowsResource::new();
    res.set_icon("Olympus.ico");
    res.compile().unwrap();
}

#[cfg(not(windows))]
fn set_executable_icon() {}

fn main() {
    capture_git_state();
    set_executable_icon();
}
