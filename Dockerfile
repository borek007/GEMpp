# Multi-stage build for GEM++ CLI
# Stage 1: Build environment
FROM ubuntu:20.04 AS builder

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    qt5-default \
    qtbase5-dev \
    qt5-qmake \
    libqt5widgets5 \
    libqt5gui5 \
    libqt5core5a \
    libqt5opengl5-dev \
    libglpk-dev \
    glpk-utils \

    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY src/ ./src/
COPY doc/ ./doc/

# Modify build configuration to exclude GUI components for CLI-only build
RUN sed -i 's/SUBDIRS = Core GUI Molecule #CostsLearning/SUBDIRS = Core # GUI Molecule CostsLearning/' src/apps/apps.pro

# Disable all external solver plugins for now

# Fix architecture detection for ARM64
RUN sed -i 's/contains(QMAKE_HOST.arch, x86_64) { ARCH=64 } else { ARCH=32 }/contains(QMAKE_HOST.arch, x86_64):ARCH=64\ncontains(QMAKE_HOST.arch, aarch64):ARCH=64\nelse:ARCH=32/' src/common.pri

# Remove GUI widgets from library build for CLI-only
RUN sed -i '/Widgets\/QConfigurationDialog.h/d' src/library/library.pro
RUN sed -i '/Widgets\/QEdgeItem.h/d' src/library/library.pro
RUN sed -i '/Widgets\/QLabelItem.h/d' src/library/library.pro
RUN sed -i '/Widgets\/QVertexItem.h/d' src/library/library.pro
RUN sed -i '/Widgets\/QScienceSpinBox.h/d' src/library/library.pro
RUN sed -i '/Widgets\/QConfigurationDialog.cpp/d' src/library/library.pro
RUN sed -i '/Widgets\/QEdgeItem.cpp/d' src/library/library.pro
RUN sed -i '/Widgets\/QLabelItem.cpp/d' src/library/library.pro
RUN sed -i '/Widgets\/QVertexItem.cpp/d' src/library/library.pro
RUN sed -i '/Widgets\/QScienceSpinBox.cpp/d' src/library/library.pro

# Create build directory
RUN rm -rf build && mkdir -p build

# Build the project
WORKDIR /app/build
RUN qmake ../src/GEM++.pro
RUN make -j$(nproc) sub-library && \
    make -j$(nproc) sub-plugins && \
    make -j$(nproc) sub-apps

# Stage 2: Runtime environment
FROM ubuntu:20.04 AS runtime

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libqt5core5a \
    libqt5xml5 \
    libqt5widgets5 \
    libqt5gui5 \
    libqt5opengl5 \
    libglpk40 \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Copy built binaries and libraries from builder stage
COPY --from=builder /app/build/linux64/release/bin/GEM++ /app/GEM++
COPY --from=builder /app/build/linux64/release/lib/ /app/lib/

# Set library path
ENV LD_LIBRARY_PATH=/app/lib:$LD_LIBRARY_PATH

# Create entrypoint script
RUN echo '#!/bin/bash\nexec /app/GEM++ "$@"' > /app/entrypoint.sh && \
    chmod +x /app/entrypoint.sh

# Set the entrypoint
ENTRYPOINT ["/app/entrypoint.sh"]

# Default command shows help
CMD ["--help"]
