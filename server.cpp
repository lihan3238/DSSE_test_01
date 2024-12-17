#include "server.h"

const int BLOOM_SIZE = 256; // ��¡��������С

// Setup �������� Trust Center ��ȡ���ݲ����浽�ļ�
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

            // �������ݵ��ļ�
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

// ���������������� Client ����
void updateServer() {
    int l = 0;
    while (true) {
        try {
            std::string k_prime;
            std::string MK;

            // ���Լ�������
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
                ifs.close(); // �ر��ļ�
            }
            else {
                throw std::runtime_error("Failed to open server_data.json.");
            }


            // ���� Dic1.json �ļ�
            Json::Value dic1Data;
            std::ifstream ifsDic1("Dic1.json");

            if (!ifsDic1.is_open()) {
                // ��� Dic1.json �ļ������ڣ��򴴽�����ʼ��Ϊһ���յ� JSON ����
                std::cout << "Dic1.json not found, creating a new empty file." << std::endl;
                dic1Data = Json::Value(Json::objectValue); // ����һ���յ� JSON ����
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
            

            // ���÷�����
            httplib::Server svr;

            svr.Post("/send_conn_indop_data", [&dic1Data](const httplib::Request& req, httplib::Response& res) {
                // ʹ�� JsonCpp �������յ��� JSON ����
                Json::Value jsonData;
                Json::Reader reader;

                // ���Խ����������е� JSON ����
                if (reader.parse(req.body, jsonData)) {
                    // ��ӡ���յ�������
                    std::cout << "Received data from client: " << req.body << std::endl;

                    // ���������� connectorData


                        for (const auto& item : jsonData) {
                            std::string connector_key = base64Decode(item["connector_key"].asString());
                            std::string connector_value = base64Decode(item["connector_value"].asString());

                            // �� connector_key �� connector_value תΪ Base64 �ַ���
                            std::string connector_key_str = base64Encode(connector_key);
                            std::string connector_value_str = base64Encode(connector_value);

                            // �� Base64 ������ַ�����Ϊ��ֵ�Ա��浽 dic1Data
                            dic1Data[connector_key_str] = connector_value_str;
                        }


                    // ���������� indopData


                        for (const auto& item : jsonData) {
                            std::string indop_key = base64Decode(item["indop_key"].asString());
                            std::string indop_value = base64Decode(item["indop_value"].asString());

                            // �� indop_key �� indop_value תΪ Base64 �ַ���
                            std::string indop_key_str = base64Encode(indop_key);
                            std::string indop_value_str = base64Encode(indop_value);

                            // �� Base64 ������ַ�����Ϊ��ֵ�Ա��浽 dic1Data
                            dic1Data[indop_key_str] = indop_value_str;
                        }


                    std::cout << "Saving data..." << std::endl;

                    // ���� dic1Data �� Dic1.json
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

                    // ���سɹ���Ӧ
                    res.set_content("Data received and saved successfully.", "text/plain");
                    return;
                }
                else {
                    std::cerr << "Failed to parse JSON data: " << req.body << std::endl;
                    res.status = 400;
                    res.set_content("Invalid JSON format.", "text/plain");
                }
                });

            // ����������
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

        // �����յ��� JSON ����
        std::istringstream ss(req.body);
        if (Json::parseFromStream(reader, ss, &root, &errs)) {
            // ��ȡ tokens ����
            const Json::Value tokens = root["tokens"];
            for (const auto& token : tokens) {
                searchTokens.push_back(token.asString());
                std::cout << "Received token: " << token.asString() << std::endl;
            }

            // ������Ӧ
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

    // ������������
    map <string, string> Dic1;

    // �� Dic1.json �ļ�
    std::ifstream ifs("Dic1.json");

    if (!ifs.is_open()) {
        std::cerr << "Failed to open Dic1.json!" << std::endl;
        return;
    }

    // ʹ�� JsonCpp �����ļ�����
    Json::CharReaderBuilder reader;
    Json::Value dic1Data;
    std::string errs;

    // ���� JSON ����
    if (Json::parseFromStream(reader, ifs, &dic1Data, &errs)) {
        std::cout << "Dic1.json data loaded successfully." << std::endl;

        // ��ӡ���м�ֵ��
        for (const auto& key : dic1Data.getMemberNames()) {
            Dic1[key] = dic1Data[key].asString();
        }
    }
    else {
        std::cerr << "Error parsing Dic1.json: " << errs << std::endl;
    }
    ifs.close();  // �ر��ļ�

    int l = 0;
    // ����ļ��Ƿ���ڣ���������
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
        sifs.close(); // �ر��ļ�
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
                // ��ȡʣ�ಿ����Ϊ st
                stwjvll = tmp1.substr(4);

                // ������
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
                item["l"] = pair.second;  // ��Ϊ�ַ����洢
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

