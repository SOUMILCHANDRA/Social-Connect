#include "Graph.h"
#include "Visualizer.h"

#include <iostream>
#include <limits>
#include <string>

namespace {
void printMenu(bool directedMode) {
    std::cout << CYAN << BOLD << "\nSOCIALCONNECT MENU" << RESET << "\n";
    std::cout << "Mode: " << (directedMode ? "DIRECTED" : "UNDIRECTED") << "\n";
    std::cout << "1. Add User\n";
    std::cout << "2. Add Connection\n";
    std::cout << "3. Remove User\n";
    std::cout << "4. Remove Connection\n";
    std::cout << "5. BFS from a user\n";
    std::cout << "6. DFS from a user\n";
    std::cout << "7. Shortest path between two users (Dijkstra)\n";
    std::cout << "8. Find MST of network (Prim's)\n";
    std::cout << "9. Display Adjacency List\n";
    std::cout << "10. Display Adjacency Matrix\n";
    std::cout << "11. Load sample social network\n";
    std::cout << "12. Generate HTML visualizer\n";
    std::cout << "13. Toggle directed/undirected mode\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter choice: ";
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}
}

int main() {
    std::ios_base::sync_with_stdio(true);
    std::cin.tie(NULL);
    std::cout.setf(std::ios::unitbuf);

    Graph graph(false);

    Visualizer::header("WELCOME TO SOCIALCONNECT");
    Visualizer::success("Console tracing enabled. HTML visualizer available from the menu.");
    Visualizer::divider();

    while (true) {
        printMenu(graph.isDirected());

        int choice = -1;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            Visualizer::error("Invalid menu choice.");
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 0) {
            Visualizer::header("EXIT");
            Visualizer::success("Goodbye from SocialConnect.");
            Visualizer::divider();
            break;
        }

        switch (choice) {
            case 1: {
                std::string name = readLine("Enter user name: ");
                graph.addNode(name);
                break;
            }
            case 2: {
                std::string from = readLine("From user: ");
                std::string to = readLine("To user: ");
                std::cout << "Weight (1-10): ";
                int weight = 1;
                if (!(std::cin >> weight)) {
                    std::cin.clear();
                    weight = 1;
                }
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                graph.addEdge(from, to, weight);
                break;
            }
            case 3: {
                std::string name = readLine("Enter user name: ");
                graph.removeNode(name);
                break;
            }
            case 4: {
                std::string from = readLine("From user: ");
                std::string to = readLine("To user: ");
                graph.removeEdge(from, to);
                break;
            }
            case 5: {
                std::string start = readLine("Start user: ");
                graph.bfs(start);
                break;
            }
            case 6: {
                std::string start = readLine("Start user: ");
                graph.dfs(start);
                break;
            }
            case 7: {
                std::string start = readLine("Start user: ");
                std::string end = readLine("End user: ");
                graph.dijkstra(start, end);
                break;
            }
            case 8: {
                std::string start = readLine("Start user for MST: ");
                graph.prim(start);
                break;
            }
            case 9: {
                Visualizer::header("ADJACENCY LIST");
                graph.printAdjacencyList();
                Visualizer::divider();
                break;
            }
            case 10: {
                graph.printAdjacencyMatrix();
                break;
            }
            case 11: {
                graph.loadSampleNetwork();
                break;
            }
            case 12: {
                std::string filename = readLine("Output HTML file [viz.html]: ");
                if (filename.empty()) {
                    filename = "viz.html";
                }
                Visualizer::generateHTML(graph, filename);
                break;
            }
            case 13: {
                graph.setDirected(!graph.isDirected());
                break;
            }
            default: {
                Visualizer::error("Unknown menu option.");
                break;
            }
        }
    }

    return 0;
}
