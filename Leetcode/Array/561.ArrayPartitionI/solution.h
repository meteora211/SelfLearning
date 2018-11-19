class Solution {
public:
    int arrayPairSum(vector<int>& nums) {
        if(nums.empty()) return 0;
        sort(nums.begin(),nums.end());
        int sums = 0;
        for(int i = 0; i < nums.size(); i += 2)
        {
            sums += nums[i];
        }
        return sums;
    }
};`
