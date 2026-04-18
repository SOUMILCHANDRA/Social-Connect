#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "Graph.h"
#include <string>

/**
 * @brief Visualizer class handles all output to the terminal and external files.
 * Uses ANSI color codes for rich terminal experience.
 */
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"
#define LINE    "═══════════════════════════"

/**
 * @brief Visualizer class handles all output to the terminal and external files.
 * Uses ANSI color codes for rich terminal experience.
 */
class Visualizer {
public:
    // Terminal Colors (ANSI)
    static const std::string RESET_S;
    static const std::string GREEN_S;
    static const std::string YELLOW_S;
    static const std::string RED_S;
    static const std::string CYAN_S;
    static const std::string MAGENTA_S;
    static const std::string BOLD_S;

    // Output Helpers
    static void printHeader(const std::string& title);
    static void printInfo(const std::string& message);
    static void printSuccess(const std::string& message);
    static void printWarning(const std::string& message);
    static void printError(const std::string& message);
    static void printStep(const std::string& step);

    // Graph Visuals
    static void displayAdjList(const Graph& graph);
    static void displayASCII(const Graph& graph);
    
    // File/Web Visualizer
    static void generateHTML(const Graph& graph, const std::string& filename = "viz.html");
};

#endif // VISUALIZER_H
