#ifndef SOCIALCONNECT_GRAPH_H
#define SOCIALCONNECT_GRAPH_H

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

class HttpServer;

struct Edge {
    std::string to;
    int weight;
};

struct BFSStep {
    std::string node;
    std::vector<std::string> queue;
};

struct DFSStep {
    std::string node;
    std::vector<std::string> stack;
};

struct DijkstraStep {
    std::string u, v;
    int newDist;
    bool updated;
};

struct MSTEdge {
    std::string u, v;
    int weight;
};

struct BFSResult {
    std::vector<std::string> order;
    std::vector<BFSStep> steps;
};

struct DFSResult {
    std::vector<std::string> order;
    std::vector<DFSStep> steps;
};

struct DijkstraResult {
    std::vector<std::string> path;
    int cost;
    std::vector<DijkstraStep> steps;
};

struct PrimsResult {
    int totalCost;
    std::vector<MSTEdge> edges;
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

    BFSResult BFS(std::string start);
    DFSResult DFS(std::string start);
    DijkstraResult Dijkstra(std::string start, std::string end);
    PrimsResult Prims(std::string start);

    void printAdjacencyList() const;
    void printAdjacencyMatrix() const;
    void loadSample();
    void clearAll();

    std::vector<std::string> getNodes() const;
    std::string toJson();
    int nodeCount() const;

    std::recursive_mutex& getMutex() { return mtx; }

private:
    bool directed;
    std::unordered_map<std::string, std::vector<Edge>> adjacency;
    mutable std::recursive_mutex mtx;

    bool hasNode(const std::string& name) const;
    bool edgeExists(const std::string& from, const std::string& to) const;
    void sortNeighbors(std::vector<Edge>& neighbors) const;
    void printDistanceTable(const std::unordered_map<std::string, int>& distances) const;
};

#endif
