/*!
 *  Copyright (c) 2015 by Contributors
 * \file thread_local.h
 * \brief Common utility for thread local storage.
 */
#ifndef MXNET_COMMON_THREAD_LOCAL_H_
#define MXNET_COMMON_THREAD_LOCAL_H_

#include <mutex>
#include <memory>
#include <vector>

namespace mxnet {
namespace common {

// macro hanlding for threadlocal variables
#ifdef __GNUC__
  #define MX_THREAD_LOCAL __thread
#elif __STDC_VERSION__ >= 201112L
  #define  MX_THREAD_LOCAL _Thread_local
#elif defined(_MSC_VER)
  #define MX_THREAD_LOCAL __declspec(thread)
#endif

#ifndef MX_THREAD_LOCAL
#message("Warning: Threadlocal is not enabled");
#endif

/*!
 * \brief A threadlocal store to store threadlocal variables.
 *  Will return a thread local singleton of type T
 * \tparam T the type we like to store
 */
template<typename T>
class ThreadLocalStore {
 public:
  /*! \return get a thread local singleton */
  static T* Get() {
    static MX_THREAD_LOCAL T* ptr = nullptr;
    if (ptr == nullptr) {
      ptr = new T();
      Singleton()->RegisterDelete(ptr);
    }
    return ptr;
  }

 private:
  /*! \brief constructor */
  ThreadLocalStore() {}
  /*! \brief destructor */
  ~ThreadLocalStore() {
    for (size_t i = 0; i < data_.size(); ++i) {
      delete data_[i];
    }
  }
  /*! \return singleton of the store */
  static ThreadLocalStore<T> *Singleton() {
    static ThreadLocalStore<T> inst;
    return &inst;
  }
  /*!
   * \brief register str for internal deletion
   * \param str the string pointer
   */
  void RegisterDelete(T *str) {
    std::unique_lock<std::mutex> lock(mutex_);
    data_.push_back(str);
    lock.unlock();
  }
  /*! \brief internal mutex */
  std::mutex mutex_;
  /*!\brief internal data */
  std::vector<T*> data_;
};
}  // namespace common
}  // namespace mxnet
#endif  // MXNET_COMMON_THREAD_LOCAL_H_
