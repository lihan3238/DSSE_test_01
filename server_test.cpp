#include <httplib.h>
#include <iostream>
//#include <thread>
#include "server.h"
//#include "client.h"
//#include "trust_center.h"

int main1561() {
	//startTrustCenter(LAMBDA, BLOOM_SIZE);
	int switcher = 1;
	while (switcher)
	{
		cout << "1. Setup Server Data" << endl;
		cout << "2. Start Server" << endl;
		cout << "3. Search" << endl;
		cin >> switcher;
		switch (switcher)
		{
		case 1:
			setupServerData();
			break;
		case 2:
			updateServer();
			break;
		case 3:
			searchServer();
		}

	}


	return 0;
}

