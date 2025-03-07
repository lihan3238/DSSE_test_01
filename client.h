#pragma once
// client.cpp
#include <httplib.h>
#include <iostream>
#include <json/json.h>  // ÓÃÓÚ JSON ½âÎö
#include "utils.h"
#include "cbf.h"


void setupClientData();

void updateClient(string updateFile);

std::vector<std::string> searchToken(const std::vector<std::string>& words,string Q,int q);

void searchClient(std::vector<std::string> searchTokens, string Q, int q, size_t startInd, size_t endInd);