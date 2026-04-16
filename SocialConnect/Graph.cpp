#include "Graph.h"
#include "Visualizer.h"
#include <iostream>
#include <queue>
#include <stack>
#include <algorithm>
#include <set>
#include <map>

Graph::Graph(GraphMode m) : mode(m) {}

void Graph::addNode(const std::string& name) {
    Visualizer::printHeader("ADD NODE");
    if (adjList.find(name) == adjList.end()) {
        adjList[name] = {};
        Visualizer::printSuccess("> Adding user: " + name);
        Visualizer::printInfo("> Current graph size: " + std::to_string(adjList.size()) + " node(s)");
    } else {
        Visualizer::printError("> User " + name + " already exists!");
    }
    Visualizer::displayAdjList(*this);
}

void Graph::addEdge(const std::string& src, const std::string& dest, int weight) {
    Visualizer::printHeader("ADD CONNECTION");
    if (adjList.find(src) == adjList.end() || adjList.find(dest) == adjList.end()) {
        Visualizer::printError("> One or both users do not exist!");
        return;
    }

    // Check if edge already exists
    for (const auto& edge : adjList[src]) {
        if (edge.first == dest) {
            Visualizer::printError("> Connection already exists!");
            return;
        }
    }

    adjList[src].push_back({dest, weight});
    Visualizer::printSuccess("> Added edge: " + src + " -> " + dest + " (Weight: " + std::to_string(weight) + ")");
    
    if (mode == GraphMode::UNDIRECTED) {
        adjList[dest].push_back({src, weight});
        Visualizer::printSuccess("> Added reverse edge (Undirected): " + dest + " -> " + src);
    }

    Visualizer::displayAdjList(*this);
}

void Graph::removeNode(const std::string& name) {
    Visualizer::printHeader("REMOVE NODE");
    if (adjList.erase(name)) {
        // Remove all edges pointing to this node
        for (auto& pair : adjList) {
            auto& neighbors = pair.second;
            neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(),
                [&name](const std::pair<std::string, int>& edge) {
                    return edge.first == name;
                }), neighbors.end());
        }
        Visualizer::printSuccess("> Removed user: " + name);
    } else {
        Visualizer::printError("> User not found!");
    }
    Visualizer::displayAdjList(*this);
}

void Graph::removeEdge(const std::string& src, const std::string& dest) {
    Visualizer::printHeader("REMOVE CONNECTION");
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
        Visualizer::printSuccess("> Removed connection between " + src + " and " + dest);
    } else {
        Visualizer::printError("> Source node not found!");
    }
    Visualizer::displayAdjList(*this);
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
    Visualizer::printHeader("BFS TRAVERSAL from " + startNode);
    if (adjList.find(startNode) == adjList.end()) {
        Visualizer::printError("> Start node not found!");
        return;
    }

    std::queue<std::string> q;
    std::set<std::string> visited;
    std::vector<std::string> order;

    q.push(startNode);
    visited.insert(startNode);

    while (!q.empty()) {
        // Print Queue State
        std::string qState = "[";
        std::queue<std::string> tempQ = q;
        while(!tempQ.empty()) {
            qState += tempQ.front() + (tempQ.size() > 1 ? ", " : "");
            tempQ.pop();
        }
        qState += "]";
        Visualizer::printStep("> Queue: " + qState);

        std::string curr = q.front();
        q.pop();
        order.push_back(curr);
        Visualizer::printStep("> Visiting: " + curr);

        for (const auto& neighbor : adjList[curr]) {
            if (visited.find(neighbor.first) == visited.end()) {
                visited.insert(neighbor.first);
                q.push(neighbor.first);
            }
        }
    }

    std::string result = "";
    for (size_t i = 0; i < order.size(); ++i) {
        result += order[i] + (i < order.size() - 1 ? " -> " : "");
    }
    Visualizer::printSuccess("> BFS Order: " + result);
    Visualizer::printHeader("END BFS");
}

void Graph::dfs(const std::string& startNode) {
    Visualizer::printHeader("DFS TRAVERSAL from " + startNode);
    if (adjList.find(startNode) == adjList.end()) {
        Visualizer::printError("> Start node not found!");
        return;
    }

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
            Visualizer::printStep("> Visiting: " + curr);

            // Print stack state (using reversed neighbors for standard DFS order)
            auto neighbors = adjList[curr];
            std::reverse(neighbors.begin(), neighbors.end());
            for (const auto& neighbor : neighbors) {
                if (visited.find(neighbor.first) == visited.end()) {
                    s.push(neighbor.first);
                }
            }
            
            std::string sState = "[";
            std::stack<std::string> tempS = s;
            std::vector<std::string> sItems;
            while(!tempS.empty()) { sItems.push_back(tempS.top()); tempS.pop(); }
            std::reverse(sItems.begin(), sItems.end());
            for(size_t i=0; i<sItems.size(); ++i) sState += sItems[i] + (i < sItems.size()-1 ? ", " : "");
            sState += "]";
            Visualizer::printStep("> Stack after pushing neighbors: " + sState);
        }
    }

    std::string result = "";
    for (size_t i = 0; i < order.size(); ++i) {
        result += order[i] + (i < order.size() - 1 ? " -> " : "");
    }
    Visualizer::printSuccess("> DFS Order: " + result);
    Visualizer::printHeader("END DFS");
}

