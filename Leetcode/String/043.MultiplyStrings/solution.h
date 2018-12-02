class Solution {
public:
    string multiply(string num1, string num2) {
        if(num1.empty() || num2.empty()) return "";
        long long numa = stringtonum(num1);
        long long numb = stringtonum(num2);
        return to_string(numa * numb);
    }
private:
    long long stringtonum(string input)
    {
        long long num = 0;
        for(auto &a : input)
        {
            num *= 10;
            num += a - '0';
        }
        return num;
    }

};
