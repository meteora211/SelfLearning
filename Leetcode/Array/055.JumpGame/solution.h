#include <algorithm>
#include <vector>
using namespace std;

class Solution {
public:
    bool canJump(vector<int>& nums) {
        int maxReached = 0;
        if(nums.size() <= 1)
        {
            return true;
        }
        for (int i = 0; i != nums.size(); i++)
        {
            if (i <= maxReached)
            {
                maxReached = max(i + nums[i],maxReached);
            }
            else
            {
                return false;
            }
        }
        return true;
    }
};
