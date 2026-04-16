#include "Visualizer.h"
#include <iostream>
#include <fstream>
#include <sstream>

const std::string Visualizer::RESET   = "\033[0m";
const std::string Visualizer::GREEN   = "\033[32m";
const std::string Visualizer::YELLOW  = "\033[33m";
const std::string Visualizer::RED     = "\033[31m";
const std::string Visualizer::CYAN    = "\033[36m";
const std::string Visualizer::MAGENTA = "\033[35m";
const std::string Visualizer::BOLD    = "\033[1m";

void Visualizer::printHeader(const std::string& title) {
    std::cout << "\n" << BOLD << MAGENTA << "=== " << title << " ===" << RESET << "\n";
    std::cout.flush();
}

void Visualizer::printInfo(const std::string& message) {
    std::cout << YELLOW << message << RESET << "\n";
    std::cout.flush();
}

void Visualizer::printSuccess(const std::string& message) {
    std::cout << GREEN << message << RESET << "\n";
    std::cout.flush();
}

void Visualizer::printError(const std::string& message) {
    std::cerr << RED << message << RESET << "\n";
    std::cerr.flush();
}

void Visualizer::printStep(const std::string& step) {
    std::cout << CYAN << "  > " << step << RESET << "\n";
    std::cout.flush();
}

// Added this since I used it in Graph.cpp
void Visualizer::printWarning(const std::string& msg) {
    std::cout << YELLOW << "[!] " << msg << RESET << "\n";
}

void Visualizer::displayAdjList(const Graph& graph) {
    std::cout << BOLD << "Current Adjacency List:" << RESET << "\n";
    for (const auto& [node, neighbors] : graph.getAdjList()) {
        std::cout << "  " << node << " -> [";
        for (size_t i = 0; i < neighbors.size(); ++i) {
            std::cout << neighbors[i].first << "(" << neighbors[i].second << ")";
            if (i < neighbors.size() - 1) std::cout << ", ";
        }
        std::cout << "]\n";
    }
}

void Visualizer::displayASCII(const Graph& graph) {
    printHeader("ASCII GRAPH VIEW");
    auto adj = graph.getAdjList();
    if (adj.empty()) {
        printError("Graph is empty.");
        return;
    }

    // Simplified ASCII visual: [A] --(3)-- [B]
    std::set<std::string> handled;
    for (const auto& [u, neighbors] : adj) {
        for (const auto& edge : neighbors) {
            std::string v = edge.first;
            int w = edge.second;
            
            // For undirected, avoid double printing
            if (graph.getMode() == GraphMode::UNDIRECTED) {
                std::string pair = (u < v) ? (u + v) : (v + u);
                if (handled.find(pair) != handled.end()) continue;
                handled.insert(pair);
                std::cout << "  [" << u << "] <---(" << w << ")---> [" << v << "]\n";
            } else {
                std::cout << "  [" << u << "] ---(" << w << ")---> [" << v << "]\n";
            }
        }
    }
}

void Visualizer::generateHTML(const Graph& graph, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        printError("Failed to create visualizer file.");
        return;
    }

    auto nodes = graph.getNodes();
    auto adj = graph.getAdjList();

    // JSON construction for D3
    std::stringstream nodesJson;
    std::stringstream linksJson;

    nodesJson << "[\n";
    for (size_t i = 0; i < nodes.size(); ++i) {
        nodesJson << "      { \"id\": \"" << nodes[i] << "\" }" << (i < nodes.size() - 1 ? "," : "") << "\n";
    }
    nodesJson << "    ]";

    linksJson << "[\n";
    bool first = true;
    std::set<std::string> handled;
    for (const auto& [u, neighbors] : adj) {
        for (const auto& edge : neighbors) {
            std::string v = edge.first;
            int w = edge.second;

            if (graph.getMode() == GraphMode::UNDIRECTED) {
                std::string pair = (u < v) ? (u + v) : (v + u);
                if (handled.find(pair) != handled.end()) continue;
                handled.insert(pair);
            }

            if (!first) linksJson << ",\n";
            linksJson << "      { \"source\": \"" << u << "\", \"target\": \"" << v << "\", \"value\": " << w << " }";
            first = false;
        }
    }
    linksJson << "\n    ]";

    file << R"(<!DOCTYPE html>
