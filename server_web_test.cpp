#include <httplib.h>
#include <iostream>
//#include <thread>
//#include <atomic>
#include "server.h"

using namespace std;
httplib::Server svr_cs;

//atomic<string> server_status = "服务器未启动";  // 共享变量，存储状态

string server_status="SERVER_OFFLINE";  // 共享变量，存储状态
//string time_cost = "ms";

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

void setup_server_handler(const httplib::Request&, httplib::Response& res) {
    server_status = "INITIALIZATING";
    setupServerData();
    server_status = "INITIALIZATED";
    res.set_content(R"({"message": "Server data initialized"})", "application/json");
}

void start_server_handler(const httplib::Request&, httplib::Response& res) {
    server_status = "UPDATE_SERVER_ONLINE";
    updateServer();
    server_status = "UPDATED";
    res.set_content(R"({"message": "Server started"})", "application/json");
}

void search_server_handler(const httplib::Request&, httplib::Response& res) {
    server_status = "SEARCH__SERVER_ONLINE";
    searchServer();
    server_status = "SEARCHED";
    Json::Value json_response;
    json_response["message"] = "Reset executed";  // 自动处理中文
    //json_response["search_time_cost"] = time_cost;  // 自动处理中文

    Json::StreamWriterBuilder writer;
    string response_body = Json::writeString(writer, json_response);

    //res.set_header("Content-Type", "application/json; charset=utf-8");
    //res.set_content(response_body, "application/json");

    // **添加 CORS 头**
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");

    res.set_header("Content-Type", "application/json; charset=utf-8");
    res.set_content(response_body, "application/json");
    //res.set_content(R"({"message": "Search executed"})", "application/json");
}

void svr_reset_handler(const httplib::Request&, httplib::Response& res) {
    server_status = "RESETING";
    svr_reSet();
    server_status = "RESETED";
    res.set_content(R"({"message": "Reset executed"})", "application/json");
}

// **状态轮询接口**
void svr_status_handler(const httplib::Request&, httplib::Response& res) {
    //string last_status = server_status;
    //while (server_status == last_status) {
    //    this_thread::sleep_for(chrono::milliseconds(500)); // 等待500ms
    //}
    Json::Value json_response;
    json_response["status"] = server_status;  // 自动处理中文

    Json::StreamWriterBuilder writer;
    string response_body = Json::writeString(writer, json_response);

    res.set_header("Content-Type", "application/json; charset=utf-8");
    res.set_content(response_body, "application/json");
}

int main144() {
    svr_cs.Get("/", svr_index_handler);
    svr_cs.Post("/setup", setup_server_handler);
    svr_cs.Post("/update", start_server_handler);
    svr_cs.Post("/search", search_server_handler);
    svr_cs.Post("/reset", svr_reset_handler);
    svr_cs.Get("/status", svr_status_handler);  // 新增状态查询接口
    cout << "Server running at http://localhost:8081" << endl;

    svr_cs.listen("0.0.0.0", 8081);

    return 0;
}
