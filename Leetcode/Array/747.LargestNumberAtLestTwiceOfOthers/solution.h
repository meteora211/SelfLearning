class Solution {
public:
    int dominantIndex(vector<int>& nums) {
        if(nums.size() < 1) return 0;
        int largest = 0;
        int second = 0;
        int index = 0;
        for(int i = 0; i < nums.size(); i ++)
        {
            if(nums[i] >= largest)
            {
                second = largest;
                largest = nums[i];
                index = i;
            }
            else if(nums[i] > second) second = nums[i];
        }
        return largest >= (second << 1) ?  index :  -1;
    }
};
