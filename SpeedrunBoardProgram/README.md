
Keeping track of a speedrunning leaderboard, ranked by fastest run using [standard competition ranking]

## functionalities.
1. Get the current leaderboard (in rank order) at any time
2. Submit a new run with a given time and runner name to the leaderboard
3. Construct a new instance of the class given an initial list of runs
4. Find the run time required to achieve a given rank
5. Find what rank a particular time would achieve if submitted
6. Find how many runs are tied for a given time

# Create a new leaderboard
leaderboard = Leaderboard()

# Submit new runs
leaderboard.submit_run(100, "Alice")
leaderboard.submit_run(120, "Bob")
leaderboard.submit_run(110, "Charlie")

# Get the current leaderboard
current_leaderboard = leaderboard.get_runs()
print("Current Leaderboard:", current_leaderboard)

# Get the rank time for rank 2
rank_time = leaderboard.get_rank_time(2)
print("Rank 2 Time:", rank_time)

# Get the possible rank for a run with time 115
possible_rank = leaderboard.get_possible_rank(115)
print("Possible Rank for 115s:", possible_rank)

# Count the number of runs with time 110
run_count = leaderboard.count_time(110)
print("Number of runs with time 110s:", run_count)
    