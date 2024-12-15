#pragma once


#include <vector>
#include <functional>  // ���� std::hash
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <sstream>  // �����Զ��� to_string

using Byte = unsigned char;

// ������¡��������
class CountingBloomFilter {
public:
    CountingBloomFilter(size_t numCounters, size_t numHashFunctions);

    // ��ʼ����ϣ����
    void init(size_t k);

    // ���������¡������
    void construct(const std::vector<Byte>& data);

    // ����Ԫ�أ����ӻ�ɾ����
    void update(const std::string& element, const std::string& op);

    // ���Ԫ���Ƿ����
    bool check(const std::string& element);

    // ���Ԫ���Ƿ���ִ����ﵽ������ֵ
    bool repeatCheck(const std::string& element, size_t r);

private:
    size_t m;  // ������������
    size_t k;  // ��ϣ����������
    std::vector<int> CBF;  // ������¡�����������������飩
    std::vector<std::function<size_t(const std::string&)>> hashFunctions;  // ��ϣ��������

    // ���ɹ�ϣ����
    void generateHashFunctions(size_t k);

    // ��ϣ����������
    size_t hash(const std::string& element, size_t index);
};

