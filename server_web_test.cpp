#include <httplib.h>
#include <iostream>
#include "server.h"

using namespace std;
httplib::Server svr_cs;

// 处理主页

void svr_index_handler(const httplib::Request&, httplib::Response& res) {
    ifstream file("server.html", ios::binary);
    if (!file) {
        res.status = 404;
        res.set_content("HTML file not found", "text/plain");
        return;
    }
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    res.set_header("Content-Type", "text/html");
    res.set_content(content, "text/html");
}

// 处理前端请求
void setup_server_handler(const httplib::Request&, httplib::Response& res) {
    setupServerData();
    res.set_content(R"({"message": "Server data initialized"})", "application/json");
}

void start_server_handler(const httplib::Request&, httplib::Response& res) {
    updateServer();
    res.set_content(R"({"message": "Server started"})", "application/json");
}

void search_server_handler(const httplib::Request&, httplib::Response& res) {
    searchServer();
    res.set_content(R"({"message": "Search executed"})", "application/json");
}

void svr_reset_handler(const httplib::Request&, httplib::Response& res) {
    svr_reSet();
    res.set_content(R"({"message": "Reset executed"})", "application/json");
}

void main5123() {
    svr_cs.Get("/", svr_index_handler);
    svr_cs.Post("/setup", setup_server_handler);
    svr_cs.Post("/start", start_server_handler);
    svr_cs.Post("/search", search_server_handler);
    svr_cs.Post("/reset", svr_reset_handler);

    cout << "Server running at http://localhost:8081" << endl;
    svr_cs.listen("0.0.0.0", 8081);
    return;
}
