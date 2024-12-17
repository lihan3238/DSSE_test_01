#include "server.h"

const int BLOOM_SIZE = 256; // 布隆过滤器大小

// Setup 函数：从 Trust Center 获取数据并保存到文件
void setupServerData() {
    std::string k_prime;
    std::string MK;
    httplib::Client cli("http://127.0.0.1:9000");
    auto res = cli.Get("/get_server_data");

    if (res && res->status == 200) {
        std::string response_data = res->body;
        Json::CharReaderBuilder reader;
        Json::Value json;
        std::istringstream s(response_data);
        std::string errs;

        if (Json::parseFromStream(reader, s, &json, &errs)) {
            k_prime = base64Decode(json["k_prime"].asString());
            MK = base64Decode(json["MK"].asString());

            // 保存数据到文件
            Json::Value saveData;
            saveData["k_prime"] = json["k_prime"];
            saveData["MK"] = json["MK"];
            saveData["l"] = 0;

            std::ofstream ofs("server_data.json");
            ofs << saveData;
            ofs.close();

            std::cout << "Server setup complete. Data saved to server_data.json." << std::endl;
        }
        else {
            std::cerr << "Error parsing JSON from Trust Center: " << errs << std::endl;
        }
    }
    else {
        std::cerr << "Failed to connect to Trust Center!" << std::endl;
    }
}

#include <thread>

