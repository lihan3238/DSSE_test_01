#pragma once
#include "cbf.h"
#include "utils.h"
#include <httplib.h>
#include <json/json.h>
using namespace std;

// Base64 ±àÂë
std::string vectorToBase64(const std::vector<Byte>&);

void startTrustCenter(int, int);
