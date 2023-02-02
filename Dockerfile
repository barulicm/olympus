FROM ubuntu:22.04

EXPOSE 8080

RUN apt-get update; \
    apt-get upgrade; \
    apt-get install -y build-essential cmake pkg-config duktape-dev libcpprest-dev nlohmann-json3-dev libgtest-dev libgmock-dev

WORKDIR /src
ADD . /src

RUN mkdir build; \
    cd build; \
    cmake -DCMAKE_BUILD_TYPE=Release ..; \
    cmake --build .; \
    cmake --install .

CMD [ "olympus" ]
