/**
 * @file utils.h
 * @brief utils functions
 * @author zhenkai.sun
 */
#pragma once
extern "C" {
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
}
// sys
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace apollo {
extern const char *HMAC_SHA1_SIGN_DELIMITER;

int64_t CurrentMilliseconds();

/**
 * @brief Calculate HmacSha1Sign with secret key
 * @param [in] raw raw data need to be encoded
 * @param [in] key secret key
 * @note: The implemention cod of this function is completely generated by
 * deepseek-r1
 * */
template <typename S1, typename S2>
std::string HmacSha1Sign(const S1 &raw, const S2 &key) {
  // HMAC-SHA1 计算
  unsigned char digest[EVP_MAX_MD_SIZE];
  unsigned int digestLength = 0;

  HMAC_CTX *hmacCtx = HMAC_CTX_new();
  if (!hmacCtx) {
    throw std::runtime_error("Failed to create HMAC context");
  }

  // 初始化 HMAC 上下文
  if (HMAC_Init_ex(hmacCtx, key.data(), static_cast<int>(key.length()), EVP_sha1(), nullptr) != 1) {
    HMAC_CTX_free(hmacCtx);
    throw std::runtime_error("HMAC initialization failed");
  }

  // 更新数据
  if (HMAC_Update(hmacCtx, reinterpret_cast<const unsigned char *>(raw.data()), raw.length()) != 1) {
    HMAC_CTX_free(hmacCtx);
    throw std::runtime_error("HMAC update failed");
  }

  // 获取最终摘要
  if (HMAC_Final(hmacCtx, digest, &digestLength) != 1) {
    HMAC_CTX_free(hmacCtx);
    throw std::runtime_error("HMAC finalization failed");
  }

  HMAC_CTX_free(hmacCtx);

  // Base64 编码
  BIO *bio = BIO_new(BIO_f_base64());
  BIO *bmem = BIO_new(BIO_s_mem());
  bio = BIO_push(bio, bmem);

  // 配置 Base64 参数
  BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
  BIO_write(bio, digest, digestLength);
  BIO_flush(bio);

  BUF_MEM *bptr;
  BIO_get_mem_ptr(bio, &bptr);

  std::string result(bptr->data, bptr->length);

  BIO_free_all(bio);

  return result;
}

template <typename T>
std::string ToString(const std::vector<T> &v) {
  std::stringstream ss;
  ss << "[";
  for (int i = 0; i < v.size(); ++i) {
    if (i != 0) ss << ",";
    ss << v[i];
  }
  ss << "]";
  return ss.str();
}

template <typename K, typename V>
std::string ToString(const std::unordered_map<K, V> &m) {
  std::stringstream ss;
  ss << "{";
  for (auto it = m.begin(); it != m.end(); ++it) {
    if (it != m.begin()) ss << ",";
    ss << "\"" << it->first << "\":" << "\"" << it->second << "\"";
  }
  ss << "}";
  return ss.str();
}
}  // namespace apollo
