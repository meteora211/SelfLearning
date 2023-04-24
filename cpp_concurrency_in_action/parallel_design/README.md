### hardware_destructive_interference_size hardware_constructive_interference_size
ref: https://en.cppreference.com/w/cpp/thread/hardware_destructive_interference_size

std::hardware_destructive_interference_size 若两个对象的内存地址距离 大于 std::hardware_destructive_interference_size，则不会在一个cache line。

std::hardware_constructive_interference_size 若两个对象的内存地址距离 小于 std::hardware_destructive_interference_size，则会在一个cache line。