// 启动服务器：处理 Client 请求
void updateServer() {
    int l = 0;
    while (true) {
        try {
            std::string k_prime;
            std::string MK;

            // 尝试加载数据
            std::ifstream ifs("server_data.json");
            if (ifs.is_open()) {
                Json::CharReaderBuilder reader;
                Json::Value savedData;
                std::string errs;

                if (Json::parseFromStream(reader, ifs, &savedData, &errs)) {
                    k_prime = base64Decode(savedData["k_prime"].asString());
                    MK = base64Decode(savedData["MK"].asString());
					if (savedData.isMember("l")) {
						l = savedData["l"].asInt() + 1;
					}
					else {
						l = 1;
					}
                    std::cout << "Server data loaded successfully." << std::endl;
                }
                else {
                    throw std::runtime_error("Error parsing server_data.json: " + errs);
                }
                ifs.close(); // 关闭文件
            }
            else {
                throw std::runtime_error("Failed to open server_data.json.");
            }


            // 处理 Dic1.json 文件
            Json::Value dic1Data;
            std::ifstream ifsDic1("Dic1.json");

            if (!ifsDic1.is_open()) {
                // 如果 Dic1.json 文件不存在，则创建并初始化为一个空的 JSON 对象
                std::cout << "Dic1.json not found, creating a new empty file." << std::endl;
                dic1Data = Json::Value(Json::objectValue); // 创建一个空的 JSON 对象
                std::ofstream ofsDic1("Dic1.json");
                if (ofsDic1.is_open()) {
                    Json::StreamWriterBuilder writer;
                    ofsDic1 << Json::writeString(writer, dic1Data);
                    ofsDic1.close();
                    std::cout << "Created empty Dic1.json." << std::endl;
                }
                else {
                    std::cerr << "Failed to create Dic1.json." << std::endl;
                    return;
                }
            }
            ifsDic1.close();
            

            // 设置服务器
            httplib::Server svr;

            svr.Post("/send_conn_indop_data", [&dic1Data](const httplib::Request& req, httplib::Response& res) {
                // 使用 JsonCpp 解析接收到的 JSON 数据
                Json::Value jsonData;
                Json::Reader reader;

                // 尝试解析请求体中的 JSON 数据
                if (reader.parse(req.body, jsonData)) {
                    // 打印接收到的数据
                    std::cout << "Received data from client: " << req.body << std::endl;

                    // 遍历并处理 connectorData


                        for (const auto& item : jsonData) {
                            std::string connector_key = base64Decode(item["connector_key"].asString());
                            std::string connector_value = base64Decode(item["connector_value"].asString());

                            // 将 connector_key 和 connector_value 转为 Base64 字符串
                            std::string connector_key_str = base64Encode(connector_key);
                            std::string connector_value_str = base64Encode(connector_value);

                            // 将 Base64 编码的字符串作为键值对保存到 dic1Data
                            dic1Data[connector_key_str] = connector_value_str;
                        }


                    // 遍历并处理 indopData


                        for (const auto& item : jsonData) {
                            std::string indop_key = base64Decode(item["indop_key"].asString());
                            std::string indop_value = base64Decode(item["indop_value"].asString());

                            // 将 indop_key 和 indop_value 转为 Base64 字符串
                            std::string indop_key_str = base64Encode(indop_key);
                            std::string indop_value_str = base64Encode(indop_value);

                            // 将 Base64 编码的字符串作为键值对保存到 dic1Data
                            dic1Data[indop_key_str] = indop_value_str;
                        }


                    std::cout << "Saving data..." << std::endl;

                    // 保存 dic1Data 到 Dic1.json
                    std::ofstream ofsDic1("Dic1.json");
                    if (ofsDic1.is_open()) {
                        Json::StreamWriterBuilder writer;
                        ofsDic1 << Json::writeString(writer, dic1Data);
                        ofsDic1.close();
                        std::cout << "Data saved to Dic1.json successfully." << std::endl;
                    }
                    else {
                        std::cerr << "Failed to open Dic1.json for writing." << std::endl;
                    }

                    // 返回成功响应
                    res.set_content("Data received and saved successfully.", "text/plain");
                    return;
                }
                else {
                    std::cerr << "Failed to parse JSON data: " << req.body << std::endl;
                    res.status = 400;
                    res.set_content("Invalid JSON format.", "text/plain");
                }
                });

            // 启动服务器
            svr.listen("127.0.0.1", 9001);
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

void handleSearch(const httplib::Request& req, httplib::Response& res) {
    vector<string> searchTokens;
    try {
        Json::CharReaderBuilder reader;
        Json::Value root;
        std::string errs;

        // 解析收到的 JSON 数据
        std::istringstream ss(req.body);
        if (Json::parseFromStream(reader, ss, &root, &errs)) {
            // 获取 tokens 数组
            const Json::Value tokens = root["tokens"];
            for (const auto& token : tokens) {
                searchTokens.push_back(token.asString());
                std::cout << "Received token: " << token.asString() << std::endl;
            }

            // 返回响应
            res.set_content("Received search tokens", "text/plain");
        }
        else {
            res.status = 400;  // Bad request
            res.set_content("Invalid JSON", "text/plain");
        }
    }
    catch (const std::exception& e) {
        res.status = 500;  // Internal server error
        res.set_content("Server error", "text/plain");
    }

    // 处理搜索令牌
    map <string, string> Dic1;

    // 打开 Dic1.json 文件
    std::ifstream ifs("Dic1.json");

    if (!ifs.is_open()) {
        std::cerr << "Failed to open Dic1.json!" << std::endl;
        return;
    }

    // 使用 JsonCpp 解析文件内容
    Json::CharReaderBuilder reader;
    Json::Value dic1Data;
    std::string errs;

    // 解析 JSON 数据
    if (Json::parseFromStream(reader, ifs, &dic1Data, &errs)) {
        std::cout << "Dic1.json data loaded successfully." << std::endl;

        // 打印所有键值对
        for (const auto& key : dic1Data.getMemberNames()) {
            Dic1[key] = dic1Data[key].asString();
        }
    }
    else {
        std::cerr << "Error parsing Dic1.json: " << errs << std::endl;
    }
    ifs.close();  // 关闭文件

    int l = 0;
    // 检查文件是否存在，加载数据
    std::ifstream sifs("server_data.json");
    if (sifs.is_open()) {
        Json::CharReaderBuilder reader;
        Json::Value savedData;
        std::string errs;
        if (Json::parseFromStream(reader, sifs, &savedData, &errs)) {
            if (savedData.isMember("l")) {
                l = savedData["l"].asInt();
            }
        }
        sifs.close(); // 关闭文件
    }



    std::vector<std::pair<std::vector<string>, int>> Rsearch;
    int DB = 0;
    std::string stwjvll;
    for (const auto& token : searchTokens) {
        string stwjvl = token;
        while (stwjvl.size() > 0) {
            string connector_key = h1(stwjvl, 1);
            string connector_value = Dic1[connector_key];
            string tmp1 = xorKeys(h2(stwjvl), connector_value);
            if (tmp1.length() >= 4) {
                std::string firstFour = tmp1.substr(0, 4);
                DB = byteStringToInt(firstFour);
                // 获取剩余部分作为 st
                stwjvll = tmp1.substr(4);

                // 输出结果
                std::cout << "Front part (as int): " << DB << std::endl;
                std::cout << "Remaining part (st): " << stwjvll << std::endl;
            }
            else {
                std::cerr << "Error: connector_value is too short." << std::endl;
            }

            vector<string> Rwjvl;
            std::string indopwjvlm;
            for (int i = 1; i <= DB; i++)
            {
                string indop_key = h34(stwjvl, i, 3);
                string indop_value = Dic1[indop_key];
                string tmp2 = xorKeys(h34(stwjvl, i, 4), indop_value);
                if (tmp2.length() >= 4) {
                    std::string firstFour = tmp2.substr(0, 4);
                    indopwjvlm = tmp2.substr(4);
                }
                Rwjvl.push_back(indopwjvlm);
            }
            Rsearch.push_back(std::make_pair(Rwjvl, l));
            stwjvl = stwjvll;
        }
        //sendRsearchtoClient and BC

        httplib::Server svr;


        svr.Get("/send_Rsearch_Request", [&](const httplib::Request&, httplib::Response& res) {
            Json::Value root(Json::arrayValue);
            for (const auto& pair : Rsearch) {
                Json::Value item;
                Json::Value strArray(Json::arrayValue);
                for (const auto& str : pair.first) {
                    strArray.append(str);
                }
                item["R"] = strArray;
                item["l"] = pair.second;  // 作为字符串存储
                root.append(item);
            }
            Json::StreamWriterBuilder writer;
            std::string response_data = Json::writeString(writer, root);

            res.set_content(response_data, "application/json");
            std::cout << "Sent data to Client." << std::endl;
            });
        std::cout << "Server running at http://localhost:9001..." << std::endl;
        svr.listen("localhost", 9001);
    }

}

void searchServer() {
    httplib::Server svr;

    svr.Post("/search", handleSearch);

    std::cout << "Server running at http://localhost:9001..." << std::endl;
    svr.listen("localhost", 9001);
}

