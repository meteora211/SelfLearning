class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        int pos = 0;
        int count = 0;
        if(nums.size() <= 1)
        {
            return nums.size();
        }
        for (int i = 1; i != nums.size(); i ++)
        {
            if(nums[i-1] == nums[i])
            {
                count ++;
                if (count < 2)
                {
                    nums[++pos] = nums[i];
                }
            }
            else
            {
                nums[++pos] = nums[i];
                count = 0;
            }
            
        }
        return pos + 1;
    }
};
