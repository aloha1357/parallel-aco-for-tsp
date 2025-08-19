# Dockerfile for GitLab CI/CD
# Multi-stage build for Parallel ACO TSP project

FROM gcc:latest as builder

# Install dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    ninja-build \
    libgtest-dev \
    cppcheck \
    valgrind \
    clang-tidy \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Build the project
RUN mkdir -p build && \
    cd build && \
    cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release && \
    ninja

# Runtime stage
FROM ubuntu:22.04 as runtime

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

# Copy built binaries
RUN mkdir -p /usr/local/bin
COPY --from=builder /app/build/ /app/build/

# Copy data files
COPY --from=builder /app/data /app/data

WORKDIR /app

# Default command
CMD ["aco_main"]
