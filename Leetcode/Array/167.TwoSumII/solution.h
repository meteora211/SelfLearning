class Solution {
public:
    vector<int> twoSum(vector<int>& numbers, int target) {
        if(numbers.empty()) return numbers;
        int i = 0, j = numbers.size() - 1;
        vector<int> res;
        while(i < j)
        {
            int tmp = numbers[i] + numbers[j];
            if(tmp == target)
            {
                res.push_back(i + 1);
                res.push_back(j + 1);
                break;
            }
            tmp > target? j-- : i++;
        }
        return res;
    }
};
