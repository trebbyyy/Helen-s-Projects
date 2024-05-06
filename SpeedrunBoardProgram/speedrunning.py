# Name: Helen Yang
# Student Number: 23072751

class Leaderboard:
    """A leaderboard of speedrunning record times.

    Each entry has a time in seconds and a runner name.
    Runners may submit multiple runs.
    The leaderboard is ranked fastest run first.
    Ties receive the same rank as each other, so for example if runners submit
    the times 10, 20, 20, and 30, they will have the ranks 1, 2, 2, and 4.
    """

    def __init__(self, runs=[]):
        self.runs = runs 
        self.insert_sort()
      
    def insert_sort(self):
        """Uses insertion sort to sort a list of (time, name) pairs

        Leaderboard is given with (time, name) pairs in any order. 
        It is then sorted.

        Returns:
          The current leaderboard as a list of (time, name) pairs.
        """
        for i in range(1, len(self.runs)): 
            previous = self.runs[i]
            j = i - 1
            while j >= 0 and (self.runs[j][0] > previous[0]) or (self.runs[j][0] == previous[0] and self.runs[j][1] > previous[1]): #if value is greater than the previous value or if it is the same and is alphabetically lower
                self.runs[j + 1] = self.runs[j] #swap spot on leaderboard
                self.runs[j] = previous
                j -= 1
        return
    
    def get_runs(self):
        """Returns the current leaderboard.

        Leaderboard is given in rank order, tie-broken by runner name.

        Returns:
            The current leaderboard as a list of (time, name) pairs.
        """
        # self.insert_sort()
        return self.runs

    def submit_run(self, time, name):
        """Adds the given run to the leaderboard

        Args:
            time: The run time in seconds.
            name: The runner's name.
        """
        self.runs.append((time,name))
        self.insert_sort()

    def get_rank_time(self, rank):
        """Get the time required to achieve at least a given rank.

        For example, `get_rank_time(5)` will give the maximum possible time
        that would be ranked fifth.

        Args:
            rank: The rank to look up.

        Returns:
            The time required to place `rank`th or better.
        """
        
        calculatedRank = 1 
        index = 0 #index starts off at 0

        # self.insert_sort()
        for i in range (0, len(self.runs)-1): 
            index +=1  #keep track of what index we are at
            if i+1 == rank:
                return self.runs[i][0]
            else:
                calculatedRank = index + 1                        
        if rank == calculatedRank: #if the given rank is the same as the rank on the current index, returns the time for that rank required
            return self.runs[index][0]
        elif rank > calculatedRank:
            return None
        else: #or if the rank is not the same as the current index, decrements index and finds the next shortest time
            for i in range (index, 0, -1):
                if self.runs[i][0] != self.runs[i-1][0]:
                    return self.runs[i-1][0]


    def get_possible_rank(self, time):
        """Determine what rank the run would get if it was submitted.

        Does not actually submit the run.

        Args:
            time: The run time in seconds.

        Returns:
            The rank this run would be if it were to be submitted.
        """  

        virtualBoard = Leaderboard()
        virtualBoard.runs = [tuple(entry) for entry in self.runs]
        virtualBoard.submit_run(time, "Name")
        # virtualBoard.insert_sort()

        calculatedRank = 1
        for i in range (0, len(virtualBoard.runs) -1):     
            if (virtualBoard.runs[i][0] != virtualBoard.runs[i+1][0]) or ((virtualBoard.runs[i-1][0] != virtualBoard.runs[i][0]) and virtualBoard.runs[i][0] == virtualBoard.runs[i+1][0]) : 
                calculatedRank = i + 1             #for the above line: if the current n and n+ 1 run not the same, or if the n - 1 run and n run aren't the same and the n run and n + 1 run are equal, current rank = current index + 1 
            if virtualBoard.runs[i][0] == time:
                return calculatedRank
            elif virtualBoard.runs[len(virtualBoard.runs)-1][0] == time:
                calculatedRank = i
                return (len(virtualBoard.runs)-1)
            
        return calculatedRank
        
    def count_time(self, time):
        """Count the number of runs with the given time.

        Args:
            time: The run time to count, in seconds.

        Returns:
            The number of submitted runs with that time.
        """

        # self.insert_sort()
        low = 0
        hi = len(self.runs) - 1
        mid = 0

        count = 0
        found = False

        while low <= hi: #binary search algorithm
            mid = (low + hi)//2
            if self.runs[mid][0] < time:
                low = mid + 1 
            elif self.runs[mid][0] > time:
                hi = mid - 1
            elif self.runs[mid][0] == time:
                found = True
                break
        else:
            count = 0
        
        index = 0
        if found == True:
            for i in range(mid, -1, -1):
                if self.runs[i][0] == time:
                    index = i
                else: break
            for i in range(index, len(self.runs), +1):
                if self.runs[i][0] == time:
                    count+=1
                else: break
        return count
