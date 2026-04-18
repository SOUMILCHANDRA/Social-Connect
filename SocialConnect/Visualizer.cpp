#include "Visualizer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <set>

const std::string Visualizer::RESET_S   = "\033[0m";
const std::string Visualizer::GREEN_S   = "\033[32m";
const std::string Visualizer::YELLOW_S  = "\033[33m";
const std::string Visualizer::RED_S     = "\033[31m";
const std::string Visualizer::CYAN_S    = "\033[36m";
const std::string Visualizer::MAGENTA_S = "\033[35m";
const std::string Visualizer::BOLD_S    = "\033[1m";

void Visualizer::printHeader(const std::string& title) {
    std::cout << "\n" << BOLD_S << CYAN_S << "═══ " << title << " ═══" << RESET_S << "\n";
    std::cout.flush();
}

void Visualizer::printInfo(const std::string& message) {
    std::cout << CYAN_S << message << RESET_S << "\n";
    std::cout.flush();
}

void Visualizer::printSuccess(const std::string& message) {
    std::cout << GREEN_S << message << RESET_S << "\n";
    std::cout.flush();
}

void Visualizer::printWarning(const std::string& message) {
    std::cout << YELLOW_S << "[!] " << message << RESET_S << "\n";
    std::cout.flush();
}

void Visualizer::printError(const std::string& message) {
    std::cerr << RED_S << message << RESET_S << "\n";
    std::cerr.flush();
}

void Visualizer::printStep(const std::string& step) {
    std::cout << "› " << step << "\n";
    std::cout.flush();
}

void Visualizer::displayAdjList(const Graph& graph) {
    for (const auto& pair : graph.getAdjList()) {
        const std::string& node = pair.first;
        const auto& neighbors = pair.second;
        std::cout << "  " << BOLD_S << node << RESET_S << " -> [";
        for (size_t i = 0; i < neighbors.size(); ++i) {
            std::cout << neighbors[i].first << "(" << neighbors[i].second << ")";
            if (i < neighbors.size() - 1) std::cout << ", ";
        }
        std::cout << "]\n";
    }
}

void Visualizer::displayASCII(const Graph& graph) {
    printHeader("ASCII GRAPH VIEW");
    // Implementation omitted for brevity or kept simple
}

