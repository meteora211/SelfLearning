#include <vector>
#include <algorithm>
#include <math.h>
using namespace std;

class Solution {
public:
    int threeSumClosest(vector<int>& nums, int target) {
        
        int res;
        if(nums.size() < 3)
        {
            for(auto& n: nums)
            {
                res += n;
            }
            return res;
        }
        res = nums[0] + nums[1] + nums[2];
        std::sort(nums.begin(),nums.end());
        for(int i = 0; i < nums.size() - 2; i ++)
        {
            int begin = i + 1;
            int end = nums.size() - 1;
            int tar = target - nums[i];
            while(begin < end)
            {
                int tsum = nums[begin] + nums[end];
                if( tsum > tar)
                {
                    res = abs(tsum - tar) > abs(res - target) ? res : tsum + nums[i];
                    end --;
                }
                else if( tsum < tar)
                {
                    res = abs(tsum - tar) > abs(res - target) ? res : tsum + nums[i];
                    begin ++;
                }
                else
                {
                    return target;
                }
            }
        }
        return res;
    }
};
