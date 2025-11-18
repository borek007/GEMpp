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