void Visualizer::generateHTML(const Graph& graph, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        printError("Failed to create visualizer file.");
        return;
    }

    auto nodes = graph.getNodes();
    auto adj = graph.getAdjList();

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
    for (const auto& pair : adj) {
        const std::string& u = pair.first;
        for (const auto& edge : pair.second) {
            const std::string& v = edge.first;
            int w = edge.second;

            if (graph.getMode() == GraphMode::UNDIRECTED) {
                std::string p = (u < v) ? (u + v) : (v + u);
                if (handled.count(p)) continue;
                handled.insert(p);
            }

            if (!first) linksJson << ",\n";
            linksJson << "      { \"source\": \"" << u << "\", \"target\": \"" << v << "\", \"weight\": " << w << " }";
            first = false;
        }
    }
    linksJson << "\n    ]";

    // The full HTML template with the dynamic data injected
    file << R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SocialConnect — Graph Network Analyzer</title>
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=IBM+Plex+Sans:wght@400;500;600&family=JetBrains+Mono:wght@400;500&display=swap" rel="stylesheet">
    <script src="https://d3js.org/d3.v7.min.js"></script>
    <style>
        :root {
            --bg-dark: #0b0e14;
            --bg-panel: #111520;
            --bg-card: #161b27;
            --accent: #4fc3f7;
            --accent2: #81c784;
            --accent3: #ffb74d;
            --accent4: #e57373;
            --text: #cdd6f4;
            --text-dim: #6c7a96;
            --border: #1e2740;
        }

        html, body {
            margin: 0;
            padding: 0;
            height: 100vh;
            overflow: hidden;
            display: flex;
            flex-direction: column;
            background: var(--bg-dark);
            color: var(--text);
            font-family: 'IBM Plex Sans', sans-serif;
        }

        #header {
            height: 48px;
            display: flex;
            align-items: center;
            justify-content: space-between;
            padding: 0 20px;
            background: #0d1220;
            border-bottom: 1px solid var(--border);
            flex-shrink: 0;
        }

        #header h1 {
            font-size: 14px;
            font-weight: 600;
            letter-spacing: 0.05em;
            text-transform: uppercase;
            margin: 0;
            color: var(--text);
        }

        #header-stats {
            display: flex;
            gap: 20px;
            font-size: 11px;
            font-family: 'JetBrains Mono', monospace;
            color: var(--text-dim);
        }

        #header-stats span {
            color: var(--accent);
        }

        #main {
            display: flex;
            flex: 1;
            overflow: hidden;
        }

        #left-panel {
            width: 200px;
            min-width: 200px;
            background: var(--bg-panel);
            border-right: 1px solid var(--border);
            padding: 16px 12px;
            display: flex;
            flex-direction: column;
            gap: 14px;
            overflow-y: auto;
        }

        #graph-panel {
            flex: 1;
            position: relative;
            background: var(--bg-dark);
        }

        #graph-panel svg {
            width: 100%;
            height: 100%;
            display: block;
        }

        #terminal-panel {
            width: 340px;
            min-width: 340px;
            background: #080c14;
            border-left: 1px solid var(--border);
            font-family: 'JetBrains Mono', monospace;
            font-size: 11px;
            overflow-y: auto;
            padding: 0;
            display: flex;
            flex-direction: column;
        }

        .section-label {
            color: var(--text-dim);
            font-size: 10px;
            letter-spacing: 0.1em;
            font-weight: 600;
            margin-bottom: 6px;
            text-transform: uppercase;
        }

        .panel-section {
            display: flex;
            flex-direction: column;
            gap: 6px;
        }

        input {
            background: var(--bg-card);
            border: 1px solid var(--border);
            color: var(--text);
            padding: 6px 8px;
            font-size: 12px;
            border-radius: 4px;
            outline: none;
            font-family: inherit;
        }

        input:focus {
            border-color: var(--accent);
        }

        button {
            background: var(--bg-card);
            border: 1px solid var(--border);
            color: var(--text);
            padding: 6px 10px;
            font-size: 11px;
            text-align: left;
            border-radius: 4px;
            cursor: pointer;
            transition: all 0.2s;
            font-weight: 500;
        }

        button:hover {
            background: var(--border);
            border-color: var(--text-dim);
        }

        button.primary {
            border-color: var(--accent);
            color: var(--accent);
        }

        button.primary:hover {
            background: rgba(79, 195, 247, 0.1);
        }

        /* Terminal Styles */
        #term-header {
            padding: 8px 12px;
            background: #0d1220;
            border-bottom: 1px solid var(--border);
            color: var(--text-dim);
            font-size: 10px;
            position: sticky;
            top: 0;
            z-index: 10;
        }

        #term-body {
            padding: 10px;
            display: flex;
            flex-direction: column;
        }

        .term-block {
            border-bottom: 1px solid var(--border);
            margin-bottom: 8px;
            padding-bottom: 8px;
        }

        .term-op {
            color: var(--accent);
            font-weight: bold;
            margin-bottom: 4px;
        }

        .term-line {
            color: var(--text);
            padding-left: 8px;
            margin-bottom: 2px;
            line-height: 1.4;
        }

        .term-arrow {
            color: var(--accent);
            margin-right: 4px;
        }

        /* SVG Styles */
        .node circle {
            fill: #1a2a3a;
            stroke: var(--accent);
            stroke-width: 2px;
            transition: fill 0.4s, stroke 0.4s;
        }

        .node text {
            fill: var(--text);
            font-family: 'JetBrains Mono', monospace;
            font-size: 10px;
            pointer-events: none;
            text-anchor: middle;
            dominant-baseline: middle;
        }

        .link {
            stroke: #1e2740;
            stroke-width: 2px;
            transition: stroke 0.4s, stroke-width 0.4s;
        }

        .link-label {
            fill: var(--text-dim);
            font-size: 9px;
            font-family: 'JetBrains Mono', monospace;
            pointer-events: none;
        }

        .grid-dot {
            fill: #1a2035;
            opacity: 0.5;
        }
    </style>
