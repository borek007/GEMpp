#!/usr/bin/env python3
"""
Verify the consistency of metadata describing unlabeled multigraph pairs.

The script cross-checks the counts, canonical signatures, and declared
isomorphism / subgraph relations between pattern and target graphs encoded as
GXL files. It exits with a non-zero status code if any assertion fails.

Example:
    ./tools/verify_unlabelled_metadata.py \
        ressources/examples/graph_isomorphism/unlabelled/metadata.json
"""

from __future__ import annotations

import argparse
import itertools
import json
import os
import sys
import xml.etree.ElementTree as ET
from collections import Counter
from dataclasses import dataclass
from typing import Counter as CounterType, Iterable, List, Optional, Sequence, Tuple


@dataclass
class Check:
    """Single verification outcome."""

    subject: str
    name: str
    ok: bool
    details: Optional[str] = None


class Graph:
    """Lightweight container for an unlabeled undirected multigraph."""

    def __init__(
        self,
        *,
        path: str,
        adjacency: List[List[int]],
        orig_ids: List[int],
        xml_to_index: dict,
        edge_count: int,
        edge_multiset: CounterType[Tuple[int, int]],
    ) -> None:
        self.path = path
        self.adjacency = adjacency
        self.orig_ids = orig_ids
        self.xml_to_index = xml_to_index
        self.edge_count = edge_count
        self.edge_multiset = edge_multiset
        self._canonical_signature: Optional[Tuple[int, ...]] = None

    @classmethod
    def from_gxl(cls, path: str) -> "Graph":
        tree = ET.parse(path)
        root = tree.getroot()
        graph_elem = root.find("graph")
        if graph_elem is None:
            raise ValueError(f"{path}: missing <graph> element")

        nodes = []
        for node in graph_elem.findall("node"):
            xml_id = node.attrib.get("id")
            if xml_id is None:
                raise ValueError(f"{path}: node without id attribute")
            attr = node.find("./attr[@name='original_id']/string")
            if attr is not None and attr.text is not None:
                orig = int(attr.text)
            else:
                if xml_id.startswith("n"):
                    orig = int(xml_id[1:])
                else:
                    raise ValueError(
                        f"{path}: cannot infer original_id for node '{xml_id}'"
                    )
            nodes.append((xml_id, orig))

        nodes.sort(key=lambda item: item[1])
        orig_ids = [orig for _, orig in nodes]
        xml_to_index = {xml_id: idx for idx, (xml_id, _) in enumerate(nodes)}
        order = len(nodes)

        adjacency = [[0 for _ in range(order)] for _ in range(order)]
        edge_multiset: CounterType[Tuple[int, int]] = Counter()
        edge_count = 0

        for edge in graph_elem.findall("edge"):
            source = edge.attrib.get("from")
            target = edge.attrib.get("to")
            if source is None or target is None:
                raise ValueError(f"{path}: edge without 'from'/'to' attributes")
            if source not in xml_to_index or target not in xml_to_index:
                raise ValueError(f"{path}: edge references unknown node")

            u = xml_to_index[source]
            v = xml_to_index[target]
            adjacency[u][v] += 1
            adjacency[v][u] += 1
            key = (u, v) if u <= v else (v, u)
            edge_multiset[key] += 1
            edge_count += 1

        return cls(
            path=path,
            adjacency=adjacency,
            orig_ids=orig_ids,
            xml_to_index=xml_to_index,
            edge_count=edge_count,
            edge_multiset=edge_multiset,
        )

    @property
    def order(self) -> int:
        return len(self.adjacency)

    @property
    def orig_to_index(self) -> dict:
        return {orig: idx for idx, orig in enumerate(self.orig_ids)}

    def canonical_signature(self) -> List[int]:
        if self._canonical_signature is None:
            best: Optional[Tuple[int, ...]] = None
            for perm in itertools.permutations(range(self.order)):
                flattened: List[int] = []
                for i in perm:
                    row = self.adjacency[i]
                    for j in perm:
                        flattened.append(row[j])
                candidate = tuple(flattened)
                if best is None or candidate < best:
                    best = candidate
            if best is None:
                raise ValueError(f"{self.path}: failed to compute canonical signature")
            self._canonical_signature = best
        return list(self._canonical_signature)

    def check_mapping(self, other: "Graph", mapping: Sequence[int]) -> bool:
        if len(mapping) != self.order:
            return False
        if len(set(mapping)) != len(mapping):
            return False
        for i in range(self.order):
            for j in range(self.order):
                if self.adjacency[i][j] != other.adjacency[mapping[i]][mapping[j]]:
                    return False
        return True

    def check_subgraph_mapping(self, other: "Graph", mapping: Sequence[int]) -> bool:
        if len(mapping) != self.order:
            return False
        if len(set(mapping)) != len(mapping):
            return False
        needed: CounterType[Tuple[int, int]] = Counter()
        for (u, v), count in self.edge_multiset.items():
            mu = mapping[u]
            mv = mapping[v]
            key = (mu, mv) if mu <= mv else (mv, mu)
            needed[key] += count
        for key, count in needed.items():
            if other.edge_multiset.get(key, 0) < count:
                return False
        return True

    def enumerate_isomorphisms(self, other: "Graph") -> List[Tuple[int, ...]]:
        if self.order != other.order or self.edge_count != other.edge_count:
            return []
        matches: List[Tuple[int, ...]] = []
        for perm in itertools.permutations(range(other.order)):
            if self.check_mapping(other, perm):
                matches.append(tuple(perm))
        return matches

    def enumerate_subgraph_embeddings(self, other: "Graph") -> List[Tuple[int, ...]]:
        matches: List[Tuple[int, ...]] = []
        for perm in itertools.permutations(range(other.order), self.order):
            if self.check_subgraph_mapping(other, perm):
                matches.append(tuple(perm))
        return matches


