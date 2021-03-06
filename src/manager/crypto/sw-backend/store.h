/*
 *  Copyright (c) 2000 - 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 */
/*
 * @file       store.h
 * @author     Bartłomiej Grzelewski (b.grzelewski@samsung.com)
 * @version    1.0
 */
#pragma once

#include <generic-backend/gobj.h>
#include <generic-backend/gstore.h>

namespace CKM {
namespace Crypto {
namespace SW {

class Store : public GStore {
public:
    explicit Store(CryptoBackend backendId);

    virtual GObjUPtr getObject(const Token &, const Password &);
    virtual TokenPair generateAKey(const CryptoAlgorithm &, const Password &, const Password &);
    virtual Token generateSKey(const CryptoAlgorithm &, const Password &);
    virtual Token import(const Data &data, const Password &);
    virtual Token importEncrypted(const Data &, const Password &, const DataEncryption &);
    virtual void destroy(const Token &) {}

private:
    Crypto::GObjShPtr m_deviceKey;
};

} // namespace SW
} // namespace Crypto
} // namespace CKM
