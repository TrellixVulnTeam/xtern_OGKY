/* Author: Junfeng Yang (junfeng@cs.columbia.edu) -*- Mode: C++ -*- */
// Refactored from Heming's Memoizer code

#ifndef __TERN_RECORDER_RUNTIME_H
#define __TERN_RECORDER_RUNTIME_H

#include <tr1/unordered_map>
#include "tern/runtime/runtime.h"
#include "tern/runtime/monitor.h"
#include "tern/runtime/record-scheduler.h"
#include <time.h>

namespace tern {

struct barrier_t {
  int count;    // barrier count
  int narrived; // number of threads arrived at the barrier
};
typedef std::tr1::unordered_map<pthread_barrier_t*, barrier_t> barrier_map;

typedef std::tr1::unordered_map<pthread_t, int> tid_map_t;
typedef std::tr1::unordered_map<void*, std::list<int> > waiting_tid_t;

template <typename _Scheduler>
struct RecorderRT: public Runtime, public _Scheduler {

  void progBegin(void);
  void progEnd(void);
  void threadBegin(void);
  void threadEnd(unsigned insid);
  void idle_sleep();

  // thread
  int pthreadCreate(unsigned insid, int &error, pthread_t *thread,  pthread_attr_t *attr,
                    void *(*thread_func)(void*), void *arg);
  int pthreadJoin(unsigned insid, int &error, pthread_t th, void **thread_return);

  // mutex
  int pthreadMutexInit(unsigned insid, int &error, pthread_mutex_t *mutex, const  pthread_mutexattr_t *mutexattr);
  int pthreadMutexDestroy(unsigned insid, int &error, pthread_mutex_t *mutex);
  int pthreadMutexLock(unsigned insid, int &error, pthread_mutex_t *mutex);
  int pthreadMutexTimedLock(unsigned insid, int &error, pthread_mutex_t *mutex,
                            const struct timespec *abstime);
  int pthreadMutexTryLock(unsigned insid, int &error, pthread_mutex_t *mutex);

  int pthreadMutexUnlock(unsigned insid, int &error, pthread_mutex_t *mutex);

  // cond var
  int pthreadCondWait(unsigned insid, int &error, pthread_cond_t *cv, pthread_mutex_t *mu);
  int pthreadCondTimedWait(unsigned insid, int &error, pthread_cond_t *cv,
                           pthread_mutex_t *mu, const struct timespec *abstime);
  int pthreadCondSignal(unsigned insid, int &error, pthread_cond_t *cv);
  int pthreadCondBroadcast(unsigned insid, int &error, pthread_cond_t *cv);

  // barrier
  int pthreadBarrierInit(unsigned insid, int &error, pthread_barrier_t *barrier, unsigned count);
  int pthreadBarrierWait(unsigned insid, int &error, pthread_barrier_t *barrier);
  int pthreadBarrierDestroy(unsigned insid, int &error, pthread_barrier_t *barrier);

  // semaphore
  int semWait(unsigned insid, int &error, sem_t *sem);
  int semTryWait(unsigned insid, int &error, sem_t *sem);
  int semTimedWait(unsigned insid, int &error, sem_t *sem, const struct timespec *abstime);
  int semPost(unsigned insid, int &error, sem_t *sem);

  void symbolic(unsigned insid, int &error, void *addr, int nbytes, const char *name);

