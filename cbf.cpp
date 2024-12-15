#pragma once
#include "cbf.h"

using namespace std;

// 自定义 to_string 功能（仅在未启用 C++11 时需要）
std::string to_string_custom(int val) {
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

CountingBloomFilter::CountingBloomFilter(size_t numCounters, size_t numHashFunctions)
    : m(numCounters), k(numHashFunctions), CBF(numCounters, 0) {
    generateHashFunctions(k);
}

void CountingBloomFilter::init(size_t k) {
    this->k = k;
    generateHashFunctions(k);
}

void CountingBloomFilter::construct(const std::vector<Byte>& data) {
    for (auto& elem : data) {
        std::string element(1, elem);  // 将元素转换为字符串形式
        update(element, "add");
    }
}

void CountingBloomFilter::generateHashFunctions(size_t k) {
    for (size_t i = 0; i < k; ++i) {
        hashFunctions.push_back([this, i](const std::string& element) {
            return std::hash<std::string>{}(element + to_string_custom(i)) % m;
            });
    }
}

size_t CountingBloomFilter::hash(const std::string& element, size_t index) {
    return std::hash<std::string>{}(element + std::to_string(index)) % m;
}

void CountingBloomFilter::update(const std::string& element, const std::string& op) {
    for (size_t i = 0; i < k; ++i) {
        size_t idx = hash(element, i);
        if (op == "add") {
            CBF[idx] += 1;
        }
        else if (op == "del") {
            if (CBF[idx] > 0) {
                CBF[idx] -= 1;
            }
        }
    }
}

bool CountingBloomFilter::check(const std::string& element) {
    for (size_t i = 0; i < k; ++i) {
        size_t idx = hash(element, i);
        if (CBF[idx] == 0) {
            return false;
        }
    }
    return true;
}

bool CountingBloomFilter::repeatCheck(const std::string& element, size_t r) {
    for (size_t i = 0; i < k; ++i) {
        size_t idx = hash(element, i);
        if (CBF[idx] < r) {
            return false;
        }
    }
    return true;
}
