FROM rust:slim-bullseye as builder
WORKDIR /usr/src/olympus
COPY . .
RUN cargo install --path .

FROM debian:bullseye-slim
COPY --from=builder /usr/local/cargo/bin/olympus /usr/local/bin/olympus
EXPOSE 3000
CMD ["olympus"]
