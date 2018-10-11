#include <vector>
#include <algorithm>
using namespace std;
class Solution {
public:
    int maxArea(vector<int>& height) {
        int i = 0;
        int j = height.size() - 1;
        int water = 0;
        while(i < j)
        {
            int h = min(height[i], height[j]);
            water = max( h * (j - i), water);
            while(i < j && height[i] <= h) i++;
            while(i < j && height[j] <= h) j--;
        }
        return water;
    }
};
