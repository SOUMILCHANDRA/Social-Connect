#include "Graph.h"
#include "Visualizer.h"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <queue>
#include <set>
#include <unordered_set>

namespace {
constexpr int INF = std::numeric_limits<int>::max() / 4;
}

Graph::Graph(bool directed) : directed(directed) {
    Visualizer::debug("Graph constructor reached");
}

void Graph::setDirected(bool isDirected) {
    Visualizer::debug("setDirected reached");
    directed = isDirected;
    Visualizer::header("SET MODE");
    Visualizer::info(std::string("Graph mode set to: ") + (directed ? "DIRECTED" : "UNDIRECTED"));
    Visualizer::divider();
}

bool Graph::isDirected() const {
    return directed;
}

bool Graph::hasNode(const std::string& name) const {
    return adjacency.find(name) != adjacency.end();
}

bool Graph::edgeExists(const std::string& from, const std::string& to) const {
    auto iterator = adjacency.find(from);
    if (iterator == adjacency.end()) {
        return false;
    }
    return std::any_of(iterator->second.begin(), iterator->second.end(),
        [&](const Edge& edge) { return edge.to == to; });
}

void Graph::sortNeighbors(std::vector<Edge>& neighbors) const {
    std::sort(neighbors.begin(), neighbors.end(), [](const Edge& left, const Edge& right) {
        return left.to < right.to;
    });
}

bool Graph::addNode(const std::string& name) {
    Visualizer::debug("addNode reached");

    if (name.empty()) {
        Visualizer::header("ADD NODE");
        Visualizer::error("User name cannot be empty.");
        Visualizer::divider();
        return false;
    }

    if (hasNode(name)) {
        Visualizer::header("ADD NODE");
        Visualizer::warn("User already exists: " + name);
        Visualizer::divider();
        return false;
    }

    adjacency[name] = {};

    Visualizer::header("ADD NODE");
    std::cout << GREEN << "› Adding user: " << RESET << name << "\n";
    std::cout << "› Current graph size: " << adjacency.size() << " node(s)\n";
    std::cout << "› Adjacency List:\n";
    printAdjacencyList();
    Visualizer::divider();
    return true;
}

bool Graph::removeNode(const std::string& name) {
    Visualizer::debug("removeNode reached");
    Visualizer::header("REMOVE NODE");

    if (!hasNode(name)) {
        Visualizer::error("User not found: " + name);
        Visualizer::divider();
        return false;
    }

    adjacency.erase(name);

    for (auto& [user, neighbors] : adjacency) {
        neighbors.erase(
            std::remove_if(neighbors.begin(), neighbors.end(),
                [&](const Edge& edge) { return edge.to == name; }),
            neighbors.end()
        );
    }

    Visualizer::success("Removed user: " + name);
    Visualizer::info("Current graph size: " + std::to_string(adjacency.size()) + " node(s)");
    Visualizer::info("Adjacency List:");
    printAdjacencyList();
    Visualizer::divider();
    return true;
}

bool Graph::addEdge(const std::string& from, const std::string& to, int weight) {
    Visualizer::debug("addEdge reached");
    Visualizer::header("ADD CONNECTION");

    if (!hasNode(from) || !hasNode(to)) {
        Visualizer::error("Both users must exist before adding a connection.");
        Visualizer::divider();
        return false;
    }

    if (from == to) {
        Visualizer::error("Self connections are not allowed.");
        Visualizer::divider();
        return false;
    }

    if (weight < 1 || weight > 10) {
        Visualizer::warn("Weight out of range. Clamping to 1-10.");
        weight = std::max(1, std::min(10, weight));
    }

    if (edgeExists(from, to)) {
        for (auto& edge : adjacency[from]) {
            if (edge.to == to) {
                edge.weight = weight;
            }
        }
    } else {
        adjacency[from].push_back({to, weight});
    }
    sortNeighbors(adjacency[from]);

    std::cout << "› Added edge: " << from << " → " << to << " (Weight: " << weight << ")\n";

    if (!directed) {
        if (edgeExists(to, from)) {
            for (auto& edge : adjacency[to]) {
                if (edge.to == from) {
                    edge.weight = weight;
                }
            }
        } else {
            adjacency[to].push_back({from, weight});
        }
        sortNeighbors(adjacency[to]);
        std::cout << "› Added reverse edge (Undirected): " << to << " → " << from << "\n";
    }

    std::cout << "› Adjacency List:\n";
    printAdjacencyList();
    Visualizer::divider();
    return true;
}

