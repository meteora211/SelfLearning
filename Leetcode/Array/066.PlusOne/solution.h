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

    vector<int> plusOne2(vector<int>& digits) {
        if (digits.empty()) return digits;
        int size = digits.size();
        int extra = 1;
        for(int i = size - 1; i >= 0; i--)
        {
            digits[i] = digits[i] + extra;
            extra = digits[i] / 10;
            digits[i] = digits[i] % 10;
        }
        if(extra)
        {
            digits.insert(digits.begin(),extra);
        }
        return digits;
    }
};
