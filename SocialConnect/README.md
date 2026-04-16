# 🌐 SocialConnect: Graph Network Visualizer

**SocialConnect** is a high-performance C++ tool designed to simulate and visualize social network structures using advanced Graph data structures and algorithms. Whether you're tracking friendships (Undirected) or follows (Directed), SocialConnect provides deep insights into network depth and connectivity.

---

## ✨ Features

### 🧠 Core Algorithms
*   **BFS (Find Network):** Discover immediate and 2nd-degree connections.
*   **DFS (Deep Explore):** Explore the structural depth of the network.
*   **Dijkstra's Algorithm:** Find the shortest path between users based on relationship strength (1-10).
*   **Prim's Algorithm (MST):** Identify the minimum spanning tree, showing the most efficient way to connect the entire network.

### 📊 Data Representations
*   **Adjacency List:** Efficient storage using `unordered_map` for large networks.
*   **Adjacency Matrix:** Grid-based view for small, dense networks.

### 🎨 Visualizations
*   **Terminal Visuals:** Color-coded, verbose step-by-step trace of every algorithm.
*   **Web Visualizer:** Interactive D3.js force-directed graph with drag-and-drop nodes.

---

## 🚀 Getting Started

### Prerequisites
*   **C++17 Compiler** (GCC/MinGW recommended)
*   **Node.js** (Optional, for hosting the web visualizer)

### 🛠️ Compilation
Compile all source files using the following command:
```bash
g++ -std=c++17 main.cpp Graph.cpp Visualizer.cpp -o socialconnect
```

### 🏃 Running the Application
Launch the terminal interface:
```bash
./socialconnect
```

---

## 📖 How to Use Properly

### 1. Building the Network
1.  **Start fresh or Load Samples:** Use option `11` in the menu to load a pre-built network of 8 users (Alice through Henry).
2.  **Add Users:** Use option `1` to add new individuals to your network.
3.  **Create Connections:** Use option `2` to define relationships. You can provide a weight (1-10) to represent the "Strength" of the connection.

### 2. Running Algorithms
*   **Finding Shortest Path:** Select option `7`, enter the names of two users, and watch the algorithm "relax" edges in real-time to find the best path.
*   **Exploring Connectivity:** Use BFS (`5`) to find "people you may know" or DFS (`6`) to traverse the graph's depth.
*   **MST Analysis:** Use option `8` to find the skeleton of your network that connects everyone with the minimum total connection weight.

### 3. Using the Web Visualizer
1.  Inside the application, select **Option 12** to "Generate D3.js Visualization".
2.  This creates a file named `viz.html`.
3.  To host it locally and see live updates, run:
    ```bash
    npx serve .
    ```
4.  Open `http://localhost:3000/viz.html` in your browser.

---

## 📂 Project Structure
*   `main.cpp`: Interactive menu logic and user input handling.
*   `Graph.cpp / .h`: Core implementations of graph algorithms and data structures.
*   `Visualizer.cpp / .h`: Terminal styling (ANSI) and HTML/D3 generation logic.
*   `.gitignore`: Excludes binaries and generated artifacts.

---

## 🛠️ Tech Stack
*   **Language:** C++17
*   **Data Structures:** STL (`unordered_map`, `priority_queue`, `stack`, `queue`)
*   **Visualization:** D3.js (Data-Driven Documents)

---
*Created with ❤️ by Antigravity for SocialConnect.*
