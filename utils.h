#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/aes.h>
#include <openssl/buffer.h>
#include <cstring>
using namespace std;
using Byte = unsigned char;
// XOR 
vector<Byte> xorKeys(const vector<Byte>&, const vector<Byte>&);

// 生成随机密钥

vector<Byte> generateRandomKey(int);

// 将 char 数组转换为 vector<Byte>
std::vector<Byte> charArrayToByteVector(const char*, size_t);

// 将 vector<Byte> 转换为 char 数组
std::vector<char> byteVectorToCharArray(const std::vector<Byte>&);

// 计算 SHA256 哈希
std::vector<Byte> computeSHA256(const std::vector<Byte>& );
// 计算哈希 h1
std::vector<Byte> h1(const std::vector<Byte>& , int );

// 计算哈希 h2
std::vector<Byte> h2(const std::vector<Byte>& message);

// 计算哈希 h34
std::vector<Byte> h34(const std::vector<Byte>& message, int index, int flag);

// 伪随机数生成函数，接收三个输入参数并输出伪随机数
std::vector<Byte> f2(const std::vector<Byte>& , const std::vector<Byte>& , const std::vector<Byte>& , size_t );

// AES 加密函数，返回加密后的数据
std::vector<Byte> aesEncrypt(const std::vector<Byte>& , const std::vector<Byte>& , const std::vector<Byte>& );

// AES 解密函数，返回解密后的数据
std::vector<Byte> aesDecrypt(const std::vector<Byte>&, const std::vector<Byte>& , const std::vector<Byte>&);

// 初始化布隆过滤器（简单占位实现）
vector<int> initializeBloomFilter(int );

// 生成种子值（基于密钥的哈希值）
unsigned int generateSeed(const std::string& );

// Fisher-Yates Shuffle 实现（带密钥）
void f1(std::vector<Byte>& , const std::string& );

// Base64 编码

std::string base64Encode(const std::vector<Byte>&);

std::vector<Byte> base64Decode(const std::string&);

std::vector<Byte> intTo4ByteVector(int value);