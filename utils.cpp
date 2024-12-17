#include "utils.h"

const size_t HASH_LEN = 32; // ��ϣֵ����
// XOR 

std::string xorKeys(const std::string& key1, const std::string& key2) {
    size_t maxLength = std::max(key1.size(), key2.size());
    std::string paddedKey1 = key1, paddedKey2 = key2;

    paddedKey1.resize(maxLength, '\0');
    paddedKey2.resize(maxLength, '\0'); // ������key�ĳ��Ȳ���Ϊ�ϳ���һ���ĳ���

    std::string result(maxLength, '\0');
    for (size_t i = 0; i < maxLength; ++i) {
        result[i] = paddedKey1[i] ^ paddedKey2[i];
    }
    return result;
}

// ���������Կ
std::string generateRandomKey(int length) {
    std::string key(length, '\0');
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(33, 126);
    // uniform_int_distribution<> dis(0, 255);
    for (int i = 0; i < length; ++i) {
        key[i] = static_cast<char>(dis(gen));
    }
    return key;
}

// �� char ����ת��Ϊ string
std::string charArrayToByteString(const char* charArray, size_t length) {
    return std::string(charArray, length);
}

// �� string ת��Ϊ char ����
std::vector<char> byteStringToCharArray(const std::string& byteString) {
    return std::vector<char>(byteString.begin(), byteString.end());
}

// ���� SHA256 ��ϣ
std::string computeSHA256(const std::string& data) {
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (mdctx == nullptr) {
        throw std::runtime_error("EVP_MD_CTX_new failed");
    }

    // ��ȡ SHA-256 �㷨��ָ��
    const EVP_MD* md = EVP_sha256();
    if (md == nullptr) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("EVP_sha256() failed");
    }

    // ��ʼ����ϣ����
    if (EVP_DigestInit_ex(mdctx, md, nullptr) != 1) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("EVP_DigestInit_ex failed");
    }

    // ���¹�ϣ����
    if (EVP_DigestUpdate(mdctx, data.data(), data.size()) != 1) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("EVP_DigestUpdate failed");
    }

    // ��ȡ���յĹ�ϣ���
    std::string hash(EVP_MD_size(md), '\0'); // ��ȡ��ϣ�Ĵ�С��SHA256 �� 32 �ֽڣ�
    unsigned int length = 0;
    if (EVP_DigestFinal_ex(mdctx, reinterpret_cast<unsigned char*>(&hash[0]), &length) != 1) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("EVP_DigestFinal_ex failed");
    }

    // �ͷ���ϢժҪ������
    EVP_MD_CTX_free(mdctx);

    return hash;
}

// �����ϣ h1
std::string h1(const std::string& message, int flag) {
    std::string data(1, static_cast<char>(flag));
    data += message; // ƴ�� flag ����Ϣ
    return computeSHA256(data);
}

// �����ϣ h2
std::string h2(const std::string& message) {
    std::string hash(HASH_LEN + 4, '\0'); // ��һ�� HASH_LEN �ֽڣ��ڶ���׷�� 4 �ֽ�
    std::string K(HASH_LEN, '\0');         // ��ʱ�洢ÿ�ֹ�ϣ�Ľ��

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new(); // ������ϣ������
    if (mdctx == nullptr) {
        throw std::runtime_error("EVP_MD_CTX_new failed");
    }

    try {
        // ��һ�ֹ�ϣ
        const EVP_MD* md = EVP_sha256();
        EVP_DigestInit_ex(mdctx, md, nullptr); // ��ʼ�� SHA-256 �㷨
        unsigned char flag1 = 1; // ��־λ 1
        EVP_DigestUpdate(mdctx, &flag1, 1); // �����־λ
        EVP_DigestUpdate(mdctx, message.data(), message.size()); // ������Ϣ
        EVP_DigestFinal_ex(mdctx, reinterpret_cast<unsigned char*>(&K[0]), nullptr); // �����һ�ֹ�ϣ���
        std::copy(K.begin(), K.end(), hash.begin()); // ����һ�ֽ�������� hash ��ǰ HASH_LEN �ֽ�

        // �ڶ��ֹ�ϣ
        EVP_DigestInit_ex(mdctx, md, nullptr); // ��ʼ�� SHA-256 �㷨
        unsigned char flag2 = 2; // ��־λ 2
        EVP_DigestUpdate(mdctx, &flag2, 1); // �����־λ
        EVP_DigestUpdate(mdctx, message.data(), message.size()); // ������Ϣ
        EVP_DigestFinal_ex(mdctx, reinterpret_cast<unsigned char*>(&K[0]), nullptr); // ����ڶ��ֹ�ϣ���
        std::copy(K.begin(), K.begin() + 4, hash.begin() + HASH_LEN); // ȡ�ڶ��ֵ�ǰ 4 �ֽ�׷�ӵ� hash �ĺ� 4 �ֽ�

    }
    catch (...) {
        EVP_MD_CTX_free(mdctx); // �����쳣ʱ�ͷ�������
        throw;
    }

    // �ͷ�������
    EVP_MD_CTX_free(mdctx);

    return hash;
}


