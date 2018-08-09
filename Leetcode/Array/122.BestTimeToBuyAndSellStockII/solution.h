#include <vector>
using std::vector;

class Solution {
public:
    int maxProfit(vector<int>& prices) {
        int sum = 0;
        if(prices.size()==0)
        {
            return sum;
        }
        for ( int i = 0; i != prices.size()-1; i++)
        {
            int diff = prices[i+1] - prices[i];
            if (diff>0)
            {
                sum += diff;
            }
        }
        return sum;
    }
};
