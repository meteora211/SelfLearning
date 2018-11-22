##思路
简单直接: 
* 初始化一个头结点作为将要返回的结果。
* 假设cur为构建的结果链表的索引，curlist为当前第i个链表的索引
* 判断cur->next是否为空，是则将curlist作为cur的下一个节点
* 否则进一步判断cur->next->val与curlist->val的大小
  * 若curlist大，则将cur->next置为curlist，然后两者均向后移
  * 否则将cur向后移一位

但是这种方法效率很低，只击败了10%的人 :(
