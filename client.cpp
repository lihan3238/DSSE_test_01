#include "client.h"

const int LAMBDA = 256; // ��ȫ���� ��
const int BLOOM_SIZE = 256; // ��¡��������С

// Setup �������� Trust Center ��ȡ���ݲ����浽�ļ�
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

            // �������ݵ��ļ�
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

// �������ͨ�ŵĺ���
void updateClient() {
    std::vector<Byte> sk;
    std::vector<Byte> MK;
    std::vector<Byte> sk_prime;
    std::unordered_map<int, std::vector<Byte>> Dic1;
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
                l = savedData["l"].asInt()+1;
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

    // �򿪲���ȡJSON�ļ�
    std::ifstream ifs2("update.json");
    if (!ifs2.is_open()) {
        std::cerr << "Failed to open the JSON file." << std::endl;
        return ;
    }

    Json::Reader reader;
    Json::Value jsonData;

    // ���� JSON �ļ�
    if (!reader.parse(ifs2, jsonData)) {
        std::cerr << "Failed to parse JSON data." << std::endl;
        return ;
    }

    // ͳ�������ܺ�
    int DB = 0;

	ifs2.close();

    // ���� JSON �ļ��ļ������� Words ����
    int wc = 0;
    for (const auto& key : jsonData.getMemberNames()) {
        if (wc < 100) {
            Words[wc++] = key; // �洢���� Words ����
        }
        else {
            std::cerr << "Words array size exceeded maximum limit." << std::endl;
            break;
        }
    }


    // ���� Words �����е�ÿ�����ʣ����� JSON �ж�Ӧ������
    for (const std::string& word : Words) {
        // ��� JSON ���Ƿ��иõ���
        if (jsonData.isMember(word)) {
            const Json::Value& wordData = jsonData[word];
            std::vector<int> numbers;

            // ��ȡ�õ��ʶ�Ӧ���������Ԫ��
            for (const auto& num : wordData) {
                numbers.push_back(num.asInt());
                DB += num.asInt();  // �ۼ������ܺ�
            }

            // ���õ��ʶ�Ӧ�������б���� index
            index[word] = numbers;
        }
    }


    // ��� index["apple"]
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