</head>
<body>

    <div id="header">
        <h1>Social Connect — Graph Network Analyzer</h1>
        <div id="header-stats">
            <div>NODES: <span id="stat-nodes">0</span></div>
            <div>MODE: <span id="stat-mode">)" << (graph.getMode() == GraphMode::DIRECTED ? "DIRECTED" : "UNDIRECTED") << R"(</span></div>
        </div>
    </div>

    <div id="main">
        <div id="left-panel">
            <div class="panel-section">
                <div class="section-label">GRAPH CONTROL</div>
                <button onclick="toggleMode()">TOGGLE DIRECTED / UNDIRECTED</button>
                <button class="primary" onclick="loadSampleNetwork()">Load Sample Network</button>
            </div>

            <div class="panel-section">
                <div class="section-label">NODE MANAGEMENT</div>
                <input type="text" id="nodeInput" placeholder="User ID">
                <button onclick="addNode()">+ Add Node</button>
                <button onclick="removeNode()">✕ Remove Node</button>
            </div>

            <div class="panel-section">
                <div class="section-label">EDGE MANAGEMENT</div>
                <input type="text" id="fromInput" placeholder="From">
                <input type="text" id="toInput" placeholder="To">
                <input type="number" id="weightInput" placeholder="Weight (1-10)" min="1" max="10" value="1">
                <button onclick="addEdge()">Link Nodes</button>
                <button onclick="removeEdge()">Unlink Nodes</button>
            </div>

            <div class="panel-section">
                <div class="section-label">ALGORITHMS</div>
                <input type="text" id="startInput" placeholder="Start Node">
                <input type="text" id="endInput" placeholder="End Node (optional)">
                <button onclick="runBFS()">▶ BFS Traverse</button>
                <button onclick="runDFS()">▶ DFS Traverse</button>
                <button onclick="runDijkstra()">▶ Dijkstra Path</button>
                <button onclick="runPrims()">▶ Prim's MST</button>
                <button style="margin-top:4px" onclick="clearHighlights()">↺ Clear Highlights</button>
            </div>
        </div>

        <div id="graph-panel"></div>

        <div id="terminal-panel">
            <div id="term-header">SESSION: GRAPH_ANALYZER@CORE &nbsp;&nbsp; TTY1</div>
            <div id="term-body"></div>
        </div>
    </div>

    <script>
        // Initial Data from C++
        let nodes = )HTML" << nodesJson.str() << R"HTML(;
        let links = )HTML" << linksJson.str() << R"HTML(;
        let directed = )HTML" << (graph.getMode() == GraphMode::DIRECTED ? "true" : "false") << R"HTML(;
        
        let highlightState = { nodes: new Set(), links: new Set() };

        // D3 Setup
        const graphPanel = document.getElementById("graph-panel");
        const svg = d3.select("#graph-panel").append("svg")
            .attr("viewBox", "0 0 1000 800")
            .attr("preserveAspectRatio", "xMidYMid meet");
        
        const width = 1000;
        const height = 800;

        // Visual Definitions
        const defs = svg.append("defs");
        
        // Grid Pattern
        defs.append("pattern")
            .attr("id", "grid")
            .attr("width", 30)
            .attr("height", 30)
            .attr("patternUnits", "userSpaceOnUse")
            .append("circle")
            .attr("cx", 1.5)
            .attr("cy", 1.5)
            .attr("r", 1.5)
            .attr("class", "grid-dot");

        svg.append("rect")
            .attr("width", "100%")
            .attr("height", "100%")
            .attr("fill", "url(#grid)");

        // Arrow Marker
        defs.append("marker")
            .attr("id", "arrowhead")
            .attr("viewBox", "0 -5 10 10")
            .attr("refX", 22)
            .attr("refY", 0)
            .attr("orient", "auto")
            .attr("markerWidth", 6)
            .attr("markerHeight", 6)
            .append("path")
            .attr("d", "M 0,-5 L 10,0 L 0,5")
            .attr("fill", "#4fc3f7");

        const linkGroup = svg.append("g").attr("class", "links");
        const nodeGroup = svg.append("g").attr("class", "nodes");

        // Force Simulation
        const simulation = d3.forceSimulation(nodes)
            .force("link", d3.forceLink(links).id(d => d.id).distance(120))
            .force("charge", d3.forceManyBody().strength(-300))
            .force("center", d3.forceCenter(width / 2, height / 2))
            .force("x", d3.forceX(width / 2).strength(0.08))
            .force("y", d3.forceY(height / 2).strength(0.08))
            .force("collision", d3.forceCollide(35));

        // Initial mapping for source/target if they are strings
        links.forEach(l => {
            if (typeof l.source === 'string') {
                l.source = nodes.find(n => n.id === l.source);
            }
            if (typeof l.target === 'string') {
                l.target = nodes.find(n => n.id === l.target);
            }
        });

        simulation.on("tick", () => {
            linkGroup.selectAll(".link")
                .attr("x1", d => d.source.x)
                .attr("y1", d => d.source.y)
                .attr("x2", d => d.target.x)
                .attr("y2", d => d.target.y);

            linkGroup.selectAll(".link-label")
                .attr("x", d => (d.source.x + d.target.x) / 2)
                .attr("y", d => (d.source.y + d.target.y) / 2);

            nodeGroup.selectAll(".node")
                .attr("transform", d => {
                    d.x = Math.max(30, Math.min(width - 30, d.x));
                    d.y = Math.max(30, Math.min(height - 30, d.y));
                    return `translate(${d.x},${d.y})`;
                });
        });

        // Core Functions
        function updateGraph() {
            document.getElementById("stat-nodes").textContent = nodes.length;
            document.getElementById("stat-mode").textContent = directed ? "DIRECTED" : "UNDIRECTED";

            // Links
            const l = linkGroup.selectAll(".link-wrap")
                .data(links, d => `${d.source.id || d.source}-${d.target.id || d.target}`);

            const lEnter = l.enter().append("g").attr("class", "link-wrap");

            lEnter.append("line")
                .attr("class", "link")
                .attr("marker-end", directed ? "url(#arrowhead)" : null);

            lEnter.append("text")
                .attr("class", "link-label")
                .attr("dy", -5)
                .attr("text-anchor", "middle")
                .text(d => d.weight);

            l.select(".link").attr("marker-end", directed ? "url(#arrowhead)" : null);
            l.select(".link-label").text(d => d.weight);
            l.exit().remove();

            // Nodes
            const n = nodeGroup.selectAll(".node")
                .data(nodes, d => d.id);

            const nEnter = n.enter().append("g")
                .attr("class", "node")
                .attr("transform", d => `translate(${width/2},${height/2})`) // Spawn at center
                .call(d3.drag()
                    .on("start", dragstarted)
                    .on("drag", dragged)
                    .on("end", dragended));

            nEnter.append("circle").attr("r", 18);
            nEnter.append("text").text(d => d.id);

            n.exit().remove();

            simulation.nodes(nodes);
            simulation.force("link").links(links);
            
            // Fixed center for robust layout
            simulation.force("center", d3.forceCenter(width / 2, height / 2));
            simulation.force("x", d3.forceX(width / 2).strength(0.08));
            simulation.force("y", d3.forceY(height / 2).strength(0.08));
            
            simulation.alpha(1).restart();
        }

        function dragstarted(event, d) {
            if (!event.active) simulation.alphaTarget(0.3).restart();
            d.fx = d.x;
            d.fy = d.y;
        }

        function dragged(event, d) {
            d.fx = event.x;
            d.fy = event.y;
        }

        function dragended(event, d) {
            if (!event.active) simulation.alphaTarget(0);
            d.fx = null;
            d.fy = null;
        }

        // Terminal Log
        function logTerm(operation, subtitle, steps, result) {
            const body = document.getElementById("term-body");
            const block = document.createElement("div");
            block.className = "term-block";
            
            let html = `<div class="term-op">═══ ${operation} ═══</div>`;
            if (subtitle) html += `<div class="term-line" style="color:var(--accent2)">${subtitle}</div>`;
            
            if (Array.isArray(steps)) {
                steps.forEach(s => {
                    html += `<div class="term-line"><span class="term-arrow">›</span> ${s}</div>`;
                });
            }
            
            if (result) html += `<div class="term-line" style="margin-top:4px; font-weight:500">${result}</div>`;
            
            block.innerHTML = html;
            body.prepend(block);
        }

        // Graph Operations
        window.toggleMode = function() {
            directed = !directed;
            logTerm("MODE CHANGE", `Switched to ${directed ? "DIRECTED" : "UNDIRECTED"}`, [], "");
            updateGraph();
        }

        window.addNode = function() {
            const id = document.getElementById("nodeInput").value.trim();
            if (!id) return;
            if (nodes.find(n => n.id === id)) {
                logTerm("ADD NODE", "ERROR", [`Node '${id}' already exists`], "");
                return;
            }
            nodes.push({ 
                id, 
                x: width / 2 + (Math.random() - 0.5) * 60,
                y: height / 2 + (Math.random() - 0.5) * 60
            });
            logTerm("ADD NODE", `Added: ${id}`, [`Current nodes: ${nodes.length}`], "");
            document.getElementById("nodeInput").value = "";
            updateGraph();
        }

        window.removeNode = function() {
            const id = document.getElementById("nodeInput").value.trim();
            const idx = nodes.findIndex(n => n.id === id);
            if (idx === -1) return;
            nodes.splice(idx, 1);
            links = links.filter(l => (l.source.id || l.source) !== id && (l.target.id || l.target) !== id);
            logTerm("REMOVE NODE", `Removed: ${id}`, [`Remaining nodes: ${nodes.length}`], "");
            updateGraph();
        }

        window.addEdge = function() {
            const s = document.getElementById("fromInput").value.trim();
            const t = document.getElementById("toInput").value.trim();
            const w = parseInt(document.getElementById("weightInput").value) || 1;
            
            if (!s || !t) return;
            if (!nodes.find(n => n.id === s) || !nodes.find(n => n.id === t)) {
                logTerm("LINK NODES", "ERROR", ["One or both nodes not found"], "");
                return;
            }

            links.push({ source: nodes.find(n=>n.id===s), target: nodes.find(n=>n.id===t), weight: w });
            logTerm("LINK NODES", `Source: ${s} → Target: ${t}`, [`Weight: ${w}`], "");
            updateGraph();
        }

        window.removeEdge = function() {
            const s = document.getElementById("fromInput").value.trim();
            const t = document.getElementById("toInput").value.trim();
            links = links.filter(l => {
                const sid = typeof l.source === 'object' ? l.source.id : l.source;
                const tid = typeof l.target === 'object' ? l.target.id : l.target;
                return !(sid === s && tid === t);
            });
            logTerm("UNLINK NODES", `Removed edge: ${s} → ${t}`, [], "");
            updateGraph();
        }

        window.loadSampleNetwork = function() {
            nodes = [];
            links = [];
            const users = ["Aarav", "Ishani", "Rohan", "Priya", "Arjun", "Sneha", "Vihaan", "Ananya"];
            users.forEach(id => nodes.push({ 
                id, 
                x: width / 2 + (Math.random() - 0.5) * 100,
                y: height / 2 + (Math.random() - 0.5) * 100
            }));
            
            const sampleLinks = [
                {s:"Aarav", t:"Ishani", w:3}, {s:"Aarav", t:"Rohan", w:1},
                {s:"Ishani", t:"Priya", w:4}, {s:"Ishani", t:"Arjun", w:2},
                {s:"Rohan", t:"Sneha", w:5}, {s:"Priya", t:"Vihaan", w:1},
                {s:"Arjun", t:"Ananya", w:3}, {s:"Sneha", t:"Ananya", w:2},
                {s:"Vihaan", t:"Ananya", w:6}
            ];
            
            sampleLinks.forEach(l => {
                const src = nodes.find(n => n.id === l.s);
                const tgt = nodes.find(n => n.id === l.t);
                if(src && tgt) links.push({ source: src, target: tgt, weight: l.w });
            });
            
            logTerm("SAMPLE NETWORK", "LOADING DATASET", users.map(u => `Added user: ${u}`), "Sample loaded successfully");
            updateGraph();
        }

        // --- ALGORITHMS ---

        function getAdj() {
            const adj = {};
            nodes.forEach(n => adj[n.id] = []);
            links.forEach(l => {
                const s = typeof l.source === 'object' ? l.source.id : l.source;
                const t = typeof l.target === 'object' ? l.target.id : l.target;
                adj[s].push({ node: t, weight: l.weight || 1 });
                if (!directed) adj[t].push({ node: s, weight: l.weight || 1 });
            });
            return adj;
        }

        function highlightNode(id, color) {
            nodeGroup.selectAll(".node")
                .filter(d => d.id === id)
                .select("circle")
                .transition().duration(300)
                .style("fill", color)
                .style("stroke", color);
        }

        function highlightEdge(u, v, color) {
            linkGroup.selectAll(".link")
                .filter(d => {
                    const sid = typeof d.source === 'object' ? d.source.id : d.source;
                    const tid = typeof d.target === 'object' ? d.target.id : d.target;
                    return (sid === u && tid === v) || (!directed && sid === v && tid === u);
                })
                .transition().duration(300)
                .style("stroke", color)
                .style("stroke-width", "4px");
        }

        window.clearHighlights = function() {
            nodeGroup.selectAll("circle")
                .transition().duration(300)
                .style("fill", "#1a2a3a")
                .style("stroke", "#4fc3f7");
            
            linkGroup.selectAll(".link")
                .transition().duration(300)
                .style("stroke", "#1e2740")
                .style("stroke-width", "2px");
        }

        window.runBFS = function() {
            const start = document.getElementById("startInput").value.trim();
            if (!start || !nodes.find(n => n.id === start)) {
                logTerm("BFS", "ERROR", ["Start node not found"], "");
                return;
            }
            clearHighlights();
            const adj = getAdj();
            const visited = [], queue = [start], seen = new Set([start]);
            const steps = [];
            
            while (queue.length) {
                const curr = queue.shift();
                visited.push(curr);
                steps.push(`Queue: [${queue.join(", ")}] | Visiting: ${curr}`);
                (adj[curr] || []).forEach(({node}) => {
                    if (!seen.has(node)) {
                        seen.add(node);
                        queue.push(node);
                    }
                });
            }
            
            logTerm("BFS TRAVERSAL", `Start: ${start}`, steps, `Order: ${visited.join(" → ")}`);
            visited.forEach((id, i) => {
                setTimeout(() => highlightNode(id, "#4fc3f7"), i * 400);
            });
        }

        window.runDFS = function() {
            const start = document.getElementById("startInput").value.trim();
            if (!start || !nodes.find(n => n.id === start)) {
                logTerm("DFS", "ERROR", ["Start node not found"], "");
                return;
            }
            clearHighlights();
            const adj = getAdj();
            const visited = [], stack = [start], seen = new Set(), steps = [];
            
            while (stack.length) {
                const curr = stack.pop();
                if (seen.has(curr)) continue;
                seen.add(curr);
                visited.push(curr);
                steps.push(`Stack Top: ${curr} | Exploring Neighbors`);
                
                const neighbors = (adj[curr] || []).slice().reverse();
                neighbors.forEach(({node}) => {
                    if (!seen.has(node)) stack.push(node);
                });
            }
            
            logTerm("DFS TRAVERSAL", `Start: ${start}`, steps, `Order: ${visited.join(" → ")}`);
            visited.forEach((id, i) => {
                setTimeout(() => highlightNode(id, "#9575cd"), i * 400);
            });
        }

        window.runDijkstra = function() {
            const start = document.getElementById("startInput").value.trim();
            const end = document.getElementById("endInput").value.trim();
            if (!start || !nodes.find(n => n.id === start)) {
                logTerm("DIJKSTRA", "ERROR", ["Start node not found"], "");
                return;
            }
            clearHighlights();
            const adj = getAdj();
            const dist = {}, prev = {}, steps = [];
            nodes.forEach(n => dist[n.id] = Infinity);
            dist[start] = 0;
            
            const pq = [{id: start, d: 0}];
            while (pq.length) {
                pq.sort((a,b) => a.d - b.d);
                const {id: u} = pq.shift();
                
                (adj[u] || []).forEach(({node: v, weight: w}) => {
                    const nd = dist[u] + w;
                    steps.push(`Relaxing ${u}→${v} | current dist[${v}]=${dist[v]===Infinity?"∞":dist[v]} | new=${nd}`);
                    if (nd < dist[v]) {
                        dist[v] = nd;
                        prev[v] = u;
                        pq.push({id: v, d: nd});
                    }
                });
            }
            
            let path = [];
            if (end && dist[end] !== Infinity) {
                let curr = end;
                while (curr) {
                    path.unshift(curr);
                    curr = prev[curr];
                }
            }
            
            logTerm("DIJKSTRA", `${start} → ${end || 'ALL'}`, steps, 
                end ? (path[0] === start ? `Path: ${path.join(" → ")} | Cost: ${dist[end]}` : "No path found") : "Calculation complete");
            
            if (path.length) {
                path.forEach((id, i) => {
                    setTimeout(() => highlightNode(id, "#ffb74d"), i * 400);
                    if (i > 0) setTimeout(() => highlightEdge(path[i-1], path[i], "#ffb74d"), i * 400);
                });
            }
        }

        window.runPrims = function() {
            if (nodes.length === 0) return;
            const startInput = document.getElementById("startInput").value.trim();
            const start = startInput || nodes[0].id;
            
            clearHighlights();
            const adj = getAdj();
            const inMST = new Set([start]), mstEdges = [], steps = [];
            let totalCost = 0;
            
            while (inMST.size < nodes.length) {
                let best = null, bestW = Infinity;
                inMST.forEach(u => {
                    (adj[u] || []).forEach(({node: v, weight: w}) => {
                        if (!inMST.has(v) && w < bestW) {
                            best = {u, v, w};
                            bestW = w;
                        }
                    });
                });
                
                if (!best) break;
                inMST.add(best.v);
                mstEdges.push(best);
                totalCost += best.w;
                steps.push(`Added edge ${best.u}—${best.v} (weight: ${best.w})`);
            }
            
            logTerm("PRIM'S MST", `Start: ${start}`, steps, `Total Weight: ${totalCost}`);
            mstEdges.forEach((e, i) => {
                setTimeout(() => {
                    highlightNode(e.u, "#81c784");
                    highlightNode(e.v, "#81c784");
                    highlightEdge(e.u, e.v, "#81c784");
                }, i * 500);
            });
        }

        // Initialize graph
        updateGraph();
    </script>
</body>
</html>
)HTML";

    file.close();
    printSuccess("> Visualization generated: " + filename);
}
