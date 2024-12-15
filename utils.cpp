
#include "utils.h"

const int HASH_LEN = 32; // ��ϣֵ����
// XOR 

vector<Byte> xorKeys(const vector<Byte>&key1, const vector<Byte>&key2) {
    size_t maxLength = max(key1.size(), key2.size());
    vector<Byte> paddedKey1 = key1, paddedKey2 = key2;

    paddedKey1.resize(maxLength, 0);
    paddedKey2.resize(maxLength, 0); // ������key�ĳ��Ȳ���Ϊ�ϳ���һ���ĳ���

    vector<Byte> result(maxLength);
    for (size_t i = 0; i < maxLength; ++i) {
        result[i] = paddedKey1[i] ^ paddedKey2[i];
    }
    return result;
}

// ���������Կ
vector<Byte> generateRandomKey(int length) {
    vector<Byte> key(length);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(33,126);
    //uniform_int_distribution<> dis(0, 255);
    for (int i = 0; i < length; ++i) {
        key[i] = static_cast<Byte>(dis(gen));
    }
    return key;
}

// �� char ����ת��Ϊ vector<Byte>
std::vector<Byte> charArrayToByteVector(const char* charArray, size_t length) {
    return std::vector<Byte>(charArray, charArray + length);
}

// �� vector<Byte> ת��Ϊ char ����
std::vector<char> byteVectorToCharArray(const std::vector<Byte>& byteVector) {
    return std::vector<char>(byteVector.begin(), byteVector.end());
}

// ���� SHA256 ��ϣ
std::vector<Byte> computeSHA256(const std::vector<Byte>& data) {
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
    std::vector<Byte> hash(EVP_MD_size(md)); // ��ȡ��ϣ�Ĵ�С��SHA256 �� 32 �ֽڣ�
    unsigned int length = 0;
    if (EVP_DigestFinal_ex(mdctx, hash.data(), &length) != 1) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("EVP_DigestFinal_ex failed");
    }

    // �ͷ���ϢժҪ������
    EVP_MD_CTX_free(mdctx);

    return hash;
}

// �����ϣ h1
std::vector<Byte> h1(const std::vector<Byte>& message, int flag) {
    std::vector<Byte> data = { static_cast<Byte>(flag) };
    data.insert(data.end(), message.begin(), message.end()); // ƴ�� flag ����Ϣ
    return computeSHA256(data);
}

// �����ϣ h2
std::vector<Byte> h2(const std::vector<Byte>& message) {
    std::vector<Byte> hash(HASH_LEN + 4); // ��һ�� HASH_LEN �ֽڣ��ڶ���׷�� 4 �ֽ�
    std::vector<Byte> K(HASH_LEN);         // ��ʱ�洢ÿ�ֹ�ϣ�Ľ��

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
        EVP_DigestFinal_ex(mdctx, K.data(), nullptr); // �����һ�ֹ�ϣ���
        std::copy(K.begin(), K.end(), hash.begin()); // ����һ�ֽ�������� hash ��ǰ HASH_LEN �ֽ�

        // �ڶ��ֹ�ϣ
        EVP_DigestInit_ex(mdctx, md, nullptr); // ��ʼ�� SHA-256 �㷨
        unsigned char flag2 = 2; // ��־λ 2
        EVP_DigestUpdate(mdctx, &flag2, 1); // �����־λ
        EVP_DigestUpdate(mdctx, message.data(), message.size()); // ������Ϣ
        EVP_DigestFinal_ex(mdctx, K.data(), nullptr); // ����ڶ��ֹ�ϣ���
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


std::vector<Byte> h34(const std::vector<Byte>& message, int index, int flag) {
    std::vector<Byte> hash(HASH_LEN); // �洢���յĹ�ϣ���
    std::vector<Byte> K(HASH_LEN);    // ��ʱ�洢��ϣ������
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
        EVP_DigestFinal_ex(mdctx, K.data(), nullptr);  // �����ϣ���洢���

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
std::vector<Byte> f2(const std::vector<Byte>& key1, const std::vector<Byte>& key2, const std::vector<Byte>& key3, size_t length) {
    // �ϲ������������
    std::vector<Byte> combinedData;
    combinedData.insert(combinedData.end(), key1.begin(), key1.end());
    combinedData.insert(combinedData.end(), key2.begin(), key2.end());
    combinedData.insert(combinedData.end(), key3.begin(), key3.end());

    // ʹ�� SHA256 �����ϣֵ
    std::vector<Byte> hashSeed = computeSHA256(combinedData);

    // ����α�����
    std::vector<Byte> prngOutput(length);
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        throw std::runtime_error("Failed to create EVP_CIPHER_CTX");
    }

    // ʹ�� SHA256 ��ϣ��Ϊ��Կ����ʼ��α�����������
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, hashSeed.data(), nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize EVP_EncryptInit_ex");
    }

    // ����α����������AES ����������α�������
    int outlen = 0;
    if (EVP_EncryptUpdate(ctx, prngOutput.data(), &outlen, prngOutput.data(), length) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to generate pseudo-random data");
    }

    // ��ɼ���
    if (EVP_EncryptFinal_ex(ctx, prngOutput.data() + outlen, &outlen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize pseudo-random data generation");
    }

    EVP_CIPHER_CTX_free(ctx);
    return prngOutput;
}

// AES ���ܺ��������ؼ��ܺ������
std::vector<Byte> aesEncrypt(const std::vector<Byte>& data, const std::vector<Byte>& key, const std::vector<Byte>& iv) {
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
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize AES encryption");
    }

    // ������ܺ�����ݴ�С
    int ciphertext_len = data.size() + AES_BLOCK_SIZE;
    std::vector<Byte> ciphertext(ciphertext_len);

    int len = 0;
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, data.data(), data.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to encrypt data");
    }

    int final_len = 0;
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &final_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize encryption");
    }

    ciphertext.resize(len + final_len);
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext;
}

