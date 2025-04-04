use crate::version::get_version;
use directories::ProjectDirs;
use include_dir::{Dir, DirEntry, include_dir};
use std::{env, path::PathBuf};

const DEFAULT_RESOURCES: Dir = include_dir!("./src/resources");

pub fn get_resource_directory_path() -> PathBuf {
    let project_dirs = ProjectDirs::from("", "barulicm", "olympus").unwrap_or(
        ProjectDirs::from_path(
            std::env::current_dir()
                .expect("Could not get current directory.")
                .join("barulicm/olympus"),
        )
        .expect("Falling back to current directory failed."),
    );

    project_dirs.data_dir().join("resources")
}

pub fn initialize_resources_directory(
    resources_path: &PathBuf,
    force_init: bool,
) -> std::io::Result<()> {
    if !resources_path.exists() {
        std::fs::create_dir_all(resources_path)?;
    }

    let version_tag = check_resources_version_tag(resources_path).unwrap_or_default();
    let resources_are_dirty = version_tag.contains("dirty");
    let code_is_dirty = env!("VERGEN_GIT_DIRTY") == "true";

    if !(force_init || code_is_dirty || resources_are_dirty || version_tag != get_version()) {
        return Ok(());
    }

    println!("Updating resources...");

    write_default_resources(resources_path)?;

    write_version_tag(resources_path)?;

    println!("Resources ready.");

    Ok(())
}

fn check_resources_version_tag(resources_path: &PathBuf) -> Option<String> {
    let version_file_path = resources_path.join("version.txt");
    if !version_file_path.exists() {
        return None;
    }
    match std::fs::read_to_string(version_file_path) {
        Ok(content) => Some(content.trim().to_string()),
        Err(_) => None,
    }
}

fn write_version_tag(resources_path: &PathBuf) -> std::io::Result<()> {
    let version_file_path = resources_path.join("version.txt");
    let dirty = if env!("VERGEN_GIT_DIRTY") == "true" {
        " (dirty)"
    } else {
        ""
    };
    let version_text = format!("{}{}", get_version(), dirty);
    std::fs::write(version_file_path, version_text)?;
    Ok(())
}

fn write_default_resources(resources_path: &PathBuf) -> std::io::Result<()> {
    for entry in DEFAULT_RESOURCES.entries() {
        write_resource(resources_path, entry)?;
    }
    Ok(())
}

fn write_resource(resources_path: &PathBuf, entry: &DirEntry) -> std::io::Result<()> {
    let target_path = resources_path.join(entry.path());
    if let Some(parent) = target_path.parent() {
        std::fs::create_dir_all(parent)?;
    }

    if let Some(file_entry) = entry.as_file() {
        std::fs::write(target_path.as_path(), file_entry.contents())?;
    }
    if let Some(dir_entry) = entry.as_dir() {
        std::fs::create_dir_all(target_path.as_path())?;
        for child in dir_entry.entries() {
            write_resource(resources_path, child)?;
        }
    }
    Ok(())
}
