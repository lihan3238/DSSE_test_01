#include <httplib.h>
#include <iostream>
// #include <thread>
// #include "server.h"
#include "client.h"
// #include "trust_center.h"

int main124412424()
{
	// startTrustCenter(LAMBDA, BLOOM_SIZE);
	int switcher = 1;
	std::string updateFile;
	size_t startInd=0;
	size_t endInd=0;
	while (switcher)
	{
		cout << "1. Setup Client Data" << endl;
		cout << "2. Connect Server" << endl;
		cout << "3. Search" << endl;
		cin >> switcher;
		switch (switcher)
		{
		case 1:
			setupClientData();
			break;
		case 2:
			std::cout << "请输入更新文件：" << std::endl;
			std::cin >> updateFile;
			updateClient(updateFile);
			break;
		case 3:

			cout << "Enter how many words" << endl;
			int q = 0;
			cin >> q;
			cout << "Enter search type(disjunctive,conjunctive)" << endl;
			string Q = "";
			cin >> Q;
			cout << "Enter the word you want to search" << endl;

			std::vector<std::string> Words;

			for (int i = 0; i < q; i++)
			{
				std::string word;
				std::cin >> word;	   // ��ȡһ������
				Words.push_back(word); // ���õ������ӵ� Words ������
			}

			std::cout << "输入总索引范围：" << endl;
			std::cout << "start:" << endl;
			std::cin >> startInd;
			std::cout << "end;" << endl;
			std::cin >> endInd;
			searchClient(searchToken(Words, Q, q), Q, q,startInd,endInd);
			break;
		}
	}

	return 0;
}