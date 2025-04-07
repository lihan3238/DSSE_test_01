#include <httplib.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <json/json.h>
#include "trust_center.h"

using namespace std;

const int LAMBDA = 256; // 安全参数 λ
const int BLOOM_SIZE = 256; // 布隆过滤器大小

//std::atomic<int> request_count(0); // 计数收到的请求
httplib::Server svr_tc;

// 触发前端更新状态
//void notifyFrontend(const std::string& message) {
//    std::cout << "Notify frontend: " << message << std::endl;
//}

// 处理主页

void tc_index_handler(const httplib::Request&, httplib::Response& res) {
    ifstream file("trust_center.html", ios::binary);
    if (!file) {
        res.status = 404;
        res.set_content("HTML file not found", "text/plain");
        return;
    }
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    res.set_header("Content-Type", "text/html");
    res.set_content(content, "text/html");
}
// 启动 Trust Center
void startTrustCenterAPI(const httplib::Request&, httplib::Response& res) {
        startTrustCenter(LAMBDA, BLOOM_SIZE);


    res.set_content(R"({"message": "Trust Center started"})", "application/json");
}

// 启动 HTTP 服务
int main111341() {
    svr_tc.Get("/", tc_index_handler);
    svr_tc.Get("/start_trust", startTrustCenterAPI);

    cout << "Trust Center API running on port 8082..." << endl;
    svr_tc.listen("0.0.0.0", 8082);
    return 0;
}
