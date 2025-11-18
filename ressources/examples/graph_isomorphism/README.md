Example graphs for testing the GEM++ subgraph isomorphism solver.

Files
-----
- `triangle_pattern.gml`: undirected pattern graph with three labeled vertices forming a triangle.
- `triangle_target_isomorphic.gml`: target graph isomorphic to the pattern (vertices permuted).
- `triangle_target_nonisomorphic.gml`: target graph with an extra vertex and edge, not isomorphic to the pattern.

Usage
-----
Use the GEM++ command-line interface from the project root (after building the CLI tools). Examples:

- `build/bin/GEM++sub triangle_pattern.gml triangle_target_isomorphic.gml`
- `build/bin/GEM++sub triangle_pattern.gml triangle_target_nonisomorphic.gml`

The first command should report an isomorphism, while the second should report that no exact subgraph isomorphism exists.

Generating Unlabeled Multigraph Examples
----------------------------------------

A Python helper script is available at `tools/generate_unlabelled_multigraphs.py` to synthesize unlabeled multigraph pairs, useful for graph isomorphism experiments. The script emits GML files without vertex or edge labels (multiple edges and optional self-loops are supported) together with a `metadata.json` description.

Example command:

- `./tools/generate_unlabelled_multigraphs.py --output-dir ressources/examples/graph_isomorphism/unlabelled --vertices 4 --max-multiplicity 2 --positive 5 --negative 5 --seed 42`

The command above creates ten pairs (five isomorphic, five non-isomorphic) under the `unlabelled` directory. Adjust the options to control graph size, multiplicity, and pair counts.





