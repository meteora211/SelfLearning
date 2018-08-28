class Solution {
public:
    int searchInsert(vector<int>& nums, int target) {
        int res = 0;
        if(nums.empty())
            return 0;
        while( res != nums.size() && target > nums[res] )
        {
            res++;
        }
        return res;
    }
};
