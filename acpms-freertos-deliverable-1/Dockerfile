# Dockerfile - builds and runs ACPMS on the FreeRTOS POSIX (Linux) simulator port
FROM gcc:13

ARG FREERTOS_TAG=V11.1.0
RUN git clone --depth 1 --branch ${FREERTOS_TAG} https://github.com/FreeRTOS/FreeRTOS-Kernel.git /opt/FreeRTOS-Kernel

WORKDIR /src
COPY *.c *.h Makefile ./
RUN make

# Unprivileged user; the program writes acpms_log.txt into its working directory
RUN useradd -m acpms && mkdir -p /out && chown acpms /out
USER acpms
CMD ["/src/app"]