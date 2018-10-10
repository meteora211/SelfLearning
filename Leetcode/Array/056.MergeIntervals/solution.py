# Definition for an interval.
class Interval(object):
    def __init__(self, s=0, e=0):
        self.start = s
        self.end = e

class Solution(object):
    def merge(self, intervals):
        """
        :type intervals: List[Interval]
        :rtype: List[Interval]
        """
        res = []
        intervals.sort(key=lambda x: x.start)
        if(len(intervals) == 0):
            return intervals
        single = intervals[0];
        index = 1
        while(index < len(intervals)):
            if(single.end < intervals[index].start):
                res.append(single)
                single = intervals[index]
            else:
                single.end = max(single.end, intervals[index].end)
            index += 1
        res.append(single)
        return res

s = Solution()
res = s.merge([Interval(1,4),Interval(3,5),Interval(6,9),Interval(10,13)])
print(res)
