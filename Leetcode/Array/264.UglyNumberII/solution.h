class Solution {
public:
    int nthUglyNumber(int n) {
        if(n <= 0)
            return 0;
        vector<int> unums = {1};
        int u2 = 0, u3 = 0, u5 = 0;
        for(int i = 0; i < n; i ++)
        {
            int u2v = unums[u2] * 2;
            int u3v = unums[u3] * 3;
            int u5v = unums[u5] * 5;
            int minimal = min(u2v,min(u3v,u5v));
            unums.push_back(minimal);
            if(u2v == minimal) u2++;
            if(u3v == minimal) u3++;
            if(u5v == minimal) u5++;
        }
        return unums[n-1];
    }
};
