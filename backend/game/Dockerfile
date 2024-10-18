FROM gcc:14 AS builder

RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    libpq-dev \
    libssl-dev \
    libasio-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src

# Copy and build the dependencies
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./external ./external

RUN cmake -S . -B build -DBUILD_ONLY_DEPENDENCIES=ON \
    && cd build \
    && make

RUN rm build/CMakeCache.txt

# Copy and build the application

COPY ./communication ./communication
COPY ./data ./data
COPY ./game ./game
COPY ./rest ./rest
COPY ./websocket ./websocket

RUN cmake -S . -B build \
    && cd build \
    && make

CMD ["/src/build/game/GameService"]
