#include "client.h"

const int LAMBDA = 256; // 安全参数 λ
const int BLOOM_SIZE = 256; // 布隆过滤器大小

// Setup 函数：从 Trust Center 获取数据并保存到文件
void setupClientData() {
    std::vector<Byte> sk;
    std::vector<Byte> MK;
    httplib::Client cli("http://127.0.0.1:9000");
    auto res = cli.Get("/get_client_data");

    if (res && res->status == 200) {
        std::string response_data = res->body;
        Json::CharReaderBuilder reader;
        Json::Value json;
        std::istringstream s(response_data);
        std::string errs;

        if (Json::parseFromStream(reader, s, &json, &errs)) {
            sk = base64Decode(json["sk"].asString());
            MK = base64Decode(json["MK"].asString());

            // 保存数据到文件
            Json::Value saveData;
            saveData["sk"] = json["sk"];
            saveData["MK"] = json["MK"];
			//saveData["Words"] = Json::Value(Json::arrayValue);
			saveData["l"] = 0;

            std::ofstream ofs("client_data.json");
            ofs << saveData;
            ofs.close();

            std::cout << "Client setup complete. Data saved to client_data.json." << std::endl;
        }
        else {
            std::cerr << "Error parsing JSON from Trust Center: " << errs << std::endl;
        }
    }
    else {
        std::cerr << "Failed to connect to Trust Center!" << std::endl;
    }
}

// 与服务器通信的函数
void updateClient() {
    std::vector<Byte> sk;
    std::vector<Byte> MK;
    std::vector<Byte> sk_prime;
    std::unordered_map<int, std::vector<Byte>> Dic1;
    std::string Words[100];
    int l = 0;
    // 检查文件是否存在，加载数据
    std::ifstream ifs("client_data.json");
    if (ifs.is_open()) {
        Json::CharReaderBuilder reader;
        Json::Value savedData;
        std::string errs;
        if (Json::parseFromStream(reader, ifs, &savedData, &errs)) {
            sk = base64Decode(savedData["sk"].asString());
            MK = base64Decode(savedData["MK"].asString());

            if (savedData.isMember("l")) {
                l = savedData["l"].asInt()+1;
            }
            else {
                l = 1;
            }
        }
        ifs.close(); // 关闭文件
    }

    

    Dic1[l] = generateRandomKey(LAMBDA / 8);

    // 保存更新后的 l 值回文件
    Json::Value updatedData;
    updatedData["sk"] = base64Encode(sk);
    updatedData["MK"] = base64Encode(MK);
	//updatedData["Words"] = Json::Value(Json::arrayValue);
    updatedData["l"] = l;


    std::ofstream ofs("client_data.json");
    if (ofs.is_open()) {
        Json::StreamWriterBuilder writer;
        ofs << Json::writeString(writer, updatedData);
        ofs.close();
        std::cout << "Updated 'l' value saved to client_data.json: " << l << std::endl;
    }
    else {
        std::cerr << "Failed to open client_data.json for writing!" << std::endl;
    }


    // Update
    sk_prime = xorKeys(MK, sk);
    std::vector<Byte> k_prime(sk.size(), 0);
    std::string sk_prime_str(sk_prime.begin(), sk_prime.end());
    f1(k_prime, sk_prime_str);

   
    std::vector<Byte> tmp1;
    std::vector<Byte> tmp2;
    std::vector<Byte> k;
	string k_str;
    std::map<std::vector<Byte>, std::vector<Byte>> st;
    std::vector<Byte> Conn_key;
    std::vector<Byte> Conn_value;

    std::map<std::string, std::vector<int>> index;

    // 打开并读取JSON文件
    std::ifstream ifs2("update.json");
    if (!ifs2.is_open()) {
        std::cerr << "Failed to open the JSON file." << std::endl;
        return ;
    }

    Json::Reader reader;
    Json::Value jsonData;

    // 解析 JSON 文件
    if (!reader.parse(ifs2, jsonData)) {
        std::cerr << "Failed to parse JSON data." << std::endl;
        return ;
    }

    // 统计数字总和
    int DB = 0;

	ifs2.close();

    // 遍历 JSON 文件的键并存入 Words 数组
    int wc = 0;
    for (const auto& key : jsonData.getMemberNames()) {
        if (wc < 100) {
            Words[wc++] = key; // 存储键到 Words 数组
        }
        else {
            std::cerr << "Words array size exceeded maximum limit." << std::endl;
            break;
        }
    }


    // 遍历 Words 数组中的每个单词，查找 JSON 中对应的数据
    for (const std::string& word : Words) {
        // 检查 JSON 中是否有该单词
        if (jsonData.isMember(word)) {
            const Json::Value& wordData = jsonData[word];
            std::vector<int> numbers;

            // 获取该单词对应数组的所有元素
            for (const auto& num : wordData) {
                numbers.push_back(num.asInt());
                DB += num.asInt();  // 累加数字总和
            }

            // 将该单词对应的数字列表存入 index
            index[word] = numbers;
        }
    }


    // 输出 index["apple"]
    if (index.find("apple") != index.end()) {
        std::cout << "Numbers in apple: ";
        for (int num : index["apple"]) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    }
    else {
        std::cout << "Key 'apple' not found in index." << std::endl;
    }

	for (int i = 0; Words[i]!=""; i++) {
		k_str = Words[i];
		k = charArrayToByteVector(k_str.c_str(), k_str.size());
        std::string sk_str(sk.begin(), sk.end());
        f1(k, sk_str);
        if (Dic1[l - 1].empty()) {
            st[Dic1[l]] = {};//question
        }
        else
        {
			st[Dic1[l - 1]] = xorKeys( xorKeys(st[Dic1[l - 1]], k),k_prime);
        }
		st[Dic1[l]] = xorKeys(xorKeys(st[Dic1[l]], k), k_prime);
		Conn_key = h1(st[Dic1[l]], 1);
		tmp2 = intTo4ByteVector(DB);
        std::copy(tmp2.begin(), tmp2.end(), std::back_inserter(tmp1));
        std::copy(st[Dic1[l-1]].begin(), st[Dic1[l - 1]].end(), std::back_inserter(tmp1));
		Conn_value = xorKeys(h2(st[Dic1[l]]),tmp1);
	}

    //httplib::Client serverCli("http://127.0.0.1:9001");
    //Json::Value data;
    //data["client_message"] = "Hello, Server!";

    //Json::StreamWriterBuilder writer;
    //std::string client_message = Json::writeString(writer, data);
    //auto serverRes = serverCli.Post("/from_client", client_message, "application/json");
    //if (serverRes && serverRes->status == 200) {
    //    std::cout << "Server response: " << serverRes->body << std::endl;
    //}
    //else {
    //    std::cerr << "Failed to connect to Server." << std::endl;
    //}
}