def parse_args(argv: Optional[Sequence[str]] = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Verify unlabeled multigraph metadata against GXL contents."
    )
    parser.add_argument(
        "metadata",
        help="Path to metadata.json describing the graph pairs.",
    )
    parser.add_argument(
        "--base-dir",
        help="Optional base directory for pattern/target paths (defaults to metadata's directory).",
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Print every check result instead of only failures.",
    )
    return parser.parse_args(argv)


def load_graph(path: str) -> Graph:
    if not os.path.exists(path):
        raise FileNotFoundError(path)
    return Graph.from_gxl(path)


def map_metadata_permutation(
    *,
    meta_perm: Sequence[int],
    pattern: Graph,
    target: Graph,
) -> Tuple[int, ...]:
    if len(meta_perm) != pattern.order:
        raise ValueError(
            f"permutation length {len(meta_perm)} does not match pattern vertex count {pattern.order}"
        )
    target_map = target.orig_to_index
    mapped: List[int] = []
    for value in meta_perm:
        if value not in target_map:
            raise ValueError(f"target original id {value} is not present in {target.path}")
        mapped.append(target_map[value])
    return tuple(mapped)


def format_details(details: Optional[str]) -> str:
    return f" ({details})" if details else ""


def add_check(checks: List[Check], subject: str, name: str, ok: bool, details: Optional[str] = None) -> None:
    checks.append(Check(subject=subject, name=name, ok=ok, details=details))


