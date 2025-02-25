#include "trust_center.h"
#include "utils.h"
#include <httplib.h>
#include <json/json.h>

using namespace std;

// Base64 编码（修改为使用 std::string 进行处理）
std::string stringToBase64(const std::string &data)
{
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    BIO_push(b64, bio);

    BIO_write(b64, data.c_str(), data.size());
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bufferPtr);

    std::string encoded(bufferPtr->data, bufferPtr->length);
    BIO_free_all(b64);

    return encoded;
}

void startTrustCenter(int key_size, int bloom_size)
{
    // Step 1: 生成主密钥 MK 和用户密钥 sk
    std::string MK = generateRandomKey(key_size / 8); // 修改为 std::string
    std::string sk = generateRandomKey(key_size / 8); // 修改为 std::string

    // Step 2: 计算 sk' 和 k'
    std::string sk_prime = xorKeys(MK, sk);          // 使用 std::string 来表示密钥
    std::string k_prime = std::string(sk.size(), 0); // 使用 std::string 初始化 k_prime
    f1(k_prime, sk_prime);                           // 调用 f1 函数处理 k_prime

    // Step 3: 初始化布隆过滤器和其他系统组件
    auto CBFList = initializeBloomFilter(bloom_size); // 占位大小
    cout << "System initialized with security parameter λ = " << key_size << "," << bloom_size << endl;

    // Step 4: 发布参数 σ
    cout << "Publishing system parameters..." << endl;

    // 启动 HTTP 服务分发密钥
    cout << "Sending sk to Client, k' to Cloud Server, and CBFList to Blockchain." << endl;

    httplib::Server svr;

    // 分发给 Client 的密钥和参数
    svr.Get("/get_client_data", [&](const httplib::Request &, httplib::Response &res)
            {
        Json::Value json;
        json["sk"] = stringToBase64(sk);  // 使用 Base64 编码后的 sk
        json["MK"] = stringToBase64(MK);  // 使用 Base64 编码后的 MK

        Json::StreamWriterBuilder writer;
        std::string response_data = Json::writeString(writer, json);

        res.set_content(response_data, "application/json");
        std::cout << "Sent data to Client." << std::endl; });

    // 分发给 Server 的密钥和参数
    svr.Get("/get_server_data", [&](const httplib::Request &, httplib::Response &res)
            {
        Json::Value json;
        json["k_prime"] = stringToBase64(k_prime);  // 使用 Base64 编码后的 k_prime
        json["MK"] = stringToBase64(MK);  // 使用 Base64 编码后的 MK

        Json::StreamWriterBuilder writer;
        std::string response_data = Json::writeString(writer, json);

        res.set_content(response_data, "application/json");
        std::cout << "Sent data to Server." << std::endl; });

    // 分发给 Server 的密钥和参数
    svr.Get("/get_blockchain_data", [&](const httplib::Request &, httplib::Response &res)
            {
        Json::Value json;
        json["k_prime"] = stringToBase64(k_prime);  // 使用 Base64 编码后的 k_prime
        json["MK"] = stringToBase64(MK);  // 使用 Base64 编码后的 MK

        Json::StreamWriterBuilder writer;
        std::string response_data = Json::writeString(writer, json);

        res.set_content(response_data, "application/json");
        std::cout << "Sent data to BlockChain." << std::endl; });

    // 启动 Trust Center 并等待连接
    std::cout << "Trust Center is running on port 9000..." << std::endl;
    std::thread serverThread([&]()
                             { svr.listen("127.0.0.1", 9000); });

    // 模拟 Trust Center 确认所有通信完成后下线
    std::this_thread::sleep_for(std::chrono::seconds(20)); // 允许足够时间完成通信
    svr.stop();                                            // 停止服务

    serverThread.join();
    std::cout << "Trust Center is shutting down." << std::endl;
}
