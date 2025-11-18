#!/usr/bin/env python3
"""
Utility to generate unlabeled multigraph pairs for graph isomorphism testing.

The script creates pairs of graphs (isomorphic and non-isomorphic) encoded as
undirected GML files without vertex or edge labels. Positive pairs share the
same underlying structure up to a random permutation, while negative pairs come
from different canonical structures. A companion metadata JSON file describes
each pair.

Example:
    ./tools/generate_unlabelled_multigraphs.py \\
        --output-dir ressources/examples/graph_isomorphism/unlabelled \\
        --vertices 4 \\
        --max-multiplicity 2 \\
        --positive 5 \\
        --negative 5 \\
        --seed 123
"""

from __future__ import annotations

import argparse
import itertools
import json
import os
import random
from dataclasses import dataclass
from typing import List, Sequence, Tuple

AdjacencyMatrix = List[List[int]]


@dataclass(frozen=True)
class Multigraph:
    """Container for a multigraph adjacency matrix and its canonical signature."""

    adjacency: AdjacencyMatrix
    canonical: Tuple[int, ...]

    @property
    def order(self) -> int:
        return len(self.adjacency)

    @property
    def size(self) -> int:
        """Return the total number of edges counting multiplicities (loops included)."""
        edges = 0
        for i in range(self.order):
            for j in range(i, self.order):
                mult = self.adjacency[i][j]
                if mult <= 0:
                    continue
                edges += mult
        return edges


def canonicalize(adjacency: AdjacencyMatrix) -> Tuple[int, ...]:
    """Return a canonical signature for an unlabeled undirected multigraph."""
    n = len(adjacency)
    best: Tuple[int, ...] | None = None

    for perm in itertools.permutations(range(n)):
        flat: List[int] = []
        for i in perm:
            row = adjacency[i]
            for j in perm:
                flat.append(row[j])
        candidate = tuple(flat)
        if best is None or candidate < best:
            best = candidate
    if best is None:
        raise ValueError("Failed to canonicalize adjacency matrix.")
    return best


def random_multigraph(
    *,
    vertices: int,
    max_multiplicity: int,
    allow_loops: bool,
    rng: random.Random,
) -> AdjacencyMatrix:
    """Generate a random undirected multigraph adjacency matrix."""
    if vertices <= 0:
        raise ValueError("Number of vertices must be positive.")
    if max_multiplicity <= 0:
        raise ValueError("Maximum edge multiplicity must be at least 1.")

    adjacency = [[0 for _ in range(vertices)] for _ in range(vertices)]

    for i in range(vertices):
        for j in range(i, vertices):
            if i == j and not allow_loops:
                continue
            multiplicity = rng.randint(0, max_multiplicity)
            if multiplicity <= 0:
                continue
            adjacency[i][j] = multiplicity
            adjacency[j][i] = multiplicity

    # Ensure the graph is not edgeless.
    if all(adjacency[i][j] == 0 for i in range(vertices) for j in range(vertices)):
        if allow_loops:
            idx = rng.randrange(vertices)
            adjacency[idx][idx] = 1
        else:
            i, j = rng.sample(range(vertices), 2)
            adjacency[i][j] = adjacency[j][i] = 1

    return adjacency


def generate_unique_multigraphs(
    *,
    count: int,
    vertices: int,
    max_multiplicity: int,
    allow_loops: bool,
    rng: random.Random,
    max_attempts: int = 10000,
) -> List[Multigraph]:
    """Generate a collection of multigraphs with distinct canonical signatures."""
    unique: dict[Tuple[int, ...], AdjacencyMatrix] = {}
    attempts = 0

    while len(unique) < count and attempts < max_attempts:
        adjacency = random_multigraph(
            vertices=vertices,
            max_multiplicity=max_multiplicity,
            allow_loops=allow_loops,
            rng=rng,
        )
        signature = canonicalize(adjacency)
        if signature not in unique:
            unique[signature] = adjacency
        attempts += 1

    if len(unique) < count:
        raise RuntimeError(
            f"Could not generate {count} unique multigraphs "
            f"within {max_attempts} attempts. "
            "Try relaxing the parameters (increase max-multiplicity or vertices)."
        )

    return [Multigraph(adjacency=adj, canonical=sig) for sig, adj in unique.items()]