<html>
<head>
    <title>SocialConnect Visualizer</title>
    <script src="https://d3js.org/d3.v7.min.js"></script>
    <style>
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #0f172a; color: white; margin: 0; overflow: hidden; }
        .header { position: absolute; top: 20px; left: 20px; background: rgba(30, 41, 59, 0.8); padding: 15px 25px; border-radius: 12px; border-left: 5px solid #38bdf8; backdrop-filter: blur(5px); }
        h1 { margin: 0; color: #38bdf8; font-size: 24px; }
        p { margin: 5px 0 0; opacity: 0.7; font-size: 14px; }
        .node { stroke: #fff; stroke-width: 2px; cursor: pointer; }
        .link { stroke: #94a3b8; stroke-opacity: 0.6; stroke-width: 2px; }
        .label { font-size: 12px; font-weight: bold; fill: white; pointer-events: none; text-shadow: 0 1px 2px black; }
        .edge-label { font-size: 10px; fill: #fbbf24; pointer-events: none; }
    </style>
</head>
<body>
    <div class="header">
        <h1>SocialConnect Visualizer</h1>
        <p>Dynamic Graph Visualization of your Network</p>
    </div>
    <svg width="100%" height="100vh"></svg>
    <script>
        const nodes = )" << nodesJson.str() << R"(;
        const links = )" << linksJson.str() << R"(;

        const svg = d3.select("svg");
        const width = window.innerWidth;
        const height = window.innerHeight;

        const simulation = d3.forceSimulation(nodes)
            .force("link", d3.forceLink(links).id(d => d.id).distance(150))
            .force("charge", d3.forceManyBody().strength(-500))
            .force("center", d3.forceCenter(width / 2, height / 2));

        const link = svg.append("g")
            .attr("class", "links")
            .selectAll("line")
            .data(links)
            .enter().append("line")
            .attr("class", "link");

        const edgeLabels = svg.append("g")
            .selectAll("text")
            .data(links)
            .enter().append("text")
            .attr("class", "edge-label")
            .text(d => d.value);

        const node = svg.append("g")
            .attr("class", "nodes")
            .selectAll("circle")
            .data(nodes)
            .enter().append("circle")
            .attr("class", "node")
            .attr("r", 15)
            .attr("fill", "#0ea5e9")
            .call(d3.drag()
                .on("start", dragstarted)
                .on("drag", dragged)
                .on("end", dragended));

        const labels = svg.append("g")
            .selectAll("text")
            .data(nodes)
            .enter().append("text")
            .attr("class", "label")
            .text(d => d.id)
            .attr("dx", 20)
            .attr("dy", 5);

        simulation.on("tick", () => {
            link
                .attr("x1", d => d.source.x)
                .attr("y1", d => d.source.y)
                .attr("x2", d => d.target.x)
                .attr("y2", d => d.target.y);

            node
                .attr("cx", d => d.x)
                .attr("cy", d => d.y);

            labels
                .attr("x", d => d.x)
                .attr("y", d => d.y);
            
            edgeLabels
                .attr("x", d => (d.source.x + d.target.x) / 2)
                .attr("y", d => (d.source.y + d.target.y) / 2);
        });

        function dragstarted(event) {
            if (!event.active) simulation.alphaTarget(0.3).restart();
            event.subject.fx = event.subject.x;
            event.subject.fy = event.subject.y;
        }

        function dragged(event) {
            event.subject.fx = event.x;
            event.subject.fy = event.y;
        }

        function dragended(event) {
            if (!event.active) simulation.alphaTarget(0);
            event.subject.fx = null;
            event.subject.fy = null;
        }

        window.addEventListener('resize', () => {
            svg.attr("width", window.innerWidth).attr("height", window.innerHeight);
            simulation.force("center", d3.forceCenter(window.innerWidth / 2, window.innerHeight / 2)).restart();
        });
    </script>
</body>
</html>)";

    file.close();
    printSuccess("> Visualization generated: " + filename);
}