void Graph::dijkstra(const std::string& startNode, const std::string& targetNode) {
    Visualizer::printHeader("DIJKSTRA'S ALGORITHM: " + startNode + " to " + targetNode);
    if (adjList.find(startNode) == adjList.end() || adjList.find(targetNode) == adjList.end()) {
        Visualizer::printError("> One or both nodes not found!");
        return;
    }

    std::unordered_map<std::string, int> dist;
    std::unordered_map<std::string, std::string> parent;
    for (auto const& [name, neighbors] : adjList) dist[name] = std::numeric_limits<int>::max();

    using NodeDist = std::pair<int, std::string>;
    std::priority_queue<NodeDist, std::vector<NodeDist>, std::greater<NodeDist>> pq;

    dist[startNode] = 0;
    pq.push({0, startNode});

    while (!pq.empty()) {
        int d = pq.top().first;
        std::string u = pq.top().second;
        pq.pop();

        if (d > dist[u]) continue;
        Visualizer::printStep("> Relaxing neighbors of " + u + " (Current Distance: " + std::to_string(d) + ")");

        for (auto const& edge : adjList[u]) {
            std::string v = edge.first;
            int weight = edge.second;

            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
                pq.push({dist[v], v});
                Visualizer::printStep("  - Updated " + v + " | Dist: " + std::to_string(dist[v]) + " | Prev: " + u);
            }
        }
    }

    if (dist[targetNode] == std::numeric_limits<int>::max()) {
        Visualizer::printError("> No path found between " + startNode + " and " + targetNode);
    } else {
        std::vector<std::string> path;
        for (std::string at = targetNode; at != ""; at = parent[at]) {
            path.push_back(at);
            if (at == startNode) break;
        }
        std::reverse(path.begin(), path.end());

        std::string pathStr = "";
        for (size_t i = 0; i < path.size(); ++i) pathStr += path[i] + (i < path.size() - 1 ? " -> " : "");
        Visualizer::printSuccess("> Shortest Path found: " + pathStr);
        Visualizer::printSuccess("> Total Connection Strength (Weight): " + std::to_string(dist[targetNode]));
    }
    Visualizer::printHeader("END DIJKSTRA");
}

void Graph::primMST() {
    Visualizer::printHeader("PRIM'S ALGORITHM - MINIMUM SPANNING TREE");
    if (adjList.empty()) {
        Visualizer::printError("> Graph is empty!");
        return;
    }

    std::string start = adjList.begin()->first;
    Visualizer::printInfo("> Starting Prim's from " + start);

    std::unordered_map<std::string, bool> inMST;
    using Edge = std::pair<int, std::pair<std::string, std::string>>; // weight, {u, v}
    std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>> pq;

    inMST[start] = true;
    for (auto const& edge : adjList[start]) {
        pq.push({edge.second, {start, edge.first}});
    }

    int totalWeight = 0;
    std::vector<std::pair<std::string, std::string>> mstEdges;

    while (!pq.empty() && mstEdges.size() < adjList.size() - 1) {
        int weight = pq.top().first;
        std::string u = pq.top().second.first;
        std::string v = pq.top().second.second;
        pq.pop();

        if (inMST[v]) continue;

        inMST[v] = true;
        totalWeight += weight;
        mstEdges.push_back({u, v});
        Visualizer::printStep("> Added edge to MST: " + u + " --(" + std::to_string(weight) + ")-- " + v);

        for (auto const& edge : adjList[v]) {
            if (!inMST[edge.first]) {
                pq.push({edge.second, {v, edge.first}});
            }
        }
    }

    if (mstEdges.size() < adjList.size() - 1) {
        Visualizer::printWarning("> Network is disconnected; MST is incomplete.");
    }

    Visualizer::printSuccess("> MST complete. Total Weight: " + std::to_string(totalWeight));
    Visualizer::printHeader("END PRIM'S");
}

void Graph::displayAdjacencyMatrix() const {
    Visualizer::printHeader("ADJACENCY MATRIX");
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
                if (edge.first == colNode) {
                    val = edge.second;
                    break;
                }
            }
            if (val > 0) std::cout << Visualizer::GREEN << std::setw(8) << val << Visualizer::RESET;
            else std::cout << std::setw(8) << ".";
        }
        std::cout << "\n";
    }
    Visualizer::printHeader("END MATRIX");
}
