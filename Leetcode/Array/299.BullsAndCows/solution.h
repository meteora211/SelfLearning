class Solution {
public:
    string getHint(string secret, string guess) { //Wrong Answer
        unordered_map<char, bool> hash_map;
        int A_num = 0;
        int B_num = 0;
        for (int i = 0; i < secret.length(); i++)
        {
            if(hash_map.find(secret[i]) == hash_map.end())
            {
                hash_map[secret[i]] = true;
            }
        }
        for (int j = 0; j < guess.length(); j ++)
        {
            if (guess[j] == secret[j])
            {
                A_num ++;
            }
            else
            {
                unordered_map<char, bool>::iterator it = hash_map.find(guess[j]);
                if (it != hash_map.end())
                {
                    if(it->second)
                    {
                        B_num ++;
                        it->second = false;
                    }
                }
            }
        }
        return to_string(A_num) + "A" + to_string(B_num) + "B";
    }
};
