#include <algorithm>
#include <vector>

using namespace std;

struct Interval
{
	int start;
    int end;
    Interval() : start(0), end(0) {}
    Interval(int a, int b) : start(a), end(b) {}
};

class Solution {
public:
    vector<Interval> merge(vector<Interval>& intervals) {
        vector <Interval> res;
        if(intervals.empty())
        {
            return intervals;
        }
        std::sort(intervals.begin(),intervals.end(),[](Interval a, Interval b){return a.start < b.start;});
        int index = 1;
        Interval single = intervals[0];
        while(index != intervals.size() )
        {
            if(single.end < intervals[index].start)
            {
                res.push_back(single);
                single.start = intervals[index].start;
                single.end = intervals[index].end;
            }
            else
            {
                single.end = max(single.end, intervals[index].end);
                single.start = min(single.start, intervals[index].start);
            }
            index ++;
        }
        res.push_back(single);
        return res;
    }
};
