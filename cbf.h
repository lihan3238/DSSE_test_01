#pragma once

#include <string>
#include <functional>  // 用于 std::hash
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <sstream>  // 用于自定义 to_string
#include <json/json.h>

using Byte = unsigned char;


// 计数布隆过滤器类
class CountingBloomFilter {
public:
    CountingBloomFilter()
        : m(3700), k(7), CBF(0) {
        // 默认构造函数，不做其他操作
    }

    CountingBloomFilter(size_t numCounters, size_t numHashFunctions);

    // 初始化哈希函数
    void init(size_t k);

    // 构造计数布隆过滤器
    void construct(const std::string& data);

    // 更新元素（增加或删除）
    void update(const std::string& data, const std::string& op);

    // 检查元素是否存在
    bool check(const std::string& data);

    // 检查元素是否出现次数达到给定阈值
    bool repeatCheck(const std::string& data, size_t r);
    Json::Value toJson() const;
    CountingBloomFilter& operator=(const CountingBloomFilter& other);
    CountingBloomFilter operator+(const CountingBloomFilter& other) const;
    static CountingBloomFilter fromJson(const Json::Value& json);
private:
    size_t m;  // 计数器的数量
    size_t k;  // 哈希函数的数量
    std::vector<int> CBF;  // 计数布隆过滤器（计数器数组）
    std::vector<std::function<size_t(const std::string&)>> hashFunctions;  // 哈希函数集合

    // 生成哈希函数
    void generateHashFunctions(size_t k);

    // 哈希函数生成器
    size_t hash(const std::string& data, size_t index);


};
