use vergen_gitcl::{Emitter, GitclBuilder};

fn main() {
    let git_instructions = GitclBuilder::default()
        .all()
        .sha(true)
        .build()
        .expect("Failed to build vergen git instructions.");
    Emitter::default()
        .add_instructions(&git_instructions)
        .expect("Failed to add vergen git instructions.")
        .emit()
        .expect("Failed to emit version information");
}
