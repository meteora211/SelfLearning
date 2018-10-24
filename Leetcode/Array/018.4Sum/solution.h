class Solution {
public:
    vector<vector<int>> fourSum(vector<int>& nums, int target) {
        vector<vector<int>> res;
        if(nums.size() < 4)
            return res;
        std::sort(nums.begin(), nums.end());
        for(int i = 0; i < nums.size() - 3; i ++)
        {
            for(int j = i + 1; j < nums.size() - 2; j++)
            {
                int ctar = target - nums[i] - nums[j];        
                int begin = j + 1;
                int end = nums.size() - 1;
                while(begin < end)
                {
                    if(nums[begin] + nums[end] > ctar)
                    {
                        end --;
                    }
                    else if(nums[begin] + nums[end] < ctar)
                    {
                        begin ++;
                    }
                    else
                    {
                        res.push_back({nums[i],nums[j],nums[begin],nums[end]});
                        begin ++;
                        end --;
                        while(nums[begin] == nums[begin-1]) begin++;
                        while(nums[end] == nums[end+1]) end--;
                    }
                }
                while(nums[j] == nums[j+1]) j++;
            }
            while(nums[i] == nums[i+1]) i++;
        }
        return res;
    }
};
