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
};
