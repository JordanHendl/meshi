use std::fmt;
#[derive(Debug)]
pub struct SlotError {}

#[derive(Debug)]
pub struct LookupError {
    pub entry: String,
}

#[derive(Debug)]
pub struct LoadingError {
    pub entry: String,
    pub path: String,
}

impl fmt::Display for SlotError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "Ran out of slots!")
    }
}

impl fmt::Display for LookupError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "Could not find requested entry {} in database!",
            self.entry
        )
    }
}

impl fmt::Display for LoadingError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "Failed to load requested entry {} in database! Attempted path: {}",
            self.entry, self.path
        )
    }
}
#[derive(Debug)]
pub enum Error {
    LookupError(LookupError),
    LoadingError(LoadingError),
    SlotError(),
}

impl From<String> for Error {
    fn from(value: String) -> Self {
        return Error::LoadingError(LoadingError {
            entry: "[UNKNOWN]".to_string(),
            path: value,
        });
    }
}

impl From<image::ImageError> for Error {
    fn from(value: image::ImageError) -> Self {
        return Error::LoadingError(LoadingError {
            entry: "[UNKNOWN]".to_string(),
            path: value.to_string(),
        });
    }
}

impl From<std::io::Error> for Error {
    fn from(value: std::io::Error) -> Self {
        return Error::LoadingError(LoadingError {
            entry: "IO Loading Error".to_string(),
            path: value.to_string(),
        });
    }
}

impl From<serde_json::Error> for Error {
    fn from(value: serde_json::Error) -> Self {
        return Error::LoadingError(LoadingError {
            entry: "JSON FILE".to_string(),
            path: value.to_string(),
        });
    }
}
//impl From<ash::vk::Result> for GPUError {
//    fn from(res: ash::vk::Result) -> Self {
//        return GPUError::VulkanError(VulkanError { res });
//    }
//}
//
//impl From<ash::LoadingError> for GPUError {
//    fn from(res: ash::LoadingError) -> Self {
//        return GPUError::LoadingError(res);
//    }
//}
