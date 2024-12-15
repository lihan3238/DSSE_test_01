#include "server.h"


const int BLOOM_SIZE = 256; // ��¡��������С

// Setup �������� Trust Center ��ȡ���ݲ����浽�ļ�
void setupServerData() {
    std::vector<Byte> k_prime;
    std::vector<Byte> MK;
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
void startServer() {
    while (true) {
        try {
            std::vector<Byte> k_prime;
            std::vector<Byte> MK;

            // ���Լ�������
            std::ifstream ifs("server_data.json");
            if (ifs.is_open()) {
                Json::CharReaderBuilder reader;
                Json::Value savedData;
                std::string errs;

                if (Json::parseFromStream(reader, ifs, &savedData, &errs)) {
                    k_prime = base64Decode(savedData["k_prime"].asString());
                    MK = base64Decode(savedData["MK"].asString());
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

            // ���÷�����
            httplib::Server svr;

            svr.Post("/from_client", [](const httplib::Request& req, httplib::Response& res) {
                Json::CharReaderBuilder reader;
                Json::Value json;
                std::istringstream s(req.body);
                std::string errs;

                if (Json::parseFromStream(reader, s, &json, &errs)) {
                    std::cout << "Received message from Client: " << json["client_message"].asString() << std::endl;
                    res.set_content("Message received.", "text/plain");
                }
                else {
                    res.set_content("Error parsing message.", "text/plain");
                }
                });

            std::cout << "Server is running on port 9001." << std::endl;

            // ���������������ʧ���׳��쳣
            if (!svr.listen("127.0.0.1", 9001)) {
                throw std::runtime_error("Failed to start server on port 9001.");
            }

            break; // ��������ɹ������˳�ѭ��
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::cerr << "Retrying in 2 seconds..." << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }
}
