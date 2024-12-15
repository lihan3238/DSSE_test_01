#pragma once
// trustcenter.cpp
#include <httplib.h>
#include <iostream>
#include <json/json.h>  // 用于 JSON 解析
#include "utils.h"
#include "cbf.h"

// 服务端处理请求，发送参数

void setupServerData();
void startServer();