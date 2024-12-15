#include "trust_center.h"

using namespace std;

// Base64 编码
std::string vectorToBase64(const std::vector<Byte>& data) {
    std::ostringstream oss;
    for (const auto& byte : data) {
        oss << static_cast<int>(byte) << " ";
    }
    return oss.str();
}

//void printAsHex(const std::vector<Byte>& data, const std::string& label) {
//    std::cout << label << ": ";
//    for (Byte b : data) {
//        std::cout << std::hex << static_cast<int>(b) << " "; // 十六进制打印
//    }
//    std::cout << std::dec << std::endl; // 恢复十进制
//}


void startTrustCenter(int key_size, int bloom_size){
    // Step 1: 生成主密钥 MK 和用户密钥 sk


    vector<Byte> MK = generateRandomKey(key_size / 8);
    vector<Byte> sk = generateRandomKey(key_size / 8);


    // Step 2: 计算 sk' 和 k'
    vector<Byte> sk_prime = xorKeys(MK, sk);
    std::vector<Byte> k_prime(sk.size(), 0);
    std::string sk_prime_str(sk_prime.begin(), sk_prime.end());
    f1(k_prime, sk_prime_str);

    // Step 3: 初始化布隆过滤器和其他系统组件
    auto CBFList = initializeBloomFilter(bloom_size); // 占位大小
    cout << "System initialized with security parameter λ = " << key_size<<","<<bloom_size << endl;

    // Step 4: 发布参数 σ
    cout << "Publishing system parameters..." << endl;
    // 打印部分数据用于调试
    //std::cout << "Received sk from Trust Center: " << base64Encode(sk) << std::endl;
    //cout << "MK (truncated): " << static_cast<int>(MK[0]) << "..." << endl;
    //cout << "sk_prime (truncated): " << static_cast<int>(sk_prime[0]) << "..." << endl;
    //cout << "k_prime (truncated): " << static_cast<int>(k_prime[0]) << "..." << endl;

    //printAsHex(sk, "Generated sk");


    //  启动 HTTP 服务分发密钥
    cout << "Sending sk to Client, k' to Cloud Server, and CBFList to Blockchain." << endl;

    httplib::Server svr;

    // 分发给 Client 的密钥和参数
    svr.Get("/get_client_data", [&](const httplib::Request&, httplib::Response& res) {
        Json::Value json;
        json["sk"] = base64Encode(sk);  // Client 专属密钥
        json["MK"] = base64Encode(MK);
        //json["bloom_size"] = bloom_size;

        Json::StreamWriterBuilder writer;
        std::string response_data = Json::writeString(writer, json);

        res.set_content(response_data, "application/json");
        std::cout << "Sent data to Client." << std::endl;
        });

    // 分发给 Server 的密钥和参数
    svr.Get("/get_server_data", [&](const httplib::Request&, httplib::Response& res) {
        Json::Value json;
        json["k_prime"] = base64Encode(k_prime);  // Server 专属密钥
        json["MK"] = base64Encode(MK);

        Json::StreamWriterBuilder writer;
        std::string response_data = Json::writeString(writer, json);

        res.set_content(response_data, "application/json");
        std::cout << "Sent data to Server." << std::endl;
        });

    // 启动 Trust Center 并等待连接
    std::cout << "Trust Center is running on port 9000..." << std::endl;
    std::thread serverThread([&]() { svr.listen("127.0.0.1", 9000); });

    // 模拟 Trust Center 确认所有通信完成后下线
    std::this_thread::sleep_for(std::chrono::seconds(20)); // 允许足够时间完成通信
    svr.stop(); // 停止服务

    serverThread.join();
    std::cout << "Trust Center is shutting down." << std::endl;

}