def verify_pair(pair: dict, base_dir: str) -> List[Check]:
    pair_id = pair.get("id", "<unknown>")
    checks: List[Check] = []

    pattern_path = os.path.join(base_dir, pair.get("pattern", ""))
    target_path = os.path.join(base_dir, pair.get("target", ""))

    try:
        pattern_graph = load_graph(pattern_path)
    except Exception as exc:  # pylint: disable=broad-exception-caught
        add_check(checks, pair_id, "pattern_graph_loaded", False, str(exc))
        return checks

    try:
        target_graph = load_graph(target_path)
    except Exception as exc:  # pylint: disable=broad-exception-caught
        add_check(checks, pair_id, "target_graph_loaded", False, str(exc))
        return checks

    if "vertex_count" in pair:
        expected = pair["vertex_count"]
        add_check(
            checks,
            pair_id,
            "pattern_vertex_count",
            pattern_graph.order == expected,
            f"expected {expected}, actual {pattern_graph.order}",
        )
        add_check(
            checks,
            pair_id,
            "target_vertex_count",
            target_graph.order == expected,
            f"expected {expected}, actual {target_graph.order}",
        )
    else:
        if "pattern_vertex_count" in pair:
            expected = pair["pattern_vertex_count"]
            add_check(
                checks,
                pair_id,
                "pattern_vertex_count",
                pattern_graph.order == expected,
                f"expected {expected}, actual {pattern_graph.order}",
            )
        if "target_vertex_count" in pair:
            expected = pair["target_vertex_count"]
            add_check(
                checks,
                pair_id,
                "target_vertex_count",
                target_graph.order == expected,
                f"expected {expected}, actual {target_graph.order}",
            )

    if "edge_count" in pair:
        expected = pair["edge_count"]
        add_check(
            checks,
            pair_id,
            "pattern_edge_count",
            pattern_graph.edge_count == expected,
            f"expected {expected}, actual {pattern_graph.edge_count}",
        )
        add_check(
            checks,
            pair_id,
            "target_edge_count",
            target_graph.edge_count == expected,
            f"expected {expected}, actual {target_graph.edge_count}",
        )
    else:
        if "pattern_edge_count" in pair:
            expected = pair["pattern_edge_count"]
            add_check(
                checks,
                pair_id,
                "pattern_edge_count",
                pattern_graph.edge_count == expected,
                f"expected {expected}, actual {pattern_graph.edge_count}",
            )
        if "target_edge_count" in pair:
            expected = pair["target_edge_count"]
            add_check(
                checks,
                pair_id,
                "target_edge_count",
                target_graph.edge_count == expected,
                f"expected {expected}, actual {target_graph.edge_count}",
            )

    if "canonical_signature" in pair:
        expected = pair["canonical_signature"]
        actual_pattern = pattern_graph.canonical_signature()
        actual_target = target_graph.canonical_signature()
        add_check(
            checks,
            pair_id,
            "pattern_canonical_signature",
            actual_pattern == expected,
            f"expected {expected}, actual {actual_pattern}",
        )
        add_check(
            checks,
            pair_id,
            "target_canonical_signature",
            actual_target == expected,
            f"expected {expected}, actual {actual_target}",
        )
    else:
        if "pattern_canonical_signature" in pair:
            expected = pair["pattern_canonical_signature"]
            actual = pattern_graph.canonical_signature()
            add_check(
                checks,
                pair_id,
                "pattern_canonical_signature",
                actual == expected,
                f"expected {expected}, actual {actual}",
            )
        if "target_canonical_signature" in pair:
            expected = pair["target_canonical_signature"]
            actual = target_graph.canonical_signature()
            add_check(
                checks,
                pair_id,
                "target_canonical_signature",
                actual == expected,
                f"expected {expected}, actual {actual}",
            )

    pair_type = pair.get("type")

    if pair_type == "isomorphic":
        isomorphisms = pattern_graph.enumerate_isomorphisms(target_graph)
        add_check(
            checks,
            pair_id,
            "declared_isomorphic",
            bool(isomorphisms),
            f"found {len(isomorphisms)} isomorphism(s)",
        )
        metadata_perms = pair.get("permutations")
        if metadata_perms is None and "permutation" in pair:
            metadata_perms = [pair["permutation"]]
        if metadata_perms:
            valid = True
            subset_ok = True
            try:
                converted = [
                    map_metadata_permutation(
                        meta_perm=perm, pattern=pattern_graph, target=target_graph
                    )
                    for perm in metadata_perms
                ]
                for mapping in converted:
                    if not pattern_graph.check_mapping(target_graph, mapping):
                        valid = False
                if valid:
                    tupled = [tuple(m) for m in converted]
                    subset_ok = all(t in isomorphisms for t in tupled)
            except ValueError as exc:
                valid = False
                subset_ok = False
                add_check(checks, pair_id, "metadata_permutations_valid", False, str(exc))
            else:
                add_check(
                    checks,
                    pair_id,
                    "metadata_permutations_valid",
                    valid,
                    f"permutations={metadata_perms}",
                )
                add_check(
                    checks,
                    pair_id,
                    "metadata_permutations_subset",
                    subset_ok,
                    f"permutations={metadata_perms}",
                )
        else:
            add_check(
                checks,
                pair_id,
                "metadata_permutations_present",
                False,
                "no permutations supplied",
            )

    elif pair_type == "non-isomorphic":
        isomorphisms = pattern_graph.enumerate_isomorphisms(target_graph)
        add_check(
            checks,
            pair_id,
            "declared_non_isomorphic",
            len(isomorphisms) == 0,
            f"found {len(isomorphisms)} isomorphism(s)",
        )

    elif pair_type == "subgraph_isomorphic":
        embeddings = pattern_graph.enumerate_subgraph_embeddings(target_graph)
        add_check(
            checks,
            pair_id,
            "declared_subgraph_isomorphic",
            bool(embeddings),
            f"found {len(embeddings)} embedding(s)",
        )
        metadata_perms = pair.get("permutations", [])
        if metadata_perms:
            try:
                converted = [
                    map_metadata_permutation(
                        meta_perm=perm, pattern=pattern_graph, target=target_graph
                    )
                    for perm in metadata_perms
                ]
            except ValueError as exc:
                add_check(checks, pair_id, "metadata_permutations_valid", False, str(exc))
            else:
                valid = True
                for mapping in converted:
                    if not pattern_graph.check_subgraph_mapping(target_graph, mapping):
                        valid = False
                        break
                add_check(
                    checks,
                    pair_id,
                    "metadata_permutations_valid",
                    valid,
                    f"permutations={metadata_perms}",
                )
        else:
            add_check(
                checks,
                pair_id,
                "metadata_permutations_present",
                False,
                "no permutations supplied",
            )

    elif pair_type == "not_subgraph_isomorphic":
        embeddings = pattern_graph.enumerate_subgraph_embeddings(target_graph)
        add_check(
            checks,
            pair_id,
            "declared_not_subgraph_isomorphic",
            len(embeddings) == 0,
            f"found {len(embeddings)} embedding(s)",
        )
    else:
        add_check(
            checks,
            pair_id,
            "recognised_type",
            False,
            f"unsupported type '{pair_type}'",
        )

    return checks


