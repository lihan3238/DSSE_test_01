#include <httplib.h>
#include <iostream>
//#include <thread>
//#include "server.h"
#include "client.h"
//#include "trust_center.h"

int main() {
	//startTrustCenter(LAMBDA, BLOOM_SIZE);
	int switcher = 1;
	while (switcher)
	{
		cout << "1. Setup Client Data" << endl;
		cout << "2. Connect Server" << endl;
		cin >> switcher;
		switch (switcher)
		{
		case 1:
			setupClientData();
			break;
		case 2:
			updateClient();
		}
	}


	return 0;
}