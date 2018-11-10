## 思路
刚开始以为这个题目在判断pivot左边与右边的和是否相等的时候，需要做两次循环遍历求和。这样遍历数组需要n次循环，每次循环又要求和再次遍历数组。算法复杂度为O(n<sup>2</sup>);
然后发现，每次循环内部的求和是没有必要的。假设第i次循环的左和为lsum<sub>i</sub>,右边和为rsum<sub>i</sub>。那么第i+1次循环
```
lsum<sub>i</sub> = lsum<sub>i+1</sub> + nums[i]; 
rsum<sub>i</sub> = rsum<sub>i+1</sub> - nums[i];
```
根据这个递推关系，可以吧复杂度降为O(n)
