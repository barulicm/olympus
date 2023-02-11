FROM ubuntu:22.04

EXPOSE 8080

WORKDIR /src
ADD . /src

RUN apt-get update; \
    DEBIAN_FRONTEND=noninteractive apt-get -y upgrade; \
    # remove all carriage returns (useful when building image on Windows host)
    sed -i $'s/\r$//' apt-requirements.txt; \
    DEBIAN_FRONTEND=noninteractive xargs -t -a apt-requirements.txt apt-get -y install

RUN mkdir build; \
    cd build; \
    cmake -DCMAKE_BUILD_TYPE=Release ..; \
    cmake --build .; \
    cmake --install .

CMD [ "olympus" ]
