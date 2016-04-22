#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include "Threads.h"

template <class T>
class Singleton {
private:
  static T     *_instance;
  static Mutex  _mutex;

public:

  Singleton( T *instance ) {
    _mutex.lock();
    _instance = instance;
    _mutex.unlock();
  }

  static T& get() {
    return *_instance;
  }
};

template <typename T> T * Singleton<T>::_instance = NULL;
template <typename T> Mutex Singleton<T>::_mutex;

#endif
