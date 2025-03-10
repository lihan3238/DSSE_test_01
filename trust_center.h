#pragma once
#include "cbf.h"
#include "utils.h"
#include <httplib.h>
#include <json/json.h>
using namespace std;

// Base64 编码
std::string stringToBase64(const std::string& data); // 修改为 std::string
void checkShutdown(httplib::Server& svr);
// 启动信任中心
void startTrustCenter(int param1, int param2);
