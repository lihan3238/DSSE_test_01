#include <httplib.h>
#include <iostream>
//#include <thread>
//#include "server.h"
//#include "client.h"
#include "trust_center.h"

// ȫ�ֲ���
const int LAMBDA = 256; // ��ȫ���� ��
const int BLOOM_SIZE = 256; // ��¡��������С
//const int HASH_LEN = 32; // ��ϣֵ����

//int main() {
//    // �������������߳�
//    std::thread trustcenterThread(startTrustCenter, LAMBDA, BLOOM_SIZE);
//
//    // ��������������߳�
//    std::thread serverThread_setup(setupServerData);
//
//    // �����ͻ��������߳�
//    std::thread clientThread_setup(setupClientData);
//
//    trustcenterThread.join();
//    serverThread_setup.join();
//    clientThread_setup.join();
//
//    // ȷ���������Ѿ��ڼ���
//    std::thread serverThread_update(startServer);
//
//    // ���߳���ʱ����
//   // std::this_thread::sleep_for(std::chrono::seconds(2));  // ȷ�����������㹻ʱ������
//
//    // �����ͻ��˸����߳�
//
//    serverThread_update.join(); 
//
//    std::this_thread::sleep_for(std::chrono::seconds(1));  // ȷ�����������㹻ʱ������
//    std::thread clientThread_update(communicateWithServer);
//
//    clientThread_update.join();
//
//    return 0;
//}

//int main() {
//	startTrustCenter(LAMBDA, BLOOM_SIZE);
//	return 0;
//}