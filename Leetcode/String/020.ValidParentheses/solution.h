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
            if(storage.empty())
                return false;
            
            if(((c == ')') && (storage.back() == '(')) || ((c == ']') && (storage.back() == '[')) || ((c == '}') && (storage.back() == '{')))
            {
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
