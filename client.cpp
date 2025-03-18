#include "client.h"

const int LAMBDA = 256; // ��ȫ���� ��
const int BLOOM_SIZE = 256; // ��¡��������С
//const int BLOOM_HASHES = 20; // ��¡��������ϣ��������
//const int BLOOM_BITS = 400000; // ��¡������λ��

const std::string IV = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10"; // IV as string
//vector<string> FileInd = {"1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26","27","28","29","30"};


void cli_reSet() {
        std::vector<std::string> filenames = {"Dic1.json", "CBFList.json", "client_data.json", "update.json" };
        for (const std::string& file : filenames) {
            // �Խض�ģʽ���ļ���std::ios::trunc��������ļ�����
            std::ofstream ofs(file, std::ios::trunc);
            if (ofs) {
                std::cout << "Cleared: " << file << std::endl;
            }
            else {
                std::cerr << "Failed to clear: " << file << std::endl;
            }
        }
}


bool verifyCBF(const CountingBloomFilter& RCBF, std::vector<std::string>& searchTokens, int BLOOM_HASHES, int BLOOM_BITS) {


        // ���Զ�ȡ��������
        map <string, CountingBloomFilter> CBFList;
        std::ifstream CBFifs("CBFList.json");
        if (CBFifs.is_open()) {
            Json::CharReaderBuilder reader;
            Json::Value root;
            std::string errs;

            // ��ȡ������ JSON �ļ�����
            if (Json::parseFromStream(reader, CBFifs, &root, &errs)) {
                if (root.isArray()) { // ȷ������������
                    for (const auto& item : root) {
                        if (item.isObject() && item.isMember("key") && item.isMember("data")) {
                            const std::string& cbf_key = item["key"].asString();
                            const Json::Value& cbf_data = item["data"];

                            // �� data ת��Ϊ CountingBloomFilter �ĸ�ʽ
                                CountingBloomFilter cbf(BLOOM_BITS, BLOOM_HASHES);
                                //std::string serialized_data;
                                //cout << cbf_data;
                                cbf.fromJson(cbf_data);
                                CBFList[cbf_key] = cbf;  // �����л� CBF

                        }
                    }
                    std::cout << "CBFList data loaded successfully." << std::endl;
                }
                else {
                    std::cerr << "JSON root is not an array." << std::endl;
                }
            }
            else {
                std::cerr << "Failed to parse CBFList.json: " << errs << std::endl;
            }
            CBFifs.close();
        }
        else {
            std::cout << "CBFList.json does not exist or is empty. Initializing new map." << std::endl;
        }

    CountingBloomFilter BCBF(BLOOM_BITS, BLOOM_HASHES);
    for (const auto& st : searchTokens)
    {
		BCBF =BCBF+ CBFList[st];
    }
	if (RCBF == BCBF)
	{
		return true;
	}
	else
	{
		return false;
	}

}

//qwq
// Setup �������� Trust Center ��ȡ���ݲ����浽�ļ�
void setupClientData(int BLOOM_HASHES, int BLOOM_BITS) {
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
            //BLOOM_HASHES = json["BLOOM_HASHES"].asInt();
            //BLOOM_BITS = json["BLOOM_BITS"].asInt();
            // �������ݵ��ļ�
            Json::Value saveData;
            saveData["sk"] = json["sk"];
            saveData["MK"] = json["MK"];
            saveData["BLOOM_HASHES"] = BLOOM_HASHES;
            saveData["BLOOM_BITS"] = BLOOM_BITS;

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
void updateClient(string updateFile) {
    int BLOOM_HASHES = -1; // Ĭ��ֵ
    int BLOOM_BITS = -1;   // Ĭ��ֵ
    std::string sk;
    std::string MK;
    std::string sk_prime;
    std::unordered_map<int, std::string> Dic1;
    std::string Words[100];
    //std::string updateFile;
    //std::cout << "����������ļ���" << std::endl;
    //std::cin >> updateFile;

    // ���Դ��ļ��Լ���Ƿ����
    std::ifstream src(updateFile, std::ios::binary);
    if (!src) {
        std::cerr << "�����ļ� " << updateFile << " �����ڣ�������һ���µ� update.json �ļ���" << std::endl;
    }
    else {
        std::cout << "���ڸ����ļ�����..." << std::endl;
    }

    // ��Ŀ���ļ���update.json������д��ģʽ
    std::ofstream dst("update.json", std::ios::binary);
    if (!dst) {
        std::cerr << "�޷��������Ŀ���ļ� update.json��" << std::endl;
        return;
    }

    // ���Դ�ļ����ڣ���������
    if (src) {
        dst << src.rdbuf();
        std::cout << "�ļ������Ѹ��Ƶ� update.json��" << std::endl;
    }
    else {
        std::cout << "���ļ� update.json �Ѵ�������ǰΪ�ա�" << std::endl;
    }

    // �ر��ļ���
    src.close();
    dst.close();

    // ȷ��д��ɹ�
    std::ifstream check("update.json");
    if (!check || check.peek() == EOF) {
        std::cerr << "���棺update.json ����Ϊ�ջ�д��ʧ�ܣ�" << std::endl;
    }
    else {
        std::cout << "update.json д��ɹ���" << std::endl;
    }
    check.close();

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
            BLOOM_HASHES = savedData["BLOOM_HASHES"].asInt();
            BLOOM_BITS = savedData["BLOOM_BITS"].asInt();

            if (savedData.isMember("l")) {
                l = savedData["l"].asInt() + 1;
            }
            else {
                l = 1;
            }
        }
        ifs.close(); // �ر��ļ�
    }