def verify_parameters(metadata: dict, pairs: Sequence[dict]) -> List[Check]:
    checks: List[Check] = []
    params = metadata.get("parameters", {})
    subject = "parameters"

    counts = Counter()
    for pair in pairs:
        counts[pair.get("type")] += 1

    expected_map = {
        "positive_pairs": counts.get("isomorphic", 0),
        "negative_pairs": counts.get("non-isomorphic", 0),
        "subgraph_positive_pairs": counts.get("subgraph_isomorphic", 0),
        "subgraph_negative_pairs": counts.get("not_subgraph_isomorphic", 0),
    }

    for key, actual in expected_map.items():
        if key in params:
            expected = params[key]
            add_check(
                checks,
                subject,
                key,
                expected == actual,
                f"expected {expected}, actual {actual}",
            )

    return checks


def print_results(results: Iterable[Check], *, verbose: bool) -> bool:
    failures = [check for check in results if not check.ok]
    if verbose:
        for check in results:
            status = "OK" if check.ok else "FAIL"
            detail = format_details(check.details)
            print(f"[{check.subject}] {check.name}: {status}{detail}")
    else:
        for check in failures:
            detail = format_details(check.details)
            print(f"[{check.subject}] {check.name}: FAIL{detail}", file=sys.stderr)
    return not failures


def main(argv: Optional[Sequence[str]] = None) -> int:
    args = parse_args(argv)
    metadata_path = args.metadata
    try:
        with open(metadata_path, "r", encoding="utf-8") as handle:
            metadata = json.load(handle)
    except Exception as exc:  # pylint: disable=broad-exception-caught
        print(f"Failed to load metadata: {exc}", file=sys.stderr)
        return 1

    base_dir = args.base_dir or os.path.dirname(os.path.abspath(metadata_path))
    pairs = metadata.get("pairs", [])

    all_checks: List[Check] = []
    for pair in pairs:
        all_checks.extend(verify_pair(pair, base_dir))
    all_checks.extend(verify_parameters(metadata, pairs))

    success = print_results(all_checks, verbose=args.verbose)

    summary = (
        f"Verified {len(pairs)} pair(s) with {len(all_checks)} check(s)."
    )
    if success:
        if not args.verbose:
            print(summary)
        return 0
    print(summary, file=sys.stderr)
    return 1


if __name__ == "__main__":
    sys.exit(main())