bool Graph::removeEdge(const std::string& from, const std::string& to) {
    Visualizer::debug("removeEdge reached");
    Visualizer::header("REMOVE CONNECTION");

    if (!hasNode(from) || !hasNode(to)) {
        Visualizer::error("Both users must exist.");
        Visualizer::divider();
        return false;
    }

    auto removeForward = [&](const std::string& source, const std::string& target) {
        auto& neighbors = adjacency[source];
        auto originalSize = neighbors.size();
        neighbors.erase(
            std::remove_if(neighbors.begin(), neighbors.end(),
                [&](const Edge& edge) { return edge.to == target; }),
            neighbors.end()
        );
        return originalSize != neighbors.size();
    };

    bool removed = removeForward(from, to);
    if (!directed) {
        removeForward(to, from);
    }

    if (!removed) {
        Visualizer::warn("Connection not found.");
        Visualizer::divider();
        return false;
    }

    Visualizer::success("Removed connection: " + from + " -> " + to);
    Visualizer::info("Adjacency List:");
    printAdjacencyList();
    Visualizer::divider();
    return true;
}

void Graph::printAdjacencyList() const {
    Visualizer::debug("printAdjacencyList reached");
    std::vector<std::string> users = getNodes();
    for (const auto& user : users) {
        std::cout << "  " << user << " -> [";
        const auto& neighbors = adjacency.at(user);
        for (std::size_t index = 0; index < neighbors.size(); ++index) {
            std::cout << neighbors[index].to << "(" << neighbors[index].weight << ")";
            if (index + 1 < neighbors.size()) {
                std::cout << ", ";
            }
        }
        std::cout << "]\n";
    }
    std::cout.flush();
}

void Graph::printAdjacencyMatrix() const {
    Visualizer::debug("printAdjacencyMatrix reached");
    Visualizer::header("ADJACENCY MATRIX");

    std::vector<std::string> users = getNodes();
    if (users.empty()) {
        Visualizer::warn("Graph is empty.");
        Visualizer::divider();
        return;
    }

    std::cout << std::setw(10) << " ";
    for (const auto& user : users) {
        std::cout << std::setw(10) << user;
    }
    std::cout << "\n";

    for (const auto& rowUser : users) {
        std::cout << std::setw(10) << rowUser;
        for (const auto& colUser : users) {
            int value = 0;
            for (const auto& edge : adjacency.at(rowUser)) {
                if (edge.to == colUser) {
                    value = edge.weight;
                    break;
                }
            }
            std::cout << std::setw(10) << value;
        }
        std::cout << "\n";
    }

    Visualizer::divider();
}

std::vector<std::string> Graph::bfs(const std::string& start) const {
    Visualizer::debug("bfs reached");
    Visualizer::header("BFS TRAVERSAL from " + start);

    if (!hasNode(start)) {
        Visualizer::error("Start node not found: " + start);
        Visualizer::divider();
        return {};
    }

    std::queue<std::string> queue;
    std::unordered_set<std::string> visited;
    std::vector<std::string> order;

    queue.push(start);
    visited.insert(start);

    while (!queue.empty()) {
        std::queue<std::string> snapshot = queue;
        std::vector<std::string> queueItems;
        while (!snapshot.empty()) {
            queueItems.push_back(snapshot.front());
            snapshot.pop();
        }

        std::string current = queue.front();
        queue.pop();
        order.push_back(current);

        std::cout << "› Queue: [" << Visualizer::join(queueItems, ", ") << "] | Visiting: " << current << "\n";

        std::vector<Edge> neighbors = adjacency.at(current);
        std::sort(neighbors.begin(), neighbors.end(), [](const Edge& left, const Edge& right) {
            return left.to < right.to;
        });

        for (const auto& edge : neighbors) {
            if (!visited.count(edge.to)) {
                visited.insert(edge.to);
                queue.push(edge.to);
            }
        }
    }

    std::cout << GREEN << "› BFS Order: " << Visualizer::join(order, " → ") << RESET << "\n";
    Visualizer::divider();
    return order;
}

