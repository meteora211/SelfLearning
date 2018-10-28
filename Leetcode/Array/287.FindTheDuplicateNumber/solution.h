class Solution {
public:
    int findDuplicate(vector<int>& nums) {
        int res = 0;
        for(int i = 0; i < nums.size(); i ++)
        {
            if(nums[i] > nums.size())
            {
                return nums[i];
            }
            while(i < nums.size() && nums[i] != i+1)
            {
                if(nums[nums[i] - 1] == nums[i])
                    return nums[i];
                else
                    swap(nums[i],nums[nums[i] - 1]);
            }            
        }
        return res;
    }
};
