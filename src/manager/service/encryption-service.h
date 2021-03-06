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
 * @file       encryption-service.h
 * @author     Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version    1.0
 */

#pragma once

#include <message-service.h>
#include <noncopyable.h>
#include <iencryption-service.h>
#include <encryption-logic.h>
#include <service-messages.h>

namespace CKM {

class EncryptionService : public ThreadMessageService<MsgKeyResponse>, public IEncryptionService {
public:
    EncryptionService();
    virtual ~EncryptionService();
    NONCOPYABLE(EncryptionService);

    // from ThreadService
    ServiceDescriptionVector GetServiceDescription();

    // Custom add custom support for ReadEvent and SecurityEvent
    // because we want to bypass security check in EncryptionService
    virtual void Event(const ReadEvent &event)
    {
        CreateEvent([this, event]() { this->CustomHandle(event); });
    }

    virtual void Event(const SecurityEvent &event)
    {
        CreateEvent([this, event]() { this->CustomHandle(event); });
    }

    void Start();
    void Stop();

protected:
    // CustomHandle is used to bypass security check
    void CustomHandle(const ReadEvent &event);
    void CustomHandle(const SecurityEvent &event);

private:
    virtual void SetCommManager(CommMgr *manager);

    bool ProcessOne(const ConnectionID &conn, ConnectionInfo &info, bool allowed);
    void ProcessMessage(MsgKeyResponse msg);
    void ProcessEncryption(const ConnectionID &conn,
                           const Credentials &cred,
                           MessageBuffer &buffer);

    // from IEncryptionService
    virtual void RespondToClient(const CryptoRequest& request,
                                 int retCode,
                                 const RawBuffer& data = RawBuffer());
    virtual void RequestKey(const CryptoRequest& request);

    EncryptionLogic m_logic;
};

} /* namespace CKM */
