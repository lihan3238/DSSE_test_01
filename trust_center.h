#pragma once
#include "cbf.h"
#include "utils.h"
#include <httplib.h>
#include <json/json.h>
using namespace std;

// Base64 ����
std::string stringToBase64(const std::string& data); // �޸�Ϊ std::string
void checkShutdown(httplib::Server& svr);
// ������������
void startTrustCenter(int param1, int param2);
