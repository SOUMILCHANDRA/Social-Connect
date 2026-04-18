#include "Visualizer.h"

#include <fstream>
#include <set>
#include <sstream>

void Visualizer::debug(const std::string& message) {
    std::cerr << "[DEBUG] " << message << std::endl;
}

void Visualizer::header(const std::string& title) {
    std::cout << CYAN << BOLD << "\n═══ " << title << " ═══" << RESET << "\n";
    std::cout.flush();
}

void Visualizer::success(const std::string& message) {
    std::cout << GREEN << "› " << message << RESET << "\n";
    std::cout.flush();
}

void Visualizer::info(const std::string& message) {
    std::cout << "› " << message << "\n";
    std::cout.flush();
}

void Visualizer::warn(const std::string& message) {
    std::cout << YELLOW << "› " << message << RESET << "\n";
    std::cout.flush();
}

void Visualizer::error(const std::string& message) {
    std::cout << RED << "› " << message << RESET << "\n";
    std::cout.flush();
}

void Visualizer::divider() {
    std::cout << LINE << "\n";
    std::cout.flush();
}

std::string Visualizer::join(const std::vector<std::string>& values, const std::string& separator) {
    std::string result;
    for (std::size_t index = 0; index < values.size(); ++index) {
        result += values[index];
        if (index + 1 < values.size()) {
            result += separator;
        }
    }
    return result;
}

void Visualizer::displayAdjList(const Graph& graph) {
    const auto& adjacency = graph.getAdjacency();
    for (const auto& node : graph.getNodes()) {
        std::cout << "  " << node << " -> [";
        const auto& neighbors = adjacency.at(node);
        for (std::size_t index = 0; index < neighbors.size(); ++index) {
            std::cout << neighbors[index].to << "(" << neighbors[index].weight << ")";
            if (index + 1 < neighbors.size()) {
                std::cout << ", ";
            }
        }
        std::cout << "]\n";
    }
}

void Visualizer::displayASCII(const Graph& graph) {
    header("ASCII GRAPH VIEW");
    displayAdjList(graph);
    divider();
}

void Visualizer::generateHTML(const Graph& graph, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        error("Failed to create HTML visualizer file.");
        return;
    }

    const auto nodes = graph.getNodes();
    const auto& adjacency = graph.getAdjacency();

    std::stringstream nodesJson;
    nodesJson << "[\n";
    for (std::size_t index = 0; index < nodes.size(); ++index) {
        nodesJson << "      { \"id\": \"" << nodes[index] << "\" }";
        if (index + 1 < nodes.size()) {
            nodesJson << ",";
        }
        nodesJson << "\n";
    }
    nodesJson << "    ]";

    std::stringstream linksJson;
    std::set<std::string> handled;
    bool first = true;
    linksJson << "[\n";
    for (const auto& node : nodes) {
        auto iterator = adjacency.find(node);
        if (iterator == adjacency.end()) {
            continue;
        }
        for (const auto& edge : iterator->second) {
            std::string key = node + "->" + edge.to;
            if (!graph.isDirected()) {
                key = node < edge.to ? node + "<->" + edge.to : edge.to + "<->" + node;
            }
            if (!handled.insert(key).second) {
                continue;
            }

            if (!first) {
                linksJson << ",\n";
            }
            first = false;
            linksJson << "      { \"source\": \"" << node << "\", \"target\": \"" << edge.to
                      << "\", \"weight\": " << edge.weight << " }";
        }
    }
    linksJson << "\n    ]";

    file << R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>SocialConnect Graph Visualizer</title>
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=IBM+Plex+Sans:wght@400;500;600&family=JetBrains+Mono:wght@400;600&display=swap" rel="stylesheet">
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
    * { box-sizing: border-box; }
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
    #header .title {
      font-size: 14px;
      letter-spacing: 0.08em;
      font-weight: 600;
    }
    #header .meta {
      color: var(--text-dim);
      font-size: 12px;
      font-family: 'JetBrains Mono', monospace;
    }
    #main { display: flex; flex: 1; overflow: hidden; }
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
      background:
        radial-gradient(circle at top, rgba(79, 195, 247, 0.05), transparent 28%),
        linear-gradient(180deg, #0b0e14 0%, #0d1321 100%);
    }
    #graph-panel svg { width: 100%; height: 100%; display: block; }
    #terminal-panel {
      width: 340px;
      min-width: 340px;
      background: #080c14;
      border-left: 1px solid var(--border);
      font-family: 'JetBrains Mono', monospace;
      font-size: 11px;
      overflow-y: auto;
      padding: 10px;
      display: flex;
      flex-direction: column;
    }
    .panel-section {
      display: flex;
      flex-direction: column;
      gap: 8px;
      padding: 10px;
      border: 1px solid var(--border);
      background: var(--bg-card);
    }
    .section-label {
      color: var(--text-dim);
      font-size: 10px;
      letter-spacing: 0.1em;
      font-weight: 600;
    }
    input, button {
      width: 100%;
      border: 1px solid var(--border);
      background: #0e1420;
      color: var(--text);
      padding: 8px 9px;
      font-size: 12px;
      font-family: inherit;
    }
    input:focus, button:focus { outline: none; border-color: var(--accent); }
    button {
      background: linear-gradient(180deg, #172033 0%, #121a29 100%);
      cursor: pointer;
      transition: background 0.2s ease, border-color 0.2s ease, transform 0.2s ease;
      text-align: left;
    }
    button:hover {
      background: #1a2335;
      border-color: #33507a;
      transform: translateY(-1px);
    }
    #term-header {
      color: var(--text-dim);
      font-size: 10px;
      border-bottom: 1px solid var(--border);
      padding-bottom: 8px;
      margin-bottom: 10px;
      letter-spacing: 0.08em;
    }
    #term-body { display: flex; flex-direction: column; }
    .term-block {
      border-bottom: 1px solid var(--border);
      margin-bottom: 8px;
      padding-bottom: 8px;
    }
    .term-op { color: var(--accent); font-weight: 700; margin-bottom: 4px; }
    .term-line { color: var(--text); padding-left: 8px; line-height: 1.45; word-break: break-word; }
    .term-arrow { color: var(--accent); margin-right: 4px; }
    .legend, .graph-status {
      position: absolute;
      left: 14px;
      background: rgba(17, 21, 32, 0.86);
      border: 1px solid var(--border);
      font-family: 'JetBrains Mono', monospace;
      backdrop-filter: blur(6px);
    }
    .graph-status {
      top: 14px;
      display: flex;
      gap: 10px;
      padding: 8px 10px;
      z-index: 2;
      font-size: 11px;
      color: var(--text-dim);
    }
    .legend {
      bottom: 14px;
      padding: 10px 12px;
      font-size: 11px;
      color: var(--text-dim);
    }
    .legend strong, .graph-status strong { color: var(--text); }
    .hint { color: var(--text-dim); font-size: 10px; line-height: 1.4; }
  </style>
