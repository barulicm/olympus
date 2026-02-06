FROM rust:slim-bullseye AS builder
WORKDIR /usr/src/olympus
COPY . .
RUN cargo install --path .

FROM debian:bullseye-slim
COPY --from=builder /usr/local/cargo/bin/olympus /usr/local/bin/olympus
EXPOSE 8080
CMD ["olympus"]
