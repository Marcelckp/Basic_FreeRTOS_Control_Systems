# Dockerfile - builds and runs SITSC on the FreeRTOS POSIX (Linux) simulator port
FROM gcc:13

ARG FREERTOS_TAG=V11.1.0
RUN git clone --depth 1 --branch ${FREERTOS_TAG} https://github.com/FreeRTOS/FreeRTOS-Kernel.git /opt/FreeRTOS-Kernel

WORKDIR /src
COPY *.c *.h Makefile ./
RUN make

# Unprivileged user; the program writes sitsc_log.txt into its working directory
RUN useradd -m sitsc && mkdir -p /out && chown sitsc /out
USER sitsc
CMD ["/src/app"]