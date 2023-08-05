FROM ubuntu:22.04
WORKDIR repro

RUN apt update && apt install -y libpq-dev netsed gcc

COPY ./protocol_desync.c ./
RUN gcc -o protocol_desync protocol_desync.c -I/usr/include/postgresql -lpq

COPY ./netsed_and_run.sh ./
ENTRYPOINT ["bash", "-c", "./netsed_and_run.sh"]
