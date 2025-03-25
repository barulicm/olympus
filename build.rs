use std::path::{Path, PathBuf};
use std::{io, fs};

fn copy_directory_recursively(src: &Path, dest: &Path) -> io::Result<()> {
  if src.is_dir() {
    if !dest.exists() {
      fs::create_dir_all(&dest)?;
    }
    for entry in fs::read_dir(src)? {
      let entry = entry?;
      let path = entry.path();
      let dest_path = dest.join(path.file_name().unwrap());
      copy_directory_recursively(&path, &dest_path)?;
    }
  } else {
    fs::copy(src, dest)?;
  }
  Ok(())
}

fn copy_directory_to_output(dir: &str, dest_path: &str) -> io::Result<()> {
  print!("cargo::rerun-if-changed={}", dir);
  let profile = std::env::var("PROFILE").unwrap();
  let out_dir = PathBuf::from(format!("target/{}/{}", profile, dest_path));
  copy_directory_recursively(Path::new(dir), &out_dir)
}

fn main() {
  copy_directory_to_output("src/resources", "resources").unwrap();
}
