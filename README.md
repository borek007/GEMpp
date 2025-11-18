Basically this is a dockerized version of GEM++ : https://github.com/jlerouge/GEMpp

## Building the container

```bash
docker build -t gempp-cli .
```

## Example: Exact Subgraph Match (Docker)

From the project root, run the command below to check exact subgraph isomorphism on the unlabeled GXL pair generated earlier:

```bash
docker run --rm \
  -v "$PWD/ressources/examples/graph_isomorphism/unlabelled:/data" \
  -w /data \
  gempp-cli sub --solver GLPK -t e pair_000_pattern.gxl pair_000_target.gxl
```