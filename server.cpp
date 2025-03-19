#include "server.h"

const int BLOOM_SIZE = 256; // ��¡��������С

// Setup �������� Trust Center ��ȡ���ݲ����浽�ļ�

void svr_reSet() {
    std::vector<std::string> filenames = { "Dic2.json", "server_data.json"};
    for (const std::string& file : filenames) {
        // 以截断模式打开文件（std::ios::trunc），清空文件内容
        std::ofstream ofs(file, std::ios::trunc);
        if (ofs) {
            std::cout << "Cleared: " << file << std::endl;
        }
        else {
            std::cerr << "Failed to clear: " << file << std::endl;
        }
    }
}

void setupServerData()//t:9000
{
    std::string k_prime;
    std::string MK;
    httplib::Client cli("http://127.0.0.1:9000");
    auto res = cli.Get("/get_server_data");

    if (res && res->status == 200)
    {
        std::string response_data = res->body;
        Json::CharReaderBuilder reader;
        Json::Value json;
        std::istringstream s(response_data);
        std::string errs;

        if (Json::parseFromStream(reader, s, &json, &errs))
        {
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
        else
        {
            std::cerr << "Error parsing JSON from Trust Center: " << errs << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to connect to Trust Center!" << std::endl;
    }
}

#include <thread>

// ���������������� Client ����
void updateServer()//svr:9001
{
    int l = 0;
    //while (true)
    //{
        try
        {
            std::string k_prime;
            std::string MK;

            // ���Լ�������
            std::ifstream ifs("server_data.json");
            if (ifs.is_open())
            {
                Json::CharReaderBuilder reader;
                Json::Value savedData;
                std::string errs;

                if (Json::parseFromStream(reader, ifs, &savedData, &errs))
                {
                    k_prime = base64Decode(savedData["k_prime"].asString());
                    MK = base64Decode(savedData["MK"].asString());
                    if (savedData.isMember("l"))
                    {
                        l = savedData["l"].asInt() + 1;
                    }
                    else
                    {
                        l = 1;
                    }
                    std::cout << "Server data loaded successfully." << std::endl;
                }
                else
                {
                    throw std::runtime_error("Error parsing server_data.json: " + errs);
                }
                ifs.close(); // �ر��ļ�
            }
            else
            {
                throw std::runtime_error("Failed to open server_data.json.");
            }

            // 保存更新后的 l 值回文件
            Json::Value updatedData;
            updatedData["k_prime"] = base64Encode(k_prime);
            updatedData["MK"] = base64Encode(MK);
            updatedData["l"] = l;

            std::ofstream ofs("server_data.json");
            if (ofs.is_open()) {
                Json::StreamWriterBuilder writer_01;
                ofs << Json::writeString(writer_01, updatedData);
                ofs.close();
                std::cout << "Updated 'l' value saved to server_data.json: " << l << std::endl;
            }
            else {
                std::cerr << "Failed to open server_data.json for writing!" << std::endl;
            }

            Json::Value dic2Data;
            std::ifstream ifsDic2("Dic2.json");

            if (!ifsDic2.is_open())
            {
                // 如果文件不存在，则创建一个新的空 JSON 文件
                std::cout << "Dic2.json not found, creating a new empty file." << std::endl;
                dic2Data = Json::Value(Json::objectValue); // 空 JSON 对象
                std::ofstream ofsDic2("Dic2.json");
                if (ofsDic2.is_open())
                {
                    Json::StreamWriterBuilder writer;
                    ofsDic2 << Json::writeString(writer, dic2Data);
                    ofsDic2.close();
                    std::cout << "Created empty Dic2.json." << std::endl;
                }
                else
                {
                    std::cerr << "Failed to create Dic2.json." << std::endl;
                    return;
                }
            }
            else
            {
                // **检查文件是否为空**
                ifsDic2.seekg(0, std::ios::end);
                if (ifsDic2.tellg() == 0)
                {
                    std::cout << "Dic2.json is empty. Initializing as an empty JSON object." << std::endl;
                    dic2Data = Json::Value(Json::objectValue); // 直接初始化为空 JSON
                    ifsDic2.close();
                }
                else
                {
                    // **文件非空，正常解析 JSON**
                    ifsDic2.seekg(0, std::ios::beg);  // 重置读取位置
                    Json::CharReaderBuilder reader;
                    std::string errs;
                    if (!Json::parseFromStream(reader, ifsDic2, &dic2Data, &errs))
                    {
                        std::cerr << "Failed to parse Dic2.json: " << errs << std::endl;
                        return;
                    }
                    std::cout << "Loaded Dic2.json content successfully." << std::endl;
                    ifsDic2.close();
                }
            }

            // ���÷�����
            httplib::Server svr;

            svr.Post("/send_conn_indop_data", [&dic2Data, &svr](const httplib::Request &req, httplib::Response &res)
                     {
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

                            // �� Base64 ������ַ�����Ϊ��ֵ�Ա��浽 dic2Data
                            dic2Data[connector_key_str] = connector_value_str;
                        }


                    // ���������� indopData


                        for (const auto& item : jsonData) {
                            std::string indop_key = base64Decode(item["indop_key"].asString());
                            std::string indop_value = base64Decode(item["indop_value"].asString());

                            // �� indop_key �� indop_value תΪ Base64 �ַ���
                            std::string indop_key_str = base64Encode(indop_key);
                            std::string indop_value_str = base64Encode(indop_value);

                            // �� Base64 ������ַ�����Ϊ��ֵ�Ա��浽 dic1Data
                            dic2Data[indop_key_str] = indop_value_str;
                        }


                    std::cout << "Saving data..." << std::endl;

                    // ���� dic1Data �� Dic1.json
                    std::ofstream ofsDic2("Dic2.json");
                    if (ofsDic2.is_open()) {
                        Json::StreamWriterBuilder writer;
                        ofsDic2 << Json::writeString(writer, dic2Data);
                        ofsDic2.close();
                        std::cout << "Data saved to Dic2.json successfully." << std::endl;
                    }
                    else {
                        std::cerr << "Failed to open Dic2.json for writing." << std::endl;
                    }

                    // ���سɹ���Ӧ
                    res.set_content("Data received and saved successfully.", "text/plain");
                    svr.stop();
                    //return;
                }
                else {
                    std::cerr << "Failed to parse JSON data: " << req.body << std::endl;
                    res.status = 400;
                    res.set_content("Invalid JSON format.", "text/plain");
                } });

            // ����������
            svr.listen("127.0.0.1", 9001);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
   // }
}


void searchServer()//svr:9008
{
    httplib::Server svr;

    std::vector<std::pair<std::vector<string>, int>> Rsearch;

    svr.Post("/search", [&Rsearch](const httplib::Request& req, httplib::Response& res)
        {
            vector<string> searchTokens;
            try
            {
                Json::CharReaderBuilder reader;
                Json::Value root;
                std::string errs;

                // �����յ��� JSON ����
                std::istringstream ss(req.body);
                if (Json::parseFromStream(reader, ss, &root, &errs))
                {
                    // ��ȡ tokens ����
                    const Json::Value tokens = root["tokens"];
                    for (const auto& token : tokens)
                    {
                        searchTokens.push_back(base64Decode(token.asString()));
                        //qwqstd::cout << "Received token: " << token.asString() << std::endl;
                    }

                    // ������Ӧ
                    res.set_content("Received search tokens", "text/plain");
                }
                else
                {
                    res.status = 400; // Bad request
                    res.set_content("Invalid JSON", "text/plain");
                }
            }
            catch (const std::exception& e)
            {
                res.status = 500; // Internal server error
                res.set_content("Server error", "text/plain");
            }

            // ������������
            map<string, string> Dic2;

            // �� Dic2.json �ļ�
            std::ifstream ifs("Dic2.json");

            if (!ifs.is_open())
            {
                std::cerr << "Failed to open Dic2.json!" << std::endl;
                return;
            }

            // ʹ�� JsonCpp �����ļ�����
            Json::CharReaderBuilder reader;
            Json::Value dic2Data;
            std::string errs;

            // ���� JSON ����
            if (Json::parseFromStream(reader, ifs, &dic2Data, &errs))
            {
                std::cout << "Dic2.json data loaded successfully." << std::endl;

                // ��ӡ���м�ֵ��
                for (const auto& key : dic2Data.getMemberNames())
                {
                    Dic2[key] = dic2Data[key].asString();
                }
            }
            else
            {
                std::cerr << "Error parsing Dic2.json: " << errs << std::endl;
            }
            ifs.close(); // �ر��ļ�

            int l = 0;
            // ����ļ��Ƿ���ڣ���������
            std::ifstream sifs("server_data.json");
            if (sifs.is_open())
            {
                Json::CharReaderBuilder reader;
                Json::Value savedData;
                std::string errs;
                if (Json::parseFromStream(reader, sifs, &savedData, &errs))
                {
                    if (savedData.isMember("l"))
                    {
                        l = savedData["l"].asInt();
                    }
                }
                sifs.close(); // �ر��ļ�
            }

            //std::vector<std::pair<std::vector<string>, int>> Rsearch;
            int DB = 0;
			int l0 = l;
            std::string stwjvll = "";

            // 计时点

            auto start = chrono::high_resolution_clock::now();


            for (const auto& token : searchTokens)
            {
                l = l0;
                string stwjvl = token;
                while (stwjvl.size() > 0)
                {
                    int flag1 = 0;
                    for (char c : stwjvl) {
                        if (c != '\0') {
                            flag1++;
                        }
                    }
                    if (flag1 == 0) {
                        break;
                    }
                    flag1 = 0;
                    string connector_key = h1(stwjvl, 1);
                    //qwq std::cout << "connector_key" << connector_key << endl;
                    //qwqstd::cout << base64Encode(connector_key) << endl;
                    string connector_value = base64Decode(Dic2[base64Encode(connector_key)]);
                    //qwqstd::cout << "connector_value" << connector_value << endl;
                    string tmp1 = xorKeys(h2(stwjvl), connector_value);
                    if (tmp1.length() >= 4)
                    {
                        std::string firstFour = tmp1.substr(0, 4);
                        DB = stringToInt(firstFour);
                        // ��ȡʣ�ಿ����Ϊ st
                        stwjvll = tmp1.substr(4);

                        // ������
                        //qwqstd::cout << "Front part (as int): " << DB << std::endl;
                        //qwqstd::cout << "Remaining part (st): " << stwjvll << std::endl;
                    }
                    else
                    {
                        std::cerr << "Error: connector_value is too short." << std::endl;
                    }

                    vector<string> Rwjvl = {};
                    std::string indopwjvlm;
                    for (int i = 0; i < DB; ++i)
                    {
                        //qwqstd::cout << " m = " << i << endl;
                        string indop_key = h34(stwjvl, i, 3);
                        string indop_value = base64Decode(Dic2[base64Encode(indop_key)]);
                        string indopwjvlm = xorKeys(h34(stwjvl, i, 4), indop_value);
                        //if (tmp2.length() >= 4)
                        //{
                        //    std::string firstFour = tmp2.substr(0, 28);
                        //    indopwjvlm = tmp2.substr(4);
                        //}

                       //qwq std::cout << "R新增 indopwjvlm BASE64: " << indopwjvlm << endl;

                        //qwqstd::cout << "indopwjvlm.size():" << indopwjvlm.size() << endl;

                        Rwjvl.push_back( base64Encode(indopwjvlm));
                    }
                    Rsearch.push_back(std::make_pair(Rwjvl, l));
                    stwjvl = stwjvll;
                    l--;
                }
                // sendRsearchtoClient and BC

            //    httplib::Server svr;

            //    svr.Get("/send_Rsearch_Request", [&](const httplib::Request &, httplib::Response &res)
            //            {
                        //std::cout << "Received Rsearch Request" << endl;
            //        Json::Value root(Json::arrayValue);
            //        for (const auto& pair : Rsearch) {
            //            Json::Value item;
            //            Json::Value strArray(Json::arrayValue);
            //            for (const auto& str : pair.first) {
            //                strArray.append(str);
            //            }
            //            item["R"] = strArray;
            //            item["l"] = pair.second;  // ��Ϊ�ַ����洢
            //            root.append(item);
            //        }
            //        Json::StreamWriterBuilder writer;
            //        std::string response_data = Json::writeString(writer, root);

            //        res.set_content(response_data, "application/json");
            //        std::cout << "Sent R data to Client." << std::endl; });
            //    std::cout << "Server running at http://localhost:9008..." << std::endl;
            //    svr.listen("localhost", 9008);
            }

            // 计时点
            
            auto stop_1 = chrono::high_resolution_clock::now();

            chrono::duration<double, milli> duration_1 = stop_1 - start;
            cout << fixed << setprecision(3) << "Time taken by searchServer: " << duration_1.count() << " ms" << endl;

        }
    );
    // sendRsearchtoClient and BC


    svr.Get("/send_Rsearch_Request", [&Rsearch, &svr](const httplib::Request&, httplib::Response& res)
        {
            std::cout << "Received Rsearch Request" << endl;
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
            std::cout << "Sent R data to Client." << std::endl; 
			svr.stop();
        });
    std::cout << "Server running at http://localhost:9008..." << std::endl;
    svr.listen("localhost", 9008);
    //std::cout << "Server running at http://localhost:9001..." << std::endl;
    //svr.listen("localhost", 9001);
}
