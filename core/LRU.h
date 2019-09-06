/*
 * Copyright 2019 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ZRL_CORE_LRU_H_
#define ZRL_CORE_LRU_H_

#include <list>
#include <unordered_map>

namespace zrl {

template <class T> class LRU {
public:
  void Push(const T &t) {
    const auto it = m_.find(t);
    if (it != m_.cend()) {
      l_.erase(it->second);
    }
    l_.push_front(t);
    m_[t] = l_.cbegin();
  }

  T Pop() {
    auto t = l_.back();
    l_.pop_back();
    m_.erase(t);
    return t;
  }

private:
  std::unordered_map<T, typename std::list<T>::const_iterator> m_;
  std::list<T> l_;
};

} // namespace zrl

#endif // ZRL_CORE_LRU_H_
