#pragma once
// trustcenter.cpp
#include <httplib.h>
#include <iostream>
#include <json/json.h>  // ���� JSON ����
#include "utils.h"
#include "cbf.h"

// ����˴������󣬷��Ͳ���

void setupServerData();
void updateServer();
void searchServer();
void svr_reSet();