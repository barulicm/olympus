FROM ubuntu:22.04

EXPOSE 8080

WORKDIR /src
ADD . /src

# Most of these deps are needed by vcpkg
RUN DEBIAN_FRONTEND=noninteractive ; \
    apt-get update; \
    apt-get -y upgrade; \
    apt-get -y install wget build-essential curl zip unzip tar git pkg-config

RUN wget https://github.com/Kitware/CMake/releases/download/v3.30.3/cmake-3.30.3-linux-x86_64.tar.gz; \
    tar -xzf cmake-3.30.3-linux-x86_64.tar.gz;

RUN PATH=/src/cmake-3.30.3-linux-x86_64/bin:$PATH; \
    cmake --preset linux-release .; \
    cmake --build --preset linux-release --target install

CMD [ "olympus" ]
