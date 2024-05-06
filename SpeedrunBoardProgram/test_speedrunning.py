#! /usr/bin/env python3

import unittest

import speedrunning
from speedrunning import *

import inspect
import warnings
from random import Random

SEED = 2200
RUNS = 100
SIZE = 500
RANGE = 100


class TestSpeedrunning(unittest.TestCase):
    def setUp(self):
        source = inspect.getsource(speedrunning)
        redflags = [
            ('import', 'It looks like you are using imports, which is not allowed!'),
            ('.sort(', 'It looks like you are using a builtin sorting method, which is not allowed!'),
            ('sorted(', 'It looks like you are using a builtin sorting method, which is not allowed!'),
        ]
        for redflag, warning in redflags:
            if redflag in source:
                warnings.warn(warning)

    def test_leaderboard_example(self):
        board = Leaderboard([
            (887, 'Arok'),
            (913, 'Msushi'),
            (876, 'Shizzal'),
            (903, 'helyonnnnnn'),
            (903, 'Dinoz'),
            (903, 'Hrvatskii'),
        ])
        runs = board.get_runs()
        self.assertEqual(runs, sorted(runs))
        self.assertEqual(board.get_rank_time(1), 876)
        self.assertEqual(board.get_rank_time(3), 903)
        self.assertEqual(board.get_rank_time(5), 903)
        self.assertEqual(board.get_rank_time(6), 913)
        self.assertEqual(board.get_possible_rank(869), 1)
        board.submit_run(869, 'SiDiouS')
        self.assertEqual(board.get_possible_rank(910), 7)
        board.submit_run(910, 'alatreph')
        self.assertEqual(board.get_possible_rank(910), 7)
        board.submit_run(910, 'RealCreative')
        self.assertEqual(board.get_possible_rank(890), 4)
        board.submit_run(890, 'Floorb')
        runs = board.get_runs()
        self.assertEqual(runs, sorted(runs))
        self.assertEqual(board.count_time(903), 3)
        self.assertEqual(board.count_time(910), 2)
        self.assertEqual(board.count_time(876), 1)
        self.assertEqual(board.count_time(900), 0)

    @staticmethod
    def __random_time(random):
        return random.randint(0, RANGE)

    @staticmethod
    def __random_name(random):
        cs = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789'
        return ''.join(random.choices(cs, k=8))

    @staticmethod
    def __random_run(random):
        time = TestSpeedrunning.__random_time(random)
        name = TestSpeedrunning.__random_name(random)
        return (time, name)

    def test_get_runs(self):
        random = Random(SEED)
        for i in range(RUNS):
            size = random.randint(0, SIZE)
            runs = [self.__random_run(random) for _ in range(size)]
            board = Leaderboard(runs.copy())
            expected = sorted(runs)
            received = board.get_runs()
            self.assertEqual(received, expected)

    def test_submit_run(self):
        random = Random(SEED)
        for i in range(RUNS):
            size = random.randint(0, SIZE)
            runs = [self.__random_run(random) for _ in range(size)]
            board = Leaderboard(runs.copy())
            for _ in range(size):
                run = self.__random_run(random)
                (time, name) = run
                board.submit_run(time, name)
                runs.append(run)
            expected = sorted(runs)
            received = board.get_runs()
            self.assertEqual(received, expected)

    def test_get_rank_time(self):
        random = Random(SEED)
        for i in range(RUNS):
            size = random.randint(0, SIZE)
            runs = [self.__random_run(random) for _ in range(size)]
            board = Leaderboard(runs.copy())
            rank = random.randint(1, len(runs))
            time = board.get_rank_time(rank)
            nlt = sum(t < time for t, _ in runs)
            msg = f'Wanted rank {rank} but there are {nlt} runs faster than {time}'
            self.assertGreater(rank, nlt, msg=msg)
            nle = sum(t <= time for t, _ in runs)
            msg = f'Wanted rank {rank} but there are {nle} runs no slower than {time}'
            self.assertLessEqual(rank, nle, msg=msg)

    def test_get_possible_rank(self):
        random = Random(SEED)
        for i in range(RUNS):
            size = random.randint(0, SIZE)
            runs = [self.__random_run(random) for _ in range(size)]
            board = Leaderboard(runs.copy())
            time = self.__random_time(random)
            rank = board.get_possible_rank(time)
            nlt = sum(t < time for t, _ in runs)
            msg = f'get_possible_rank() gave {rank} but there are {nlt} runs faster than {time}'
            self.assertEqual(rank, nlt+1, msg=msg)

    def test_count_time(self):
        random = Random(SEED)
        for i in range(RUNS):
            size = random.randint(0, SIZE)
            runs = [self.__random_run(random) for _ in range(size)]
            board = Leaderboard(runs.copy())
            time = self.__random_time(random)
            count = board.count_time(time)
            neq = sum(t == time for t, _ in runs)
            self.assertEqual(count, neq)


if __name__ == '__main__':
    unittest.main()
