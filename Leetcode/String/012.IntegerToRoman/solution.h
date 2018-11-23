class Solution {
public:
    string intToRoman(int num) {
        string res;
        if(num < 1) return res;
        vector<char> sym = {'I','V','X','L','C', 'D',  'M'};
        vector<int>  val = { 1,  5,  10, 50, 100, 500, 1000};
        int index = 6;
        while(index >= 0)
        {
            int tempval = val[index];
            if(num >= tempval)
            {
                int numbers = num / tempval;
                num = num % tempval;
                if(numbers == 4)
                {
                    res = res + sym[index] + sym[index + 1];
                    index --;
                }
                else
                {
                    if(index > 0 && num / val[index - 1] == 4)
                    {
                        res = res + sym[index - 1] + sym[index + 1];
                        num = num % val[index - 1];
                        index -= 2;
                    }
                    else
                    {
                        res += string(numbers, sym[index]);
                    }
                }
                
            }
            else
            {
                index--;
            }
        }
        return res;
    }
};
