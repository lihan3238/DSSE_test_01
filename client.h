#pragma once
// client.cpp
#include <httplib.h>
#include <iostream>
#include <json/json.h>  // ”√”⁄ JSON Ω‚Œˆ
#include "utils.h"
#include "cbf.h"


void setupClientData();

void updateClient(string updateFile);

std::vector<std::string> searchToken(const std::vector<std::string>& words,string Q,int q);

bool verifyCBF(const CountingBloomFilter& RCBF, std::vector<std::string>& searchTokens);

void searchClient(std::vector<std::string> searchTokens, string Q, int q, size_t startInd, size_t endInd);