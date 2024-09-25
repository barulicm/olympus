FROM ubuntu:22.04

EXPOSE 8080

WORKDIR /src
ADD . /src

RUN cmake --workflow --preset linux-package

CMD [ "olympus" ]
