#include "httplib.h"
#include <iostream>
#include <json/json.h>
#include "client.h"

using namespace std;

httplib::Server svr;


// ������ҳ

void index_handler(const httplib::Request&, httplib::Response& res) {
    int BLOOM_HASHES = -1; // Ĭ��ֵ
    int BLOOM_BITS = -1;   // Ĭ��ֵ
    ifstream file("client.html", ios::binary);
    if (!file) {
        res.status = 404;
        res.set_content("HTML file not found", "text/plain");
        return;
    }
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    res.set_header("Content-Type", "text/html");
    res.set_content(content, "text/html");
}

// ���� "Setup Client Data"
void setup_data_handler(const httplib::Request& req, httplib::Response& res) {
    Json::Value json_request;
    Json::CharReaderBuilder reader;
    string errs;
    istringstream ss(req.body);

    if (!Json::parseFromStream(reader, ss, &json_request, &errs)) {
        res.status = 400;
        res.set_content(R"({"error": "Invalid JSON"})", "application/json");
        return;
    }

    int BLOOM_HASHES = json_request["bloom_hashes"].asInt();
    int BLOOM_BITS = json_request["bloom_bits"].asInt();
    setupClientData( BLOOM_HASHES,BLOOM_BITS);
    res.set_content(R"({"message": "Client data set up successfully"})", "application/json");
}

// ���� "Connect Server"
void update_client_handler(const httplib::Request& req, httplib::Response& res) {
    if (req.body.empty()) {
        res.status = 400;
        res.set_content(R"({"error": "Empty request body"})", "application/json");
        return;
    }
    Json::CharReaderBuilder reader;
    Json::Value json_request;
    string errs;
    std::istringstream ss(req.body);

    if (!Json::parseFromStream(reader, ss, &json_request, &errs)) {
        res.status = 400;
        res.set_content(R"({"error": "Invalid JSON"})", "application/json");
        return;
    }

    string updateFile = json_request["update_file"].asString();

    std::string time_cost=updateClient(updateFile);

    Json::Value json_response;
    json_response["message"] = "Client updated successfully";  // �Զ���������
    json_response["cli_update_time_cost"] = time_cost;  // �Զ���������

    Json::StreamWriterBuilder writer;
    string response_body = Json::writeString(writer, json_response);


    // **��� CORS ͷ**
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");

    res.set_header("Content-Type", "application/json; charset=utf-8");
    res.set_content(response_body, "application/json");
}

// ���� "Search"
void search_handler(const httplib::Request& req, httplib::Response& res) {
    Json::CharReaderBuilder reader;
    Json::Value json_request;
    string errs;
    std::istringstream ss(req.body);

    if (!Json::parseFromStream(reader, ss, &json_request, &errs)) {
        res.status = 400;
        res.set_content(R"({"error": "Invalid JSON"})", "application/json");
        return;
    }

    int q = json_request["word_count"].asInt();
    string search_type = json_request["search_type"].asString();
    size_t startInd = json_request["start_index"].asUInt64();
    size_t endInd = json_request["end_index"].asUInt64();
    vector<string> Words;
    for (const auto& word : json_request["words"]) {
        Words.push_back(word.asString());
    }

    vector<string> Finalset = searchClient(searchToken(Words, search_type, q), search_type, q, startInd, endInd);

    Json::Value response;
    response["message"] = "Search completed";
    Json::Value results(Json::arrayValue);
    // 1. ȡ����һ����Ԫ�أ�ʹ�� std::move ���⿽����
    string verify_status = std::move(Finalset.front());

    Finalset.erase(Finalset.begin());

    string svr_search_time_cost = std::move(Finalset.front());

    Finalset.erase(Finalset.begin());

    for (const auto& item : Finalset) {
        results.append(item);
    }
    response["verify_status"] = verify_status;
    response["svr_search_time_cost"] = svr_search_time_cost;
    response["results"] = results;

    Json::StreamWriterBuilder writer;

    // **��� CORS ͷ**
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");

    res.set_header("Content-Type", "application/json; charset=utf-8");
    res.set_content(Json::writeString(writer, response), "application/json");
}

void cli_reset_handler(const httplib::Request&, httplib::Response& res) {
    cli_reSet();
    res.set_content(R"({"message": "Reset executed"})", "application/json");
}

void main312312() {
    svr.Get("/", index_handler);
    svr.Post("/setup", setup_data_handler);
    svr.Post("/connect", update_client_handler);
    svr.Post("/search", search_handler);
    svr.Post("/reset", cli_reset_handler);

    cout << "Server running at http://localhost:8080" << endl;
    svr.listen("0.0.0.0", 8080);
    return;
}
