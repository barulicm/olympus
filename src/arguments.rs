use clap::Parser;

#[derive(Parser, Debug)]
#[command(version, about, long_about = None)]
pub struct Arguments {
    /// Force initialization of default resources, even if they already exist
    #[arg(long)]
    pub init_resources: bool,

    /// Sets the directory where resource files will be saved
    #[arg(long)]
    pub resources_dir: Option<String>,
}
