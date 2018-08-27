#include <string>
#include <vector>
using namespace std;
class Solution {
public:
    bool isValid(string s) {
        vector<char> storage;
        for(char &c : s)
        {
            if(c == '(' || c == '[' || c == '{')
            {
                storage.push_back(c);
                continue;
            }
            if(((c == ')') && (storage.back() == '(')) || ((c == '[') && (storage.back() == ']')) || ((c == '{') && (storage.back() == '}')))
            {
                if (storage.empty())
                    return false;
                else
                    storage.pop_back();
            }
            else
            {
                return false;
            }
        }
        return storage.empty();
    }
};
