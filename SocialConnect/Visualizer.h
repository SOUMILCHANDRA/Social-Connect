#ifndef SOCIALCONNECT_VISUALIZER_H
#define SOCIALCONNECT_VISUALIZER_H

#include "Graph.h"

#include <iostream>
#include <string>
#include <vector>

#define CYAN "\033[36m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define BOLD "\033[1m"
#define RESET "\033[0m"
#define LINE "═══════════════════════════"

class Visualizer {
public:
    static void debug(const std::string& message);
    static void header(const std::string& title);
    static void success(const std::string& message);
    static void info(const std::string& message);
    static void warn(const std::string& message);
    static void error(const std::string& message);
    static void divider();
    static std::string join(const std::vector<std::string>& values, const std::string& separator);
    static void displayAdjList(const Graph& graph);
    static void displayASCII(const Graph& graph);
    static void generateHTML(const Graph& graph, const std::string& filename = "viz.html");
};

#endif
