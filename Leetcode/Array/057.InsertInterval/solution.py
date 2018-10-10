# Definition for an interval.
# class Interval(object):
#     def __init__(self, s=0, e=0):
#         self.start = s
#         self.end = e

class Solution(object):
    def insert(self, intervals, newInterval):
        """
        :type intervals: List[Interval]
        :type newInterval: Interval
        :rtype: List[Interval]
        """
        res = []
        length = len(intervals)
        if( length == 0):
            res.append(newInterval)
            return res
        index = 0
        while( index < length and intervals[index].end < newInterval.start):
            res.append(intervals[index])
            index += 1
        while( index < length and intervals[index].start <= newInterval.end):
            newInterval.end = max(newInterval.end, intervals[index].end)
            newInterval.start = min(newInterval.start, intervals[index].start)
            index += 1
        res.append(newInterval)
        while(index < len(intervals)):
            res.append(intervals[index])
            index += 1
        return res
