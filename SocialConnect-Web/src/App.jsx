import React, { useState, useEffect, useRef } from 'react';
import * as d3 from 'd3';
import { 
  Network, UserPlus, Link, Trash2, Search, Play, 
  Zap, Settings, Terminal, Plus, ChevronRight, 
  Cpu, Database, Activity, Command
} from 'lucide-react';
import { Graph } from './GraphLogic';
import './App.css';

const App = () => {
  const [graph] = useState(new Graph());
  const [version, setVersion] = useState(0); // Trigger for re-renders
  const [logs, setLogs] = useState([]);
  const [nodeName, setNodeName] = useState('');
  const [edgeSrc, setEdgeSrc] = useState('');
  const [edgeDest, setEdgeDest] = useState('');
  const [weight, setWeight] = useState(1);
  const [mode, setMode] = useState('UNDIRECTED');
  const [activeNode, setActiveNode] = useState(null);
  const [activeLinks, setActiveLinks] = useState([]);
  const d3Container = useRef(null);
  const terminalRef = useRef(null);

  useEffect(() => {
    graph.onLog = (msg, type) => {
      setLogs(prev => [...prev.slice(-100), { msg, type, id: `${Date.now()}-${Math.random()}` }]);
    };
    graph.onUpdate = () => {
      triggerUpdate();
    };

    // Override bridge event handler to trigger UI lights
    const originalHandler = graph.handleEvent.bind(graph);
    graph.handleEvent = (e) => {
      originalHandler(e);
      if (e.type === 'BFS_STEP' || e.type === 'DFS_STEP') {
        setActiveNode(e.node);
        setTimeout(() => setActiveNode(null), 800);
      }
      if (e.type === 'DIJKSTRA_RELAX' && e.updated) {
        setActiveLinks(prev => [...prev, {u: e.u, v: e.v}]);
        setTimeout(() => setActiveLinks(prev => prev.filter(l => l.u !== e.u || l.v !== e.v)), 1000);
      }
      if (e.type === 'MST_EDGE') {
        setActiveLinks(prev => [...prev, {u: e.u, v: e.v}]);
      }
      if (e.type === 'CLEAR_ALL' || e.type.endsWith('_COMPLETE')) {
        if (e.type === 'CLEAR_ALL') setActiveLinks([]);
      }
    };
  }, []);

  useEffect(() => {
    if (terminalRef.current) {
      terminalRef.current.scrollTop = terminalRef.current.scrollHeight;
    }
  }, [logs]);

  useEffect(() => {
    renderGraph();
    const handleResize = () => renderGraph();
    window.addEventListener('resize', handleResize);
    return () => window.removeEventListener('resize', handleResize);
  }, [version, mode]);

  const triggerUpdate = () => setVersion(v => v + 1);

  const loadSamples = () => {
    graph.apiCommand('load_sample', {});
  };

  const renderGraph = () => {
    if (!d3Container.current) return;
    const { nodes, links } = graph.getD3Data();
    d3.select(d3Container.current).selectAll("*").remove();
    const svg = d3.select(d3Container.current).append("svg");
    const width = 1000;
    const height = 800;

    svg.attr("viewBox", `0 0 ${width} ${height}`)
       .attr("preserveAspectRatio", "xMidYMid meet");

    const simulation = d3.forceSimulation(nodes)
      .force("link", d3.forceLink(links).id(d => d.id).distance(120))
      .force("charge", d3.forceManyBody().strength(-300))
      .force("center", d3.forceCenter(width / 2, height / 2))
      .force("x", d3.forceX(width / 2).strength(0.08))
      .force("y", d3.forceY(height / 2).strength(0.08));

    const link = svg.append("g")
      .selectAll("line")
      .data(links)
      .join("line")
      .attr("class", "link")
      .attr("stroke-width", 1.5);

    const edgeLabels = svg.append("g")
      .selectAll("text")
      .data(links)
      .join("text")
      .attr("fill", "#71717a")
      .style("font-size", "9px")
      .style("font-weight", "600")
      .text(d => d.value);

    const nodeGroup = svg.append("g")
      .selectAll("g")
      .data(nodes)
      .join("g")
      .call(d3.drag()
        .on("start", (event, d) => {
          if (!event.active) simulation.alphaTarget(0.3).restart();
          d.fx = d.x; d.fy = d.y;
        })
        .on("drag", (event, d) => { d.fx = event.x; d.fy = event.y; })
        .on("end", (event, d) => {
          if (!event.active) simulation.alphaTarget(0);
          d.fx = null; d.fy = null;
        }));

    nodeGroup.append("circle")
      .attr("class", d => `node ${activeNode === d.id ? 'node-active' : ''}`)
      .attr("r", 8)
      .attr("fill", d => activeNode === d.id ? "var(--accent)" : "#38bdf8");

    link.attr("class", d => {
      const isActive = activeLinks.some(al => (al.u === d.source.id && al.v === d.target.id) || (mode === 'UNDIRECTED' && al.u === d.target.id && al.v === d.source.id));
      return `link ${isActive ? 'link-active' : ''}`;
    });

    nodeGroup.append("text")
      .attr("class", "label")
      .attr("dx", 12)
      .attr("dy", 4)
      .text(d => d.id);

    simulation.on("tick", () => {
      link.attr("x1", d => d.source.x).attr("y1", d => d.source.y)
          .attr("x2", d => d.target.x).attr("y2", d => d.target.y);
      nodeGroup.attr("transform", d => `translate(${d.x},${d.y})`);
      edgeLabels.attr("x", d => (d.source.x + d.target.x) / 2)
                 .attr("y", d => (d.source.y + d.target.y) / 2);
    });
  };

  return (
    <div className="dashboard">
      <header className="header">
        <div className="logo"><Cpu size={16} /> SOCIAL<span>CONNECT</span></div>
        <div style={{display:'flex', gap:'20px', fontSize:'0.75rem', color:'#71717a'}}>
          <span>NODES: <b>{graph.adjList.size}</b></span>
          <span>MODE: <b style={{color:'#38bdf8'}}>{mode}</b></span>
        </div>
      </header>

      <aside className="sidebar">
        <div className="nav-section">
          <h3>Management</h3>
          <input className="input-box" placeholder="User ID" value={nodeName} onChange={e=>setNodeName(e.target.value)} />
          <button className="btn-small" onClick={() => { if(graph.addNode(nodeName)) { triggerUpdate(); setNodeName(''); } }}>
            <Plus size={12} /> Add Node
          </button>
          
          <div style={{display:'flex', gap:'4px', marginTop:'12px'}}>
            <input className="input-box" placeholder="From" value={edgeSrc} onChange={e=>setEdgeSrc(e.target.value)} />
            <input className="input-box" placeholder="To" value={edgeDest} onChange={e=>setEdgeDest(e.target.value)} />
          </div>
          <input className="input-box" type="number" placeholder="Weight" value={weight} onChange={e=>setWeight(e.target.value)} />
          <button className="btn-small" onClick={() => { if(graph.addEdge(edgeSrc, edgeDest, parseInt(weight))) { triggerUpdate(); } }}>
            Link Nodes
          </button>
        </div>

        <div className="nav-section">
          <h3>Analysis</h3>
          <button className="nav-item" onClick={() => graph.bfs(prompt('Start Node'))}>
            <Search size={14} /> BFS Traverse
          </button>
          <button className="nav-item" onClick={() => graph.dfs(prompt('Start Node'))}>
            <Zap size={14} /> DFS Traverse
          </button>
          <button className="nav-item" onClick={() => graph.dijkstra(prompt('Start'), prompt('End'))}>
            <Play size={14} /> Pathfinding
          </button>
          <button className="nav-item" onClick={() => graph.primMST()}>
            <Network size={14} /> MST Analysis
          </button>
        </div>

        <div className="nav-section" style={{marginTop:'auto'}}>
          <button className="nav-item" onClick={() => { 
            const newMode = mode === 'UNDIRECTED' ? 'DIRECTED' : 'UNDIRECTED';
            graph.mode = newMode; setMode(newMode); triggerUpdate();
          }}>
            <Settings size={14} /> Toggle Engine
          </button>
          <button className="nav-item" style={{color:'#f43f5e'}} onClick={() => location.reload()}>
            <Trash2 size={14} /> Purge Data
          </button>
        </div>
      </aside>

      <main className="main-area" ref={d3Container}></main>

      <div className="terminal-pane">
        <div className="terminal-top">
          <span><Terminal size={12} style={{marginRight:'6px'}}/> SESSION: SOUMILCHANDRA@CORE</span>
          <span>TTY1</span>
        </div>
        <div className="terminal-body" ref={terminalRef}>
          {logs.map(log => (
            <div key={log.id} className={`log-${log.type}`}>
              {log.type === 'header' ? log.msg : <span><ChevronRight size={10} /> {log.msg}</span>}
            </div>
          ))}
        </div>
      </div>
    </div>
  );
};

export default App;
