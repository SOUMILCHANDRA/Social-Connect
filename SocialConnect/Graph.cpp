#include "Graph.h"
#include "Visualizer.h"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <queue>
#include <set>
#include <stack>
#include <unordered_set>
#include <sstream>

namespace {
constexpr int INF = 1e9;
}

Graph::Graph(bool directed) : directed(directed) {}

void Graph::setDirected(bool isDirected) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    directed = isDirected;
}

bool Graph::isDirected() const {
    return directed;
}

bool Graph::hasNode(const std::string& name) const {
    return adjacency.find(name) != adjacency.end();
}

bool Graph::edgeExists(const std::string& from, const std::string& to) const {
    auto iterator = adjacency.find(from);
    if (iterator == adjacency.end()) return false;
    return std::any_of(iterator->second.begin(), iterator->second.end(),
        [&](const Edge& edge) { return edge.to == to; });
}

void Graph::sortNeighbors(std::vector<Edge>& neighbors) const {
    std::sort(neighbors.begin(), neighbors.end(), [](const Edge& left, const Edge& right) {
        return left.to < right.to;
    });
}

bool Graph::addNode(const std::string& name) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (name.empty() || hasNode(name)) return false;
    adjacency[name] = {};
    return true;
}

bool Graph::removeNode(const std::string& name) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (!hasNode(name)) return false;
    adjacency.erase(name);
    for (auto& [user, neighbors] : adjacency) {
        neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(),
            [&](const Edge& edge) { return edge.to == name; }), neighbors.end());
    }
    return true;
}

bool Graph::addEdge(const std::string& from, const std::string& to, int weight) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (!hasNode(from) || !hasNode(to) || from == to) return false;
    
    if (edgeExists(from, to)) {
        for (auto& edge : adjacency[from]) if (edge.to == to) edge.weight = weight;
    } else {
        adjacency[from].push_back({to, weight});
    }
    sortNeighbors(adjacency[from]);

    if (!directed) {
        if (edgeExists(to, from)) {
            for (auto& edge : adjacency[to]) if (edge.to == from) edge.weight = weight;
        } else {
            adjacency[to].push_back({from, weight});
        }
        sortNeighbors(adjacency[to]);
    }
    return true;
}

bool Graph::removeEdge(const std::string& from, const std::string& to) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (!hasNode(from) || !hasNode(to)) return false;

    auto removeForward = [&](const std::string& src, const std::string& tgt) {
        auto& neighbors = adjacency[src];
        auto originalSize = neighbors.size();
        neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(),
            [&](const Edge& edge) { return edge.to == tgt; }), neighbors.end());
        return originalSize != neighbors.size();
    };

    bool removed = removeForward(from, to);
    if (!directed) removeForward(to, from);
    return removed;
}

BFSResult Graph::BFS(std::string start) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    BFSResult result;
    if (!hasNode(start)) return result;

    std::queue<std::string> q;
    std::unordered_set<std::string> visited;
    q.push(start);
    visited.insert(start);
    int stepNum = 0;

    while (!q.empty()) {
        BFSStep step;
        step.node = q.front();
        
        std::queue<std::string> tempQ = q;
        tempQ.pop(); // Show queue AFTER this node is processed
        while (!tempQ.empty()) { step.queue.push_back(tempQ.front()); tempQ.pop(); }
        result.steps.push_back(step);

        std::string curr = q.front();
        q.pop();
        result.order.push_back(curr);

        Visualizer::printBFSStep(++stepNum, curr, step.queue, result.order);

        for (const auto& edge : adjacency.at(curr)) {
            if (visited.find(edge.to) == visited.end()) {
                visited.insert(edge.to);
                q.push(edge.to);
            }
        }
    }
    Visualizer::printBFSComplete(result.order);
    return result;
}

DFSResult Graph::DFS(std::string start) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    DFSResult result;
    if (!hasNode(start)) return result;

    std::vector<std::string> stack = {start};
    std::unordered_set<std::string> visited;
    int stepNum = 0;

    while (!stack.empty()) {
        std::string curr = stack.back();
        stack.pop_back();

        if (visited.find(curr) == visited.end()) {
            visited.insert(curr);
            result.order.push_back(curr);

            DFSStep step;
            step.node = curr;
            step.stack = stack;
            result.steps.push_back(step);

            Visualizer::printDFSStep(++stepNum, curr, step.stack, result.order);

            const auto& neighbors = adjacency.at(curr);
            for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
                if (visited.find(it->to) == visited.end()) {
                    stack.push_back(it->to);
                }
            }
        }
    }
    Visualizer::printDFSComplete(result.order);
    return result;
}

