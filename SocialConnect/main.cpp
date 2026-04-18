#include "crow_all.h"
#include "Graph.h"
#include "Visualizer.h"
#include <thread>
#include <atomic>

Graph graph;

int main() {
    graph.loadSample();
    Visualizer::printBanner();
    Visualizer::print("Sample network loaded: 8 nodes");
    Visualizer::print("API server starting on http://localhost:8081");

    crow::SimpleApp app;
    app.loglevel(crow::LogLevel::Debug);

    auto cors = [](crow::response& res) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
    };

    CROW_ROUTE(app, "/")
    ([&]() {
        std::ifstream file("viz.html");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        crow::response res(content);
        res.add_header("Content-Type", "text/html");
        return res;
    });

    CROW_ROUTE(app, "/health")
    ([]() {
        return "{\"status\":\"ok\",\"engine\":\"crow\"}";
    });

    CROW_ROUTE(app, "/state")
    ([&]() {
        crow::response res(graph.toJson());
        cors(res);
        res.add_header("Content-Type", "application/json");
        return res;
    });

    CROW_ROUTE(app, "/bfs").methods(crow::HTTPMethod::Post)
    ([&](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "{\"error\":\"bad json\"}");
        std::string start = body["start"].s();

        Visualizer::printAlgoHeader("BFS TRAVERSAL", start);
        auto result = graph.BFS(start);

        std::string json = "{\"order\":[";
        for (size_t i = 0; i < result.order.size(); i++) {
            json += "\"" + result.order[i] + "\"";
            if (i < result.order.size()-1) json += ",";
        }
        json += "],\"steps\":[";
        for (size_t i = 0; i < result.steps.size(); i++) {
            json += "{\"node\":\"" + result.steps[i].node + "\",\"queue\":[";
            for (size_t j = 0; j < result.steps[i].queue.size(); j++) {
                json += "\"" + result.steps[i].queue[j] + "\"";
                if (j < result.steps[i].queue.size()-1) json += ",";
            }
            json += "]}";
            if (i < result.steps.size()-1) json += ",";
        }
        json += "]}";

        crow::response res(json);
        cors(res);
        res.add_header("Content-Type", "application/json");
        return res;
    });

    CROW_ROUTE(app, "/dfs").methods(crow::HTTPMethod::Post)
    ([&](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "{\"error\":\"bad json\"}");
        std::string start = body["start"].s();

        Visualizer::printAlgoHeader("DFS TRAVERSAL", start);
        auto result = graph.DFS(start);

        std::string json = "{\"order\":[";
        for (size_t i = 0; i < result.order.size(); i++) {
            json += "\"" + result.order[i] + "\"";
            if (i < result.order.size()-1) json += ",";
        }
        json += "],\"steps\":[";
        for (size_t i = 0; i < result.steps.size(); i++) {
            json += "{\"node\":\"" + result.steps[i].node + "\",\"stack\":[";
            for (size_t j = 0; j < result.steps[i].stack.size(); j++) {
                json += "\"" + result.steps[i].stack[j] + "\"";
                if (j < result.steps[i].stack.size()-1) json += ",";
            }
            json += "]}";
            if (i < result.steps.size()-1) json += ",";
        }
        json += "]}";

        crow::response res(json);
        cors(res);
        res.add_header("Content-Type", "application/json");
        return res;
    });

    CROW_ROUTE(app, "/dijkstra").methods(crow::HTTPMethod::Post)
    ([&](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "{\"error\":\"bad json\"}");
        std::string start = body["start"].s();
        std::string end   = body["end"].s();

        Visualizer::printAlgoHeader("DIJKSTRA", start + " → " + end);
        auto result = graph.Dijkstra(start, end);

        std::string json = "{\"path\":[";
        for (size_t i = 0; i < result.path.size(); i++) {
            json += "\"" + result.path[i] + "\"";
            if (i < result.path.size()-1) json += ",";
        }
        json += "],\"cost\":" + std::to_string(result.cost);
        json += ",\"steps\":[";
        for (size_t i = 0; i < result.steps.size(); i++) {
            auto& s = result.steps[i];
            json += "{\"u\":\""+s.u+"\",\"v\":\""+s.v+"\",\"newDist\":"+std::to_string(s.newDist)+",\"updated\":"+std::string(s.updated?"true":"false")+"}";
            if (i < result.steps.size()-1) json += ",";
        }
        json += "]}";

        crow::response res(json);
        cors(res);
        res.add_header("Content-Type", "application/json");
        return res;
    });

    CROW_ROUTE(app, "/prims").methods(crow::HTTPMethod::Post)
    ([&](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "{\"error\":\"bad json\"}");
        std::string start = body["start"].s();

        Visualizer::printAlgoHeader("PRIM'S MST", start);
        auto result = graph.Prims(start);

        std::string json = "{\"totalCost\":" + std::to_string(result.totalCost);
        json += ",\"edges\":[";
        for (size_t i = 0; i < result.edges.size(); i++) {
            json += "{\"u\":\""+result.edges[i].u+"\",\"v\":\""+result.edges[i].v+"\",\"weight\":"+std::to_string(result.edges[i].weight)+"}";
            if (i < result.edges.size()-1) json += ",";
        }
        json += "]}";

        crow::response res(json);
        cors(res);
        res.add_header("Content-Type", "application/json");
        return res;
    });

    CROW_ROUTE(app, "/add_node").methods(crow::HTTPMethod::Post)
    ([&](const crow::request& req) {
        auto body = crow::json::load(req.body);
        std::string name = body["name"].s();
        graph.addNode(name);
        crow::response res("{\"status\":\"ok\",\"nodeCount\":" + std::to_string(graph.nodeCount()) + "}");
        cors(res); return res;
    });

    CROW_ROUTE(app, "/add_edge").methods(crow::HTTPMethod::Post)
    ([&](const crow::request& req) {
        auto body = crow::json::load(req.body);
        std::string from = body["from"].s(), to = body["to"].s();
        int weight = body["weight"].i();
        graph.addEdge(from, to, weight);
        crow::response res("{\"status\":\"ok\"}");
        cors(res); return res;
    });

    CROW_ROUTE(app, "/remove_node").methods(crow::HTTPMethod::Post)
    ([&](const crow::request& req) {
        auto body = crow::json::load(req.body);
        graph.removeNode(body["name"].s());
        crow::response res("{\"status\":\"ok\"}");
        cors(res); return res;
    });

    CROW_ROUTE(app, "/remove_edge").methods(crow::HTTPMethod::Post)
    ([&](const crow::request& req) {
        auto body = crow::json::load(req.body);
        graph.removeEdge(body["from"].s(), body["to"].s());
        crow::response res("{\"status\":\"ok\"}");
        cors(res); return res;
    });

    CROW_ROUTE(app, "/load_sample").methods(crow::HTTPMethod::Post)
    ([&]() {
        graph.loadSample();
        Visualizer::print("Sample network reloaded");
        crow::response res("{\"status\":\"ok\"}");
        cors(res); return res;
    });

    CROW_ROUTE(app, "/clear_all").methods(crow::HTTPMethod::Post)
    ([&]() {
        graph.clearAll();
        Visualizer::print("Graph cleared");
        crow::response res("{\"status\":\"ok\"}");
        cors(res); return res;
    });

    CROW_ROUTE(app, "/set_directed").methods(crow::HTTPMethod::Post)
    ([&](const crow::request& req) {
        auto body = crow::json::load(req.body);
        graph.setDirected(body["directed"].b());
        crow::response res("{\"status\":\"ok\"}");
        cors(res); return res;
    });

    CROW_ROUTE(app, "/<path>").methods(crow::HTTPMethod::Options)
    ([](const crow::request&, crow::response& res, std::string) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
        res.code = 204;
        res.end();
    });

    std::thread serverThread([&]() {
        app.port(8081).multithreaded().run();
    });
    serverThread.detach();

    Visualizer::print("✓ API ready at http://localhost:8081");
    Visualizer::print("✓ Open viz.html in browser");
    Visualizer::printDivider();

    int choice;
    while (true) {
        Visualizer::printMenu();
        if (!(std::cin >> choice)) break;
        if (choice == 0) break;
        
        if (choice == 1) {
            std::string name; std::cout << "  Name: "; std::cin >> name;
            graph.addNode(name);
        } else if (choice == 3) {
            std::string f, t; int w;
            std::cout << "  From: "; std::cin >> f;
            std::cout << "  To: "; std::cin >> t;
            std::cout << "  Weight: "; std::cin >> w;
            graph.addEdge(f, t, w);
        } else if (choice == 5) {
            std::string s; std::cout << "  Start Node: "; std::cin >> s;
            graph.BFS(s);
        } else if (choice == 6) {
            std::cout << "\n" << graph.toJson() << "\n";
        }
    }

    app.stop();
    return 0;
}
