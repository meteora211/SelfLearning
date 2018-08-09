#include <vector>
using namespace std;
class Solution {
public:
    void moveZeroes(vector<int>& nums) {
        auto k = 0;
        for(auto element : nums)
        {
            if(element)
                nums[k++] = element;
        }
        for(auto i = k; i < nums.size(); i ++)
        {
            nums[i] = 0;
        }
    }
};