DijkstraResult Graph::Dijkstra(std::string start, std::string end) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    DijkstraResult result;
    if (!hasNode(start) || !hasNode(end)) return result;

    std::unordered_map<std::string, int> dists;
    std::unordered_map<std::string, std::string> parent;
    for (const auto& node : adjacency) dists[node.first] = INF;
    dists[start] = 0;

    using pii = std::pair<int, std::string>;
    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;
    pq.push({0, start});

    int stepNum = 0;
    while (!pq.empty()) {
        std::string u = pq.top().second;
        int d = pq.top().first;
        pq.pop();

        if (d > dists[u]) continue;

        for (const auto& edge : adjacency.at(u)) {
            bool updated = false;
            if (dists[u] + edge.weight < dists[edge.to]) {
                dists[edge.to] = dists[u] + edge.weight;
                parent[edge.to] = u;
                pq.push({dists[edge.to], edge.to});
                updated = true;
            }
            DijkstraStep step = {u, edge.to, dists[edge.to], updated};
            result.steps.push_back(step);
            Visualizer::printDijkstraStep(++stepNum, u, edge.to, dists[edge.to], updated);
        }
    }

    if (dists[end] != INF) {
        result.cost = dists[end];
        for (std::string v = end; !v.empty(); v = parent[v]) {
            result.path.push_back(v);
            if (v == start) break;
        }
        std::reverse(result.path.begin(), result.path.end());
    } else {
        result.cost = -1;
    }
    
    Visualizer::printDijkstraComplete(result.path, result.cost);
    return result;
}

PrimsResult Graph::Prims(std::string start) {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    PrimsResult result;
    result.totalCost = 0;
    if (!hasNode(start)) return result;

    std::unordered_set<std::string> inMST = {start};
    using pii = std::pair<int, std::pair<std::string, std::string>>;
    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;

    for (const auto& edge : adjacency.at(start)) pq.push({edge.weight, {start, edge.to}});

    int stepNum = 0;
    while (!pq.empty() && inMST.size() < adjacency.size()) {
        auto top = pq.top(); pq.pop();
        int w = top.first;
        std::string u = top.second.first;
        std::string v = top.second.second;

        if (inMST.count(v)) continue;

        inMST.insert(v);
        result.totalCost += w;
        result.edges.push_back({u, v, w});
        Visualizer::printMSTEdge(++stepNum, u, v, w, result.totalCost);

        for (const auto& edge : adjacency.at(v)) {
            if (!inMST.count(edge.to)) pq.push({edge.weight, {v, edge.to}});
        }
    }
    Visualizer::printMSTComplete(result.totalCost);
    return result;
}

std::string Graph::toJson() {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    std::stringstream ss;
    ss << "{\"nodes\":[";
    bool first = true;
    for (auto const& [id, _] : adjacency) {
        if (!first) ss << ",";
        ss << "{\"id\":\"" << id << "\"}";
        first = false;
    }
    ss << "],\"links\":[";
    first = true;
    for (auto const& [u, neighbors] : adjacency) {
        for (auto const& edge : neighbors) {
            if (!directed && u > edge.to) continue;
            if (!first) ss << ",";
            ss << "{\"source\":\"" << u << "\",\"target\":\"" << edge.to << "\",\"weight\":" << edge.weight << "}";
            first = false;
        }
    }
    ss << "],\"directed\":" << (directed ? "true" : "false") << "}";
    return ss.str();
}

void Graph::loadSample() {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    clearAll();
    std::vector<std::string> users = {"Aarav", "Ishani", "Rohan", "Priya", "Arjun", "Sneha", "Vihaan", "Ananya"};
    for (const auto& u : users) addNode(u);
    addEdge("Aarav", "Ishani", 2); addEdge("Aarav", "Rohan", 4);
    addEdge("Ishani", "Priya", 3); addEdge("Rohan", "Priya", 1);
    addEdge("Rohan", "Arjun", 5); addEdge("Priya", "Sneha", 2);
    addEdge("Arjun", "Vihaan", 3); addEdge("Sneha", "Ananya", 4);
}

void Graph::clearAll() {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    adjacency.clear();
}

int Graph::nodeCount() const {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    return (int)adjacency.size();
}

std::vector<std::string> Graph::getNodes() const {
    std::lock_guard<std::recursive_mutex> lock(mtx);
    std::vector<std::string> nodes;
    for (const auto& [id, _] : adjacency) nodes.push_back(id);
    std::sort(nodes.begin(), nodes.end());
    return nodes;
}

void Graph::printAdjacencyList() const {
    for (const auto& [u, neighbors] : adjacency) {
        std::cout << "  " << u << " -> ";
        for (const auto& e : neighbors) std::cout << e.to << "(" << e.weight << ") ";
        std::cout << "\n";
    }
}
