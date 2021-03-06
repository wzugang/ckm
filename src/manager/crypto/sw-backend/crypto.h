/* Copyright (c) 2000 - 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 *
 *
 * @file        crypto.h
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief       Crypto module implementation.
 */
#pragma once

#include <vector>

#include <openssl/evp.h>

#include <dpl/log/log.h>
#include <dpl/raw-buffer.h>

#include <generic-backend/exception.h>

namespace CKM {
namespace Crypto {
namespace SW {
namespace Cipher {

template<class T>
struct Base {
    Base()
      : m_ctx(EVP_CIPHER_CTX_new())
    {
        static_assert(sizeof(typename T::value_type) == 1, "Unsupported type inside conatainer.");
    }
    Base(const Base&) = delete;
    Base(Base &&) = delete;
    Base<T>& operator=(const Base&) = delete;
    Base<T>& operator=(Base &&) = delete;

    // Low level api.
    // Allows various cipher specific parameters to be determined and set.
    int Control(int type, int arg, void *ptr)
    {
        return EVP_CIPHER_CTX_ctrl(m_ctx, type, arg, ptr);
    }

    virtual void AppendAAD(const T&) = 0;
    virtual T Append(const T&) = 0;
    virtual T Finalize() = 0;
    virtual ~Base()
    {
        EVP_CIPHER_CTX_free(m_ctx);
    }

protected:
    EVP_CIPHER_CTX *m_ctx;
};

template<class T>
class EvpCipherWrapper : public Base<T> {
public:
    using Base<T>::m_ctx;

    EvpCipherWrapper(const EVP_CIPHER *type, const T &key, const T &iv, bool encryption)
    {
        if (static_cast<int>(key.size()) != EVP_CIPHER_key_length(type))
            ThrowErr(Exc::Crypto::InternalError, "Wrong key size! Expected: ", EVP_CIPHER_key_length(type), " Get: ", key.size());

        if (static_cast<int>(iv.size()) < EVP_CIPHER_iv_length(type))
            ThrowErr(Exc::Crypto::InternalError, "Wrong iv size! Expected: ", EVP_CIPHER_iv_length(type), " Get: ", iv.size());

        if (1 != EVP_CipherInit_ex(m_ctx, type, NULL, key.data(), iv.data(), encryption ? 1 : 0))
            ThrowErr(Exc::Crypto::InternalError, "Failed in EVP_CipherInit");

        EVP_CIPHER_CTX_set_padding(m_ctx, 1);
    }

    void AppendAAD(const T& data)
    {
        static_assert(sizeof(typename T::value_type) == 1, "Unsupported type inside container.");
        int bytesLen;
        if (1 != EVP_CipherUpdate(m_ctx, NULL, &bytesLen, data.data(), data.size()))
            ThrowErr(Exc::Crypto::InternalError, "AppendAAD(): Failed in EVP_CipherUpdate");
    }

    T Append(const T& data)
    {
        static_assert(sizeof(typename T::value_type) == 1, "Unsupported type inside container.");
        int bytesLen = static_cast<int>(data.size() + EVP_CIPHER_CTX_block_size(m_ctx));
        T output(bytesLen);
        if (1 != EVP_CipherUpdate(m_ctx, output.data(), &bytesLen, data.data(), data.size()))
            ThrowErr(Exc::Crypto::InternalError, "Append(): Failed in EVP_CipherUpdate");

        output.resize(bytesLen);
        return output;
    }

    T Finalize()
    {
        int bytesLen = EVP_CIPHER_CTX_block_size(m_ctx);
        T output(bytesLen);
        if (1 != EVP_CipherFinal_ex(m_ctx, output.data(), &bytesLen))
            ThrowErr(Exc::Crypto::InternalError, "Failed in EVP_CipherFinal");

        output.resize(bytesLen);
        return output;
    }
};

#define DEFINE_CIPHER(__classname, __type, __evpcipher, __encryption) \
class __classname : public EvpCipherWrapper<__type> {                 \
public:                                                               \
    __classname(const __type &key, const __type &iv)                  \
      : EvpCipherWrapper(__evpcipher, key, iv, __encryption)          \
    {}                                                                \
}

DEFINE_CIPHER(AesCbcEncryption128, RawBuffer, EVP_aes_128_cbc(), true);
DEFINE_CIPHER(AesCbcDecryption128, RawBuffer, EVP_aes_128_cbc(), false);
DEFINE_CIPHER(AesCbcEncryption192, RawBuffer, EVP_aes_192_cbc(), true);
DEFINE_CIPHER(AesCbcDecryption192, RawBuffer, EVP_aes_192_cbc(), false);
DEFINE_CIPHER(AesCbcEncryption256, RawBuffer, EVP_aes_256_cbc(), true);
DEFINE_CIPHER(AesCbcDecryption256, RawBuffer, EVP_aes_256_cbc(), false);

DEFINE_CIPHER(AesGcmEncryption128, RawBuffer, EVP_aes_128_gcm(), true);
DEFINE_CIPHER(AesGcmDecryption128, RawBuffer, EVP_aes_128_gcm(), false);
DEFINE_CIPHER(AesGcmEncryption192, RawBuffer, EVP_aes_192_gcm(), true);
DEFINE_CIPHER(AesGcmDecryption192, RawBuffer, EVP_aes_192_gcm(), false);
DEFINE_CIPHER(AesGcmEncryption256, RawBuffer, EVP_aes_256_gcm(), true);
DEFINE_CIPHER(AesGcmDecryption256, RawBuffer, EVP_aes_256_gcm(), false);

DEFINE_CIPHER(AesCtrEncryption128, RawBuffer, EVP_aes_128_ctr(), true);
DEFINE_CIPHER(AesCtrDecryption128, RawBuffer, EVP_aes_128_ctr(), false);
DEFINE_CIPHER(AesCtrEncryption192, RawBuffer, EVP_aes_192_ctr(), true);
DEFINE_CIPHER(AesCtrDecryption192, RawBuffer, EVP_aes_192_ctr(), false);
DEFINE_CIPHER(AesCtrEncryption256, RawBuffer, EVP_aes_256_ctr(), true);
DEFINE_CIPHER(AesCtrDecryption256, RawBuffer, EVP_aes_256_ctr(), false);

DEFINE_CIPHER(AesCfbEncryption128, RawBuffer, EVP_aes_128_cfb(), true);
DEFINE_CIPHER(AesCfbDecryption128, RawBuffer, EVP_aes_128_cfb(), false);
DEFINE_CIPHER(AesCfbEncryption192, RawBuffer, EVP_aes_192_cfb(), true);
DEFINE_CIPHER(AesCfbDecryption192, RawBuffer, EVP_aes_192_cfb(), false);
DEFINE_CIPHER(AesCfbEncryption256, RawBuffer, EVP_aes_256_cfb(), true);
DEFINE_CIPHER(AesCfbDecryption256, RawBuffer, EVP_aes_256_cfb(), false);

#undef DEFINE_CIPHER

} // namespace Cipher
} // namespace SW
} // namespace Crypto
} // namespace CKM