def apply_random_permutation(
    adjacency: AdjacencyMatrix, rng: random.Random
) -> Tuple[AdjacencyMatrix, List[int]]:
    """Return a permuted copy of adjacency and the mapping from original to new IDs."""
    n = len(adjacency)
    if n == 0:
        return [], []
    new_order = list(range(n))
    rng.shuffle(new_order)
    permuted = [[0 for _ in range(n)] for _ in range(n)]
    for new_i, old_i in enumerate(new_order):
        for new_j, old_j in enumerate(new_order):
            permuted[new_i][new_j] = adjacency[old_i][old_j]
    # Map original vertex index -> new index
    mapping = [0 for _ in range(n)]
    for new_idx, old_idx in enumerate(new_order):
        mapping[old_idx] = new_idx
    return permuted, mapping


def write_gml(adjacency: AdjacencyMatrix, path: str) -> None:
    """Write the adjacency matrix to a GML file without labels."""
    n = len(adjacency)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", encoding="utf-8") as handle:
        handle.write("graph [\n")
        handle.write("  directed 0\n")
        for node_id in range(n):
            handle.write("  node [\n")
            handle.write(f"    id {node_id}\n")
            handle.write("  ]\n")
        for i in range(n):
            for j in range(i, n):
                multiplicity = adjacency[i][j]
                if multiplicity <= 0:
                    continue
                for _ in range(multiplicity):
                    handle.write("  edge [\n")
                    handle.write(f"    source {i}\n")
                    handle.write(f"    target {j}\n")
                    handle.write("  ]\n")
        handle.write("]\n")


def write_metadata(path: str, metadata: dict) -> None:
    with open(path, "w", encoding="utf-8") as handle:
        json.dump(metadata, handle, indent=2)
        handle.write("\n")


