FROM ubuntu:22.04

EXPOSE 8080

WORKDIR /src
ADD . /src

RUN apt-get update; \
    apt-get upgrade; \
    cat apt-requirements.txt | xargs sudo apt-get -y install

RUN mkdir build; \
    cd build; \
    cmake -DCMAKE_BUILD_TYPE=Release ..; \
    cmake --build .; \
    cmake --install .

CMD [ "olympus" ]
