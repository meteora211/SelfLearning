class Solution(object):
    def canJump(self, nums):
        """
        :type nums: List[int]
        :rtype: bool
        """
        if len(nums) <= 1:
            return True
        maxstep = len(nums) - 1;
        reaches = 0
        for k,v in enumerate(nums):
            reaches = max(reaches, k + v)
            if(reaches < k + 1):
                return False
            if(reaches >= maxstep):
                return True
