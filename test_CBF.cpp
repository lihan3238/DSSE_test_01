#include <iostream>
#include <vector>
#include <string>
#include "cbf.h"

int main1() {
    // 设置计数布隆过滤器参数：计数器数目为 10，哈希函数数目为 3
    CountingBloomFilter cbf(35000, 7);

    // 构建计数布隆过滤器（加入一些元素）
    std::vector<unsigned char> data = { 'a', 'b', 'c', 'd', 'e' };
    cbf.construct(data);

    // 检查元素是否存在
    std::string element = "a";
    if (cbf.check(element)) {
        std::cout << "Element '" << element << "' exists in the CBF.\n";
    }
    else {
        std::cout << "Element '" << element << "' does not exist in the CBF.\n";
    }
	cbf.update("a", "add");
    // 检查元素重复次数
    size_t r = 2;
    if (cbf.repeatCheck(element, r)) {
        std::cout << "Element '" << element << "' is repeated " << r << " times in the CBF.\n";
    }
    else {
        std::cout << "Element '" << element << "' is not repeated " << r << " times.\n";
    }
    
    // 删除元素
    cbf.update("a", "del");
    cbf.update("a", "del");

    // 再次检查元素是否存在
    if (cbf.check("a")) {
        std::cout << "Element 'a' exists in the CBF.\n";
    }
    else {
        std::cout << "Element 'a' does not exist in the CBF.\n";
    }

    return 0;
}