</head>
<body>
  <div id="header">
    <div class="title">SOCIAL CONNECT — Graph Network Analyzer</div>
    <div class="meta" id="header-meta">NODES: 0 | MODE: UNDIRECTED</div>
  </div>
  <div id="main">
    <div id="left-panel">
      <div class="panel-section">
        <div class="section-label">GRAPH CONTROL</div>
        <button id="toggleModeBtn">UNDIRECTED / DIRECTED</button>
        <button id="loadSampleBtn">Load Sample Network</button>
      </div>
      <div class="panel-section">
        <div class="section-label">NODE MANAGEMENT</div>
        <input id="nodeInput" placeholder="User ID" />
        <div class="hint">Click any node in the graph to load its ID into the controls.</div>
        <button id="addNodeBtn">+ Add Node</button>
        <button id="removeNodeBtn">✕ Remove Node</button>
      </div>
      <div class="panel-section">
        <div class="section-label">EDGE MANAGEMENT</div>
        <input id="fromInput" placeholder="From" />
        <input id="toInput" placeholder="To" />
        <input id="weightInput" placeholder="Weight (1-10)" type="number" min="1" max="10" value="1" />
        <button id="addEdgeBtn">Link Nodes</button>
        <button id="removeEdgeBtn">Unlink Nodes</button>
      </div>
      <div class="panel-section">
        <div class="section-label">ALGORITHMS</div>
        <input id="startInput" placeholder="Start Node" />
        <input id="endInput" placeholder="End Node (optional)" />
        <button id="bfsBtn">▶ BFS Traverse</button>
        <button id="dfsBtn">▶ DFS Traverse</button>
        <button id="dijkstraBtn">▶ Dijkstra Path</button>
        <button id="primsBtn">▶ Prim's MST</button>
        <button id="clearBtn">↺ Clear Highlights</button>
      </div>
    </div>
    <div id="graph-panel">
      <div class="graph-status">
        <div><strong id="stat-nodes">0</strong> nodes</div>
        <div><strong id="stat-links">0</strong> links</div>
        <div>mode: <strong id="stat-mode">undirected</strong></div>
        <div>selection: <strong id="stat-selection">none</strong></div>
      </div>
      <div class="legend">
        <strong>Legend</strong>
        Default node: Research blue<br />
        BFS: Ice blue<br />
        DFS: Violet<br />
        Path: Orange<br />
        MST: Green
      </div>
    </div>
    <div id="terminal-panel">
      <div id="term-header">SESSION: GRAPH_ANALYZER@CORE &nbsp;&nbsp; TTY1</div>
      <div id="term-body"></div>
    </div>
  </div>
  <script>
    const terminalLog = [];
    let nodes = )HTML" << nodesJson.str() << R"HTML(;
    let links = )HTML" << linksJson.str() << R"HTML(;
    let directed = )HTML" << (graph.isDirected() ? "true" : "false") << R"HTML(;
    let nodeSelection;
    let nodeLabelSelection;
    let linkSelection;
    let weightSelection;
    let selectedNodeId = null;

    const highlightState = {
      nodes: new Set(),
      dfsNodes: new Set(),
      pathNodes: new Set(),
      mstNodes: new Set(),
      pathEdges: new Set(),
      mstEdges: new Set()
    };

    const graphPanel = document.getElementById("graph-panel");
    const termBody = document.getElementById("term-body");
    const headerMeta = document.getElementById("header-meta");
    const statNodes = document.getElementById("stat-nodes");
    const statLinks = document.getElementById("stat-links");
    const statMode = document.getElementById("stat-mode");
    const statSelection = document.getElementById("stat-selection");

    const svg = d3.select("#graph-panel").append("svg").attr("preserveAspectRatio", "xMidYMid meet");
    const defs = svg.append("defs");
    const gridPattern = defs.append("pattern").attr("id", "dot-grid").attr("width", 30).attr("height", 30).attr("patternUnits", "userSpaceOnUse");
    gridPattern.append("circle").attr("cx", 15).attr("cy", 15).attr("r", 1.1).attr("fill", "#1a2035").attr("opacity", 0.5);
    defs.append("marker")
      .attr("id", "arrowhead")
      .attr("viewBox", "-0 -5 10 10")
      .attr("refX", 26)
      .attr("refY", 0)
      .attr("orient", "auto")
      .attr("markerWidth", 6)
      .attr("markerHeight", 6)
      .append("path")
      .attr("d", "M 0,-5 L 10,0 L 0,5")
      .attr("fill", "#4fc3f7");
    const backgroundRect = svg.append("rect").attr("fill", "url(#dot-grid)");
    const graphLayer = svg.append("g");
    const linkLayer = graphLayer.append("g");
    const weightLayer = graphLayer.append("g");
    const nodeLayer = graphLayer.append("g");
    const labelLayer = graphLayer.append("g");

    const simulation = d3.forceSimulation()
      .force("link", d3.forceLink().id(d => d.id).distance(120))
      .force("charge", d3.forceManyBody().strength(-300))
      .force("center", d3.forceCenter())
      .force("x", d3.forceX().strength(0.05))
      .force("y", d3.forceY().strength(0.05))
      .force("collision", d3.forceCollide(35));

    function width() { return graphPanel.clientWidth || window.innerWidth * 0.6; }
    function height() { return graphPanel.clientHeight || window.innerHeight; }
    function normalizeLinkEndpoints(link) { return { source: typeof link.source === "object" ? link.source.id : link.source, target: typeof link.target === "object" ? link.target.id : link.target }; }
    function edgeKey(a, b) { return directed ? `${a}->${b}` : [a, b].sort().join("<->"); }
    function syncViewport() {
      const w = width(), h = height();
      svg.attr("viewBox", `0 0 ${w} ${h}`);
      backgroundRect.attr("width", w).attr("height", h);
      simulation.force("center", d3.forceCenter(w / 2, h / 2));
      simulation.force("x", d3.forceX(w / 2).strength(0.05));
      simulation.force("y", d3.forceY(h / 2).strength(0.05));
    }
    function updateHeader() {
      headerMeta.textContent = `NODES: ${nodes.length} | MODE: ${directed ? "DIRECTED" : "UNDIRECTED"}`;
      statNodes.textContent = String(nodes.length);
      statLinks.textContent = String(links.length);
      statMode.textContent = directed ? "directed" : "undirected";
      statSelection.textContent = selectedNodeId || "none";
    }
    function buildAdjList() {
      const adj = {};
      nodes.forEach(node => { adj[node.id] = []; });
      links.forEach(link => {
        const { source, target } = normalizeLinkEndpoints(link);
        const weight = Number(link.weight || 1);
        if (adj[source]) adj[source].push({ node: target, weight });
        if (!directed && adj[target]) adj[target].push({ node: source, weight });
      });
      Object.values(adj).forEach(neighbors => neighbors.sort((a, b) => a.node.localeCompare(b.node)));
      return adj;
    }
    function formatAdjacencySnapshot() {
      const adj = buildAdjList();
      return Object.keys(adj).sort((a, b) => a.localeCompare(b)).map(nodeId => {
        const neighbors = adj[nodeId].map(({ node, weight }) => `${node}(${weight})`).join(", ");
        return `${nodeId} -> [${neighbors}]`;
      });
    }
    function clearHighlightState() {
      highlightState.nodes.clear();
      highlightState.dfsNodes.clear();
      highlightState.pathNodes.clear();
      highlightState.mstNodes.clear();
      highlightState.pathEdges.clear();
      highlightState.mstEdges.clear();
    }
    function resolveNodeColor(id) {
      if (highlightState.pathNodes.has(id)) return "#ffb74d";
      if (highlightState.mstNodes.has(id)) return "#81c784";
      if (highlightState.dfsNodes.has(id)) return "#9575cd";
      if (highlightState.nodes.has(id)) return "#4fc3f7";
      return "#1a2a3a";
    }
    function resolveEdgeColor(link) {
      const { source, target } = normalizeLinkEndpoints(link);
      const key = edgeKey(source, target);
      if (highlightState.pathEdges.has(key)) return "#ffb74d";
      if (highlightState.mstEdges.has(key)) return "#81c784";
      return "#1e2740";
    }
    function resolveEdgeWidth(link) {
      const { source, target } = normalizeLinkEndpoints(link);
      const key = edgeKey(source, target);
      return highlightState.pathEdges.has(key) || highlightState.mstEdges.has(key) ? 3 : 2;
    }
    function applyHighlights() {
      if (nodeSelection) {
        nodeSelection.attr("fill", d => resolveNodeColor(d.id)).attr("stroke", d => resolveNodeColor(d.id) === "#1a2a3a" ? "#4fc3f7" : resolveNodeColor(d.id));
        nodeSelection.attr("stroke-width", d => d.id === selectedNodeId ? 4 : 2);
      }
      if (linkSelection) {
        linkSelection.attr("stroke", d => resolveEdgeColor(d)).attr("stroke-width", d => resolveEdgeWidth(d)).attr("marker-end", d => directed ? "url(#arrowhead)" : null);
      }
    }
    function logTerm(operation, ...lines) {
      const flatLines = lines.flat().filter(line => line !== undefined && line !== null && String(line).trim() !== "");
      terminalLog.unshift({ operation, lines: flatLines.map(line => String(line)) });
      const block = document.createElement("div");
      block.className = "term-block";
      const title = document.createElement("div");
      title.className = "term-op";
      title.textContent = `═══ ${operation} ═══`;
      block.appendChild(title);
      flatLines.forEach(line => {
        const lineDiv = document.createElement("div");
        lineDiv.className = "term-line";
        lineDiv.innerHTML = `<span class="term-arrow">›</span>${line}`;
        block.appendChild(lineDiv);
      });
      termBody.prepend(block);
      document.getElementById("terminal-panel").scrollTop = 0;
    }
    function logToTerminal(operation, lines) { logTerm(operation, ...(Array.isArray(lines) ? lines : [lines])); }
    function setSelection(id) {
      selectedNodeId = id || null;
      if (selectedNodeId) {
        document.getElementById("nodeInput").value = selectedNodeId;
        if (!document.getElementById("startInput").value.trim()) document.getElementById("startInput").value = selectedNodeId;
        if (!document.getElementById("fromInput").value.trim()) {
          document.getElementById("fromInput").value = selectedNodeId;
        } else if (!document.getElementById("toInput").value.trim() && document.getElementById("fromInput").value.trim() !== selectedNodeId) {
          document.getElementById("toInput").value = selectedNodeId;
        }
      }
      updateHeader();
      applyHighlights();
    }
    function highlightNode(id, color) {
      if (color === "#4fc3f7") highlightState.nodes.add(id);
      if (color === "#9575cd") highlightState.dfsNodes.add(id);
      if (color === "#ffb74d") highlightState.pathNodes.add(id);
      if (color === "#81c784") highlightState.mstNodes.add(id);
      applyHighlights();
    }
    function highlightEdge(from, to, color) {
      const key = edgeKey(from, to);
      if (color === "#ffb74d") highlightState.pathEdges.add(key);
      if (color === "#81c784") highlightState.mstEdges.add(key);
      applyHighlights();
    }
    function clearHighlights(log = true) {
      clearHighlightState();
      applyHighlights();
      if (log) logTerm("CLEAR HIGHLIGHTS", "Reset all node and edge colors to default.");
    }
    function updateGraph() {
      syncViewport();
      linkSelection = linkLayer.selectAll("line").data(links, d => {
        const endpoints = normalizeLinkEndpoints(d);
        return `${edgeKey(endpoints.source, endpoints.target)}:${d.weight ?? 1}`;
      });
      linkSelection.exit().remove();
      const linkEnter = linkSelection.enter().append("line").attr("stroke", "#1e2740").attr("stroke-width", 2).attr("marker-end", d => directed ? "url(#arrowhead)" : null);
      linkSelection = linkEnter.merge(linkSelection);

      weightSelection = weightLayer.selectAll("text").data(links, d => {
        const endpoints = normalizeLinkEndpoints(d);
        return `label:${edgeKey(endpoints.source, endpoints.target)}:${d.weight ?? 1}`;
      });
      weightSelection.exit().remove();
      const weightEnter = weightSelection.enter().append("text").attr("text-anchor", "middle").attr("font-family", "JetBrains Mono").attr("font-size", 10).attr("fill", "#6c7a96").attr("pointer-events", "none");
      weightSelection = weightEnter.merge(weightSelection).text(d => d.weight ?? 1);

      const dragBehavior = d3.drag()
        .on("start", (event, d) => { if (!event.active) simulation.alphaTarget(0.3).restart(); d.fx = d.x; d.fy = d.y; })
        .on("drag", (event, d) => { d.fx = Math.max(30, Math.min(width() - 30, event.x)); d.fy = Math.max(30, Math.min(height() - 30, event.y)); })
        .on("end", (event, d) => { if (!event.active) simulation.alphaTarget(0); d.fx = null; d.fy = null; });

      nodeSelection = nodeLayer.selectAll("circle").data(nodes, d => d.id);
      nodeSelection.exit().remove();
      const nodeEnter = nodeSelection.enter().append("circle").attr("r", 18).attr("fill", "#1a2a3a").attr("stroke", "#4fc3f7").attr("stroke-width", 2).call(dragBehavior);
      nodeSelection = nodeEnter.merge(nodeSelection);
      nodeSelection.on("click", (_, d) => {
        setSelection(d.id);
        logToTerminal("NODE SELECT", [`Active node: ${d.id}`, `Neighbors: ${(buildAdjList()[d.id] || []).map(item => `${item.node}(${item.weight})`).join(", ") || "none"}`]);
      });

      nodeLabelSelection = labelLayer.selectAll("text").data(nodes, d => d.id);
      nodeLabelSelection.exit().remove();
      const labelEnter = nodeLabelSelection.enter().append("text").attr("text-anchor", "middle").attr("dominant-baseline", "middle").attr("font-family", "JetBrains Mono").attr("font-size", 10).attr("fill", "#cdd6f4").attr("pointer-events", "none");
      nodeLabelSelection = labelEnter.merge(nodeLabelSelection).text(d => d.id);

      simulation.nodes(nodes).on("tick", () => {
        nodeSelection.attr("cx", d => d.x = Math.max(30, Math.min(width() - 30, d.x))).attr("cy", d => d.y = Math.max(30, Math.min(height() - 30, d.y)));
        nodeLabelSelection.attr("x", d => d.x).attr("y", d => d.y);
        linkSelection
          .attr("x1", d => (typeof d.source === "object" ? d.source.x : nodes.find(node => node.id === d.source)?.x) ?? width() / 2)
          .attr("y1", d => (typeof d.source === "object" ? d.source.y : nodes.find(node => node.id === d.source)?.y) ?? height() / 2)
          .attr("x2", d => (typeof d.target === "object" ? d.target.x : nodes.find(node => node.id === d.target)?.x) ?? width() / 2)
          .attr("y2", d => (typeof d.target === "object" ? d.target.y : nodes.find(node => node.id === d.target)?.y) ?? height() / 2);
        weightSelection
          .attr("x", d => (((typeof d.source === "object" ? d.source.x : nodes.find(node => node.id === d.source)?.x) ?? 0) + ((typeof d.target === "object" ? d.target.x : nodes.find(node => node.id === d.target)?.x) ?? 0)) / 2)
          .attr("y", d => ((((typeof d.source === "object" ? d.source.y : nodes.find(node => node.id === d.source)?.y) ?? 0) + ((typeof d.target === "object" ? d.target.y : nodes.find(node => node.id === d.target)?.y) ?? 0)) / 2) - 8);
      });

      simulation.force("link").links(links);
      simulation.alpha(0.5).restart();
      applyHighlights();
      updateHeader();
    }
    function addNode(customId) {
      const id = (customId ?? document.getElementById("nodeInput").value).trim();
      if (!id) return logTerm("ADD NODE", "ERROR: User ID is required.");
      if (nodes.some(node => node.id === id)) return logTerm("ADD NODE", `User already exists: ${id}`);
      nodes.push({ id, x: width() / 2 + (Math.random() - 0.5) * 60, y: height() / 2 + (Math.random() - 0.5) * 60 });
      updateGraph();
      setSelection(id);
      logTerm("ADD NODE", `Adding user: ${id}`, `Current graph size: ${nodes.length} node(s)`, formatAdjacencySnapshot());
      document.getElementById("nodeInput").value = "";
    }
    function removeNode() {
      const id = document.getElementById("nodeInput").value.trim();
      if (!id) return logTerm("REMOVE NODE", "ERROR: User ID is required.");
      if (!nodes.some(node => node.id === id)) return logTerm("REMOVE NODE", `Node not found: ${id}`);
      nodes = nodes.filter(node => node.id !== id);
      links = links.filter(link => { const endpoints = normalizeLinkEndpoints(link); return endpoints.source !== id && endpoints.target !== id; });
      clearHighlights(false);
      updateGraph();
      if (selectedNodeId === id) setSelection(null);
      logTerm("REMOVE NODE", `Removed user: ${id}`, `Current graph size: ${nodes.length} node(s)`, formatAdjacencySnapshot());
      document.getElementById("nodeInput").value = "";
    }
    function addEdge(fromValue, toValue, weightValue) {
      const from = (fromValue ?? document.getElementById("fromInput").value).trim();
      const to = (toValue ?? document.getElementById("toInput").value).trim();
      const weight = Number(weightValue ?? document.getElementById("weightInput").value ?? 1);
      if (!from || !to) return logTerm("ADD CONNECTION", "ERROR: Both From and To fields are required.");
      if (!nodes.find(node => node.id === from) || !nodes.find(node => node.id === to)) return logTerm("ADD CONNECTION", `ERROR: Missing node. From=${from}, To=${to}`);
      if (from === to) return logTerm("ADD CONNECTION", "ERROR: Self connections are not allowed.");
      const clampedWeight = Math.max(1, Math.min(10, Number.isFinite(weight) ? weight : 1));
      const existing = links.find(link => { const endpoints = normalizeLinkEndpoints(link); return edgeKey(endpoints.source, endpoints.target) === edgeKey(from, to); });
      if (existing) {
        existing.weight = clampedWeight;
        updateGraph();
        logTerm("ADD CONNECTION", `Updated edge: ${from} → ${to} (Weight: ${clampedWeight})`, formatAdjacencySnapshot());
      } else {
        links.push({ source: from, target: to, weight: clampedWeight });
        updateGraph();
        logTerm("ADD CONNECTION", `Added edge: ${from} → ${to} (Weight: ${clampedWeight})`, directed ? "Graph mode: Directed" : `Added reverse edge logically (Undirected): ${to} → ${from}`, formatAdjacencySnapshot());
      }
      document.getElementById("fromInput").value = "";
      document.getElementById("toInput").value = "";
      document.getElementById("weightInput").value = "1";
    }
    function removeEdge() {
      const from = document.getElementById("fromInput").value.trim();
      const to = document.getElementById("toInput").value.trim();
      if (!from || !to) return logTerm("REMOVE CONNECTION", "ERROR: Both From and To fields are required.");
      const originalLength = links.length;
      links = links.filter(link => { const endpoints = normalizeLinkEndpoints(link); return edgeKey(endpoints.source, endpoints.target) !== edgeKey(from, to); });
      if (links.length === originalLength) return logTerm("REMOVE CONNECTION", `Connection not found: ${from} ↔ ${to}`);
      clearHighlights(false);
      updateGraph();
      logTerm("REMOVE CONNECTION", `Removed edge: ${from} → ${to}`, formatAdjacencySnapshot());
      document.getElementById("fromInput").value = "";
      document.getElementById("toInput").value = "";
    }
    function runBFS() {
      clearHighlights(false);
      const start = document.getElementById("startInput").value.trim() || selectedNodeId || "";
      if (!start || !nodes.find(node => node.id === start)) return logTerm("BFS TRAVERSAL", `ERROR: Node not found: ${start}`);
      const adj = buildAdjList(), visited = [], queue = [start], seen = new Set([start]), steps = [];
      while (queue.length) {
        const current = queue.shift();
        visited.push(current);
        steps.push(`Queue state: [${queue.join(", ")}] | Visiting: ${current}`);
        (adj[current] || []).forEach(({ node }) => { if (!seen.has(node)) { seen.add(node); queue.push(node); } });
      }
      logTerm("BFS TRAVERSAL", `Start: ${start}`, steps, `Order: ${visited.join(" → ")}`);
      visited.forEach((id, index) => setTimeout(() => highlightNode(id, "#4fc3f7"), index * 400));
    }
    function runDFS() {
      clearHighlights(false);
      const start = document.getElementById("startInput").value.trim() || selectedNodeId || "";
      if (!start || !nodes.find(node => node.id === start)) return logTerm("DFS TRAVERSAL", `ERROR: Node not found: ${start}`);
      const adj = buildAdjList(), visited = [], stack = [start], seen = new Set(), steps = [];
      while (stack.length) {
        const current = stack.pop();
        if (seen.has(current)) continue;
        seen.add(current);
        visited.push(current);
        steps.push(`Stack: [${stack.join(", ")}] | Visiting: ${current} | Neighbors: ${(adj[current] || []).map(item => item.node).join(", ")}`);
        (adj[current] || []).slice().reverse().forEach(({ node }) => { if (!seen.has(node)) stack.push(node); });
      }
      logTerm("DFS TRAVERSAL", `Start: ${start}`, steps, `Order: ${visited.join(" → ")}`);
      visited.forEach((id, index) => setTimeout(() => highlightNode(id, "#9575cd"), index * 400));
    }
    function runDijkstra() {
      clearHighlights(false);
      const start = document.getElementById("startInput").value.trim() || selectedNodeId || "";
      const end = document.getElementById("endInput").value.trim();
      if (!start) return logTerm("DIJKSTRA", "ERROR: Start node required.");
      if (!nodes.find(node => node.id === start)) return logTerm("DIJKSTRA", `ERROR: Start node not found: ${start}`);
      if (end && !nodes.find(node => node.id === end)) return logTerm("DIJKSTRA", `ERROR: End node not found: ${end}`);
      const adj = buildAdjList(), dist = {}, prev = {}, steps = [], visited = new Set();
      nodes.forEach(node => { dist[node.id] = Infinity; });
      dist[start] = 0;
      const pq = [{ node: start, d: 0 }];
      while (pq.length) {
        pq.sort((a, b) => a.d - b.d);
        const { node: u } = pq.shift();
        if (visited.has(u)) continue;
        visited.add(u);
        if (end && u === end) break;
        (adj[u] || []).forEach(({ node: v, weight: w }) => {
          const nextDistance = dist[u] + w;
          const updated = nextDistance < dist[v];
          steps.push(`Relaxing edge ${u}→${v}, dist=${nextDistance} ${updated ? "✓ UPDATED" : "✗ kept"}`);
          if (updated) { dist[v] = nextDistance; prev[v] = u; pq.push({ node: v, d: nextDistance }); }
        });
      }
      if (!end) {
        const summary = nodes.map(node => `${node.id}:${Number.isFinite(dist[node.id]) ? dist[node.id] : "∞"}`).join(" | ");
        return logTerm("DIJKSTRA", `${start}→all`, steps, `Distances: ${summary}`);
      }
      const path = [];
      let cursor = end;
      while (cursor) {
        path.unshift(cursor);
        if (cursor === start) break;
        cursor = prev[cursor];
      }
      if (!path.length || path[0] !== start || !Number.isFinite(dist[end])) return logTerm("DIJKSTRA", `${start}→${end}`, steps, "No path found.");
      logTerm("DIJKSTRA", `${start}→${end}`, steps, `Path: ${path.join(" → ")} | Cost: ${dist[end]}`);
      path.forEach((id, index) => setTimeout(() => highlightNode(id, "#ffb74d"), index * 400));
      for (let index = 0; index < path.length - 1; index += 1) highlightEdge(path[index], path[index + 1], "#ffb74d");
    }
    function runPrims() {
      clearHighlights(false);
      if (directed) return logTerm("PRIM'S MST", "ERROR: Prim's algorithm requires UNDIRECTED mode.");
      const start = document.getElementById("startInput").value.trim() || selectedNodeId || nodes[0]?.id;
      if (!start) return logTerm("PRIM'S MST", "ERROR: No nodes in graph.");
      const adj = buildAdjList(), inMST = new Set([start]), mstEdges = [], steps = [];
      let totalCost = 0;
      while (inMST.size < nodes.length) {
        let best = null, bestWeight = Infinity;
        inMST.forEach(u => {
          (adj[u] || []).forEach(({ node: v, weight: w }) => { if (!inMST.has(v) && w < bestWeight) { best = { u, v, w }; bestWeight = w; } });
        });
        if (!best) break;
        inMST.add(best.v);
        mstEdges.push(best);
        totalCost += best.w;
        steps.push(`Added edge ${best.u}—${best.v} (weight: ${best.w}) | MST nodes: [${Array.from(inMST).join(", ")}]`);
      }
      logTerm("PRIM'S MST", `Start: ${start}`, steps, `Total MST weight: ${totalCost} | Edges: ${mstEdges.length}`);
      mstEdges.forEach(({ u, v }, index) => setTimeout(() => { highlightNode(u, "#81c784"); highlightNode(v, "#81c784"); highlightEdge(u, v, "#81c784"); }, index * 500));
    }
    function loadSampleNetwork() {
      nodes = [];
      links = [];
      clearHighlights(false);
      ["Alice", "Bob", "Charlie", "Dave", "Eve", "Frank", "Grace", "Henry"].forEach(name => {
        nodes.push({ id: name, x: width() / 2 + (Math.random() - 0.5) * 60, y: height() / 2 + (Math.random() - 0.5) * 60 });
        logTerm("ADD NODE", `Adding user: ${name}`, `Current graph size: ${nodes.length} node(s)`);
      });
      [["Alice", "Bob", 3], ["Alice", "Charlie", 1], ["Bob", "Dave", 4], ["Bob", "Eve", 2], ["Charlie", "Frank", 5], ["Dave", "Grace", 1], ["Eve", "Henry", 3], ["Frank", "Henry", 2], ["Grace", "Henry", 6]].forEach(([source, target, weight]) => {
        links.push({ source, target, weight });
        logTerm("ADD CONNECTION", `Added edge: ${source} → ${target} (Weight: ${weight})`, directed ? "Graph mode: Directed" : `Added reverse edge logically (Undirected): ${target} → ${source}`);
      });
      updateGraph();
      setSelection("Alice");
      document.getElementById("startInput").value = "Alice";
      document.getElementById("endInput").value = "Henry";
      logTerm("LOAD SAMPLE NETWORK", "Loaded users: Alice, Bob, Charlie, Dave, Eve, Frank, Grace, Henry", "Loaded 9 weighted connections.", formatAdjacencySnapshot());
    }
    function toggleMode() {
      directed = !directed;
      clearHighlights(false);
      updateGraph();
      logTerm("TOGGLE MODE", `Graph mode changed to ${directed ? "DIRECTED" : "UNDIRECTED"}.`);
    }
    document.getElementById("toggleModeBtn").addEventListener("click", toggleMode);
    document.getElementById("loadSampleBtn").addEventListener("click", loadSampleNetwork);
    document.getElementById("addNodeBtn").addEventListener("click", () => addNode());
    document.getElementById("removeNodeBtn").addEventListener("click", removeNode);
    document.getElementById("addEdgeBtn").addEventListener("click", () => addEdge());
    document.getElementById("removeEdgeBtn").addEventListener("click", removeEdge);
    document.getElementById("bfsBtn").addEventListener("click", runBFS);
    document.getElementById("dfsBtn").addEventListener("click", runDFS);
    document.getElementById("dijkstraBtn").addEventListener("click", runDijkstra);
    document.getElementById("primsBtn").addEventListener("click", runPrims);
    document.getElementById("clearBtn").addEventListener("click", () => clearHighlights(true));
    ["nodeInput", "fromInput", "toInput", "startInput", "endInput"].forEach(id => {
      document.getElementById(id).addEventListener("keydown", event => {
        if (event.key !== "Enter") return;
        if (id === "nodeInput") addNode();
        if (id === "fromInput" || id === "toInput") addEdge();
        if (id === "startInput") runBFS();
        if (id === "endInput") runDijkstra();
      });
    });
    window.addEventListener("resize", () => { syncViewport(); updateGraph(); });
    nodes = nodes.map(node => ({ ...node, x: width() / 2 + (Math.random() - 0.5) * 60, y: height() / 2 + (Math.random() - 0.5) * 60 }));
    syncViewport();
    updateGraph();
    logTerm("SESSION START", "Visualizer initialized.", "Use the left panel to create and analyze your network.");
  </script>
</body>
</html>
)HTML";

    file.close();
    success("Visualization generated: " + filename);
}
