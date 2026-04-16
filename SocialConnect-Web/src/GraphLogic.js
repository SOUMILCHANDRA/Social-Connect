export class Graph {
  constructor(mode = 'UNDIRECTED') {
    this.adjList = new Map();
    this.mode = mode;
    this.onLog = null; // Callback for terminal logs
  }

  log(message, type = 'info') {
    if (this.onLog) this.onLog(message, type);
  }

  addNode(name) {
    if (!this.adjList.has(name)) {
      this.adjList.set(name, []);
      this.log(`=== ADD NODE ===`, 'header');
      this.log(`Adding user: ${name}`, 'success');
      this.log(`Current graph size: ${this.adjList.size} node(s)`, 'info');
      return true;
    }
    this.log(`User ${name} already exists!`, 'error');
    return false;
  }

  addEdge(u, v, weight = 1) {
    if (!this.adjList.has(u) || !this.adjList.has(v)) {
      this.log(`One or both users do not exist!`, 'error');
      return false;
    }
    
    const uEdges = this.adjList.get(u);
    if (uEdges.some(e => e.node === v)) {
      this.log(`Connection already exists!`, 'error');
      return false;
    }

    this.log(`=== ADD CONNECTION ===`, 'header');
    uEdges.push({ node: v, weight });
    this.log(`Added edge: ${u} -> ${v} (Weight: ${weight})`, 'success');

    if (this.mode === 'UNDIRECTED') {
      this.adjList.get(v).push({ node: u, weight });
      this.log(`Added reverse edge (Undirected): ${v} -> ${u}`, 'success');
    }
    return true;
  }

  removeNode(name) {
    if (this.adjList.has(name)) {
      this.adjList.delete(name);
      for (let [node, edges] of this.adjList) {
        this.adjList.set(node, edges.filter(e => e.node !== name));
      }
      this.log(`=== REMOVE NODE ===`, 'header');
      this.log(`Removed user: ${name}`, 'success');
      return true;
    }
    return false;
  }

  async bfs(startNode) {
    if (!this.adjList.has(startNode)) return;
    this.log(`=== BFS TRAVERSAL from ${startNode} ===`, 'header');
    
    let queue = [startNode];
    let visited = new Set([startNode]);
    let order = [];

    while (queue.length > 0) {
      this.log(`Queue: [${queue.join(', ')}]`, 'step');
      let curr = queue.shift();
      order.push(curr);
      this.log(`Visiting: ${curr}`, 'step');

      for (let neighbor of this.adjList.get(curr)) {
        if (!visited.has(neighbor.node)) {
          visited.add(neighbor.node);
          queue.push(neighbor.node);
        }
      }
      await new Promise(r => setTimeout(r, 400)); // Animated log pause
    }
    this.log(`BFS Order: ${order.join(' -> ')}`, 'success');
  }

  async dfs(startNode) {
    if (!this.adjList.has(startNode)) return;
    this.log(`=== DFS TRAVERSAL from ${startNode} ===`, 'header');
    
    let stack = [startNode];
    let visited = new Set();
    let order = [];

    while (stack.length > 0) {
      let curr = stack.pop();
      if (!visited.has(curr)) {
        visited.add(curr);
        order.push(curr);
        this.log(`Visiting: ${curr}`, 'step');

        const neighbors = [...this.adjList.get(curr)].reverse();
        for (let neighbor of neighbors) {
          if (!visited.has(neighbor.node)) {
            stack.push(neighbor.node);
          }
        }
        this.log(`Stack after neighbors: [${stack.join(', ')}]`, 'step');
        await new Promise(r => setTimeout(r, 400));
      }
    }
    this.log(`DFS Order: ${order.join(' -> ')}`, 'success');
  }

  async dijkstra(start, end) {
    if (!this.adjList.has(start) || !this.adjList.has(end)) return;
    this.log(`=== DIJKSTRA'S ALGORITHM: ${start} to ${end} ===`, 'header');

    let distances = {};
    let parents = {};
    let pq = [{ node: start, dist: 0 }];

    for (let node of this.adjList.keys()) distances[node] = Infinity;
    distances[start] = 0;

    while (pq.length > 0) {
      pq.sort((a, b) => a.dist - b.dist);
      let { node: u, dist: d } = pq.shift();

      if (d > distances[u]) continue;
      this.log(`Relaxing neighbors of ${u} (Dist: ${d})`, 'step');

      for (let { node: v, weight: w } of this.adjList.get(u)) {
        if (distances[u] + w < distances[v]) {
          distances[v] = distances[u] + w;
          parents[v] = u;
          pq.push({ node: v, dist: distances[v] });
          this.log(`  - Updated ${v} | Dist: ${distances[v]} | Prev: ${u}`, 'step');
        }
      }
      await new Promise(r => setTimeout(r, 400));
    }

    if (distances[end] === Infinity) {
      this.log(`No path found!`, 'error');
    } else {
      let path = [];
      for (let at = end; at; at = parents[at]) path.push(at);
      this.log(`Shortest Path: ${path.reverse().join(' -> ')}`, 'success');
      this.log(`Total Strength: ${distances[end]}`, 'success');
    }
  }

  async primMST() {
    if (this.adjList.size === 0) return;
    this.log(`=== PRIM'S ALGORITHM (MST) ===`, 'header');
    
    let start = this.adjList.keys().next().value;
    let inMST = new Set([start]);
    let edges = [];
    let totalWeight = 0;

    while (inMST.size < this.adjList.size) {
      let minEdge = { u: null, v: null, w: Infinity };
      
      for (let u of inMST) {
        for (let { node: v, weight: w } of this.adjList.get(u)) {
          if (!inMST.has(v) && w < minEdge.w) {
            minEdge = { u, v, w };
          }
        }
      }

      if (minEdge.u === null) break;

      inMST.add(minEdge.v);
      edges.push(minEdge);
      totalWeight += minEdge.w;
      this.log(`Added edge: ${minEdge.u} --(${minEdge.w})-- ${minEdge.v}`, 'step');
      await new Promise(r => setTimeout(r, 400));
    }

    this.log(`MST complete. Total Weight: ${totalWeight}`, 'success');
  }

  getD3Data() {
    const nodes = Array.from(this.adjList.keys()).map(id => ({ id }));
    const links = [];
    const handled = new Set();

    for (let [u, neighbors] of this.adjList) {
      for (let { node: v, weight } of neighbors) {
        if (this.mode === 'UNDIRECTED') {
          const pair = u < v ? u + v : v + u;
          if (handled.has(pair)) continue;
          handled.add(pair);
        }
        links.push({ source: u, target: v, value: weight });
      }
    }
    return { nodes, links };
  }
}
