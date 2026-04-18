#include "Graph.h"
#include "Visualizer.h"
#include <iostream>
#include <queue>
#include <stack>
#include <algorithm>
#include <set>
#include <map>
#include <limits>

Graph::Graph(GraphMode m) : mode(m) {}

void Graph::addNode(const std::string& name) {
    if (adjList.find(name) == adjList.end()) {
        adjList[name] = {};
        
        std::cout << CYAN << BOLD << "\n═══ ADD NODE ═══" << RESET << "\n";
        std::cout << GREEN << "› Adding user: " << RESET << name << "\n";
        std::cout << "› Current graph size: " << adjList.size() << " node(s)\n";
        std::cout << "› Adjacency List:\n";
        Visualizer::displayAdjList(*this);
    }
}

void Graph::addEdge(const std::string& src, const std::string& dest, int weight) {
    if (adjList.find(src) == adjList.end() || adjList.find(dest) == adjList.end()) {
        return;
    }

    // Check if edge already exists
    for (const auto& edge : adjList[src]) {
        if (edge.first == dest) return;
    }

    adjList[src].push_back({dest, weight});
    
    std::cout << CYAN << BOLD << "\n═══ ADD CONNECTION ═══" << RESET << "\n";
    std::cout << "› Added edge: " << src << " → " << dest << " (Weight: " << weight << ")\n";
    
    if (mode == GraphMode::UNDIRECTED) {
        adjList[dest].push_back({src, weight});
        std::cout << "› Added reverse edge (Undirected): " << dest << " → " << src << "\n";
    }

    Visualizer::displayAdjList(*this);
}

void Graph::removeNode(const std::string& name) {
    if (adjList.erase(name)) {
        for (auto& pair : adjList) {
            auto& neighbors = pair.second;
            neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(),
                [&name](const std::pair<std::string, int>& edge) {
                    return edge.first == name;
                }), neighbors.end());
        }
        std::cout << CYAN << BOLD << "\n═══ REMOVE NODE ═══" << RESET << "\n";
        std::cout << RED << "› Removed user: " << RESET << name << "\n";
        Visualizer::displayAdjList(*this);
    }
}

void Graph::removeEdge(const std::string& src, const std::string& dest) {
    if (adjList.find(src) != adjList.end()) {
        auto& neighbors = adjList[src];
        neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(),
            [&dest](const std::pair<std::string, int>& edge) {
                return edge.first == dest;
            }), neighbors.end());
        
        if (mode == GraphMode::UNDIRECTED && adjList.find(dest) != adjList.end()) {
            auto& dNeighbors = adjList[dest];
            dNeighbors.erase(std::remove_if(dNeighbors.begin(), dNeighbors.end(),
                [&src](const std::pair<std::string, int>& edge) {
                    return edge.first == src;
                }), dNeighbors.end());
        }
        std::cout << CYAN << BOLD << "\n═══ REMOVE CONNECTION ═══" << RESET << "\n";
        std::cout << RED << "› Removed edge: " << RESET << src << " → " << dest << "\n";
        Visualizer::displayAdjList(*this);
    }
}

const std::unordered_map<std::string, std::vector<std::pair<std::string, int>>>& Graph::getAdjList() const {
    return adjList;
}

std::vector<std::string> Graph::getNodes() const {
    std::vector<std::string> nodes;
    for (const auto& pair : adjList) nodes.push_back(pair.first);
    return nodes;
}

GraphMode Graph::getMode() const { return mode; }
void Graph::setMode(GraphMode m) { mode = m; }

void Graph::bfs(const std::string& startNode) {
    if (adjList.find(startNode) == adjList.end()) return;

    std::cout << CYAN << BOLD << "\n═══ BFS TRAVERSAL from " << startNode << " ═══" << RESET << "\n";

    std::queue<std::string> q;
    std::set<std::string> visited;
    std::vector<std::string> order;

    q.push(startNode);
    visited.insert(startNode);

    while (!q.empty()) {
        std::string qContents = "";
        std::queue<std::string> tempQ = q;
        while(!tempQ.empty()) {
            qContents += tempQ.front() + (tempQ.size() > 1 ? ", " : "");
            tempQ.pop();
        }

        std::string curr = q.front();
        q.pop();
        order.push_back(curr);

        std::cout << "› Queue: [" << qContents << "] | Visiting: " << curr << "\n";

        for (const auto& neighbor : adjList[curr]) {
            if (visited.find(neighbor.first) == visited.end()) {
                visited.insert(neighbor.first);
                q.push(neighbor.first);
            }
        }
    }

    std::string orderStr = "";
    for (size_t i = 0; i < order.size(); ++i) {
        orderStr += order[i] + (i < order.size() - 1 ? " → " : "");
    }
    std::cout << GREEN << "› BFS Order: " << orderStr << RESET << "\n";
}

