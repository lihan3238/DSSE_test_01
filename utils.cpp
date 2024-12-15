
#include "utils.h"

const int HASH_LEN = 32; // 哈希值长度
// XOR 

vector<Byte> xorKeys(const vector<Byte>&key1, const vector<Byte>&key2) {
    size_t maxLength = max(key1.size(), key2.size());
    vector<Byte> paddedKey1 = key1, paddedKey2 = key2;

    paddedKey1.resize(maxLength, 0);
    paddedKey2.resize(maxLength, 0); // 将两个key的长度补齐为较长的一个的长度

    vector<Byte> result(maxLength);
    for (size_t i = 0; i < maxLength; ++i) {
        result[i] = paddedKey1[i] ^ paddedKey2[i];
    }
    return result;
}

// 生成随机密钥
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

// 将 char 数组转换为 vector<Byte>
std::vector<Byte> charArrayToByteVector(const char* charArray, size_t length) {
    return std::vector<Byte>(charArray, charArray + length);
}

// 将 vector<Byte> 转换为 char 数组
std::vector<char> byteVectorToCharArray(const std::vector<Byte>& byteVector) {
    return std::vector<char>(byteVector.begin(), byteVector.end());
}

// 计算 SHA256 哈希
std::vector<Byte> computeSHA256(const std::vector<Byte>& data) {
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (mdctx == nullptr) {
        throw std::runtime_error("EVP_MD_CTX_new failed");
    }

    // 获取 SHA-256 算法的指针
    const EVP_MD* md = EVP_sha256();
    if (md == nullptr) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("EVP_sha256() failed");
    }

    // 初始化哈希计算
    if (EVP_DigestInit_ex(mdctx, md, nullptr) != 1) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("EVP_DigestInit_ex failed");
    }

    // 更新哈希计算
    if (EVP_DigestUpdate(mdctx, data.data(), data.size()) != 1) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("EVP_DigestUpdate failed");
    }

    // 获取最终的哈希结果
    std::vector<Byte> hash(EVP_MD_size(md)); // 获取哈希的大小（SHA256 是 32 字节）
    unsigned int length = 0;
    if (EVP_DigestFinal_ex(mdctx, hash.data(), &length) != 1) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("EVP_DigestFinal_ex failed");
    }

    // 释放消息摘要上下文
    EVP_MD_CTX_free(mdctx);

    return hash;
}

// 计算哈希 h1
std::vector<Byte> h1(const std::vector<Byte>& message, int flag) {
    std::vector<Byte> data = { static_cast<Byte>(flag) };
    data.insert(data.end(), message.begin(), message.end()); // 拼接 flag 和消息
    return computeSHA256(data);
}

// 计算哈希 h2
std::vector<Byte> h2(const std::vector<Byte>& message) {
    std::vector<Byte> hash(HASH_LEN + 4); // 第一轮 HASH_LEN 字节，第二轮追加 4 字节
    std::vector<Byte> K(HASH_LEN);         // 临时存储每轮哈希的结果

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new(); // 创建哈希上下文
    if (mdctx == nullptr) {
        throw std::runtime_error("EVP_MD_CTX_new failed");
    }

    try {
        // 第一轮哈希
        const EVP_MD* md = EVP_sha256();
        EVP_DigestInit_ex(mdctx, md, nullptr); // 初始化 SHA-256 算法
        unsigned char flag1 = 1; // 标志位 1
        EVP_DigestUpdate(mdctx, &flag1, 1); // 加入标志位
        EVP_DigestUpdate(mdctx, message.data(), message.size()); // 加入消息
        EVP_DigestFinal_ex(mdctx, K.data(), nullptr); // 计算第一轮哈希结果
        std::copy(K.begin(), K.end(), hash.begin()); // 将第一轮结果拷贝到 hash 的前 HASH_LEN 字节

        // 第二轮哈希
        EVP_DigestInit_ex(mdctx, md, nullptr); // 初始化 SHA-256 算法
        unsigned char flag2 = 2; // 标志位 2
        EVP_DigestUpdate(mdctx, &flag2, 1); // 加入标志位
        EVP_DigestUpdate(mdctx, message.data(), message.size()); // 加入消息
        EVP_DigestFinal_ex(mdctx, K.data(), nullptr); // 计算第二轮哈希结果
        std::copy(K.begin(), K.begin() + 4, hash.begin() + HASH_LEN); // 取第二轮的前 4 字节追加到 hash 的后 4 字节

    }
    catch (...) {
        EVP_MD_CTX_free(mdctx); // 捕获异常时释放上下文
        throw;
    }

    // 释放上下文
    EVP_MD_CTX_free(mdctx);

    return hash;
}


