#include <vector>
using std::vector;
class Solution {
public:
    int removeElement(vector<int>& nums, int val) {
        vector<int>::iterator iter = nums.begin();
        while(iter != nums.end())
        {
            if(*iter == val)
            {
                nums.erase(iter);
            }
            else{
                iter ++;
            }
            
        }
        return nums.size();
    }

    int removeElement2(vector<int>& nums, int val) {
        if(nums.empty()) return 0;
        int i = 0, j = 0;
        while(j < nums.size())
        {
            if(nums[j] != val) nums[i++] = nums[j];
            j++;
        }
        return i;
    }
};
