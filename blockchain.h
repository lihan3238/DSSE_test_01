#pragma once

#include <httplib.h>
#include <iostream>
#include <json/json.h>  // 用于 JSON 解析
#include <unordered_map>

#include "utils.h"
#include "cbf.h"

// 区块链服务端处理请求，发送参数
void startBlockChainServer();
void updateBlockChain();
