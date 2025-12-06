#ifndef GEMPP_ADJACENCYMATRIXPARSER_H
#define GEMPP_ADJACENCYMATRIXPARSER_H

#include <utility>
#include "Graph.h"

/**
 * @brief The AdjacencyMatrixParser class provides functionality to parse
 * text files containing adjacency matrices for two graphs in the format
 * specified for graph matching competitions.
 *
 * Format:
 * - First line: number of vertices for first graph
 * - Following lines: adjacency matrix rows for first graph
 * - Next line: number of vertices for second graph
 * - Following lines: adjacency matrix rows for second graph
 * - Optional additional data may follow
 */
class DLL_EXPORT AdjacencyMatrixParser {
public:
    /**
     * @brief Parses a file containing two graphs in adjacency matrix format.
     * @param filename the path to the input file
     * @return a pair of graphs (query, target)
     */
    static std::pair<Graph*, Graph*> parseFile(const QString &filename);

    /**
     * @brief Parses adjacency matrix data from a string.
     * @param data the string containing the adjacency matrix data
     * @return a pair of graphs (query, target)
     */
    static std::pair<Graph*, Graph*> parseData(const QString &data);

private:
    /**
     * @brief Parses a single graph from adjacency matrix format.
     * @param lines the lines containing the adjacency matrix data
     * @param startLine the starting line index
     * @param graphIndex index for naming (0 for first graph, 1 for second)
     * @return the parsed graph and the next line index
     */
    static std::pair<Graph*, int> parseSingleGraph(const QStringList &lines, int startLine, int graphIndex);
};

#endif /* GEMPP_ADJACENCYMATRIXPARSER_H */
