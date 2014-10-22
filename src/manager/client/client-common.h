/*
 *  Copyright (c) 2000 - 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Contact: Bumjin Im <bj.im@samsung.com>
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
 * @file        client-common.h
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief       This file constains implementation of common types
 *              used in Central Key Manager.
 */

#ifndef _KEY_MANAGER_CLIENT_
#define _KEY_MANAGER_CLIENT_

#include <vector>
#include <functional>

#include <noncopyable.h>
#include <ckm/ckm-type.h>
#include <message-buffer.h>

#define KEY_MANAGER_API __attribute__((visibility("default")))

extern "C" {
    struct msghdr;
}

namespace CKM {

class AliasSupport
{
    public:
        AliasSupport(const Alias &alias);

        const std::string & getLabel() const;
        const Alias & getAlias() const;
        bool isLabelEmpty() const;

        static Alias merge(const std::string &label, const std::string &alias);
    private:
        std::string m_alias;
        std::string m_label;
};

int connectSocket(int& sock, char const * const interface);

int sendToServer(char const * const interface, const RawBuffer &send, MessageBuffer &recv);


class SockRAII {
public:
    SockRAII()
      : m_sock(-1)
    {}

    NONCOPYABLE(SockRAII);

    virtual ~SockRAII() {
        if (m_sock > -1)
            close(m_sock);
    }

    int Connect(char const * const interface) {
        return CKM::connectSocket(m_sock, interface);
    }

    int Get() const {
        return m_sock;
    }

private:
    int m_sock;
};

/*
 * Decorator function that performs frequently repeated exception handling in
 * SS client API functions. Accepts lambda expression as an argument.
 */
int try_catch(const std::function<int()>& func);

void try_catch_async(const std::function<void()>& func, const std::function<void(int)>& error);

} // namespace CKM

#endif // _KEY_MANAGER_CLIENT_
