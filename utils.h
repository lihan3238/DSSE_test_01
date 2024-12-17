#pragma once
#include <iostream>
#include <string>
#include <random>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/aes.h>
#include <openssl/buffer.h>
#include <cstring>
#include <tuple>
#include <utility> 
using namespace std;

// XOR
std::string xorKeys(const std::string&, const std::string&);

// 生成随机密钥
std::string generateRandomKey(int);

// 将 char 数组转换为 string
std::string charArrayToByteString(const char*, size_t);

// 将 string 转换为 char 数组
std::vector<char> byteStringToCharArray(const std::string&);

// 计算 SHA256 哈希
std::string computeSHA256(const std::string&);

// 计算哈希 h1
std::string h1(const std::string&, int);

// 计算哈希 h2
std::string h2(const std::string&);

// 计算哈希 h34
std::string h34(const std::string&, int, int);

// 伪随机数生成函数，接收二个输入参数并输出伪随机数
std::string f2(const std::string&, const std::string&, size_t);

// AES 加密函数，返回加密后的数据
std::string aesEncrypt(const std::string&, const std::string&, const std::string&);

// AES 解密函数，返回解密后的数据
std::string aesDecrypt(const std::string&, const std::string&, const std::string&);

// 初始化布隆过滤器（简单占位实现）
std::vector<int> initializeBloomFilter(int);

// 生成种子值（基于密钥的哈希值）
unsigned int generateSeed(const std::string&);

// Fisher-Yates Shuffle 实现（带密钥）
void f1(std::string&, const std::string&);

// Base64 编码
std::string base64Encode(const std::string&);

std::string base64Decode(const std::string&);

std::string intTo4ByteString(int value);
std::string intTo32ByteString(int value);
std::string stringTo4ByteString(const std::string& input);
int byteStringToInt(const std::string& bytes);