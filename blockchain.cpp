    #include "blockchain.h"
    #include  "utils.h"
    #include  "cbf.h"
    #include <httplib.h>
    #include <map>
    #include <string>
    #include <vector>
    #include <iostream>
    #include <json/json.h>
    #include <unordered_map>
    #include <tuple>

    std::map<int, std::string> Dic1;  // 存储 l 和 v 对应关系
    std::map<std::string, CountingBloomFilter> CBFList;  // 存储 CBF 对应关系

    // Blockchain handler for receiving Up data and processing it

    void processUpData(const Json::Value& upData) {
	    cout << upData << endl;
        std::string k_prime;
	    std::string MK;
        std::string st;
        int l_new = upData["l_new"].asInt();
        std::string v_new = base64Decode(upData["v_new"].asString());

        // 尝试加载数据
        std::ifstream ifs("blockchain_data.json");
        if (ifs.is_open()) {
            Json::CharReaderBuilder reader;
            Json::Value savedData;
            std::string errs;

            if (Json::parseFromStream(reader, ifs, &savedData, &errs)) {
                k_prime = base64Decode(savedData["k_prime"].asString());
                MK = base64Decode(savedData["MK"].asString());
                std::cout << "Blockchain data loaded successfully." << std::endl;
            }
            else {
                throw std::runtime_error("Error parsing blockchain_data.json: " + errs);
            }
            ifs.close(); // 关闭文件
        }
        else {
            throw std::runtime_error("Failed to open blockchain_data.json.");
        }

        // 尝试加载数据
        std::ifstream cbfifs("CBFList.json");
        if (cbfifs.is_open()) {
            Json::CharReaderBuilder reader;
            Json::Value savedData;
            std::string errs;

            if (Json::parseFromStream(reader, ifs, &savedData, &errs)) {
                k_prime = base64Decode(savedData["k_prime"].asString());
                MK = base64Decode(savedData["MK"].asString());
                std::cout << "CBFList.json data loaded successfully." << std::endl;
            }
            else {
                throw std::runtime_error("Error parsing CBFList.json: " + errs);
            }


        for (const auto& item : upData) {
            std::string k = base64Decode(item["k"].asString());  // Decode k
            int l = item["l"].asInt();  // Get l value
            std::string CBFjStr = item["cbf"].asString();  // Get CBFj as string


            // Step 1: Set Dic1[l] = vl
		    Dic1[l_new] = v_new;

            // Process the Up data for each item (k, l, CBFj)
            std::string vl = Dic1[l];

            // Step 2: Loop while Up is not empty (this loop represents handling each Up item)
                        // Pop the current Up item
                //upData.removeIndex(0); // In practice, use actual removal from collection

                // Step 3: Update vl based on k, CBFj
            std::string tmp1 = xorKeys(k_prime, k);
            st = xorKeys(tmp1, vl);
            // Step 4: Create trapdoor (the method depends on encryption logic)
            std::string trapdoor = st;
            int ctr = l;
            CountingBloomFilter CBFj(3500, 7);
            while (ctr>=1) {
                ctr--;
			    vl = Dic1[ctr];
			    tmp1 = xorKeys(k_prime, k);
			    std::string trapdoor_tmp = xorKeys(tmp1, vl);
                // Step 5: Look for matching CBF
                if (CBFList.find(trapdoor_tmp) == CBFList.end()) {
                    continue;  // If no match, continue to next loop
                }
                else {
                    // CBF found, use it
                    CBFj = CBFList[trapdoor_tmp];
                    break;
                }
            }
		    CBFj.construct(CBFjStr);  // Construct CBFj from string
            // Step 6: Update the CBF List with the new CBFj
            CBFList[trapdoor] = CBFj;  // Store the updated CBF


            // Perform any additional logic, like sending responses, logging, etc.
        }

        cbfifs.close(); // 关闭文件
        }
        else {
            throw std::runtime_error("Failed to open CBFList.json.");
        }
    }

    // Handler to process the request from the client
    void handleUpData(const httplib::Request& req, httplib::Response& res) {
        // Parse the JSON body from the request
        Json::CharReaderBuilder reader;
        Json::Value root;
        std::string errs;
        std::istringstream s(req.body);


        if (!Json::parseFromStream(reader, s, &root, &errs)) {
            res.status = 400;
            res.set_content("Invalid JSON format", "text/plain");
            return;
        }

        // Step 1: Process the Up data
		cout << root["up_data"] << endl;
        processUpData(root["up_data"]);

        // Step 2: Respond with success
        res.status = 200;
        res.set_content("Up data processed successfully.", "text/plain");
    }

    void updateBlockChain() {
        httplib::Server svr;

        // Define the endpoint for receiving Up data
        svr.Post("/send_up_data", handleUpData);

        std::cout << "Blockchain server is running on port 9003..." << std::endl;
        svr.listen("127.0.0.1", 9003);
    }


    void startBlockChainServer()
    {
        std::string k_prime;
        std::string MK;
        httplib::Client cli("http://127.0.0.1:9000");
        auto res = cli.Get("/get_blockchain_data");

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

                std::ofstream ofs("block_data.json");
                ofs << saveData;
                ofs.close();

                std::cout << "Server setup complete. Data saved to block_data.json." << std::endl;
            }
            else {
                std::cerr << "Error parsing JSON from Trust Center: " << errs << std::endl;
            }
        }
        else {
            std::cerr << "Failed to connect to Trust Center!" << std::endl;
        }
    }