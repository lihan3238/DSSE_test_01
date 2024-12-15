#pragma once

#include <httplib.h>
#include <iostream>
#include <json/json.h>  // 用于 JSON 解析
#include <unordered_map>
#pragma once
#include "utils.h"
#include "cbf.h"

// 区块链服务端处理请求，发送参数
class BlockchainNode {
private:
    std::unordered_map<std::string, std::vector<Byte>> storage;
    std::string storageFile = "blockchain_data.json";

public:
    // 插入数据
    void put(const std::string& key, const std::vector<Byte>& value);

    // 查询数据
    std::vector<Byte> get(const std::string& key);

    // 返回整个存储
    std::unordered_map<std::string, std::vector<Byte>> getAllData();
    // 序列化数据为 JSON
    Json::Value serialize();

    // 从 JSON 反序列化数据
    void deserialize(const Json::Value& json);

    // 保存数据到本地文件
    void saveToFile();

    // 从本地文件加载数据
    void loadFromFile();
};
void startBlockChainServer();