std::vector<std::string> Graph::dfs(const std::string& start) const {
    Visualizer::debug("dfs reached");
    Visualizer::header("DFS TRAVERSAL from " + start);

    if (!hasNode(start)) {
        Visualizer::error("Start node not found: " + start);
        Visualizer::divider();
        return {};
    }

    std::vector<std::string> stack = {start};
    std::unordered_set<std::string> visited;
    std::vector<std::string> order;

    while (!stack.empty()) {
        std::string current = stack.back();
        stack.pop_back();

        if (visited.count(current)) {
            continue;
        }

        visited.insert(current);
        order.push_back(current);

        std::vector<Edge> neighbors = adjacency.at(current);
        std::sort(neighbors.begin(), neighbors.end(), [](const Edge& left, const Edge& right) {
            return left.to > right.to;
        });

        std::vector<std::string> neighborNames;
        for (const auto& edge : neighbors) {
            neighborNames.push_back(edge.to);
            if (!visited.count(edge.to)) {
                stack.push_back(edge.to);
            }
        }

        std::cout << "› Stack top: " << current << " | Neighbors: "
                  << (neighborNames.empty() ? "(none)" : Visualizer::join(neighborNames, ", "))
                  << "\n";
    }

    std::cout << GREEN << "› DFS Order: " << Visualizer::join(order, " → ") << RESET << "\n";
    Visualizer::divider();
    return order;
}

void Graph::printDistanceTable(const std::unordered_map<std::string, int>& distances) const {
    std::vector<std::string> users = getNodes();
    std::cout << "› Distance Table:\n";
    for (const auto& user : users) {
        auto iterator = distances.find(user);
        if (iterator == distances.end() || iterator->second >= INF) {
            std::cout << "  " << user << " : INF\n";
        } else {
            std::cout << "  " << user << " : " << iterator->second << "\n";
        }
    }
}

std::vector<std::string> Graph::dijkstra(const std::string& start, const std::string& end) const {
    Visualizer::debug("dijkstra reached");
    Visualizer::header("DIJKSTRA: " + start + " → " + end);

    if (!hasNode(start) || !hasNode(end)) {
        Visualizer::error("Both start and end nodes must exist.");
        Visualizer::divider();
        return {};
    }

    std::unordered_map<std::string, int> distances;
    std::unordered_map<std::string, std::string> previous;
    std::set<std::pair<int, std::string>> minQueue;

    for (const auto& user : getNodes()) {
        distances[user] = INF;
    }
    distances[start] = 0;
    minQueue.insert({0, start});

    printDistanceTable(distances);

    while (!minQueue.empty()) {
        auto [currentDistance, current] = *minQueue.begin();
        minQueue.erase(minQueue.begin());

        if (currentDistance > distances[current]) {
            continue;
        }

        for (const auto& edge : adjacency.at(current)) {
            int candidateDistance = distances[current] + edge.weight;
            bool updated = candidateDistance < distances[edge.to];

            std::cout << "› Relaxing " << current << "→" << edge.to
                      << " | dist[" << edge.to << "] = "
                      << candidateDistance
                      << (updated ? " ✓ UPDATED" : " ✗ kept") << "\n";

            if (updated) {
                auto existing = minQueue.find({distances[edge.to], edge.to});
                if (existing != minQueue.end()) {
                    minQueue.erase(existing);
                }
                distances[edge.to] = candidateDistance;
                previous[edge.to] = current;
                minQueue.insert({candidateDistance, edge.to});
                printDistanceTable(distances);
            }
        }
    }

    printDistanceTable(distances);

    if (distances[end] >= INF) {
        Visualizer::warn("No path found.");
        Visualizer::divider();
        return {};
    }

    std::vector<std::string> path;
    for (std::string current = end; !current.empty();) {
        path.push_back(current);
        auto iterator = previous.find(current);
        if (current == start) {
            break;
        }
        if (iterator == previous.end()) {
            break;
        }
        current = iterator->second;
    }
    std::reverse(path.begin(), path.end());

    Visualizer::success("Shortest path: " + Visualizer::join(path, " → "));
    Visualizer::info("Total cost: " + std::to_string(distances[end]));
    Visualizer::divider();
    return path;
}

