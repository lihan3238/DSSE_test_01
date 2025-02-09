#include <iostream>
#include <vector>
#include <cassert>
#include "utils.h"
#include "cbf.h" // 假设这是你 Bloom Filter 的头文件

//int main1() {
//    // 初始化 Counting Bloom Filter，100 个计数器，3 个哈希函数
//    CountingBloomFilter cbf(100, 3);
//
//    // 准备测试数据
//    std::vector<Byte> data1 = { 0x12, 0x34, 0x56, 0x78 };
//    std::vector<Byte> data2 = { 0x90, 0xAB, 0xCD, 0xEF };
//    std::vector<Byte> data3 = { 0x00, 0x11, 0x22, 0x33 };
//
//    // 1. 测试 construct 和 check 功能
//    std::cout << "Testing construct and check..." << std::endl;
//
//    cbf.construct(data1);
//    assert(cbf.check(data1) == true);  // 数据1应该存在
//
//    assert(cbf.check(data2) == false); // 数据2应该不存在（未插入）
//
//    cbf.construct(data2);
//    assert(cbf.check(data2) == true);  // 数据2插入后应该存在
//
//    std::cout << "PASS: construct and check" << std::endl;
//
//    // 2. 测试重复插入（repeatCheck 功能）
//    std::cout << "Testing repeatCheck..." << std::endl;
//
//    // 插入 data1 两次
//    cbf.construct(data1); // 再次插入 data1
//    assert(cbf.repeatCheck(data1, 2) == true);  // 重复两次应返回 true
//    assert(cbf.repeatCheck(data1, 3) == false); // 超过插入次数应返回 false
//
//    std::cout << "PASS: repeatCheck" << std::endl;
//
//    // 3. 测试删除功能 (update)
//    std::cout << "Testing update (deletion)..." << std::endl;
//
//    cbf.update(data1, "del"); // 删除一次 data1
//    assert(cbf.repeatCheck(data1, 1) == true);  // 现在只剩一次
//    assert(cbf.check(data1) == true);           // 仍然存在
//
//    cbf.update(data1, "del"); // 再次删除 data1
//    assert(cbf.check(data1) == false);          // data1 应该不存在
//
//    std::cout << "PASS: update (deletion)" << std::endl;
//
//    // 4. 测试边界条件（不存在的元素删除、空数据等）
//    std::cout << "Testing edge cases..." << std::endl;
//
//    // 删除不存在的元素
//    cbf.update(data3, "del"); // 删除从未插入过的 data3
//    assert(cbf.check(data3) == false); // 仍然不存在
//
//    // 插入空数据
//    std::vector<Byte> emptyData;
//    cbf.construct(emptyData); // 插入空数据
//    assert(cbf.check(emptyData) == true); // 空数据应该能查到
//
//    std::cout << "PASS: edge cases" << std::endl;
//
//    // 5. 大规模测试
//    std::cout << "Testing scalability..." << std::endl;
//
//    const size_t numElements = 1000;
//    std::vector<std::vector<Byte>> elements;
//
//    // 创建 1000 个随机数据并插入
//    for (size_t i = 0; i < numElements; ++i) {
//        std::vector<Byte> elem = { static_cast<Byte>(i & 0xFF), static_cast<Byte>((i >> 8) & 0xFF) };
//        elements.push_back(elem);
//        cbf.construct(elem);
//    }
//
//    // 检查所有插入的数据是否都存在
//    for (size_t i = 0; i < numElements; ++i) {
//        assert(cbf.check(elements[i]) == true);
//    }
//
//    // 检查一些未插入的数据是否不存在
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