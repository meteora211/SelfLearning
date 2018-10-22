#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;
class Solution {
public:
    vector<vector<int>> threeSum_lowperformance(vector<int>& nums) {
        vector<vector<int>> res;
        if(nums.size() < 3)
            return res;
        int length = nums.size();
        std::sort(nums.begin(),nums.end());
        for(int i = 0; i < length; i++)
        {
            unordered_map<int, int> cmap;
            int target = -nums[i];
            for(int j = i + 1; j < length; j++)
            {
                
                if(cmap.find(nums[j]) != cmap.end())
                {
                    
                    res.push_back({nums[i], nums[j], nums[cmap[nums[j]]]});
                    while(j < length - 1 && nums[j] == nums[j + 1])
                    {
                        j++;
                    }
                }
                else
                {
                    cmap[target - nums[j]] = j;
                }
                
            }
            while(i < length - 1 && nums[i] == nums[i + 1])
            {
                i++;
            }
        }
        return res;
    }

    // Solution using 2 pointer without hashmap.
    vector<vector<int>> threeSum(vector<int>& nums) {
        vector<vector<int>> res;
        if(nums.size() < 3)
            return res;
        int length = nums.size();
        std::sort(nums.begin(),nums.end());
        for(int i = 0; i < length - 2; i++)
        {
            int begin = i + 1;
            int end = length - 1;
            int target = -nums[i];
            
            while(begin < end)
            {
                if(nums[begin] + nums[end] > target)
                {
                    end --;
                }
                else if(nums[begin] + nums[end] < target)
                {
                    begin ++;
                }
                else
                {
                    res.push_back({nums[i],nums[begin],nums[end]});
                    begin ++;
                    end --;
                    
                    while(nums[begin] == nums[begin -1] && begin < end) begin++;
                    while(nums[end] == nums[end + 1] && begin < end) end--;
                }
            }
           
            while(i < length - 1 && nums[i] == nums[i + 1])
            {
                i++;
            }
        }
        return res;
    }
};
