### lost wake-up and spurious wake-up

#### lost wake up

See the example in `LostWakeUp`.
- lost wake-up
  - thread blocks forever though it should not
  - bug, solved by condition variable(with predicate)

> A call to pthread_cond_signal() or pthread_cond_broadcast() when the thread does not hold the mutex lock associated with the condition can lead to lost wake-up bugs.

> A lost wake-up occurs when all of the following conditions are in effect:

> - A thread calls pthread_cond_signal() or pthread_cond_broadcast()

>  - Another thread is between the test of the condition and the call to pthread_cond_wait()

>  - No threads are waiting

>  - The signal has no effect, and therefore is lost

ref:

https://docs.oracle.com/cd/E19120-01/open.solaris/816-5137/sync-30/index.html

https://www.modernescpp.com/index.php/c-core-guidelines-be-aware-of-the-traps-of-condition-variables

#### spurious wake up

- spurious wake-up
  - thread wakes up though it should not
  - Unavailable problem, solved by while loop.

ref:

https://en.wikipedia.org/wiki/Spurious_wakeup

https://www.cnblogs.com/tqyysm/articles/9765667.html


### useful link
- [future](https://juejin.cn/post/7076786451316211748)
