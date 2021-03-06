/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file       exception.cpp
 * @author     Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version    1.0
 */
#include <exception.h>

#include <dpl/log/log.h>

namespace CKM {
namespace Exc {

PrintError::PrintError(
       const std::string &path,
       const std::string &function,
       int line,
       int error,
       const std::string &message)
{
    LogErrorPosition(message << " (Error: " << error << ")", path.c_str(), line, function.c_str());
}

PrintDebug::PrintDebug(
       const std::string &path,
       const std::string &function,
       int line,
       int error,
       const std::string &message)
{
    LogDebugPosition(message << " (Error: " << error << ")", path.c_str(), line, function.c_str());
}

} // namespace Exc
} // namespace CKM