std::vector<MSTEdge> Graph::prim(const std::string& start) const {
    Visualizer::debug("prim reached");
    Visualizer::header("PRIM'S MST from " + start);

    if (directed) {
        Visualizer::error("Prim's MST requires undirected mode.");
        Visualizer::divider();
        return {};
    }

    if (!hasNode(start)) {
        Visualizer::error("Start node not found: " + start);
        Visualizer::divider();
        return {};
    }

    struct Candidate {
        int weight;
        std::string from;
        std::string to;
    };

    auto comparator = [](const Candidate& left, const Candidate& right) {
        return left.weight > right.weight;
    };

    std::priority_queue<Candidate, std::vector<Candidate>, decltype(comparator)> minHeap(comparator);
    std::unordered_set<std::string> visited = {start};
    std::vector<MSTEdge> mst;
    int totalWeight = 0;

    for (const auto& edge : adjacency.at(start)) {
        minHeap.push({edge.weight, start, edge.to});
    }

    while (!minHeap.empty() && visited.size() < adjacency.size()) {
        Candidate best = minHeap.top();
        minHeap.pop();

        if (visited.count(best.to)) {
            continue;
        }

        visited.insert(best.to);
        mst.push_back({best.from, best.to, best.weight});
        totalWeight += best.weight;

        std::cout << "› Added: " << best.from << " — " << best.to
                  << " (weight: " << best.weight << ") | Total so far: " << totalWeight << "\n";

        for (const auto& edge : adjacency.at(best.to)) {
            if (!visited.count(edge.to)) {
                minHeap.push({edge.weight, best.to, edge.to});
            }
        }
    }

    if (visited.size() != adjacency.size()) {
        Visualizer::warn("Graph is disconnected. MST is partial.");
    }

    std::cout << GREEN << "› MST Complete. Total Weight: " << totalWeight << RESET << "\n";
    Visualizer::divider();
    return mst;
}

void Graph::clear() {
    Visualizer::debug("clear reached");
    adjacency.clear();
}

std::vector<std::string> Graph::getNodes() const {
    std::vector<std::string> users;
    users.reserve(adjacency.size());
    for (const auto& [user, _] : adjacency) {
        users.push_back(user);
    }
    std::sort(users.begin(), users.end());
    return users;
}

const std::unordered_map<std::string, std::vector<Edge>>& Graph::getAdjacency() const {
    return adjacency;
}

std::size_t Graph::nodeCount() const {
    return adjacency.size();
}

void Graph::loadSampleNetwork() {
    Visualizer::debug("loadSampleNetwork reached");
    Visualizer::header("LOAD SAMPLE NETWORK");

    clear();

    const std::vector<std::string> sampleUsers = {
        "Aarav", "Ishani", "Rohan", "Priya", "Arjun", "Sneha", "Vihaan", "Ananya"
    };

    for (const auto& user : sampleUsers) {
        adjacency[user] = {};
        Visualizer::info("Loaded user: " + user);
    }

    const std::vector<MSTEdge> sampleEdges = {
        {"Aarav", "Ishani", 3},
        {"Aarav", "Rohan", 1},
        {"Ishani", "Priya", 4},
        {"Ishani", "Arjun", 2},
        {"Rohan", "Sneha", 5},
        {"Priya", "Vihaan", 1},
        {"Arjun", "Ananya", 3},
        {"Sneha", "Ananya", 2},
        {"Vihaan", "Ananya", 6}
    };

    for (const auto& edge : sampleEdges) {
        if (!edgeExists(edge.from, edge.to)) {
            adjacency[edge.from].push_back({edge.to, edge.weight});
        }
        if (!directed && !edgeExists(edge.to, edge.from)) {
            adjacency[edge.to].push_back({edge.from, edge.weight});
        }
        sortNeighbors(adjacency[edge.from]);
        if (!directed) {
            sortNeighbors(adjacency[edge.to]);
        }
        Visualizer::info("Loaded edge: " + edge.from + " -> " + edge.to + " (weight " + std::to_string(edge.weight) + ")");
    }

    Visualizer::info("Adjacency List:");
    printAdjacencyList();
    Visualizer::divider();
}