// ���Լ�������
    Json::Value root;
    //map <int, string> Dic2;

        // ���Զ�ȡ��������
    std::ifstream dic1ifs("Dic1.json");

    if (dic1ifs.is_open()) {
        // ����ļ��Ƿ�Ϊ��
        dic1ifs.seekg(0, std::ios::end);
        if (dic1ifs.tellg() == 0) {
            std::cout << "Dic1.json is empty. Initializing new map." << std::endl;
            dic1ifs.close();
        }
        else {
            // �ļ��ǿգ�������������
            dic1ifs.seekg(0, std::ios::beg);  // ����ȡλ�����õ��ļ���ͷ
            Json::CharReaderBuilder reader;
            std::string errs;

            if (Json::parseFromStream(reader, dic1ifs, &root, &errs)) {
                for (const auto& item : root.getMemberNames()) {
                    int l = std::stoi(item);
                    Dic1[l] = root[item].asString();
                }
                std::cout << "Dic1 data loaded successfully." << std::endl;
            }
            else {
                std::cerr << "Failed to parse Dic1.json: " << errs << std::endl;
            }
            dic1ifs.close();
        }
    }
    else {
        std::cout << "Dic1.json does not exist or could not be opened." << std::endl;
    }




    Dic1[l] = generateRandomKey(LAMBDA / 8);

    // ������º�� l ֵ���ļ�
    Json::Value updatedData;
    updatedData["sk"] = base64Encode(sk);
    updatedData["MK"] = base64Encode(MK);
    updatedData["BLOOM_HASHES"] = BLOOM_HASHES;
    updatedData["BLOOM_BITS"] = BLOOM_BITS;

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

    //qwqstd::cout << "sk'= " << sk_prime << endl;
    //qwqstd::cout << "k'= " << k_prime << endl;

    std::string tmp1;
    std::string tmp2;
    std::string k;
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

    int DB[10000] = {};
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
			//DB =0 ;  // ���� DB ����
            // ��ȡ�õ��ʶ�Ӧ���������Ԫ��
            for (const auto& num : wordData) {
                numbers.push_back(num.asInt());
                DB[i]++;  // DB����
            }

            // ���õ��ʶ�Ӧ�������б���� index
            index[word] = numbers;
            //qwqstd::cout << "geted Word: " << word << ", Numbers: " << "DB: " << DB[i] << endl;
            //qwqfor (const auto& num : numbers) {
            //qwq    std::cout << num << " ";
            //qwq}
            //qwqstd::cout << endl;

        }
    }

    std::vector<std::tuple<std::string, int, CountingBloomFilter>> Up;
    std::map<std::string, std::string> connectorData;
    std::map<std::string, std::string> indopData;

    // ��ʱ��

	auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < wc; i++) {
        word = Words[i];
        // ȷ���õ����� index �д���
        if (index.find(word) == index.end()) {
            std::cerr << "Warning: Word '" << word << "' not found in index, skipping..." << std::endl;
            continue; // ����δ�� index ���ҵ��ĵ���
        }
        k = word;
        f1(k, sk);

        //qwqstd::cout << "k: " << k << endl;

        //qwqstd::cout << "l: " << l << endl;

        //qwqstd::cout << "Dic1[l]: " << Dic1[l] << endl;

        if (Dic1[l - 1].empty()) {
            st[Dic1[l]] = {}; // question
        }
        else {
            st[Dic1[l - 1]] = xorKeys(xorKeys(Dic1[l - 1], k), k_prime);
        }
        st[Dic1[l]] = xorKeys(xorKeys(Dic1[l], k), k_prime);

        Conn_key = h1(st[Dic1[l]], 1);
        //qwqstd::cout <<" word: "<<word << " st: " << st[Dic1[l]] << " h1(st)(conn_key)= " << h1(st[Dic1[l]], 1) << endl;
        tmp2 = intTo4ByteString(DB[i]);
        tmp1 = "";
        tmp1.append(tmp2);
        tmp1.append(st[Dic1[l - 1]]);
        Conn_value = xorKeys(h2(st[Dic1[l]]), tmp1);

        //qwqstd::cout << " h2(st): " << h2(st[Dic1[l]]) << " DB(word)|||stl - 1: " <<tmp2<<" + "<< st[Dic1[l - 1]]<<" -> "<<tmp1 << " Conn_value(h2(st) xor DB(word)|||stl - 1) : " << Conn_value << endl;

        connectorData[Conn_key] = Conn_value;

        std::string skey = f2(Dic1[l], sk, 32);

        //qwqstd::cout << "!!!!!!!skey= Dic1[ " << l << " ] ( " << Dic1[l] << " ),sk( " << sk << " )" << endl;

        //qwqstd::cout << "skey: " << skey << endl;

        // ���� CBFj,vl
        CountingBloomFilter CBFj_vl(BLOOM_BITS, BLOOM_HASHES);
        std::string tmp3;

        for (size_t m = 0; m < index[word].size(); ++m) {
            //qwqstd::cout << "m: " << m << " index[word][m]: " << index[word][m] << endl;
            std::string indwj_vl_m = intTo28ByteString(index[word][m]);

            std::string opwj_vl_m = "addw"; // �ٶ���������Ϊ addw

            // ���� CBFj,vl

			//cout <<word << " indwj_vl_m: " << indwj_vl_m << " opwj_vl_m: " << opwj_vl_m << endl;
            CBFj_vl.update(indwj_vl_m, opwj_vl_m);
            tmp3 = opwj_vl_m;
            // ���ɼ��ܵ� indopwj_vl,m
            tmp3.append(indwj_vl_m);

            //qwqstd::cout << "indwj_vl_m: " << indwj_vl_m << " opwj_vl_m: " << opwj_vl_m << " tmp3: " << tmp3 << endl;

            //qwqstd::cout << " size of tmp3: " << tmp3.size() << endl;

            std::string indopwj_vl_m = aesEncrypt(tmp3, skey, IV);

            //qwqstd::cout << "indopwj_vl_m: " << indopwj_vl_m << endl;

            // ���� indop_key �� indop_value
            std::string indop_key = h34(st[Dic1[l]], m, 3);

            //qwqstd::cout << "indop_key: " << indop_key << endl;

            std::string indop_value = xorKeys(h34(st[Dic1[l]], m, 4), indopwj_vl_m);

            //qwqstd::cout << "h4(st) : " << h34(st[Dic1[l]], m, 4) << endl;

            //qwqstd::cout << "indop_value: " << indop_value << endl;

            indopData[indop_key] = indop_value;
        }

        Up.push_back(std::make_tuple(k, l, CBFj_vl));
    }

	// ��ʱ��
    auto stop_1 = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration_1 = stop_1 - start;
	cout << fixed << setprecision(3) << "Time taken by updateClient: " << duration_1.count() << " ms" << endl;


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

 //   //������
 //   // 
 //   // ���Լ�������
 //   Json::Value root_bc;
	////map <int, string> Dic2;

 //   // ���Զ�ȡ��������
 //   std::ifstream dic1ifs_bc("Dic1.json");
 //   if (dic1ifs.is_open()) {
 //       Json::CharReaderBuilder reader_bc;
 //       Json::Value root_bc;
 //       std::string errs_bc;

 //       // ��ȡ������ JSON �ļ�����
 //       if (Json::parseFromStream(reader_bc, dic1ifs_bc, &root_bc, &errs_bc)) {
 //           for (const auto& item : root.getMemberNames()) {
 //               int l_bc = std::stoi(item);
 //               Dic1[l_bc] = root[item].asString();
 //           }
 //           std::cout << "Dic1 data loaded successfully." << std::endl;
 //       }
 //       else {
 //           std::cerr << "Failed to parse Dic1.json: " << errs_bc << std::endl;
 //       }
 //       dic1ifs_bc.close();
 //   }
 //   else {
 //       std::cout << "Dic1.json does not exist or is empty. Initializing new map." << std::endl;
 //   }

    // �� map ���ݴ��� JSON ����
    for (const auto& item : Dic1) {
        root[std::to_string(item.first)] = item.second;
    }

    // ���浽 Dic1.json �ļ�
    std::ofstream dic1ofs("Dic1.json");
    if (dic1ofs.is_open()) {
        Json::StreamWriterBuilder writer;
        dic1ofs << Json::writeString(writer, root);
        dic1ofs.close();
        std::cout << "Dic1 data saved successfully." << std::endl;
    }
    else {
        std::cerr << "Failed to open Dic1.json for writing!" << std::endl;
    }

    //Up