std::vector<Byte> h34(const std::vector<Byte>& message, int index, int flag) {
    std::vector<Byte> hash(HASH_LEN); // 存储最终的哈希结果
    std::vector<Byte> K(HASH_LEN);    // 临时存储哈希计算结果
    unsigned char cI[4] = { 0 };      // 存储索引部分

    // 初始化 cI
    for (int i = 0; i < 4; i++) {
        cI[i] = (index >> (i * 8)) & 0xFF;
    }

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new(); // 创建哈希上下文
    if (mdctx == nullptr) {
        throw std::runtime_error("EVP_MD_CTX_new failed");
    }

    try {
        // 初始化哈希上下文并进行哈希计算
        const EVP_MD* md = EVP_sha256();
        EVP_DigestInit_ex(mdctx, md, nullptr);
        EVP_DigestUpdate(mdctx, &flag, sizeof(flag));  // 处理 flag
        EVP_DigestUpdate(mdctx, cI, 4);                 // 处理索引
        EVP_DigestUpdate(mdctx, message.data(), message.size()); // 处理消息
        EVP_DigestFinal_ex(mdctx, K.data(), nullptr);  // 计算哈希并存储结果

        // 将结果拷贝到 hash 向量
        std::copy(K.begin(), K.end(), hash.begin());

    }
    catch (...) {
        EVP_MD_CTX_free(mdctx); // 捕获异常时释放上下文
        throw;
    }

    // 释放哈希上下文
    EVP_MD_CTX_free(mdctx);

    return hash; // 返回计算出的哈希值
}

// 伪随机数生成函数，接收三个输入参数并输出伪随机数
std::vector<Byte> f2(const std::vector<Byte>& key1, const std::vector<Byte>& key2, const std::vector<Byte>& key3, size_t length) {
    // 合并三个输入参数
    std::vector<Byte> combinedData;
    combinedData.insert(combinedData.end(), key1.begin(), key1.end());
    combinedData.insert(combinedData.end(), key2.begin(), key2.end());
    combinedData.insert(combinedData.end(), key3.begin(), key3.end());

    // 使用 SHA256 计算哈希值
    std::vector<Byte> hashSeed = computeSHA256(combinedData);

    // 生成伪随机数
    std::vector<Byte> prngOutput(length);
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        throw std::runtime_error("Failed to create EVP_CIPHER_CTX");
    }

    // 使用 SHA256 哈希作为密钥，初始化伪随机数生成器
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, hashSeed.data(), nullptr) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize EVP_EncryptInit_ex");
    }

    // 加密伪随机数输出（AES 加密以生成伪随机数）
    int outlen = 0;
    if (EVP_EncryptUpdate(ctx, prngOutput.data(), &outlen, prngOutput.data(), length) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to generate pseudo-random data");
    }

    // 完成加密
    if (EVP_EncryptFinal_ex(ctx, prngOutput.data() + outlen, &outlen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize pseudo-random data generation");
    }

    EVP_CIPHER_CTX_free(ctx);
    return prngOutput;
}

// AES 加密函数，返回加密后的数据
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

    // 初始化 AES 加密
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize AES encryption");
    }

    // 计算加密后的数据大小
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

// AES 解密函数，返回解密后的数据
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

    // 初始化 AES 解密
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize AES decryption");
    }

    // 计算解密后的数据大小
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

// 初始化布隆过滤器（简单占位实现）
vector<int> initializeBloomFilter(int size) {
    return vector<int>(size, 0); // 初始化为空
}

// 生成种子值（基于密钥的哈希值）
unsigned int generateSeed(const std::string& key) {
    std::hash<std::string> hasher;
    return static_cast<unsigned int>(hasher(key)); // 生成哈希值作为种子
}

// Fisher-Yates Shuffle 实现（带密钥）
void f1(std::vector<Byte>& array, const std::string& key) {
    unsigned int seed = generateSeed(key); // 基于密钥生成随机数种子
    std::mt19937 gen(seed);               // 初始化随机数生成器

    // 从后向前遍历数组
    for (size_t i = array.size() - 1; i > 0; --i) {
        std::uniform_int_distribution<> dis(0, i); // [0, i] 范围随机数
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
    std::vector<Byte> bytes(4); // 创建一个大小为 4 的 vector
    for (int i = 0; i < 4; i++) {
        bytes[i] = (value >> (i * 8)) & 0xFF; // 提取每个字节并存入 vector
    }
    return bytes;
}