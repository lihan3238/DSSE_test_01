#include "client.h"

const int LAMBDA = 256; // ��ȫ���� ��
const int BLOOM_SIZE = 256; // ��¡��������С
const std::string IV = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10"; // IV as string

// Setup �������� Trust Center ��ȡ���ݲ����浽�ļ�
void setupClientData() {
    std::string sk;
    std::string MK;
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

            // �������ݵ��ļ�
            Json::Value saveData;
            saveData["sk"] = json["sk"];
            saveData["MK"] = json["MK"];
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

// �������ͨ�ŵĺ���
void updateClient() {
    std::string sk;
    std::string MK;
    std::string sk_prime;
    std::unordered_map<int, std::string> Dic1;
    std::string Words[100];
    int l = 0;
    // ����ļ��Ƿ���ڣ���������
    std::ifstream ifs("client_data.json");
    if (ifs.is_open()) {
        Json::CharReaderBuilder reader;
        Json::Value savedData;
        std::string errs;
        if (Json::parseFromStream(reader, ifs, &savedData, &errs)) {
            sk = base64Decode(savedData["sk"].asString());
            MK = base64Decode(savedData["MK"].asString());

            if (savedData.isMember("l")) {
                l = savedData["l"].asInt() + 1;
            }
            else {
                l = 1;
            }
        }
        ifs.close(); // �ر��ļ�
    }

    Dic1[l] = generateRandomKey(LAMBDA / 8);

    // ������º�� l ֵ���ļ�
    Json::Value updatedData;
    updatedData["sk"] = base64Encode(sk);
    updatedData["MK"] = base64Encode(MK);
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
    std::string k_prime(sk_prime.size(), '\0');
    f1(k_prime, sk_prime);

    std::string tmp1;
    std::string tmp2;
    std::string k;
    std::string k_str;
    std::map<std::string, std::string> st;
    std::string Conn_key;
    std::string Conn_value;

    std::map<std::string, std::vector<int>> index;

    // �򿪲���ȡJSON�ļ�
    std::ifstream ifs2("update.json");
    if (!ifs2.is_open()) {
        std::cerr << "Failed to open the JSON file." << std::endl;
        return;
    }

    Json::Reader reader;
    Json::Value jsonData;

    // ���� JSON �ļ�
    if (!reader.parse(ifs2, jsonData)) {
        std::cerr << "Failed to parse JSON data." << std::endl;
        return;
    }

    int DB = 0;
    ifs2.close();

    // ���� JSON �ļ��ļ������� Words ����
    int wc = 0;
    for (const auto& key : jsonData.getMemberNames()) {
        if (key.empty()) {
            std::cout << "Encountered empty string, stopping the loop." << std::endl;
            break; // ����ѭ��
        }
        if (wc < 100) {
            Words[wc++] = key; // �洢���� Words ����
        }
        else {
            std::cerr << "Words array size exceeded maximum limit." << std::endl;
            break;
        }
    }

    std::string word;
    // ���� Words �����е�ÿ�����ʣ����� JSON �ж�Ӧ������
    for (int i = 0; i < wc; i++) {
        word = Words[i];
        if (word.empty()) {
            std::cout << "Encountered empty string, stopping the loop." << std::endl;
            break; // ����ѭ��
        }
        // ��� JSON ���Ƿ��иõ���
        if (jsonData.isMember(word)) {
            const Json::Value& wordData = jsonData[word];
            std::vector<int> numbers;

            // ��ȡ�õ��ʶ�Ӧ���������Ԫ��
            for (const auto& num : wordData) {
                numbers.push_back(num.asInt());
                DB++;  // DB����
            }

            // ���õ��ʶ�Ӧ�������б���� index
            index[word] = numbers;
        }
    }

    std::vector<std::tuple<std::string, int, CountingBloomFilter>> Up;
    std::map<std::string, std::string> connectorData;
    std::map<std::string, std::string> indopData;

    for (int i = 0; i < wc; i++) {
        word = Words[i];
        // ȷ���õ����� index �д���
        if (index.find(word) == index.end()) {
            std::cerr << "Warning: Word '" << word << "' not found in index, skipping..." << std::endl;
            continue; // ����δ�� index ���ҵ��ĵ���
        }
        k_str = word;
        k = k_str;
        std::string sk_str = sk;
        f1(k, sk_str);

        if (Dic1[l - 1].empty()) {
            st[Dic1[l]] = {}; // question
        }
        else {
            st[Dic1[l - 1]] = xorKeys(xorKeys(st[Dic1[l - 1]], k), k_prime);
        }
        st[Dic1[l]] = xorKeys(xorKeys(st[Dic1[l]], k), k_prime);
        Conn_key = h1(st[Dic1[l]], 1);
        tmp2 = intTo4ByteString(DB);
        tmp1.append(tmp2);
        tmp1.append(st[Dic1[l - 1]]);
        Conn_value = xorKeys(h2(st[Dic1[l]]), tmp1);
        connectorData[Conn_key] = Conn_value;

        std::string skey = f2(Dic1[l], sk, 32);

        // ���� CBFj,vl
        CountingBloomFilter CBFj_vl(3500, 7);
        std::string tmp3;

        for (size_t m = 0; m < index[word].size(); ++m) {
            std::string indwj_vl_m = intTo32ByteString(index[word][m]);
            std::string opwj_vl_m = "add"; // �ٶ���������Ϊ add

            // ���� CBFj,vl
            CBFj_vl.update(indwj_vl_m, opwj_vl_m);
            tmp3 = stringTo4ByteString(opwj_vl_m);
            // ���ɼ��ܵ� indopwj_vl,m
            tmp3.append(indwj_vl_m);

            std::string indopwj_vl_m = aesEncrypt(tmp3, skey, IV);

            // ���� indop_key �� indop_value
            std::string indop_key = h34(st[Dic1[l]], m, 3);
            std::string tmp4(4, '\0');
            tmp4.append(indopwj_vl_m);
            std::string indop_value = xorKeys(h34(st[Dic1[l]], m, 4), tmp4);

            indopData[indop_key] = indop_value;
        }

        Up.push_back(std::make_tuple(k, l, CBFj_vl));
    }

    httplib::Client serverCli("http://127.0.0.1:9001");
    Json::Value data;

    // ʹ�ô�ͳ�ķ�ʽ���� connectorData
    for (auto it = connectorData.begin(); it != connectorData.end(); ++it) {
        Json::Value item;

        // �� key �� value תΪ Base64 �ַ��������� JSON
        item["connector_key"] = base64Encode(it->first);   // it->first Ϊ key
        item["connector_value"] = base64Encode(it->second); // it->second Ϊ value

        data.append(item);  // �� item ���뵽 JSON ����
    }

    for (auto it = indopData.begin(); it != indopData.end(); ++it) {
        Json::Value item;
        // �� key �� value תΪ Base64 �ַ��������� JSON
        item["indop_key"] = base64Encode(it->first);   // it->first Ϊ key
        item["indop_value"] = base64Encode(it->second); // it->second Ϊ value
        data.append(item);  // �� item ���뵽 JSON ����
    }

    Json::StreamWriterBuilder writer;
    std::string client_message = Json::writeString(writer, data);
    auto res = serverCli.Post("/send_conn_indop_data", client_message, "application/json");

    if (res && res->status == 200) {
        std::cout << "Connector data sent to CS." << std::endl;
    }
    else {
        std::cerr << "Failed to send Connector data to CS." <<res->status<< std::endl;
    }

    //������
    // 
    // ���Լ�������
    Json::Value root;
	map <int, string> Dic2;

    // ���Զ�ȡ��������
    std::ifstream dic2ifs("Dic2.json");
    if (dic2ifs.is_open()) {
        Json::CharReaderBuilder reader;
        Json::Value root;
        std::string errs;

        // ��ȡ������ JSON �ļ�����
        if (Json::parseFromStream(reader, dic2ifs, &root, &errs)) {
            for (const auto& item : root.getMemberNames()) {
                int l = std::stoi(item);
                Dic2[l] = root[item].asString();
            }
            std::cout << "Dic2 data loaded successfully." << std::endl;
        }
        else {
            std::cerr << "Failed to parse Dic2.json: " << errs << std::endl;
        }
        dic2ifs.close();
    }
    else {
        std::cout << "Dic2.json does not exist or is empty. Initializing new map." << std::endl;
    }

	Dic2[l] = Dic1[l];
    // �� map ���ݴ��� JSON ����
    for (const auto& item : Dic2) {
        root[std::to_string(item.first)] = item.second;
    }

    // ���浽 Dic2.json �ļ�
    std::ofstream dic2ofs("Dic2.json");
    if (dic2ofs.is_open()) {
        Json::StreamWriterBuilder writer;
        dic2ofs << Json::writeString(writer, root);
        dic2ofs.close();
        std::cout << "Dic2 data saved successfully." << std::endl;
    }
    else {
        std::cerr << "Failed to open Dic2.json for writing!" << std::endl;
    }

    //Up

        // ���Զ�ȡ��������
	map <string, CountingBloomFilter> CBFList;
    std::ifstream CBFifs("CBFList.json");
    if (CBFifs.is_open()) {
        Json::CharReaderBuilder reader;
        Json::Value root;
        std::string errs;

        // ��ȡ������ JSON �ļ�����
        if (Json::parseFromStream(reader, CBFifs, &root, &errs)) {
            for (const auto& item : root.getMemberNames()) {
                const std::string& cbf_key = item;
                const std::string& cbf_data = root[cbf_key].asString();
                CountingBloomFilter cbf(3500, 7);
                cbf.construct(cbf_data);
                CBFList[cbf_key] =cbf;  // �����л� CBF
            }
            std::cout << "CBFList data loaded successfully." << std::endl;
        }
        else {
            std::cerr << "Failed to parse CBFList.json: " << errs << std::endl;
        }
        CBFifs.close();
    }
    else {
        std::cout << "Dic2.json does not exist or is empty. Initializing new map." << std::endl;
    }

        string stwjvl;
        for (const auto& item : Up) {
            std::string k = get<0>(item);
            int l = get<1>(item);
			CountingBloomFilter CBFjStr(3500, 7);
			CBFjStr = get<2>(item);

            // Step 1: Set Dic1[l] = vl
            Dic1[l] = Dic2[l];

            // Process the Up data for each item (k, l, CBFj)
            std::string vl = Dic2[l];

            // Step 2: Loop while Up is not empty (this loop represents handling each Up item)
                        // Pop the current Up item
                //upData.removeIndex(0); // In practice, use actual removal from collection

                // Step 3: Update vl based on k, CBFj
            std::string tmp1 = xorKeys(k_prime, k);
            stwjvl = xorKeys(tmp1, vl);
            // Step 4: Create trapdoor (the method depends on encryption logic)
            std::string trapdoor = stwjvl;
            int ctr = l;
            CountingBloomFilter CBFj(3500, 7);
            while (ctr >= 1) {
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
            CBFj=CBFj+CBFjStr;  // Construct CBFj from string
            // Step 6: Update the CBF List with the new CBFj
            CBFList[trapdoor] = CBFj;  // Store the updated CBF


            // Perform any additional logic, like sending responses, logging, etc.
        }

		// ������º�� CBFList ����
		Json::Value cbfData;
        

		for (const auto& item : CBFList) {
			Json::Value cbfItem;
			cbfItem["key"] = item.first;
			cbfItem["data"] = item.second.toJson();
			cbfData.append(cbfItem);
		}
		std::ofstream cbfOfs("CBFList.json");
		if (cbfOfs.is_open()) {
			Json::StreamWriterBuilder writer;
			cbfOfs << Json::writeString(writer, cbfData);
			cbfOfs.close();
			std::cout << "CBFList data saved successfully." << std::endl;
		}
        else {
            std::cerr << "Failed to open CBFList.json for writing!" << std::endl;
        }

    return;
}

std::vector<std::string> searchToken(const std::vector<std::string>& words, string Q, int q) {
    std::map<int, std::string> Dic2;
    int l_n = 0;
    std::vector<std::string> searchTokens;

    // ���Զ�ȡ��������
    std::ifstream dic2ifs("Dic2.json");
    if (dic2ifs.is_open()) {
        Json::CharReaderBuilder reader;
        Json::Value root;
        std::string errs;

        // ��ȡ������ JSON �ļ�����
        if (Json::parseFromStream(reader, dic2ifs, &root, &errs)) {
            for (const auto& item : root.getMemberNames()) {
                int l = std::stoi(item);
                if (l > l_n) {
                    l_n = l;
                }
                Dic2[l] = root[item].asString();
            }
            std::cout << "Dic2 data loaded successfully." << std::endl;
        }
        else {
            std::cerr << "Failed to parse Dic2.json: " << errs << std::endl;
        }
        dic2ifs.close();
    }
    else {
        std::cout << "Dic2.json does not exist or is empty. Initializing new map." << std::endl;
    }

    std::string vl = Dic2[l_n];
    std::string sk;
    std::string MK;

    // ����ļ��Ƿ���ڣ���������
    std::ifstream ifs("client_data.json");
    if (ifs.is_open()) {
        Json::CharReaderBuilder reader;
        Json::Value savedData;
        std::string errs;
        if (Json::parseFromStream(reader, ifs, &savedData, &errs)) {
            sk = base64Decode(savedData["sk"].asString());
            MK = base64Decode(savedData["MK"].asString());
        }
        ifs.close(); // �ر��ļ�
    }

    std::string sk_prime = xorKeys(MK, sk);
    std::string k_prime(sk_prime.size(), '\0');
    f1(k_prime, sk_prime); // ���ܴ���

    // ��ÿ�����ʽ��д���
    for (const auto& word : words) {
        std::string k = word;
        f1(k, sk); // �ٴμ���
        searchTokens.push_back(xorKeys(xorKeys(k, k_prime), vl)); // ������������
    }

    return searchTokens; // ���ش�������������
}


void searchClient(std::vector<std::string> searchTokens, string Q, int q) {

    map <int, string> Dic2;

    // ���Զ�ȡ��������
    std::ifstream dic2ifs("Dic2.json");
    if (dic2ifs.is_open()) {
        Json::CharReaderBuilder reader;
        Json::Value root;
        std::string errs;

        // ��ȡ������ JSON �ļ�����
        if (Json::parseFromStream(reader, dic2ifs, &root, &errs)) {
            for (const auto& item : root.getMemberNames()) {
                int l = std::stoi(item);
                Dic2[l] = root[item].asString();
            }
            std::cout << "Dic2 data loaded successfully." << std::endl;
        }
        else {
            std::cerr << "Failed to parse Dic2.json: " << errs << std::endl;
        }
        dic2ifs.close();
    }
    else {
        std::cout << "Dic2.json does not exist or is empty. Initializing new map." << std::endl;
    }

	//sendtoserver
    std::string jsonData = "{ \"tokens\": [";
    for (size_t i = 0; i < searchTokens.size(); ++i) {
        jsonData += "\"" + searchTokens[i] + "\"";
        if (i != searchTokens.size() - 1) {
            jsonData += ", ";
        }
    }
    jsonData += "] }";

    // ʹ�� httplib ���� HTTP ����
    httplib::Client cli("http://localhost:9001");  // ��������ַ�Ͷ˿�
    auto res = cli.Post("/search", jsonData, "application/json");

    if (res && res->status == 200) {
        std::cout << "Server response: " << res->body << std::endl;
    }
    else {
        std::cerr << "Request failed!" << std::endl;
    }


	//request

    httplib::Client cli1("http://127.0.0.1:9001");
    auto res1 = cli1.Get("/send_Rsearch_Request");

    std::vector<std::pair<std::vector<string>, int>> Rsearch;
    std::vector<string> Rwjvl;
    if (res1 && res1->status == 200) {
        std::string response_data = res1->body;

        Json::CharReaderBuilder reader;
        Json::Value root;
        std::string errs;
        std::istringstream ss(res1->body);

        if (Json::parseFromStream(reader, ss, &root, &errs)) {
            const Json::Value items = root;

            std::cout << "Received Rsearch data:" << std::endl;
            for (const auto& item : items) {
                int l = item["l"].asInt();
                for (const auto& str : item["R"]) {
                    Rwjvl.push_back(str.asString());
                }
                Rsearch.push_back(std::make_pair(Rwjvl, l));
                std::cout << "l: " << l << ", st: " << Rwjvl[0] << std::endl;
            }
        }
        else {
            std::cerr << "Error parsing response: " << errs << std::endl;
        }

        std::vector<string> Ind;
        std::vector<string> del;

        for (const auto& item : Rsearch) {
            std::vector<string> Rwjvl = item.first;
            int l = item.second;
            std::string st = Rwjvl[0];
            std::string sk;
            std::string MK;
            // ����ļ��Ƿ���ڣ���������
            std::ifstream ifs("client_data.json");
            if (ifs.is_open()) {
                Json::CharReaderBuilder reader;
                Json::Value savedData;
                std::string errs;
                if (Json::parseFromStream(reader, ifs, &savedData, &errs)) {
                    sk = base64Decode(savedData["sk"].asString());
                    MK = base64Decode(savedData["MK"].asString());
                }
                ifs.close(); // �ر��ļ�
            }
            std::string sk_prime = xorKeys(MK, sk);
            std::string k_prime(sk_prime.size(), '\0');
            f1(k_prime, sk_prime); // ���ܴ���
            std::string k = xorKeys(xorKeys(st, k_prime), Dic2[l]);
            std::string skey = f2(Dic2[l], sk, 32);
            std::string tmp1 = aesDecrypt(k, skey, IV);
            std::string op = tmp1.substr(0, 4);
            std::string ind = tmp1.substr(4);
            if (op == "add") {
                Ind.push_back(ind);
            }
            else if (op == "del") {
                del.push_back(ind);
            }
        }

        //Ind = Ind - del;
        for (const std::string& item : del) {
            // �Ƴ� Ind �����е��� item ��Ԫ��
            Ind.erase(std::remove(Ind.begin(), Ind.end(), item), Ind.end());
        }

        CountingBloomFilter RCBF(3500, 7);

        for (const std::string& item : Ind) {
            RCBF.update(item, "add");
        }
        //send RCBF to bc

        //advice=Verify(RCBF,searchtoken);
        //if(advice==true)
        vector<string> Finalset;
        for (const std::string& item : Ind) {
            if (Q == "disjunctive")
            {
                if (RCBF.check(item))
                {
                    Finalset.push_back(item);
                }
            }
            else if (Q == "conjunctive")
            {
                if (RCBF.repeatCheck(item, q))
                {
                    Finalset.push_back(item);
                }
            }
        }

        for (const std::string& item : Finalset) {
            std::cout << "Final set: " << item << std::endl;
        }
    }
    }
