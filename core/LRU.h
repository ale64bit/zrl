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