// ���Զ�ȡ��������
    map<string, CountingBloomFilter> CBFList;
    std::ifstream CBFifs("CBFList.json");

    if (CBFifs.is_open()) {
        // **����ļ��Ƿ�Ϊ��**
        CBFifs.seekg(0, std::ios::end);
        if (CBFifs.tellg() == 0) {  // �ļ���СΪ 0
            std::cout << "CBFList.json is empty. Initializing as an empty JSON array." << std::endl;
            CBFifs.close();
        }
        else {
            // **������λ�ã�ȷ��������ȷ��ȡ����**
            CBFifs.seekg(0, std::ios::beg);

            Json::CharReaderBuilder reader;
            Json::Value root;
            std::string errs;

            // ��ȡ������ JSON �ļ�����
            if (Json::parseFromStream(reader, CBFifs, &root, &errs)) {
                if (root.isArray()) { // ȷ������������
                    for (const auto& item : root) {
                        if (item.isObject() && item.isMember("key") && item.isMember("data")) {
                            const std::string& cbf_key = item["key"].asString();
                            const Json::Value& cbf_data = item["data"];

                            // �� data ת��Ϊ CountingBloomFilter �ĸ�ʽ
                            CountingBloomFilter cbf(BLOOM_BITS, BLOOM_HASHES);
                            cbf.fromJson(cbf_data);
                            CBFList[cbf_key] = cbf;  // �����л� CBF
                        }
                    }
                    std::cout << "CBFList data loaded successfully." << std::endl;
                }
                else {
                    std::cerr << "JSON root is not an array." << std::endl;
                }
            }
            else {
                std::cerr << "Failed to parse CBFList.json: " << errs << std::endl;
            }
            CBFifs.close();
        }
    }
    else {
        std::cout << "CBFList.json does not exist. Initializing new map." << std::endl;
    }




        string stwjvl;
        for (const auto& item : Up) {
            std::string k = get<0>(item);
            int l = get<1>(item);
			CountingBloomFilter CBFjStr(BLOOM_BITS, BLOOM_HASHES);
			CBFjStr = get<2>(item);

            // Step 1: Set Dic1[l] = vl
            //Dic1[l] = Dic2[l];

            // Process the Up data for each item (k, l, CBFj)
            std::string vl = Dic1[l];

            // Step 2: Loop while Up is not empty (this loop represents handling each Up item)
                        // Pop the current Up item
                //upData.removeIndex(0); // In practice, use actual removal from collection

                // Step 3: Update vl based on k, CBFj
            std::string tmp1 = xorKeys(k_prime, k);
            stwjvl = xorKeys(tmp1, vl);
            // Step 4: Create trapdoor (the method depends on encryption logic)
            std::string trapdoor = stwjvl;
            int ctr = l;
            CountingBloomFilter CBFj(BLOOM_BITS, BLOOM_HASHES);
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
    int BLOOM_HASHES = -1; // Ĭ��ֵ
    int BLOOM_BITS = -1;   // Ĭ��ֵ
    std::map<int, std::string> Dic1;
    int l_n = 0;
    std::vector<std::string> searchTokens;

    // ���Զ�ȡ��������
    std::ifstream dic1ifs("Dic1.json");
    if (dic1ifs.is_open()) {
        Json::CharReaderBuilder reader;
        Json::Value root;
        std::string errs;

        // ��ȡ������ JSON �ļ�����
        if (Json::parseFromStream(reader, dic1ifs, &root, &errs)) {
            for (const auto& item : root.getMemberNames()) {
                int l = std::stoi(item);
                if (l > l_n) {
                    l_n = l;
                }
                Dic1[l] = root[item].asString();
            }
            std::cout << "Dic1 data loaded successfully." << std::endl;
        }
        else {
            std::cerr << "Failed to parse Dic1.json: " << errs << std::endl;
        }
        dic1ifs.close();
    }
    else {
        std::cout << "Dic1.json does not exist or is empty. Initializing new map." << std::endl;
    }

    std::string vl = Dic1[l_n];
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
            BLOOM_HASHES = savedData["BLOOM_HASHES"].asInt();
            BLOOM_BITS = savedData["BLOOM_BITS"].asInt();
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
		std::cout << searchTokens[0] << endl;
    }

    return searchTokens; // ���ش�������������
}


