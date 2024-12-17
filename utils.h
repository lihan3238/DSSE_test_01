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

// ���������Կ
std::string generateRandomKey(int);

// �� char ����ת��Ϊ string
std::string charArrayToByteString(const char*, size_t);

// �� string ת��Ϊ char ����
std::vector<char> byteStringToCharArray(const std::string&);

// ���� SHA256 ��ϣ
std::string computeSHA256(const std::string&);

// �����ϣ h1
std::string h1(const std::string&, int);

// �����ϣ h2
std::string h2(const std::string&);

// �����ϣ h34
std::string h34(const std::string&, int, int);

// α��������ɺ��������ն���������������α�����
std::string f2(const std::string&, const std::string&, size_t);

// AES ���ܺ��������ؼ��ܺ������
std::string aesEncrypt(const std::string&, const std::string&, const std::string&);

// AES ���ܺ��������ؽ��ܺ������
std::string aesDecrypt(const std::string&, const std::string&, const std::string&);

// ��ʼ����¡����������ռλʵ�֣�
std::vector<int> initializeBloomFilter(int);

// ��������ֵ��������Կ�Ĺ�ϣֵ��
unsigned int generateSeed(const std::string&);

// Fisher-Yates Shuffle ʵ�֣�����Կ��
void f1(std::string&, const std::string&);

// Base64 ����
std::string base64Encode(const std::string&);

std::string base64Decode(const std::string&);

std::string intTo4ByteString(int value);
std::string intTo32ByteString(int value);
std::string stringTo4ByteString(const std::string& input);
int byteStringToInt(const std::string& bytes);