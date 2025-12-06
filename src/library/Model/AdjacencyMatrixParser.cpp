#include "AdjacencyMatrixParser.h"
#include "Core/FileUtils.h"
#include "Core/Exception.h"

std::pair<Graph*, Graph*> AdjacencyMatrixParser::parseFile(const QString &filename) {
    QString data = FileUtils::load(filename);
    return parseData(data);
}

std::pair<Graph*, Graph*> AdjacencyMatrixParser::parseData(const QString &data) {
    QStringList lines = data.split('\n', QString::SkipEmptyParts);

    // Remove empty lines and trim whitespace
    for(int i = 0; i < lines.size(); ++i) {
        lines[i] = lines[i].trimmed();
    }

    // Remove empty lines from the end
    while(!lines.isEmpty() && lines.last().isEmpty()) {
        lines.removeLast();
    }

    if(lines.size() < 2) {
        Exception("File must contain at least two graphs (vertex count lines)");
    }

    // Parse first graph
    auto firstGraphResult = parseSingleGraph(lines, 0, 0);
    Graph* graph1 = firstGraphResult.first;
    int nextLine = firstGraphResult.second;

    // Parse second graph
    auto secondGraphResult = parseSingleGraph(lines, nextLine, 1);
    Graph* graph2 = secondGraphResult.first;

    return std::make_pair(graph1, graph2);
}

std::pair<Graph*, int> AdjacencyMatrixParser::parseSingleGraph(const QStringList &lines, int startLine, int graphIndex) {
    if(startLine >= lines.size()) {
        Exception(QString("Unexpected end of file while parsing graph %1").arg(graphIndex + 1));
    }

    // Read number of vertices
    bool ok;
    int vertexCount = lines[startLine].toInt(&ok);
    if(!ok || vertexCount <= 0) {
        Exception(QString("Invalid vertex count '%1' for graph %2").arg(lines[startLine]).arg(graphIndex + 1));
    }

    int currentLine = startLine + 1;

    // Read adjacency matrix
    if(currentLine + vertexCount > lines.size()) {
        Exception(QString("Not enough lines for adjacency matrix of graph %1").arg(graphIndex + 1));
    }

    // Create graph
    Graph* graph = new Graph(Graph::DIRECTED);
    graph->setID(QString("graph_%1").arg(graphIndex));

    // Add vertices
    for(int i = 0; i < vertexCount; ++i) {
        Vertex* v = new Vertex();
        graph->addVertex(v, QString::number(i));
    }

    // Parse adjacency matrix and create edges
    for(int i = 0; i < vertexCount; ++i) {
        QString line = lines[currentLine + i];
        QStringList values = line.split(' ', QString::SkipEmptyParts);

        if(values.size() != vertexCount) {
            Exception(QString("Adjacency matrix row %1 of graph %2 has %3 values, expected %4")
                     .arg(i + 1).arg(graphIndex + 1).arg(values.size()).arg(vertexCount));
        }

        for(int j = 0; j < vertexCount; ++j) {
            bool ok;
            int weight = values[j].toInt(&ok);
            if(!ok) {
                Exception(QString("Invalid adjacency matrix value '%1' at position (%2,%3) in graph %4")
                         .arg(values[j]).arg(i + 1).arg(j + 1).arg(graphIndex + 1));
            }

            // For directed graphs, create edge if weight > 0
            // The spec says adjacency matrix contains 0 or 1 for graphs (not multigraphs)
            if(weight != 0) {
                if(weight != 1) {
                    Exception(QString("Adjacency matrix value %1 at position (%2,%3) in graph %4 is not 0 or 1")
                             .arg(weight).arg(i + 1).arg(j + 1).arg(graphIndex + 1));
                }

                Edge* edge = new Edge();
                edge->setOrigin(graph->getVertex(i));
                edge->setTarget(graph->getVertex(j));
                graph->addEdge(edge);

                // Connect the edge to vertices
                graph->getVertex(i)->addEdge(edge, Vertex::EDGE_OUT);
                graph->getVertex(j)->addEdge(edge, Vertex::EDGE_IN);
            }
        }
    }

    return std::make_pair(graph, currentLine + vertexCount);
}
