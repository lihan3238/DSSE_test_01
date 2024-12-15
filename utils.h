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

// ���������Կ

vector<Byte> generateRandomKey(int);

// �� char ����ת��Ϊ vector<Byte>
std::vector<Byte> charArrayToByteVector(const char*, size_t);

// �� vector<Byte> ת��Ϊ char ����
std::vector<char> byteVectorToCharArray(const std::vector<Byte>&);

// ���� SHA256 ��ϣ
std::vector<Byte> computeSHA256(const std::vector<Byte>& );
// �����ϣ h1
std::vector<Byte> h1(const std::vector<Byte>& , int );

// �����ϣ h2
std::vector<Byte> h2(const std::vector<Byte>& message);

// �����ϣ h34
std::vector<Byte> h34(const std::vector<Byte>& message, int index, int flag);

// α��������ɺ�������������������������α�����
std::vector<Byte> f2(const std::vector<Byte>& , const std::vector<Byte>& , const std::vector<Byte>& , size_t );

// AES ���ܺ��������ؼ��ܺ������
std::vector<Byte> aesEncrypt(const std::vector<Byte>& , const std::vector<Byte>& , const std::vector<Byte>& );

// AES ���ܺ��������ؽ��ܺ������
std::vector<Byte> aesDecrypt(const std::vector<Byte>&, const std::vector<Byte>& , const std::vector<Byte>&);

// ��ʼ����¡����������ռλʵ�֣�
vector<int> initializeBloomFilter(int );

// ��������ֵ��������Կ�Ĺ�ϣֵ��
unsigned int generateSeed(const std::string& );

// Fisher-Yates Shuffle ʵ�֣�����Կ��
void f1(std::vector<Byte>& , const std::string& );

// Base64 ����

std::string base64Encode(const std::vector<Byte>&);

std::vector<Byte> base64Decode(const std::string&);

std::vector<Byte> intTo4ByteVector(int value);