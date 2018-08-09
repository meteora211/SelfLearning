# include<vector>
using namespace std;
class Solution {
public:
    vector<int> plusOne(vector<int>& digits) {
        auto i = digits.size() - 1;
        int plus = 1;
        while(plus)
        {
            if(digits[i] == 9)
            {
                digits[i] = 0;
                if (i == 0)
                {
                    digits.push_back(0);
                }
                else
                {
                    i--;
                }
            }
            else
            {
                digits[i] += plus;
                plus = 0;
            }
        }
        return digits;
    }
};
