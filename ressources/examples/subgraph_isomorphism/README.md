# Subgraph Isomorphism Example (GML)

This directory contains a simple example for subgraph isomorphism using the GML format.

## Files

- `pattern.gml`: A triangle graph (3 nodes, 3 edges).
- `target.gml`: A square graph with a diagonal (4 nodes, 5 edges), which contains the triangle pattern.

## Usage

You can run the subgraph isomorphism command using the Docker container.

### Using Docker

```bash
docker run --rm \
  -v "$(pwd)/ressources/examples/subgraph_isomorphism:/data" \
  -w /data \
  gempp-cli sub --solver GLPK -t e -v -o solution.sol pattern.gml target.gml
```

### Expected Output

If the pattern is found in the target, the output will be the cost of the matching. For an exact subgraph isomorphism (`-t e`), the cost should be `0` (zero error).

```
0
```

- The `-v` flag enables verbose output, showing solver details.
- The `-o solution.sol` flag saves the matching solution to `solution.sol`.
