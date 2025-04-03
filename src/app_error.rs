use std::sync::PoisonError;

use axum::http::StatusCode;
use axum::response::{IntoResponse, Response};

pub struct AppError {
    pub status: StatusCode,
    pub message: String,
}

impl AppError {
    pub fn new(status: StatusCode, message: String) -> AppError {
        AppError { status, message }
    }
}

impl IntoResponse for AppError {
    fn into_response(self) -> Response {
        (self.status, self.message).into_response()
    }
}

impl<T> From<PoisonError<T>> for AppError {
    fn from(_: PoisonError<T>) -> Self {
        AppError {
            status: StatusCode::INTERNAL_SERVER_ERROR,
            message: String::from("A mutex locking error occurred in the server."),
        }
    }
}

impl From<(StatusCode, &str)> for AppError {
    fn from((status, message): (StatusCode, &str)) -> Self {
        AppError {
            status,
            message: String::from(message),
        }
    }
}

impl From<(StatusCode, String)> for AppError {
    fn from((status, message): (StatusCode, String)) -> Self {
        AppError {
            status,
            message: message,
        }
    }
}

impl From<std::io::Error> for AppError {
    fn from(value: std::io::Error) -> Self {
        AppError {
            status: StatusCode::INTERNAL_SERVER_ERROR,
            message: value.to_string(),
        }
    }
}

impl From<serde_json::Error> for AppError {
    fn from(value: serde_json::Error) -> Self {
        AppError {
            status: StatusCode::INTERNAL_SERVER_ERROR,
            message: format!("An unexpected JSON error ocurred: {}", value),
        }
    }
}
