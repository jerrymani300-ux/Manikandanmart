# Multi-stage Dockerfile for ManiMart C++20 Drogon E-Commerce Platform
FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

# Install build dependencies, compilers, CMake, Ninja, libsodium, sqlite3, curl
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
    libsqlite3-dev \
    libsodium-dev \
    libcurl4-openssl-dev \
    ca-certificates \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . /app

# Build ManiMart with embedded high-performance Drogon HTTP engine
RUN mkdir -p build && cd build && \
    cmake -B . -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_TESTING=OFF \
      .. && \
    cmake --build . --target manimart -j$(nproc)

# Final Minimal Runtime Image
FROM ubuntu:24.04 AS runner

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
    libsqlite3-0 \
    libsodium23 \
    libcurl4 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy binary and runtime assets
COPY --from=builder /app/build/manimart /app/manimart
COPY --from=builder /app/config /app/config
COPY --from=builder /app/db /app/db
COPY --from=builder /app/frontend /app/frontend

EXPOSE 8080

ENV SERVER_HOST=0.0.0.0
ENV PORT=8080
ENV DB_TYPE=sqlite
ENV SQLITE_DB_PATH=manimart.db
ENV AI_PROVIDER=mock

ENTRYPOINT ["/app/manimart"]
