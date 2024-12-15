#pragma once

#include <httplib.h>
#include <iostream>
#include <json/json.h>  // ���� JSON ����
#include <unordered_map>
#pragma once
#include "utils.h"
#include "cbf.h"

// ����������˴������󣬷��Ͳ���
class BlockchainNode {
private:
    std::unordered_map<std::string, std::vector<Byte>> storage;
    std::string storageFile = "blockchain_data.json";

public:
    // ��������
    void put(const std::string& key, const std::vector<Byte>& value);

    // ��ѯ����
    std::vector<Byte> get(const std::string& key);

    // ���������洢
    std::unordered_map<std::string, std::vector<Byte>> getAllData();
    // ���л�����Ϊ JSON
    Json::Value serialize();

    // �� JSON �����л�����
    void deserialize(const Json::Value& json);

    // �������ݵ������ļ�
    void saveToFile();

    // �ӱ����ļ���������
    void loadFromFile();
};
void startBlockChainServer();