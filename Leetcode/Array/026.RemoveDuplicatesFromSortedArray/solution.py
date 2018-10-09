import numpy.testing as npt

class Solution(object):
    def removeDuplicates(self, nums):
        """ 
        :type nums: List[int]
        :rtype: int 
        """ 
        pos = 0;
        if nums==[]:
            return 0;
        for i in range(1,len(nums)):
            if nums[pos] != nums[i]:
                pos += 1;
                nums[pos] = nums[i];
        return pos+1;

L1 = [1,1,2];
L2 = [1,1,2,2,2,3,3,3,4,4,4,4,5];
s = Solution();
res1 = s.removeDuplicates(L1);
res2 = s.removeDuplicates(L2);
npt.assert_equal(L1[:res1],[1,2]);
npt.assert_equal(L2[:res2],[1,2,3,4,5]);
print('test finished')
