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
    static void printBanner();
    static void print(const std::string& message);
    static void printDivider();
    static void printMenu();
    static void printAlgoHeader(const std::string& algo, const std::string& start);

    static void printBFSStep(int step, const std::string& node, const std::vector<std::string>& queue, const std::vector<std::string>& order);
    static void printBFSComplete(const std::vector<std::string>& order);

    static void printDFSStep(int step, const std::string& node, const std::vector<std::string>& stack, const std::vector<std::string>& order);
    static void printDFSComplete(const std::vector<std::string>& order);

    static void printDijkstraStep(int step, const std::string& u, const std::string& v, int dist, bool updated);
    static void printDijkstraComplete(const std::vector<std::string>& path, int cost);

    static void printMSTEdge(int step, const std::string& u, const std::string& v, int weight, int total);
    static void printMSTComplete(int total);

    // Helpers
    static std::string join(const std::vector<std::string>& values, const std::string& separator);
    static std::string jsonKeyValue(const std::string& key, const std::string& value);
    static std::string jsonArray(const std::vector<std::string>& elements);

    // Legacy Support
    static void debug(const std::string& message);
    static void header(const std::string& title);
    static void success(const std::string& message);
    static void info(const std::string& message);
    static void warn(const std::string& message);
    static void error(const std::string& message);
    static void divider();
    static void displayAdjList(const Graph& graph);
};

#endif
