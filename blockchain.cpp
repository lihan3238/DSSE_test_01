#include "blockchain.h"


    // ��������
    void BlockchainNode::put(const std::string& key, const std::vector<Byte>& value) {
        storage[key] = value;
        std::cout << "Data stored with key: " << key << std::endl;
    }

    // ��ѯ����
    std::vector<Byte>  BlockchainNode::get(const std::string& key) {
        if (storage.find(key) != storage.end()) {
            return storage[key];
        }
        else {
            throw std::runtime_error("Key not found: " + key);
        }
    }

    // ���������洢
    std::unordered_map<std::string, std::vector<Byte>>  BlockchainNode::getAllData() {
        return storage;
    }

    // ���л�����Ϊ JSON
    Json::Value BlockchainNode::serialize() {
        Json::Value json;
        for (auto it = storage.begin(); it != storage.end(); ++it) {
            const std::string& key = it->first;
            const std::vector<Byte>& value = it->second;
            std::string encoded_value(value.begin(), value.end());
            json[key] = encoded_value;
        }
        return json;
    }

    // �� JSON �����л�����
    void  BlockchainNode::deserialize(const Json::Value& json) {
        for (const auto& key : json.getMemberNames()) {
            std::string encoded_value = json[key].asString();
            std::vector<Byte> value(encoded_value.begin(), encoded_value.end());
            storage[key] = value;
        }
    }

    // �������ݵ������ļ�
    void  BlockchainNode::saveToFile() {
        Json::Value json = serialize();
        std::ofstream ofs(storageFile);
        if (ofs.is_open()) {
            Json::StreamWriterBuilder writer;
            ofs << Json::writeString(writer, json);
            ofs.close();
            std::cout << "Blockchain data saved to " << storageFile << std::endl;
        }
        else {
            std::cerr << "Failed to open file for saving: " << storageFile << std::endl;
        }
    }

    // �ӱ����ļ���������
    void  BlockchainNode::loadFromFile() {
        std::ifstream ifs(storageFile);
        if (ifs.is_open()) {
            Json::CharReaderBuilder reader;
            Json::Value json;
            std::string errs;

            if (Json::parseFromStream(reader, ifs, &json, &errs)) {
                deserialize(json);
                std::cout << "Blockchain data loaded from " << storageFile << std::endl;
            }
            else {
                std::cerr << "Failed to parse blockchain data file: " << errs << std::endl;
            }
            ifs.close();
        }
        else {
            std::cout << "No previous blockchain data file found. Starting fresh." << std::endl;
        }
    }

void startBlockChainServer() {
    BlockchainNode node;

    // ����ʱ��������
    node.loadFromFile();

    // ���������� HTTP ����
    httplib::Server svr;

    svr.Post("/put", [&](const httplib::Request& req, httplib::Response& res) {
        Json::CharReaderBuilder reader;
        Json::Value json;
        std::string errs;

        // ֱ�Ӵ��ַ�������
        if (Json::CharReaderBuilder().newCharReader()->parse(req.body.c_str(), req.body.c_str() + req.body.size(), &json, &errs)) {
            std::string key = json["key"].asString();
            std::string value_str = json["value"].asString();
            std::vector<Byte> value(value_str.begin(), value_str.end());

            node.put(key, value);

            res.set_content("Data inserted successfully.", "text/plain");
        }
        else {
            res.set_content("Invalid JSON.", "text/plain");
        }
        });

    svr.Get(R"(/get/(\w+))", [&](const httplib::Request& req, httplib::Response& res) {
        std::string key = req.matches[1];
        try {
            std::vector<Byte> value = node.get(key);
            std::string value_str(value.begin(), value.end());
            res.set_content(value_str, "application/octet-stream");
        }
        catch (const std::exception& e) {
            res.set_content(e.what(), "text/plain");
        }
        });

    svr.Get("/get_all", [&](const httplib::Request&, httplib::Response& res) {
        Json::Value json = node.serialize();
        Json::StreamWriterBuilder writer;
        res.set_content(Json::writeString(writer, json), "application/json");
        });

    std::cout << "Blockchain Node is running on port 8080..." << std::endl;

    // ���з���
    svr.listen("127.0.0.1", 8080);

    // ֹͣʱ��������
    node.saveToFile();
}