// AES ���ܺ��������ؽ��ܺ������
std::vector<Byte> aesDecrypt(const std::vector<Byte>& encryptedData, const std::vector<Byte>& key, const std::vector<Byte>& iv) {
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
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize AES decryption");
    }

    // ������ܺ�����ݴ�С
    std::vector<Byte> decryptedData(encryptedData.size());
    int len = 0;

    if (EVP_DecryptUpdate(ctx, decryptedData.data(), &len, encryptedData.data(), encryptedData.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to decrypt data");
    }

    int final_len = 0;
    if (EVP_DecryptFinal_ex(ctx, decryptedData.data() + len, &final_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize decryption");
    }

    decryptedData.resize(len + final_len);
    EVP_CIPHER_CTX_free(ctx);

    return decryptedData;
}

// ��ʼ����¡����������ռλʵ�֣�
vector<int> initializeBloomFilter(int size) {
    return vector<int>(size, 0); // ��ʼ��Ϊ��
}

// ��������ֵ��������Կ�Ĺ�ϣֵ��
unsigned int generateSeed(const std::string& key) {
    std::hash<std::string> hasher;
    return static_cast<unsigned int>(hasher(key)); // ���ɹ�ϣֵ��Ϊ����
}

// Fisher-Yates Shuffle ʵ�֣�����Կ��
void f1(std::vector<Byte>& array, const std::string& key) {
    unsigned int seed = generateSeed(key); // ������Կ�������������
    std::mt19937 gen(seed);               // ��ʼ�������������

    // �Ӻ���ǰ��������
    for (size_t i = array.size() - 1; i > 0; --i) {
        std::uniform_int_distribution<> dis(0, i); // [0, i] ��Χ�����
        size_t j = dis(gen);
        std::swap(array[i], array[j]);
    }
}

std::string base64Encode(const std::vector<Byte>& data) {
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

std::vector<Byte> base64Decode(const std::string& encoded) {
    BIO* bio, * b64;
    int decodeLen = encoded.length();
    std::vector<Byte> decoded(decodeLen);

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(encoded.data(), -1);
    BIO_push(b64, bio);

    decodeLen = BIO_read(b64, decoded.data(), decodeLen);
    decoded.resize(decodeLen);
    BIO_free_all(b64);

    return decoded;
}

std::vector<Byte> intTo4ByteVector(int value) {
    std::vector<Byte> bytes(4); // ����һ����СΪ 4 �� vector
    for (int i = 0; i < 4; i++) {
        bytes[i] = (value >> (i * 8)) & 0xFF; // ��ȡÿ���ֽڲ����� vector
    }
    return bytes;
}