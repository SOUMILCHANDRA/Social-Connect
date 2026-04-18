#include "Visualizer.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

void Visualizer::printBanner() {
    std::cout << CYAN << BOLD << "\n"
              << "  SOCIALCONNECT CORE v2.0 (CROW API EDITION)\n"
              << "  " << LINE << "\n" << RESET;
}

void Visualizer::print(const std::string& message) {
    std::cout << "  " << message << "\n";
}

void Visualizer::printDivider() {
    std::cout << CYAN << "  " << LINE << RESET << "\n";
}

void Visualizer::printMenu() {
    std::cout << "\n  " << BOLD << "MAIN MENU" << RESET << "\n"
              << "  1. Add User       2. Remove User\n"
              << "  3. Connect Users  4. Disconnect Users\n"
              << "  5. Algorithms     6. State (JSON)\n"
              << "  0. Exit\n"
              << "\n  Choice: ";
}

void Visualizer::printAlgoHeader(const std::string& algo, const std::string& start) {
    std::cout << "\n  " << YELLOW << BOLD << "⚟ " << algo << " FROM: " << start << RESET << "\n";
}

void Visualizer::printBFSStep(int step, const std::string& node, const std::vector<std::string>& queue, const std::vector<std::string>& order) {
    std::cout << "  [" << step << "] Visiting: " << BOLD << node << RESET 
              << " | Queue: [" << join(queue, ", ") << "]\n";
}

void Visualizer::printBFSComplete(const std::vector<std::string>& order) {
    std::cout << "  " << GREEN << BOLD << "✓ BFS COMPLETE: " << RESET << join(order, " → ") << "\n";
}

void Visualizer::printDFSStep(int step, const std::string& node, const std::vector<std::string>& stack, const std::vector<std::string>& order) {
    std::cout << "  [" << step << "] Visiting: " << BOLD << node << RESET 
              << " | Stack: [" << join(stack, ", ") << "]\n";
}

void Visualizer::printDFSComplete(const std::vector<std::string>& order) {
    std::cout << "  " << GREEN << BOLD << "✓ DFS COMPLETE: " << RESET << join(order, " → ") << "\n";
}

void Visualizer::printDijkstraStep(int step, const std::string& u, const std::string& v, int dist, bool updated) {
    std::cout << "  [" << step << "] Relax: " << u << " → " << v 
              << " | Dist: " << dist << (updated ? std::string(GREEN) + " (Updated)" : std::string(RED) + " (Kept)") << RESET << "\n";
}

void Visualizer::printDijkstraComplete(const std::vector<std::string>& path, int cost) {
    if (path.empty()) std::cout << "  " << RED << "✕ NO PATH FOUND" << RESET << "\n";
    else std::cout << "  " << GREEN << BOLD << "✓ SHORTEST PATH: " << RESET << join(path, " → ") << " (Cost: " << cost << ")\n";
}

void Visualizer::printMSTEdge(int step, const std::string& u, const std::string& v, int weight, int total) {
    std::cout << "  [" << step << "] Added: " << u << " — " << v 
              << " (w=" << weight << ") | Total: " << total << "\n";
}

void Visualizer::printMSTComplete(int total) {
    std::cout << "  " << GREEN << BOLD << "✓ MST COMPLETE: " << RESET << "Total Weight = " << total << "\n";
}

std::string Visualizer::join(const std::vector<std::string>& values, const std::string& separator) {
    std::string result = "";
    for (size_t i = 0; i < values.size(); ++i) {
        result += values[i];
        if (i < values.size() - 1) result += separator;
    }
    return result;
}

std::string Visualizer::jsonKeyValue(const std::string& key, const std::string& value) {
    return "\"" + key + "\":\"" + value + "\"";
}

std::string Visualizer::jsonArray(const std::vector<std::string>& elements) {
    std::string result = "[";
    for (size_t i = 0; i < elements.size(); ++i) {
        result += "\"" + elements[i] + "\"";
        if (i < elements.size() - 1) result += ",";
    }
    return result + "]";
}

// Keep these for backward compat if needed, but clean up body
void Visualizer::debug(const std::string& text) { std::cout << "[DEBUG] " << text << "\n"; }
void Visualizer::header(const std::string& text) { std::cout << "\n" << CYAN << BOLD << "═══ " << text << " ═══" << RESET << "\n"; }
void Visualizer::success(const std::string& text) { std::cout << GREEN << "› " << text << RESET << "\n"; }
void Visualizer::info(const std::string& text) { std::cout << CYAN << "› " << text << RESET << "\n"; }
void Visualizer::warn(const std::string& text) { std::cout << YELLOW << "› " << text << RESET << "\n"; }
void Visualizer::error(const std::string& text) { std::cerr << RED << BOLD << "✕ ERROR: " << text << RESET << "\n"; }
void Visualizer::divider() { printDivider(); }
void Visualizer::displayAdjList(const Graph& graph) { graph.printAdjacencyList(); }
