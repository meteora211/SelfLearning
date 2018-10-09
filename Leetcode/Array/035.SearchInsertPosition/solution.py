import numpy.testing as npt
class Solution(object):
    def searchInsert(self, nums, target):
        """
        :type nums: List[int]
        :type target: int
        :rtype: int
        """
        if nums==[]:
            return 1;
        for key,value in enumerate(nums):
            if target <= value:
                return key
        return len(nums)

s = Solution();
target = 7;
nums = [1,3,5,6];
npt.assert_equal(s.searchInsert(nums,target),4);
print('test finished')
