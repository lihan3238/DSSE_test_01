#pragma once
#include "cbf.h"
#include <sstream>
#include <functional>
#include <vector>
#include <json/json.h>

using namespace std;

// ������¡��������
// ������¡�������෽��ʵ��

CountingBloomFilter::CountingBloomFilter(size_t numCounters, size_t numHashFunctions)
    : m(numCounters), k(numHashFunctions), CBF(numCounters, 0) {
    generateHashFunctions(k);
}



void CountingBloomFilter::init(size_t k) {
    this->k = k;
    generateHashFunctions(k);
}

void CountingBloomFilter::construct(const std::string& data) {
    // ֱ�ӽ�������ַ�����Ϊ��ϣ����
    update(data, "add");
}

void CountingBloomFilter::generateHashFunctions(size_t k) {
    for (size_t i = 0; i < k; ++i) {
        // ��ʽת��Ϊ std::function
        std::function<size_t(const std::string&)> hashFunc = [this, i](const std::string& element) {
            return std::hash<std::string>{}(element + to_string(i)) % m;
        };
        hashFunctions.push_back(hashFunc); // push_back һ�� std::function
    }
}

size_t CountingBloomFilter::hash(const std::string& data, size_t index) {
    // ���� string �Ĺ�ϣ����
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
            return false; // ���ĳ��������Ϊ0�����ʾ��Ԫ�ز��ڲ�¡��������
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
            return false; // Ԫ�س��ִ���������ֵ
        }
    }
    return true;
}

Json::Value CountingBloomFilter::toJson() const {
    Json::Value jsonArray(Json::arrayValue);
    for (int count : CBF) {
        jsonArray.append(count); // ������������תΪ JSON ����
    }
    return jsonArray;
}

CountingBloomFilter CountingBloomFilter::fromJson(const Json::Value& json) {
    // ��ȡ������¡�������ļ����������͹�ϣ��������
    size_t numCounters = json["numCounters"].asUInt();
    size_t numHashFunctions = json["numHashFunctions"].asUInt();

    // ����һ���µ� CountingBloomFilter ����
    CountingBloomFilter cbf(numCounters, numHashFunctions);

    // ��������������
    const Json::Value& jsonArray = json["CBF"];
    if (jsonArray.isArray()) {
        // �� JSON �����лָ���������ֵ
        for (Json::Value::ArrayIndex i = 0; i < jsonArray.size(); ++i) {
            int count = jsonArray[i].asInt();
            cbf.CBF[i] = count;  // �ָ�������ֵ
        }
    }

    return cbf;
}

// ��ֵ���������
CountingBloomFilter& CountingBloomFilter::operator=(const CountingBloomFilter& other) {
    // �ȼ�����Ҹ�ֵ
    if (this != &other) {
        // ���Ƽ���������
        this->CBF = other.CBF;

        // ���ƹ�ϣ�����б�
        this->hashFunctions = other.hashFunctions;

        // ����������Ա����
        this->m = other.m;
        this->k = other.k;
    }
    return *this;  // ���ص�ǰ���������
}

// ������¡�������ĺϲ����������
CountingBloomFilter CountingBloomFilter::operator+(const CountingBloomFilter& other) const {
    // ȷ��������¡�������Ĳ���һ��
    if (this->m != other.m || this->k != other.k) {
        throw std::invalid_argument("Cannot merge CountingBloomFilters with different sizes or hash functions.");
    }

    // ����һ���µ� CountingBloomFilter ���洢�ϲ����
    CountingBloomFilter result(this->m, this->k);

    // �ϲ�����������
    for (size_t i = 0; i < this->m; ++i) {
        result.CBF[i] = this->CBF[i] + other.CBF[i]; // ��Ӧλ�õļ��������
    }

    // �ϲ���ϣ��������������ֻȡ���ߵĹ�ϣ�������У�
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