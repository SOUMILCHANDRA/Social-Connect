# 🌐 SocialConnect: Research-Grade Graph Analysis Tool

**SocialConnect** is a professional-grade C++ ecosystem designed for the simulation, analysis, and visualization of complex social network topologies. It combines high-performance algorithmic logic with a modern, dual-interface visualization stack (Terminal ANSI + D3.js Web).

---

## 🎨 Unified Experience
SocialConnect offers three distinct ways to interact with your data:
1.  **C++ Terminal (Core)**: Verbose algorithm tracing with ANSI color-coding.
2.  **Standalone Visualizer (`viz.html`)**: A generated, self-contained D3.js research tool.
3.  **React Dashboard**: A high-speed, interactive web dashboard built with Vite and React.

---

## ✨ Features

### 🧠 Core Algorithmic Engine
*   **BFS (Relationship Discovery)**: Discover network breadth and tiered connectivity.
*   **DFS (Depth Exploration)**: Explore structural depth and recursive paths.
*   **Dijkstra's Algorithm**: Calculate mathematically optimal paths based on relationship weight (1-10).
*   **Prim's MST**: Identify the network "skeleton" that connects all users with minimum cost.

### 📊 Professional Data Architecture
*   **Regional Localization**: Ships with a pre-configured Indian-region dataset (**Aarav, Ishani, Rohan, Priya, Arjun, Sneha, Vihaan, Ananya**).
*   **Hybrid Storage**: Utilizing specialized STL containers (`unordered_map` and adjacency vectors) for $O(1)$ and $O(V+E)$ performance.
*   **Robust Centering**: Visualizations use a standardized `1000x800` coordinate system and force-directed logic for perfect centering across all devices.

---

## 🚀 Getting Started

### 🛠️ Backend (C++ Core)
Compile using a C++17 compliant compiler:
```powershell
g++ -std=c++17 main.cpp Graph.cpp Visualizer.cpp -o SocialConnect
./SocialConnect
```

### ⚛️ Frontend (React Dashboard)
Experience the full interactive dashboard:
```powershell
cd SocialConnect-Web
npm install
npm run dev
```
Navigate to `http://localhost:5173/` to see the real-time graph.

---

## 📂 Project Ecosystem
```text
├── SocialConnect/
│   ├── main.cpp          # System entry point & CLI
│   ├── Graph.cpp         # Algorithmic implementations
│   ├── Visualizer.cpp    # HTML Generation Template & ANSI logic
│   └── viz.html          # Generated research tool
└── SocialConnect-Web/
    ├── src/
    │   ├── App.jsx       # React application logic
    │   └── GraphLogic.js # D3 Implementation
    └── index.html        # Web entry
```

---

## 📈 Design Philosophy
SocialConnect is built with an **Academic/Research aesthetic**:
*   **Typography**: IBM Plex Sans (Labels) and JetBrains Mono (Terminal).
*   **Color Palette**: Midnight Dark (`#0b0e14`) with Ice Blue (`#4fc3f7`) accents.
*   **Stability**: D3 Force Simulation is hardened with `1000x800` viewport constraints to ensure zero-overlap and zero-offscreen spawning.

---
*Maintained by the Soumil Chandra*
