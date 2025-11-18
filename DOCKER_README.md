# GEM++ CLI Docker Container

This Docker container provides the command-line interface for GEM++ (Graph Extraction and Matching C++ software).

## Building the Container

```bash
docker build -t gempp-cli .
```

Or using docker-compose:

```bash
docker-compose build
```

## Running the Container

### Using Docker directly

```bash
# Show help
docker run --rm gempp-cli

# Run a command (mount your data directory)
docker run --rm -v $(pwd)/data:/data -w /data gempp-cli dist graph1.gml graph2.gml
```

### Using Docker Compose

```bash
# Show help
docker-compose run --rm gempp-cli

# Run commands with your data
docker-compose run --rm gempp-cli dist graph1.gml graph2.gml
```

## Available Commands

The GEM++ CLI supports the following commands:

- `dist`: Compute edit distance between two graphs
- `sub`: Search subgraph isomorphism
- `multidist`: Compute distance matrix for multiple graphs
- `multisub`: Search subgraph isomorphism in multiple graphs

## Examples

```bash
# Compute graph edit distance
docker run --rm -v $(pwd)/graphs:/data gempp-cli dist pattern.gml target.gml

# Search for subgraph isomorphism
docker run --rm -v $(pwd)/graphs:/data gempp-cli sub pattern.gml target.gml

# Compute distance matrix for multiple graphs
docker run --rm -v $(pwd)/graphs:/data gempp-cli multidist graphs.txt

# Use custom weights configuration
docker run --rm -v $(pwd)/graphs:/data -v $(pwd)/weights:/weights gempp-cli dist -w /weights/weights.xml pattern.gml target.gml
```

## File Formats

GEM++ supports:
- **GML** (Graph Modeling Language)
- **GXL** (Graph eXchange Language)

## Solver Support

The container includes support for:
- **Gurobi** (default)
- Other solvers can be added by modifying the Dockerfile

## Data Persistence

Mount your data directory to `/data` in the container:

```bash
docker run --rm -v /path/to/your/data:/data gempp-cli <command> <args>
```

## Building from Source

The Dockerfile uses a multi-stage build:
1. **Builder stage**: Compiles GEM++ with Qt5 and dependencies
2. **Runtime stage**: Minimal Ubuntu image with only runtime dependencies

## Troubleshooting

If you encounter library loading issues, ensure all required files are present in your working directory and properly mounted.

For more information about GEM++ and its algorithms, see the main README.md file.
