#include <vector>
#include <unordered_map>
using std::vector;
using std::unordered_map;

class Solution {
public:
    bool containsDuplicate(vector<int>& nums) {
        unordered_map<int,int> hashmap;
        for (int i = 0; i != nums.size(); i ++)
        {
            if (hashmap.find(nums[i])!=hashmap.end())
            {
                return true;
            }
            else
            {
                hashmap[nums[i]] = i;
            }
        }
        return false;
    }
};
