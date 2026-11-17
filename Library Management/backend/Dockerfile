# ==============================================================================
# Multi-stage Dockerfile for C++17 Drogon Backend (Ubuntu 24.04)
# ==============================================================================

# Stage 1: Build stage
FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    libdrogon-dev \
    libjsoncpp-dev \
    uuid-dev \
    zlib1g-dev \
    libssl-dev \
    libpq-dev \
    libsqlite3-dev \
    libmariadb-dev \
    libmariadb-dev-compat \
    libbrotli-dev \
    libhiredis-dev \
    libyaml-cpp-dev \
    libboost-dev \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Fix Ubuntu 24.04 multiarch path for Drogon's legacy FindMySQL.cmake
RUN mkdir -p /usr/lib/mariadb && \
    find /usr/lib -name "libmariadb*.so*" -exec ln -sf {} /usr/lib/mariadb/libmariadbclient.so \; 2>/dev/null || true

WORKDIR /app
RUN mkdir -p /app/logs /app/public /app/uploads
COPY . .

RUN mkdir -p build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release .. \
    && make -j$(nproc)

# Stage 2: Minimal runtime image
FROM ubuntu:24.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    libdrogon1t64 \
    libjsoncpp25 \
    libuuid1 \
    zlib1g \
    libssl3t64 \
    libpq5 \
    libsqlite3-0 \
    libmariadb3 \
    libbrotli1 \
    libhiredis1.1.0 \
    libyaml-cpp0.8 \
    ca-certificates \
    curl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
RUN mkdir -p /app/logs /app/public /app/uploads

COPY --from=builder /app/build/library_backend /app/library_backend
COPY --from=builder /app/config.json /app/config.json
COPY --from=builder /app/sql /app/sql

EXPOSE 8080

HEALTHCHECK --interval=10s --timeout=3s --retries=5 \
  CMD curl -f http://localhost:8080/api/health || exit 1

ENTRYPOINT ["/app/library_backend"]
