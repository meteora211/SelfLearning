###

some notes:

对于 std::shared_mutex，通常在读线程中用 std::shared_lock 管理，在写线程中用 std::unique_lock 管理


some link:

[stack on process and thread](https://zhuanlan.zhihu.com/p/344350873)

[process and thread](https://zhuanlan.zhihu.com/p/258049386)

[shared_mutex](https://juejin.cn/post/7066602715782381576)