std::string h34(const std::string& message, int index, int flag) {
    std::string hash(HASH_LEN, '\0'); // �洢���յĹ�ϣ���
    std::string K(HASH_LEN, '\0');    // ��ʱ�洢��ϣ������
    unsigned char cI[4] = { 0 };      // �洢��������

    // ��ʼ�� cI
    for (int i = 0; i < 4; i++) {
        cI[i] = (index >> (i * 8)) & 0xFF;
    }

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new(); // ������ϣ������
    if (mdctx == nullptr) {
        throw std::runtime_error("EVP_MD_CTX_new failed");
    }

    try {
        // ��ʼ����ϣ�����Ĳ����й�ϣ����
        const EVP_MD* md = EVP_sha256();
        EVP_DigestInit_ex(mdctx, md, nullptr);
        EVP_DigestUpdate(mdctx, &flag, sizeof(flag));  // ���� flag
        EVP_DigestUpdate(mdctx, cI, 4);                 // ��������
        EVP_DigestUpdate(mdctx, message.data(), message.size()); // ������Ϣ
        EVP_DigestFinal_ex(mdctx, reinterpret_cast<unsigned char*>(&K[0]), nullptr);  // �����ϣ���洢���

        // ����������� hash ����
        std::copy(K.begin(), K.end(), hash.begin());

    }
    catch (...) {
        EVP_MD_CTX_free(mdctx); // �����쳣ʱ�ͷ�������
        throw;
    }

    // �ͷŹ�ϣ������
    EVP_MD_CTX_free(mdctx);

    return hash; // ���ؼ�����Ĺ�ϣֵ
}

// α��������ɺ�������������������������α�����
std::string f2(const std::string& key1, const std::string& key2, size_t length) {
    // �ϲ������������
    std::string combinedData = key1 + key2;

    // ʹ�� SHA256 �����ϣֵ
    std::string hashSeed = computeSHA256(combinedData);

    // ����α�����
    std::string prngOutput(length, '\0');
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        throw std::runtime_error("Failed to create EVP_CIPHER_CTX");
    }

    // ʹ�� SHA256 ��ϣ��Ϊ��Կ����ʼ��α�����������
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(hashSeed.data()), nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize EVP_EncryptInit_ex");
    }

    // ����α����������AES ����������α�������
    int outlen = 0;
    if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(&prngOutput[0]), &outlen, reinterpret_cast<const unsigned char*>(&prngOutput[0]), length) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to generate pseudo-random data");
    }

    // ��ɼ���
    if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&prngOutput[0]) + outlen, &outlen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize pseudo-random data generation");
    }

    EVP_CIPHER_CTX_free(ctx);
    return prngOutput;
}


std::string aesEncrypt(const std::string& data, const std::string& key, const std::string& iv) {
    if (key.size() != 2 * AES_BLOCK_SIZE) {
        throw std::invalid_argument("AES key must be 256 bits (32 bytes).");
    }
    if (iv.size() != AES_BLOCK_SIZE) {
        throw std::invalid_argument("AES IV must be 128 bits (16 bytes).");
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        throw std::runtime_error("Failed to create EVP_CIPHER_CTX");
    }

    // ��ʼ�� AES ����
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.data()), reinterpret_cast<const unsigned char*>(iv.data())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize AES encryption");
    }

    // ������ܺ�����ݴ�С
    int ciphertext_len = data.size() + AES_BLOCK_SIZE;
    std::string ciphertext(ciphertext_len, '\0');

    int len = 0;
    if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(&ciphertext[0]), &len, reinterpret_cast<const unsigned char*>(data.data()), data.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to encrypt data");
    }

    int final_len = 0;
    if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&ciphertext[0]) + len, &final_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize encryption");
    }

    ciphertext.resize(len + final_len);
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext;
}

