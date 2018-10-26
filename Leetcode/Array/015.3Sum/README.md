# 3Sum

## 思路
主要思路和2sum类似，构建一个hash_map.嵌套循环找到所有的解
## 问题
- 由于数组内可能出现重复的数字，比如[0,0,0,0,0],这会导致结果出现重复答案[[0,0,0],[0,0,0]];
  解决方法: 先对数组进行排序，每次找到解后都判断下一个数值是否与当前解有相同的值，若是则跳过。
- 复杂度O(n<sup>2</sup>),但是性能太差，耗时690ms
## 解决
- 先排序解决重复解问题
- 使用两个数组指针避免hashmap的使用:
  - 在第i次循环中，定义首指针和末指针分别指向nums[i]和nums[nums.size()]
  - 将两个指针中的数值相加,由于数组已经排序，如果其和大于-nums[i]则相应减小末指针指向的值，即向左移动末指针。否则向右移动首指针。
## 其他
- vector<vector<int>> res 在push_back 中初始化存入的vector<int>类型的值:
  ```c++
  res.push_back({nums[i],nums[begin],nums[end]});
  ```