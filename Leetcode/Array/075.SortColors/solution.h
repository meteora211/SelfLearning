#include <vector>
using namespace std;
class Solution {
public:
    void sortColors(vector<int>& nums) {
        vector<int> colormap(3,0);
        for(int i = 0; i != nums.size(); i ++)
        {
            colormap[nums[i]] ++; 
        }
        int k = 0;
        for(int i =0; i !=colormap.size(); i++)
        {
            while(colormap[i]--)
            {
                nums[k++] = i;
            }
        }
    }
};
