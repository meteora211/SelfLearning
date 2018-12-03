class Solution_Wrong {
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

class Solution {
public:
    string multiply(string num1, string num2) {
        if(num1.empty() || num2.empty()) return "";
        string res(num1.size()+num2.size(),'0');
        for(int i = num1.size() - 1; 0 <= i; i --)
        {
            int carry = 0;
            for(int j = num2.size() - 1; 0 <= j; j --)
            {
                int mult = carry + (res[i + j + 1] - '0') + (num1[i] - '0') * (num2[j] - '0');
                carry = mult / 10;
                res[i + j + 1] = mult%10 + '0'; 
            }
            res[i] += carry;
        }
        
        size_t pos = res.find_first_not_of('0');
        if(pos == string::npos) return "0";
        return res.substr(pos);
    }
};
