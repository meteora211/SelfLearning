class Solution {
public:
    int findMaxConsecutiveOnes(vector<int>& nums) {
        int i = 0, j = 0;
        int len = 0;
        while(j < nums.size())
        {
            if(nums[j++] == 0)
            {
                len = max(len,j - i -1);
                i = j;
            }
        }
        return max(len,j - i );
    }
};