void Graph::dfs(const std::string& startNode) {
    if (adjList.find(startNode) == adjList.end()) return;

    std::cout << CYAN << BOLD << "\n═══ DFS TRAVERSAL from " << startNode << " ═══" << RESET << "\n";

    std::stack<std::string> s;
    std::set<std::string> visited;
    std::vector<std::string> order;

    s.push(startNode);

    while (!s.empty()) {
        std::string curr = s.top();
        s.pop();

        if (visited.find(curr) == visited.end()) {
            visited.insert(curr);
            order.push_back(curr);

            std::string neighborsStr = "";
            auto neighbors = adjList[curr];
            for(size_t i=0; i<neighbors.size(); ++i) 
                neighborsStr += neighbors[i].first + (i < neighbors.size()-1 ? ", " : "");

            std::cout << "› Stack top: " << curr << " | Neighbors: " << neighborsStr << "\n";

            std::reverse(neighbors.begin(), neighbors.end());
            for (const auto& neighbor : neighbors) {
                if (visited.find(neighbor.first) == visited.end()) {
                    s.push(neighbor.first);
                }
            }
        }
    }

    std::string orderStr = "";
    for (size_t i = 0; i < order.size(); ++i) {
        orderStr += order[i] + (i < order.size() - 1 ? " → " : "");
    }
    std::cout << GREEN << "› DFS Order: " << orderStr << RESET << "\n";
}

void Graph::dijkstra(const std::string& startNode, const std::string& targetNode) {
    if (adjList.find(startNode) == adjList.end()) return;

    std::cout << CYAN << BOLD << "\n═══ DIJKSTRA: " << startNode << " → " << targetNode << " ═══" << RESET << "\n";

    std::unordered_map<std::string, int> dist;
    std::unordered_map<std::string, std::string> parent;
    for (auto const& pair : adjList) dist[pair.first] = std::numeric_limits<int>::max();

    std::cout << "› Initial distances: All ∞, dist[" << startNode << "] = 0\n";

    using NodeDist = std::pair<int, std::string>;
    std::priority_queue<NodeDist, std::vector<NodeDist>, std::greater<NodeDist>> pq;

    dist[startNode] = 0;
    pq.push({0, startNode});

    while (!pq.empty()) {
        int d = pq.top().first;
        std::string u = pq.top().second;
        pq.pop();

        if (d > dist[u]) continue;

        for (auto const& edge : adjList[u]) {
            std::string v = edge.first;
            int weight = edge.second;
            int newDist = dist[u] + weight;

            bool updated = false;
            std::cout << "› Relaxing " << u << "→" << v << " | dist[" << v << "] = " << newDist;
            if (newDist < dist[v]) {
                dist[v] = newDist;
                parent[v] = u;
                pq.push({dist[v], v});
                updated = true;
                std::cout << GREEN << " ✓ UPDATED" << RESET << "\n";
            } else {
                std::cout << RED << " ✗ kept" << RESET << "\n";
            }
        }
    }

    std::cout << "\n› Final Distance Table:\n";
    for(auto const& pair : dist) {
        std::cout << "  " << pair.first << ": " << (pair.second == std::numeric_limits<int>::max() ? "∞" : std::to_string(pair.second)) << "\n";
    }

    if (targetNode != "" && dist.find(targetNode) != dist.end() && dist[targetNode] != std::numeric_limits<int>::max()) {
        std::vector<std::string> path;
        for (std::string at = targetNode; at != ""; at = parent[at]) {
            path.push_back(at);
            if (at == startNode) break;
        }
        std::reverse(path.begin(), path.end());
        std::cout << GREEN << "› Shortest Path: ";
        for(size_t i=0; i<path.size(); ++i) std::cout << path[i] << (i<path.size()-1 ? " → " : "");
        std::cout << " (Cost: " << dist[targetNode] << ")" << RESET << "\n";
    }
}

void Graph::primMST() {
    if (adjList.empty()) return;

    std::string start = adjList.begin()->first;
    std::cout << CYAN << BOLD << "\n═══ PRIM'S MST from " << start << " ═══" << RESET << "\n";

    std::unordered_map<std::string, bool> inMST;
    using Edge = std::pair<int, std::pair<std::string, std::string>>;
    std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>> pq;

    inMST[start] = true;
    for (auto const& edge : adjList[start]) pq.push({edge.second, {start, edge.first}});

    int totalCost = 0;
    int edgesCount = 0;

    while (!pq.empty() && edgesCount < (int)adjList.size() - 1) {
        int w = pq.top().first;
        std::string u = pq.top().second.first;
        std::string v = pq.top().second.second;
        pq.pop();

        if (inMST[v]) continue;

        inMST[v] = true;
        totalCost += w;
        edgesCount++;
        std::cout << "› Added: " << u << " — " << v << " (weight: " << w << ") | Total so far: " << totalCost << "\n";

        for (auto const& edge : adjList[v]) {
            if (!inMST[edge.first]) pq.push({edge.second, {v, edge.first}});
        }
    }

    std::cout << GREEN << "› MST Complete. Total Weight: " << totalCost << RESET << "\n";
}

void Graph::displayAdjacencyMatrix() const {
    std::cout << CYAN << BOLD << "\n═══ ADJACENCY MATRIX ═══" << RESET << "\n";
    auto nodes = getNodes();
    std::sort(nodes.begin(), nodes.end());

    std::cout << "      ";
    for (const auto& n : nodes) std::cout << std::setw(8) << n;
    std::cout << "\n";

    for (const auto& rowNode : nodes) {
        std::cout << std::setw(6) << rowNode << " ";
        for (const auto& colNode : nodes) {
            int val = 0;
            for (const auto& edge : adjList.at(rowNode)) {
                if (edge.first == colNode) { val = edge.second; break; }
            }
            if (val > 0) std::cout << GREEN << std::setw(8) << val << RESET;
            else std::cout << std::setw(8) << ".";
        }
        std::cout << "\n";
    }
}