vector<string> searchClient(std::vector<std::string> searchTokens, string Q, int q,size_t startInd,size_t endInd) {
    int BLOOM_HASHES = -1; // Ĭ��ֵ
    int BLOOM_BITS = -1;   // Ĭ��ֵ
    //size_t startInd=0;
    //size_t endInd = 0;
        // ����ļ��Ƿ���ڣ���������
    std::ifstream ifs("client_data.json");
    if (ifs.is_open()) {
        Json::CharReaderBuilder reader;
        Json::Value savedData;
        std::string errs;

        if (Json::parseFromStream(reader, ifs, &savedData, &errs)) {

            BLOOM_HASHES = savedData["BLOOM_HASHES"].asInt();
            BLOOM_BITS = savedData["BLOOM_BITS"].asInt();
        }
        ifs.close(); // �ر��ļ�
    }

    map <int, string> Dic1;

    // ���Զ�ȡ��������
    std::ifstream dic1ifs("Dic1.json");
    if (dic1ifs.is_open()) {
        Json::CharReaderBuilder reader;
        Json::Value root;
        std::string errs;

        // ��ȡ������ JSON �ļ�����
        if (Json::parseFromStream(reader, dic1ifs, &root, &errs)) {
            for (const auto& item : root.getMemberNames()) {
                int l = std::stoi(item);
                Dic1[l] = root[item].asString();
            }
            std::cout << "Dic1 data loaded successfully." << std::endl;
        }
        else {
            std::cerr << "Failed to parse Dic1.json: " << errs << std::endl;
        }
        dic1ifs.close();
    }
    else {
        std::cout << "Dic1.json does not exist or is empty. Initializing new map." << std::endl;
    }

	////sendtoserver
 //   std::string jsonData = "{ \"tokens\": [";
 //   for (size_t i = 0; i < searchTokens.size(); ++i) {
 //       jsonData += "\"" + searchTokens[i] + "\"";
 //       if (i != searchTokens.size() - 1) {
 //           jsonData += ", ";
 //       }
 //   }
 //   jsonData += "] }";

    // ���� JSON ����
    Json::Value jsonData;
    Json::Value tokens(Json::arrayValue);

    // ���������Ʊ���Ϊ Base64 ����ӵ� JSON ������
    for (const auto& token : searchTokens) {
        tokens.append(base64Encode(token));
    }
    jsonData["tokens"] = tokens;

    Json::StreamWriterBuilder writertoken;
    std::string jsonString = Json::writeString(writertoken, jsonData);

    // ʹ�� httplib ���� HTTP ����
    httplib::Client cli("http://localhost:9001");  // ��������ַ�Ͷ˿�
    auto res = cli.Post("/search", jsonString, "application/json");

    if (res && res->status == 200) {
        std::cout << "Server response: " << res->body << std::endl;
    }
    else {
        std::cerr << "Request failed!" << std::endl;
    }


	//request
	std::cout << "/send_Rsearch_Request" << endl;

    //httplib::Client cli1("http://127.0.0.1:9008");
    //auto res1 = cli1.Get("/send_Rsearch_Request");
    auto res1 = cli.Get("/send_Rsearch_Request");
	std::cout << "res1->status: " << res1->status << endl;

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
                Rwjvl = {};
                int l = item["l"].asInt();
                for (const auto& str : item["R"]) {
                    Rwjvl.push_back(str.asString());
                }
                Rsearch.push_back(std::make_pair(Rwjvl, l));
                //qwqstd::cout << "l: " << l << endl;
                //qwqfor (const auto& str : Rwjvl) {
                //qwq    std::cout << "Rwjvl: " << str << std::endl;
                //qwq}
            }
        }
        else {
            std::cerr << "Error parsing response: " << errs << std::endl;
        }

        std::vector<string> Ind;
        std::vector<string> del;
        int indexNum=0;
        for (const auto& item : Rsearch) {
            std::vector<string> Rwjvl = item.first;
            int l = item.second;


            //��lΪ1��
            std::string indopwjvl = "";
             indexNum = 0;
            for (const auto& str : Rwjvl) {
                indexNum++;
                indopwjvl = Rwjvl[indexNum-1];
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
                //std::string k = xorKeys(xorKeys(st, k_prime), Dic1[l]);
                std::string skey = f2(Dic1[l], sk, 32);

                //std::cout << "k: " << k << endl;

               //qwq std::cout << "skey: " << skey << endl;

                //qwqstd::cout << "!!!!!!!skey= Dic1[ " << l << " ] ( " << Dic1[l] << " ),sk( " << sk << " )" << endl;

                //qwqstd::cout << "IV: " << IV << endl;

                //qwqstd::cout << "indopwjvl Base64:" << indopwjvl << endl;
				indopwjvl = base64Decode(indopwjvl);
                //qwqstd::cout << "indopwjvl.size():" << indopwjvl.size() << endl;

                //qwqstd::cout << "indopwjvl:" << indopwjvl << endl;

                //qwqstd::cout << "skey.size():" << skey.size() << endl;

                //qwqstd::cout << "IV.size():" << IV.size() << endl;

                std::string tmp1 = aesDecrypt(indopwjvl, skey, IV);

                //qwqstd::cout << "tmp1: " << tmp1 << endl;
                std::string op = tmp1.substr(0, 4);
                std::string ind = tmp1.substr(4);
                //!!!!//ind = to_string(byteStringToInt(ind));
                //qwqstd::cout << "op: " << op << endl;

                //qwqstd::cout << "ind: " << ind << endl;
                //qwqstd::cout << "tmp1.size():" << tmp1.size() << endl;
                //qwqstd::cout << "ind.size():" << ind.size() << endl;




                if (op == "addw") {
                    Ind.push_back(ind);
                }
                else if (op == "dele") {
                    del.push_back(ind);
                }

            }
        }

        //Ind = Ind - del;
        for (const std::string& item : del) {
            // �Ƴ� Ind �����е��� item ��Ԫ��
            Ind.erase(std::remove(Ind.begin(), Ind.end(), item), Ind.end());
        }

        CountingBloomFilter RCBF(BLOOM_BITS, BLOOM_HASHES);

        for (const std::string& item : Ind) {
            //std::cout << "insert to RCBF: "<<item << endl;
			//cout << "insert to RCBF: " << item << endl;
            RCBF.update(item, "addw");
        }


        vector<string> Finalset;

        //send RCBF to bc
        bool advice=verifyCBF(RCBF,searchTokens,BLOOM_HASHES,BLOOM_BITS);
		if (advice == true) {
			//std::cout << "The advice is true" << endl;
			Finalset.push_back("true");
		}
		else {
			//std::cout << "The advice is false" << endl;
			Finalset.push_back("false");
		}



        //std::cout << "������������Χ��" << endl;
        //std::cout << "��ʼ��" << endl;
        //cin >> startInd;
        //std::cout << "������" << endl;
        //cin >> endInd;

        for (size_t i=startInd;i<=endInd;i++) {

            string item = to_string(i);

            if (Q == "disjunctive")
            {
                //qwqstd::cout << "\n disjunctive :" << endl;
                //qwqstd::cout << "check " << item << endl;
                if (RCBF.check(intTo28ByteString(i)))
                {
                    //qwqstd::cout << "insert " << item << endl;
                    Finalset.push_back(item);
                }
            }
            else if (Q == "conjunctive")
            {
                //qwq std::cout << "\n conjunctive :" << endl;
                //qwqstd::cout << "repeatCheck " << item << endl;
                if (RCBF.repeatCheck(intTo28ByteString(i), q))
                {
                    //qwq std::cout << "insert " << item << endl;
                    Finalset.push_back(item);
                }
            }
        }
        //std::cout << "Final set: " << endl;
        //for (const std::string& item : Finalset) {
        //     std::cout<< item << std::endl;
        //}
        return Finalset;

    }
    }
