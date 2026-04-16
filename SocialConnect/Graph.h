#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <stack>
#include <set>
#include <algorithm>
#include <limits>
#include <iomanip>

/**
 * @brief Represents the types of connections in the social network.
 */
enum class GraphMode {
    UNDIRECTED, // Friendship (Mutual)
    DIRECTED    // Follow (One-way)
};

/**
 * @brief Graph class to manage users (nodes) and their connections (edges).
 * Uses STL containers: unordered_map for Adjacency List, vector for Adjacency Matrix.
 */
class Graph {
private:
    // Adjacency List: key = user name, value = list of pairs (neighbor name, weight)
    std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> adjList;
    GraphMode mode;

public:
    Graph(GraphMode m = GraphMode::UNDIRECTED);

    // Core Operations
    void addNode(const std::string& name);
    void addEdge(const std::string& src, const std::string& dest, int weight = 1);
    void removeNode(const std::string& name);
    void removeEdge(const std::string& src, const std::string& dest);
    
    // Getters
    const std::unordered_map<std::string, std::vector<std::pair<std::string, int>>>& getAdjList() const;
    std::vector<std::string> getNodes() const;
    GraphMode getMode() const;
    void setMode(GraphMode m);

    // Algorithms (with terminal trace logic)
    void bfs(const std::string& startNode);
    void dfs(const std::string& startNode);
    void dijkstra(const std::string& startNode, const std::string& targetNode);
    void primMST();

    // Matrix Representation
    void displayAdjacencyMatrix() const;
};

#endif // GRAPH_H
