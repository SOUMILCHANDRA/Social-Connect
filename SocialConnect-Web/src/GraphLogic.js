export class Graph {
  constructor(mode = 'UNDIRECTED') {
    this.adjList = new Map();
    this.mode = mode;
    this.onLog = null; 
    this.nodes = [];
    this.links = [];
    this.directed = false;
    this.sse = null;
    this.onUpdate = null; // Callback for React re-render

    this.connect();
  }

  log(message, type = 'info') {
    if (this.onLog) this.onLog(message, type);
  }

  connect() {
    if (this.sse) this.sse.close();
    
    const host = window.location.hostname + ':8080';
    this.sse = new EventSource(`http://${host}/events`);
    
    this.sse.onopen = () => {
      this.log('SSE BRIDGE: Connected to C++ Backend', 'success');
      this.fetchState(host);
    };

    this.sse.onerror = (e) => {
      this.log('SSE BRIDGE: Disconnected. Retrying...', 'error');
    };

    this.sse.onmessage = (event) => {
      const data = JSON.parse(event.data);
      this.handleEvent(data);
    };
  }

  async fetchState(host) {
    try {
      const res = await fetch(`http://${host || (window.location.hostname + ':8080')}/state`);
      const data = await res.json();
      this.updateLocalState(data);
    } catch (e) {
      console.error('Fetch state failed', e);
    }
  }

  updateLocalState(data) {
    this.nodes = data.nodes || [];
    this.links = data.links || [];
    this.directed = data.directed || false;
    this.mode = this.directed ? 'DIRECTED' : 'UNDIRECTED';
    
    // Update adjList size for UI node count display
    this.adjList = new Map(this.nodes.map(n => [n.id, []]));
    
    if (this.onUpdate) this.onUpdate();
  }

  handleEvent(e) {
    switch(e.type) {
      case "STATE_SYNC":
        this.updateLocalState(e);
        break;
      case "LOG":
        e.lines.forEach(line => this.log(line, 'step'));
        break;
      case "BFS_STEP":
      case "DFS_STEP":
        this.log(`${e.type}: Visiting ${e.node}`, 'info');
        // We could also highlight in D3 here if we had a reference
        break;
      case "BFS_COMPLETE":
      case "DFS_COMPLETE":
        this.log(`${e.type}: ${e.order.join(' -> ')}`, 'success');
        break;
      case "DIJKSTRA_RELAX":
        if (e.updated) this.log(`Dijkstra: Updated ${e.v} (Dist: ${e.dist})`, 'info');
        break;
      case "DIJKSTRA_COMPLETE":
        this.log(`Dijkstra: Path Found! Cost: ${e.cost} | ${e.path.join(' -> ')}`, 'success');
        break;
      case "MST_EDGE":
        this.log(`MST: Added ${e.u} -- ${e.v}`, 'info');
        break;
      case "MST_COMPLETE":
        this.log(`MST: Found! Total Cost: ${e.totalCost}`, 'success');
        break;
    }
  }

  async apiCommand(command, params) {
    const host = window.location.hostname + ':8080';
    try {
      await fetch(`http://${host}/api/command`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ command, ...params })
      });
    } catch (e) {
      this.log('API Error: ' + e.message, 'error');
    }
  }

  addNode(name) {
    if (!name) return false;
    this.apiCommand('add_node', { name });
    return true; 
  }

  addEdge(from, to, weight = 1) {
    if (!from || !to) return false;
    this.apiCommand('add_edge', { from, to, weight });
    return true;
  }

  removeNode(name) {
    this.apiCommand('remove_node', { name });
    return true;
  }

  bfs(start) { this.apiCommand('bfs', { start }); }
  dfs(start) { this.apiCommand('dfs', { start }); }
  dijkstra(start, end) { this.apiCommand('dijkstra', { start, end }); }
  primMST(start) { this.apiCommand('prims', { start: start || this.nodes[0]?.id }); }

  getD3Data() {
    return { nodes: this.nodes, links: this.links.map(l => ({
      source: l.source,
      target: l.target,
      value: l.weight
    })) };
  }
}
