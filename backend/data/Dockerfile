FROM gcc:14 AS builder

RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    libpq-dev \
    libssl-dev \
    libasio-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src

COPY ./communication ./communication
COPY ./data ./data
COPY ./external ./external
COPY ./game ./game
COPY ./rest ./rest
COPY ./websocket ./websocket
COPY ./CMakeLists.txt ./CMakeLists.txt

RUN cmake -S . -B build \
    && cd build \
    && make

# FROM debian:bullseye-slim
#
# RUN apt-get update && apt-get install -y \
#       libstdc++6 \
#     && rm -rf /var/lib/apt/lists/*
#
# # Copy runtime dependencies from the builder stage
# COPY --from=builder /usr/local/bin /usr/local/bin
#
# COPY --from=builder /usr/lib /usr/lib
# COPY --from=builder /usr/local/lib /usr/local/lib
#
# # Copy the compiled application from the builder stage
# COPY --from=builder /src/build/rest/RestService /usr/bin/RestService
#
# # Copy shared objects
# COPY --from=builder /src/build/external/libevent/lib/* /usr/local/lib/
#
# ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

CMD ["/src/build/data/DataService"]
