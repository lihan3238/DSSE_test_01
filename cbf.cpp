#pragma once
#include "cbf.h"
#include <sstream>
#include <functional>
#include <vector>
#include <json/json.h>

using namespace std;

// 计数布隆过滤器类
// 计数布隆过滤器类方法实现

CountingBloomFilter::CountingBloomFilter(size_t numCounters, size_t numHashFunctions)
    : m(numCounters), k(numHashFunctions), CBF(numCounters, 0) {
    generateHashFunctions(k);
}



void CountingBloomFilter::init(size_t k) {
    this->k = k;
    generateHashFunctions(k);
}

void CountingBloomFilter::construct(const std::string& data) {
    // 直接将输入的字符串作为哈希输入
    update(data, "add");
}

void CountingBloomFilter::generateHashFunctions(size_t k) {
    for (size_t i = 0; i < k; ++i) {
        // 显式转换为 std::function
        std::function<size_t(const std::string&)> hashFunc = [this, i](const std::string& element) {
            return std::hash<std::string>{}(element + to_string(i)) % m;
        };
        hashFunctions.push_back(hashFunc); // push_back 一个 std::function
    }
}

size_t CountingBloomFilter::hash(const std::string& data, size_t index) {
    // 基于 string 的哈希计算
    return hashFunctions[index](data);
}

void CountingBloomFilter::update(const std::string& data, const std::string& op) {
    for (size_t i = 0; i < k; ++i) {
        size_t idx = hash(data, i);
        if (op == "addw") {
            CBF[idx] += 1;
        } else if (op == "dele") {
            if (CBF[idx] > 0) {
                CBF[idx] -= 1;
            }
        }
    }
}

bool CountingBloomFilter::check(const std::string& data) {
    for (size_t i = 0; i < k; ++i) {
        size_t idx = hash(data, i);
        if (CBF[idx] == 0) {
            return false; // 如果某个计数器为0，则表示该元素不在布隆过滤器中
        }
    }
    return true;
}

bool CountingBloomFilter::repeatCheck(const std::string& data, size_t r) {
    size_t count = 0;
    for (size_t i = 0; i < k; ++i) {
        size_t idx = hash(data, i);
        count += CBF[idx];
        if (count < r) {
            return false; // 元素出现次数超过阈值
        }
    }
    return true;
}

Json::Value CountingBloomFilter::toJson() const {
    Json::Value jsonArray(Json::arrayValue);
    for (int count : CBF) {
        jsonArray.append(count); // 将计数器数组转为 JSON 数组
    }
    return jsonArray;
}

CountingBloomFilter CountingBloomFilter::fromJson(const Json::Value& json) {
    // 获取计数布隆过滤器的计数器数量和哈希函数数量
    size_t numCounters = json["numCounters"].asUInt();
    size_t numHashFunctions = json["numHashFunctions"].asUInt();

    // 创建一个新的 CountingBloomFilter 对象
    CountingBloomFilter cbf(numCounters, numHashFunctions);

    // 解析计数器数组
    const Json::Value& jsonArray = json["CBF"];
    if (jsonArray.isArray()) {
        // 从 JSON 数组中恢复计数器的值
        for (Json::Value::ArrayIndex i = 0; i < jsonArray.size(); ++i) {
            int count = jsonArray[i].asInt();
            cbf.CBF[i] = count;  // 恢复计数器值
        }
    }

    return cbf;
}

// 赋值运算符重载
CountingBloomFilter& CountingBloomFilter::operator=(const CountingBloomFilter& other) {
    // 先检查自我赋值
    if (this != &other) {
        // 复制计数器数组
        this->CBF = other.CBF;

        // 复制哈希函数列表
        this->hashFunctions = other.hashFunctions;

        // 复制其他成员变量
        this->m = other.m;
        this->k = other.k;
    }
    return *this;  // 返回当前对象的引用
}

// 计数布隆过滤器的合并运算符重载
CountingBloomFilter CountingBloomFilter::operator+(const CountingBloomFilter& other) const {
    // 确保两个布隆过滤器的参数一致
    if (this->m != other.m || this->k != other.k) {
        throw std::invalid_argument("Cannot merge CountingBloomFilters with different sizes or hash functions.");
    }

    // 创建一个新的 CountingBloomFilter 来存储合并结果
    CountingBloomFilter result(this->m, this->k);

    // 合并计数器数据
    for (size_t i = 0; i < this->m; ++i) {
        result.CBF[i] = this->CBF[i] + other.CBF[i]; // 对应位置的计数器相加
    }

    // 合并哈希函数（假设我们只取两者的哈希函数并列）
    result.hashFunctions = this->hashFunctions;
    //result.hashFunctions.insert(result.hashFunctions.end(), other.hashFunctions.begin(), other.hashFunctions.end());

    return result;
}


//int main() {
//
//	vector<string> Ind = { "a", "b", "c", "d" };
//    CountingBloomFilter RCBF(3500, 7);
//
//    for (const std::string& item : Ind) {
//        cout << "insert to RCBF: " << item << endl;
//        RCBF.update(item, "addw");
//    }
//	if (RCBF.check("a") == true)
//	{
//		cout << "a is in RCBF" << endl;
//	}
//	else
//	{
//		cout << "a is not in RCBF" << endl;
//	}
//}