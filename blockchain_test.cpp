#include <httplib.h>
#include <iostream>
//#include <thread>
#include "blockchain.h"
//#include "client.h"
//#include "trust_center.h"

int main1111() {
	//startTrustCenter(LAMBDA, BLOOM_SIZE);
	int switcher = 1;
	while (switcher)
	{
		cout << "1. Start Blockchain Server" << endl;
		cout << "2. Update Blockchain Data" << endl;
		cin >> switcher;
		switch (switcher)
		{
		case 1:
			startBlockChainServer();
			break;

		case 2:
			updateBlockChain();

		}

	}


	return 0;
}