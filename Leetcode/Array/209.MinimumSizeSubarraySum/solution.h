class Solution {
public:
    int minSubArrayLen(int s, vector<int>& nums) {
        int res = 0;
        if(nums.empty()) return res;
        int i = 0, j = 0, sum = 0;
        while(j < nums.size())
        {
            
            if(nums[j] < s - sum)
            {
                sum += nums[j++];
            }
            else
            {
                res == 0? res = j - i + 1: res = min(j - i + 1, res);
                if(nums[j] > s)
                {
                    return 1;
                }
                else
                {
                    sum -= nums[i++];
                }
            }
        }
        
        return res;
    }
};
