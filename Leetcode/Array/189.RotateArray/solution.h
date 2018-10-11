class Solution {
public:
    void rotate(vector<int>& nums, int k) {
        for(int i = 0; i != k; i ++)
        {
            vector<int>::iterator it = nums.end()-1;
            nums.erase(nums.end()-1);
            nums.insert(nums.begin(),*it);
        }
    }
    void rotate2(vector<int>& nums, int k) {
        vector<int> copy = nums;
        int n = nums.size();
        for (int i = 0; i < n; i ++)
        {
            nums[(i + k)%n] = copy[i];
        }
    }
   void rotate3(vector<int>& nums, int k) {
        k = k%nums.size();
        reverse(nums.end() - k, nums.end());
        reverse(nums.begin(), nums.end() - k);
	    reverse(nums.begin(), nums.end());
    } 
};
