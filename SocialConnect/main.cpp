#include "Graph.h"
#include "Visualizer.h"
#include <iostream>
#include <string>
#include <limits>

/**
 * @brief Load sample social network data as per requirements.
 */
void loadSampleData(Graph& g) {
    Visualizer::printHeader("LOADING SAMPLE NETWORK");
    
    // Users (Indian Names)
    std::vector<std::string> users = {"Aarav", "Ishani", "Rohan", "Priya", "Arjun", "Sneha", "Vihaan", "Ananya"};
    for (auto const& u : users) g.addNode(u);

    // Edges with weights
    g.addEdge("Aarav", "Ishani", 3);
    g.addEdge("Aarav", "Rohan", 1);
    g.addEdge("Ishani", "Priya", 4);
    g.addEdge("Ishani", "Arjun", 2);
    g.addEdge("Rohan", "Sneha", 5);
    g.addEdge("Priya", "Vihaan", 1);
    g.addEdge("Arjun", "Ananya", 3);
    g.addEdge("Sneha", "Ananya", 2);
    g.addEdge("Vihaan", "Ananya", 6);

    Visualizer::printSuccess("> Sample social network (India Region) loaded successfully!");
}

void showMenu() {
    std::cout << "\n" << Visualizer::BOLD_S << Visualizer::CYAN_S << "--- SOCIALCONNECT MAIN MENU ---" << Visualizer::RESET_S << "\n";
    std::cout << "1.  Add User (Node)\n";
    std::cout << "2.  Add Connection (Edge)\n";
    std::cout << "3.  Remove User\n";
    std::cout << "4.  Remove Connection\n";
    std::cout << "5.  BFS Traversal (Find Network)\n";
    std::cout << "6.  DFS Traversal (Explore Deep)\n";
    std::cout << "7.  Shortest Path (Dijkstra)\n";
    std::cout << "8.  Find MST (Prim's)\n";
    std::cout << "9.  Display Adjacency List\n";
    std::cout << "10. Display Adjacency Matrix\n";
    std::cout << "11. Load Sample Social Network\n";
    std::cout << "12. Generate D3.js Visualization (HTML)\n";
    std::cout << "13. Toggle Mode (Directed/Undirected)\n";
    std::cout << "0.  Exit\n";
    std::cout << "Selection: ";
}

int main() {
    std::ios_base::sync_with_stdio(true);
    std::cout.setf(std::ios::unitbuf);
    std::cin.tie(NULL);
    Graph socialGraph(GraphMode::UNDIRECTED);
    int choice = -1;

    Visualizer::printHeader("WELCOME TO SOCIALCONNECT");
    std::cout << "Starting in " << Visualizer::GREEN_S << "UNDIRECTED" << Visualizer::RESET_S << " mode (Friendships).\n";

    while (true) {
        showMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            Visualizer::printError("Invalid input. Please enter a number.");
            continue;
        }

        if (choice == 0) break;

        std::string u1, u2;
        int w;

        switch (choice) {
            case 1:
                std::cout << "Enter user name: ";
                std::cin >> u1;
                socialGraph.addNode(u1);
                break;
            case 2:
                std::cout << "Enter source user: ";
                std::cin >> u1;
                std::cout << "Enter target user: ";
                std::cin >> u2;
                std::cout << "Enter weight (1-10): ";
                std::cin >> w;
                socialGraph.addEdge(u1, u2, w);
                break;
            case 3:
                std::cout << "Enter user name to remove: ";
                std::cin >> u1;
                socialGraph.removeNode(u1);
                break;
            case 4:
                std::cout << "Enter source user: ";
                std::cin >> u1;
                std::cout << "Enter target user: ";
                std::cin >> u2;
                socialGraph.removeEdge(u1, u2);
                break;
            case 5:
                std::cout << "Enter start user for BFS: ";
                std::cin >> u1;
                socialGraph.bfs(u1);
                break;
            case 6:
                std::cout << "Enter start user for DFS: ";
                std::cin >> u1;
                socialGraph.dfs(u1);
                break;
            case 7:
                std::cout << "Enter source user: ";
                std::cin >> u1;
                std::cout << "Enter destination user: ";
                std::cin >> u2;
                socialGraph.dijkstra(u1, u2);
                break;
            case 8:
                socialGraph.primMST();
                break;
            case 9:
                Visualizer::printHeader("ADJACENCY LIST");
                Visualizer::displayAdjList(socialGraph);
                break;
            case 10:
                socialGraph.displayAdjacencyMatrix();
                break;
            case 11:
                loadSampleData(socialGraph);
                break;
            case 12:
                Visualizer::generateHTML(socialGraph);
                break;
            case 13:
                if (socialGraph.getMode() == GraphMode::UNDIRECTED) {
                    socialGraph.setMode(GraphMode::DIRECTED);
                    Visualizer::printInfo("Switched to DIRECTED mode (Follows).");
                } else {
                    socialGraph.setMode(GraphMode::UNDIRECTED);
                    Visualizer::printInfo("Switched to UNDIRECTED mode (Friendships).");
                }
                break;
            default:
                Visualizer::printError("Invalid selection.");
        }
    }

    Visualizer::printHeader("GOODBYE!");
    return 0;
}
