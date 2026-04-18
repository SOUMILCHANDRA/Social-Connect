#ifndef SOCIALCONNECT_GRAPH_H
#define SOCIALCONNECT_GRAPH_H

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

struct Edge {
    std::string to;
    int weight;
};

struct MSTEdge {
    std::string from;
    std::string to;
    int weight;
};

class Graph {
public:
    explicit Graph(bool directed = false);

    void setDirected(bool isDirected);
    bool isDirected() const;

    bool addNode(const std::string& name);
    bool removeNode(const std::string& name);
    bool addEdge(const std::string& from, const std::string& to, int weight = 1);
    bool removeEdge(const std::string& from, const std::string& to);

    std::vector<std::string> bfs(const std::string& start) const;
    std::vector<std::string> dfs(const std::string& start) const;
    std::vector<std::string> dijkstra(const std::string& start, const std::string& end) const;
    std::vector<MSTEdge> prim(const std::string& start) const;

    void printAdjacencyList() const;
    void printAdjacencyMatrix() const;
    void loadSampleNetwork();
    void clear();

    std::vector<std::string> getNodes() const;
    const std::unordered_map<std::string, std::vector<Edge>>& getAdjacency() const;
    std::size_t nodeCount() const;

private:
    bool directed;
    std::unordered_map<std::string, std::vector<Edge>> adjacency;

    bool hasNode(const std::string& name) const;
    bool edgeExists(const std::string& from, const std::string& to) const;
    void sortNeighbors(std::vector<Edge>& neighbors) const;
    void printDistanceTable(const std::unordered_map<std::string, int>& distances) const;
};

#endif
