#pragma once

#include <string>
#include <functional>  // ���� std::hash
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <sstream>  // �����Զ��� to_string
#include <json/json.h>

using Byte = unsigned char;


// ������¡��������
class CountingBloomFilter {
public:
    CountingBloomFilter()
        : m(3700), k(7), CBF(0) {
        // Ĭ�Ϲ��캯����������������
    }

    CountingBloomFilter(size_t numCounters, size_t numHashFunctions);

    // ��ʼ����ϣ����
    void init(size_t k);

    // ���������¡������
    void construct(const std::string& data);

    // ����Ԫ�أ����ӻ�ɾ����
    void update(const std::string& data, const std::string& op);

    // ���Ԫ���Ƿ����
    bool check(const std::string& data);

    // ���Ԫ���Ƿ���ִ����ﵽ������ֵ
    bool repeatCheck(const std::string& data, size_t r);
    Json::Value toJson() const;
    CountingBloomFilter& operator=(const CountingBloomFilter& other);
    CountingBloomFilter operator+(const CountingBloomFilter& other) const;
    static CountingBloomFilter fromJson(const Json::Value& json);
private:
    size_t m;  // ������������
    size_t k;  // ��ϣ����������
    std::vector<int> CBF;  // ������¡�����������������飩
    std::vector<std::function<size_t(const std::string&)>> hashFunctions;  // ��ϣ��������

    // ���ɹ�ϣ����
    void generateHashFunctions(size_t k);

    // ��ϣ����������
    size_t hash(const std::string& data, size_t index);


};
