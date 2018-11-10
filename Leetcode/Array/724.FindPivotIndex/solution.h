class Solution {
public:
    int pivotIndex(vector<int>& nums) {
        if(nums.size() < 1) return -1;
        int sum = 0;
        for(auto&n : nums)
            sum += n;
        int leftsum = 0;
        int rightsum = sum ;
        for(int i = 0; i < nums.size(); i ++)
        {
            if(i >= 1) leftsum += nums[i-1];
            rightsum -= nums[i];
            if(leftsum == rightsum) return i;
        }
        return -1;
    }
};