def build_pairs(
    *,
    positive: int,
    negative: int,
    base_graphs: List[Multigraph],
    vertices: int,
    output_dir: str,
    rng: random.Random,
) -> dict:
    """Construct graph pairs and emit files plus metadata."""
    if not base_graphs:
        raise ValueError("No base graphs were generated.")

    os.makedirs(output_dir, exist_ok=True)

    metadata: dict = {
        "description": (
            "Automatically generated unlabeled multigraph pairs for "
            "graph isomorphism testing."
        ),
        "parameters": {
            "vertices": vertices,
            "positive_pairs": positive,
            "negative_pairs": negative,
        },
        "pairs": [],
    }

    pair_index = 0
    graph_pool = base_graphs.copy()

    if positive > 0 and len(graph_pool) == 1:
        # Ensure we can produce non-trivial permutations even when only one base graph exists.
        if graph_pool[0].order < 2:
            raise RuntimeError(
                "Cannot create isomorphic pairs with fewer than 2 vertices."
            )

    # Generate isomorphic pairs.
    for idx in range(positive):
        base = graph_pool[idx % len(graph_pool)]
        permuted, mapping = apply_random_permutation(base.adjacency, rng)

        pair_id = f"pair_{pair_index:03d}"
        pattern_path = os.path.join(output_dir, f"{pair_id}_pattern.gml")
        target_path = os.path.join(output_dir, f"{pair_id}_target.gml")

        write_gml(base.adjacency, pattern_path)
        write_gml(permuted, target_path)

        metadata["pairs"].append(
            {
                "id": pair_id,
                "type": "isomorphic",
                "pattern": os.path.basename(pattern_path),
                "target": os.path.basename(target_path),
                "vertex_count": base.order,
                "edge_count": base.size,
                "canonical_signature": list(base.canonical),
                "permutation": mapping,
            }
        )
        pair_index += 1

    # Generate non-isomorphic pairs.
    if negative > 0 and len(graph_pool) < 2:
        raise RuntimeError(
            "Need at least two distinct base graphs to create non-isomorphic pairs."
        )

    for idx in range(negative):
        left = graph_pool[(positive + 2 * idx) % len(graph_pool)]
        right = graph_pool[(positive + 2 * idx + 1) % len(graph_pool)]
        # Guarantee distinct canonical signatures; fall back to random selection if needed.
        if left.canonical == right.canonical:
            candidates = [g for g in graph_pool if g.canonical != left.canonical]
            if not candidates:
                raise RuntimeError(
                    "Unable to find two distinct canonical forms for negative pair."
                )
            right = rng.choice(candidates)

        pair_id = f"pair_{pair_index:03d}"
        pattern_path = os.path.join(output_dir, f"{pair_id}_pattern.gml")
        target_path = os.path.join(output_dir, f"{pair_id}_target.gml")

        write_gml(left.adjacency, pattern_path)
        write_gml(right.adjacency, target_path)

        metadata["pairs"].append(
            {
                "id": pair_id,
                "type": "non-isomorphic",
                "pattern": os.path.basename(pattern_path),
                "target": os.path.basename(target_path),
                "vertex_count": left.order,
                "pattern_edge_count": left.size,
                "target_edge_count": right.size,
                "pattern_canonical_signature": list(left.canonical),
                "target_canonical_signature": list(right.canonical),
            }
        )
        pair_index += 1

    return metadata


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate unlabeled multigraph pairs for isomorphism testing.",
    )
    parser.add_argument(
        "--output-dir",
        required=True,
        help="Directory where the GML files and metadata.json will be written.",
    )
    parser.add_argument(
        "--vertices",
        type=int,
        default=4,
        help="Number of vertices per graph (default: 4).",
    )
    parser.add_argument(
        "--max-multiplicity",
        type=int,
        default=2,
        help="Maximum multiplicity per edge (default: 2).",
    )
    parser.add_argument(
        "--allow-loops",
        action="store_true",
        help="Allow self-loops when generating graphs.",
    )
    parser.add_argument(
        "--positive",
        type=int,
        default=5,
        help="Number of isomorphic pairs to generate (default: 5).",
    )
    parser.add_argument(
        "--negative",
        type=int,
        default=5,
        help="Number of non-isomorphic pairs to generate (default: 5).",
    )
    parser.add_argument(
        "--seed",
        type=int,
        default=None,
        help="Optional random seed for reproducibility.",
    )
    parser.add_argument(
        "--max-attempts",
        type=int,
        default=20000,
        help="Maximum attempts to find the requested number of unique graphs.",
    )

    args = parser.parse_args(argv)

    if args.positive < 0 or args.negative < 0:
        raise SystemExit("Pair counts must be non-negative.")
    if args.positive == 0 and args.negative == 0:
        raise SystemExit("Request at least one pair (positive or negative).")

    return args


def main(argv: Sequence[str] | None = None) -> None:
    args = parse_args(argv)
    rng = random.Random(args.seed)

    unique_required = max(1, args.positive + args.negative)
    if args.negative > 0:
        unique_required = max(unique_required, 2)

    base_graphs = generate_unique_multigraphs(
        count=unique_required,
        vertices=args.vertices,
        max_multiplicity=args.max_multiplicity,
        allow_loops=args.allow_loops,
        rng=rng,
        max_attempts=args.max_attempts,
    )

    metadata = build_pairs(
        positive=args.positive,
        negative=args.negative,
        base_graphs=base_graphs,
        vertices=args.vertices,
        output_dir=args.output_dir,
        rng=rng,
    )

    metadata_path = os.path.join(args.output_dir, "metadata.json")
    write_metadata(metadata_path, metadata)


if __name__ == "__main__":
    main()