std::string aesDecrypt(const std::string& encryptedData, const std::string& key, const std::string& iv) {
    if (key.size() != AES_BLOCK_SIZE) {
        throw std::invalid_argument("AES key must be 256 bits (32 bytes).");
    }
    if (iv.size() != AES_BLOCK_SIZE) {
        throw std::invalid_argument("AES IV must be 128 bits (16 bytes).");
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        throw std::runtime_error("Failed to create EVP_CIPHER_CTX");
    }

    // ��ʼ�� AES ����
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key.data()), reinterpret_cast<const unsigned char*>(iv.data())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize AES decryption");
    }

    // ������ܺ�����ݴ�С
    std::string decryptedData(encryptedData.size(), '\0');
    int len = 0;

    if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(&decryptedData[0]), &len, reinterpret_cast<const unsigned char*>(encryptedData.data()), encryptedData.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to decrypt data");
    }

    int final_len = 0;
    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&decryptedData[0]) + len, &final_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize decryption");
    }

    decryptedData.resize(len + final_len);
    EVP_CIPHER_CTX_free(ctx);

    return decryptedData;
}

// ��ʼ����¡����������ռλʵ�֣�
std::vector<int> initializeBloomFilter(int size) {
    return std::vector<int>(size, 0); // ��ʼ��Ϊ��
}

// ��������ֵ��������Կ�Ĺ�ϣֵ��
unsigned int generateSeed(const std::string& key) {
    std::hash<std::string> hasher;
    return static_cast<unsigned int>(hasher(key)); // ���ɹ�ϣֵ��Ϊ����
}

// Fisher-Yates Shuffle ʵ�֣�����Կ��
void f1(std::string& array, const std::string& key) {
    unsigned int seed = generateSeed(key); // ������Կ�������������
    std::mt19937 gen(seed);               // ��ʼ�������������

    // �Ӻ���ǰ��������
    for (size_t i = array.size() - 1; i > 0; --i) {
        std::uniform_int_distribution<> dis(0, i); // [0, i] ��Χ�����
        size_t j = dis(gen);
        std::swap(array[i], array[j]);
    }
}

std::string base64Encode(const std::string& data) {
    BIO* bio, * b64;
    BUF_MEM* bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    BIO_push(b64, bio);

    BIO_write(b64, data.data(), data.size());
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bufferPtr);

    std::string encoded(bufferPtr->data, bufferPtr->length);
    BIO_free_all(b64);

    return encoded;
}

std::string base64Decode(const std::string& encoded) {
    BIO* bio, * b64;
    int decodeLen = encoded.length();
    std::string decoded(decodeLen, '\0');

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(encoded.data(), -1);
    BIO_push(b64, bio);

    decodeLen = BIO_read(b64, &decoded[0], decodeLen);
    decoded.resize(decodeLen);
    BIO_free_all(b64);

    return decoded;
}

std::string intTo4ByteString(int value) {
    std::string bytes(4, '\0'); // ����һ����СΪ 4 ���ַ���
    for (int i = 0; i < 4; i++) {
        bytes[i] = static_cast<char>((value >> (i * 8)) & 0xFF); // ��ȡÿ���ֽڲ������ַ���
    }
    return bytes;
}

std::string intTo32ByteString(int value) {
    std::string bytes(32, '\0'); // ��ʼ��һ����СΪ 32 ���ַ�����Ĭ��ֵΪ 0
    for (int i = 0; i < 4; i++) {   // ֻ����� 4 �ֽ�
        bytes[i] = static_cast<char>((value >> (i * 8)) & 0xFF); // ��ȡÿ���ֽڲ������ַ���
    }
    return bytes;
}

std::string stringTo4ByteString(const std::string& input) {
    std::string bytes(4, '\0'); // ��ʼ��Ϊ 4 �� '\0'
    for (size_t i = 0; i < 4 && i < input.size(); ++i) {
        bytes[i] = input[i]; // ת���ַ�Ϊ Byte
    }
    return bytes;
}

int byteStringToInt(const std::string& bytes) {
    int value = 0;
    for (int i = 0; i < 4; i++) {
        value |= (static_cast<unsigned char>(bytes[i]) << (i * 8));  // ��ÿ���ֽڰ�λ��ƴ������
    }
    return value;
}