  // socket & file
  int __socket(unsigned insid, int &error, int domain, int type, int protocol);
  int __listen(unsigned insid, int &error, int sockfd, int backlog);
  int __accept(unsigned insid, int &error, int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);
  int __accept4(unsigned insid, int &error, int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen, int flags);
  int __connect(unsigned insid, int &error, int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);
  //struct hostent *__gethostbyname(unsigned insid, int &error, const char *name);
  //struct hostent *__gethostbyaddr(unsigned insid, int &error, const void *addr, int len, int type);
  ssize_t __send(unsigned insid, int &error, int sockfd, const void *buf, size_t len, int flags);
  ssize_t __sendto(unsigned insid, int &error, int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
  ssize_t __sendmsg(unsigned insid, int &error, int sockfd, const struct msghdr *msg, int flags);
  ssize_t __recv(unsigned insid, int &error, int sockfd, void *buf, size_t len, int flags);
  ssize_t __recvfrom(unsigned insid, int &error, int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
  ssize_t __recvmsg(unsigned insid, int &error, int sockfd, struct msghdr *msg, int flags);
  int __shutdown(unsigned insid, int &error, int sockfd, int how);
  int __getpeername(unsigned insid, int &error, int sockfd, struct sockaddr *addr, socklen_t *addrlen);  
  int __getsockopt(unsigned insid, int &error, int sockfd, int level, int optname, void *optval, socklen_t *optlen);
  int __setsockopt(unsigned insid, int &error, int sockfd, int level, int optname, const void *optval, socklen_t optlen);
  int __close(unsigned insid, int &error, int fd);
  ssize_t __read(unsigned insid, int &error, int fd, void *buf, size_t count);
  ssize_t __write(unsigned insid, int &error, int fd, const void *buf, size_t count);
  int __select(unsigned insid, int &error, int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);  
  int __epoll_wait(unsigned insid, int &error, int epfd, struct epoll_event *events, int maxevents, int timeout);
  int __sigwait(unsigned insid, int &error, const sigset_t *set, int *sig); 
  char *__fgets(unsigned insid, int &error, char *s, int size, FILE *stream);
  pid_t __fork(unsigned insid, int &error);
  pid_t __wait(unsigned insid, int &error, int *status);
  time_t __time(unsigned ins, int &error, time_t *t);
  int __clock_getres(unsigned ins, int &error, clockid_t clk_id, struct timespec *res);
  int __clock_gettime(unsigned ins, int &error, clockid_t clk_id, struct timespec *tp);
  int __clock_settime(unsigned ins, int &error, clockid_t clk_id, const struct timespec *tp);
  int __gettimeofday(unsigned ins, int &error, struct timeval *tv, struct timezone *tz);
  int __settimeofday(unsigned ins, int &error, const struct timeval *tv, const struct timezone *tz);

  // sleep
  unsigned int sleep(unsigned insid, int &error, unsigned int seconds);
  int usleep(unsigned insid, int &error, useconds_t usec);
  int nanosleep(unsigned insid, int &error, const struct timespec *req, struct timespec *rem);
  int __pthread_rwlock_rdlock(unsigned ins, int &error, pthread_rwlock_t *rwlock);
  int ___pthread_rwlock_wrlock(unsigned ins, int &error, pthread_rwlock_t *rwlock);
  int ___pthread_rwlock_tryrdlock(unsigned ins, int &error, pthread_rwlock_t *rwlock);
  int ___pthread_rwlock_trywrlock(unsigned ins, int &error, pthread_rwlock_t *rwlock);
  int ___pthread_rwlock_unlock(unsigned ins, int &error, pthread_rwlock_t *rwlock);

  RecorderRT(): _Scheduler() {
    int ret;
    ret = sem_init(&thread_begin_sem, 0, 0);
    assert(!ret && "can't initialize semaphore!");
    ret = sem_init(&thread_begin_done_sem, 0, 0);
    assert(!ret && "can't initialize semaphore!");

    if (options::launch_monitor)
      monitor = new RuntimeMonitor();
    else
      monitor = NULL;
  }

  ~RecorderRT() {
    if (monitor)
    {
      delete monitor; 
      monitor = NULL;
    }
  }

protected:

  int wait(void *chan, unsigned timeout = Scheduler::FOREVER);
  void signal(void *chan, bool all=false);
  int absTimeToTurn(const struct timespec *abstime);
  int relTimeToTurn(const struct timespec *reltime);

  int pthreadMutexLockHelper(pthread_mutex_t *mutex, unsigned timeout = Scheduler::FOREVER);
  int pthreadRWLockHelper(pthread_rwlock_t *rwlock, unsigned timeout = Scheduler::FOREVER);
  
  /// for each pthread barrier, track the count of the number and number
  /// of threads arrived at the barrier
  barrier_map barriers;

  /// need these semaphores to assign tid deterministically; see comments
  /// for pthreadCreate() and threadBegin()
  sem_t thread_begin_sem;
  sem_t thread_begin_done_sem;

  RuntimeMonitor *monitor;
};
#if 0
struct RRuntime: public Runtime {

  void progBegin(void);
  void progEnd(void);
  void threadBegin(void);
  void threadEnd(unsigned insid);

  // thread
  int pthreadMutexInit(unsigned insid, int &error, pthread_mutex_t *mutex, const  pthread_mutexattr_t *mutexattr);
  int pthreadMutexDestroy(unsigned insid, int &error, pthread_mutex_t *mutex);
  int pthreadCreate(unsigned insid, int &error, pthread_t *thread,  pthread_attr_t *attr,
                    void *(*thread_func)(void*), void *arg);
  int pthreadJoin(unsigned insid, int &error, pthread_t th, void **thread_return);
  int pthreadCancel(unsigned insid, int &error, pthread_t th);

  // mutex
  int pthreadMutexLock(unsigned insid, int &error, pthread_mutex_t *mutex);
  int pthreadMutexTimedLock(unsigned insid, int &error, pthread_mutex_t *mutex,
                            const struct timespec *abstime);
  int pthreadMutexTryLock(unsigned insid, int &error, pthread_mutex_t *mutex);

  int pthreadMutexUnlock(unsigned insid, int &error, pthread_mutex_t *mutex);

  // cond var
  int pthreadCondWait(unsigned insid, int &error, pthread_cond_t *cv, pthread_mutex_t *mu);
  int pthreadCondTimedWait(unsigned insid, int &error, pthread_cond_t *cv,
                           pthread_mutex_t *mu, const struct timespec *abstime);
  int pthreadCondSignal(unsigned insid, int &error, pthread_cond_t *cv);
  int pthreadCondBroadcast(unsigned insid, int &error, pthread_cond_t *cv);

  // barrier
  int pthreadBarrierInit(unsigned insid, int &error, pthread_barrier_t *barrier, unsigned count);
  int pthreadBarrierWait(unsigned insid, int &error, pthread_barrier_t *barrier);
  int pthreadBarrierDestroy(unsigned insid, int &error, pthread_barrier_t *barrier);

  // semaphore
  int semWait(unsigned insid, int &error, sem_t *sem);
  int semTryWait(unsigned insid, int &error, sem_t *sem);
  int semTimedWait(unsigned insid, int &error, sem_t *sem, const struct timespec *abstime);
  int semPost(unsigned insid, int &error, sem_t *sem);

  void symbolic(unsigned insid, int &error, void *addr, int nbytes, const char *name);

  // socket & file
  int __socket(unsigned insid, int &error, int domain, int type, int protocol);
  int __listen(unsigned insid, int &error, int sockfd, int backlog);
  int __accept(unsigned insid, int &error, int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);
  int __accept4(unsigned insid, int &error, int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen, int flags);
  int __connect(unsigned insid, int &error, int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);
  //struct hostent *__gethostbyname(unsigned insid, int &error, const char *name);
  //struct hostent *__gethostbyaddr(unsigned insid, int &error, const void *addr, int len, int type);
  ssize_t __send(unsigned insid, int &error, int sockfd, const void *buf, size_t len, int flags);
  ssize_t __sendto(unsigned insid, int &error, int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
  ssize_t __sendmsg(unsigned insid, int &error, int sockfd, const struct msghdr *msg, int flags);
  ssize_t __recv(unsigned insid, int &error, int sockfd, void *buf, size_t len, int flags);
  ssize_t __recvfrom(unsigned insid, int &error, int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
  ssize_t __recvmsg(unsigned insid, int &error, int sockfd, struct msghdr *msg, int flags);
  int __shutdown(unsigned insid, int &error, int sockfd, int how);
  int __getpeername(unsigned insid, int &error, int sockfd, struct sockaddr *addr, socklen_t *addrlen);
  int __getsockopt(unsigned insid, int &error, int sockfd, int level, int optname, void *optval, socklen_t *optlen);
  int __setsockopt(unsigned insid, int &error, int sockfd, int level, int optname, const void *optval, socklen_t optlen);
  int __close(unsigned insid, int &error, int fd);
  ssize_t __read(unsigned insid, int &error, int fd, void *buf, size_t count);
  ssize_t __write(unsigned insid, int &error, int fd, const void *buf, size_t count);
  int __select(unsigned insid, int &error, int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);  

  
  RRuntime() {
    threadCount = 0;
    turnCount = 0;
    token = 0;
    pthread_mutex_init(&sched_mutex, NULL);
    pthread_mutex_init(&pthread_mutex, NULL);
    pthread_cond_init(&thread_create_cv, NULL);
    pthread_cond_init(&thread_begin_cv, NULL);

    for (int i = 0; i < MAX_THREAD_COUNT; ++i)
      pthread_cond_init(&thread_waitcv[i], NULL);
  }

  ~RRuntime() {
    pthread_mutex_destroy(&sched_mutex);
    pthread_mutex_destroy(&pthread_mutex);
    pthread_cond_destroy(&thread_create_cv);
    pthread_cond_destroy(&thread_begin_cv);
    for (int i = 0; i < MAX_THREAD_COUNT; ++i)
      pthread_cond_destroy(&thread_waitcv[i]);
  }

protected:
//====  scheduling algorithm
  union res_t {
    void *handle_arg;
    int int_arg;
  };

  union args_t {
    void *handle_arg;
    int int_arg;
    struct {
      void *mutex;
      void *cv;
    } mutex_cond_arg;
  };

  struct operation_t
  {
    int op;
    args_t args;
    res_t res;
  };

  enum THREAD_STATUS {
    RUNNING = 1,
    SCHEDULING = 2,
    CLOSED = 4,
    INVALID
  };

  static const int MAX_THREAD_COUNT = 100;

  void do_sched();
  int find_next_thread();
  void move_next();
  void block();
  void wakeup();

  int incThreadCount() 
  { 
    int ret = threadCount++; 
    tid_map[pthread_self()] = ret;
    return ret;
  }

  int incTurnCount() { return turnCount++; }
  void incToken()
  {
    ++token; 
    if (token == threadCount) 
      token = 0;
  }

  THREAD_STATUS thread_status[MAX_THREAD_COUNT];
  operation_t operation[MAX_THREAD_COUNT];

  pthread_mutex_t sched_mutex;
  pthread_cond_t thread_waitcv[MAX_THREAD_COUNT];
  pthread_cond_t thread_create_cv;
  pthread_cond_t thread_begin_cv;
  pthread_mutex_t pthread_mutex;

  typedef std::tr1::unordered_map<void*, bool> mutex_status_t;
  mutex_status_t mutex_status;

  int threadCount; 
  int turnCount;
  int token;
  int new_tid;

  static __thread int tid; 

  waiting_tid_t waiting_tid;
//====

//  void pthreadMutexLockHelper(pthread_mutex_t *mutex);

  /// for each pthread barrier, track the count of the number and number
  /// of threads arrived at the barrier
  barrier_map barriers;
  tid_map_t tid_map;
};
#endif
} // namespace tern

#endif
