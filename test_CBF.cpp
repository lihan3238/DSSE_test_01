#include <iostream>
#include <vector>
#include <cassert>
#include "utils.h"
#include "cbf.h" // ���������� Bloom Filter ��ͷ�ļ�

//int main1() {
//    // ��ʼ�� Counting Bloom Filter��100 ����������3 ����ϣ����
//    CountingBloomFilter cbf(100, 3);
//
//    // ׼����������
//    std::vector<Byte> data1 = { 0x12, 0x34, 0x56, 0x78 };
//    std::vector<Byte> data2 = { 0x90, 0xAB, 0xCD, 0xEF };
//    std::vector<Byte> data3 = { 0x00, 0x11, 0x22, 0x33 };
//
//    // 1. ���� construct �� check ����
//    std::cout << "Testing construct and check..." << std::endl;
//
//    cbf.construct(data1);
//    assert(cbf.check(data1) == true);  // ����1Ӧ�ô���
//
//    assert(cbf.check(data2) == false); // ����2Ӧ�ò����ڣ�δ���룩
//
//    cbf.construct(data2);
//    assert(cbf.check(data2) == true);  // ����2�����Ӧ�ô���
//
//    std::cout << "PASS: construct and check" << std::endl;
//
//    // 2. �����ظ����루repeatCheck ���ܣ�
//    std::cout << "Testing repeatCheck..." << std::endl;
//
//    // ���� data1 ����
//    cbf.construct(data1); // �ٴβ��� data1
//    assert(cbf.repeatCheck(data1, 2) == true);  // �ظ�����Ӧ���� true
//    assert(cbf.repeatCheck(data1, 3) == false); // �����������Ӧ���� false
//
//    std::cout << "PASS: repeatCheck" << std::endl;
//
//    // 3. ����ɾ������ (update)
//    std::cout << "Testing update (deletion)..." << std::endl;
//
//    cbf.update(data1, "del"); // ɾ��һ�� data1
//    assert(cbf.repeatCheck(data1, 1) == true);  // ����ֻʣһ��
//    assert(cbf.check(data1) == true);           // ��Ȼ����
//
//    cbf.update(data1, "del"); // �ٴ�ɾ�� data1
//    assert(cbf.check(data1) == false);          // data1 Ӧ�ò�����
//
//    std::cout << "PASS: update (deletion)" << std::endl;
//
//    // 4. ���Ա߽������������ڵ�Ԫ��ɾ���������ݵȣ�
//    std::cout << "Testing edge cases..." << std::endl;
//
//    // ɾ�������ڵ�Ԫ��
//    cbf.update(data3, "del"); // ɾ����δ������� data3
//    assert(cbf.check(data3) == false); // ��Ȼ������
//
//    // ���������
//    std::vector<Byte> emptyData;
//    cbf.construct(emptyData); // ���������
//    assert(cbf.check(emptyData) == true); // ������Ӧ���ܲ鵽
//
//    std::cout << "PASS: edge cases" << std::endl;
//
//    // 5. ���ģ����
//    std::cout << "Testing scalability..." << std::endl;
//
//    const size_t numElements = 1000;
//    std::vector<std::vector<Byte>> elements;
//
//    // ���� 1000 ��������ݲ�����
//    for (size_t i = 0; i < numElements; ++i) {
//        std::vector<Byte> elem = { static_cast<Byte>(i & 0xFF), static_cast<Byte>((i >> 8) & 0xFF) };
//        elements.push_back(elem);
//        cbf.construct(elem);
//    }
//
//    // ������в���������Ƿ񶼴���
//    for (size_t i = 0; i < numElements; ++i) {
//        assert(cbf.check(elements[i]) == true);
//    }
//
//    // ���һЩδ����������Ƿ񲻴���
//    for (size_t i = numElements; i < numElements + 10; ++i) {
//        std::vector<Byte> elem = { static_cast<Byte>(i & 0xFF), static_cast<Byte>((i >> 8) & 0xFF) };
//        assert(cbf.check(elem) == false);
//    }
//
//    std::cout << "PASS: scalability" << std::endl;
//
//    std::cout << "All tests passed successfully!" << std::endl;
//
//    return 0;
//}
int main000000() {
	std::string skey = f2("av", "ab", 32);
	aesDecrypt("av", skey, "ab");
	aesEncrypt("av", skey, "ab");
	return 0;
}