#include <iostream>
#include <vector>
#include <string>
#include "cbf.h"

int main1() {
    // ���ü�����¡��������������������ĿΪ 10����ϣ������ĿΪ 3
    CountingBloomFilter cbf(35000, 7);

    // ����������¡������������һЩԪ�أ�
    std::vector<unsigned char> data = { 'a', 'b', 'c', 'd', 'e' };
    cbf.construct(data);

    // ���Ԫ���Ƿ����
    std::string element = "a";
    if (cbf.check(element)) {
        std::cout << "Element '" << element << "' exists in the CBF.\n";
    }
    else {
        std::cout << "Element '" << element << "' does not exist in the CBF.\n";
    }
	cbf.update("a", "add");
    // ���Ԫ���ظ�����
    size_t r = 2;
    if (cbf.repeatCheck(element, r)) {
        std::cout << "Element '" << element << "' is repeated " << r << " times in the CBF.\n";
    }
    else {
        std::cout << "Element '" << element << "' is not repeated " << r << " times.\n";
    }
    
    // ɾ��Ԫ��
    cbf.update("a", "del");
    cbf.update("a", "del");

    // �ٴμ��Ԫ���Ƿ����
    if (cbf.check("a")) {
        std::cout << "Element 'a' exists in the CBF.\n";
    }
    else {
        std::cout << "Element 'a' does not exist in the CBF.\n";
    }

    return 0;
}
