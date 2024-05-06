## Overview

- This program can read the data from a csv (comma separated values) file provided to you and return different analytical results of the COVID-19 cases for entire world.
- The function returns two dictionaries: **dict_country** and **dict_continent**.
- **dict_country**: Contains the following data for each country:
    - Total number of recorded positive cases of COVID-19 for each month of the year.
    - Total number of recorded deaths due to COVID-19 for each month of the year.
    - Total number of days for each month of the year when the recorded positive cases of COVID-19 were greater than the average for that month.
    - Total number of days for each month of the year when the recorded deaths due to COVID-19 were greater than the average for that month.
- **dict_continent**: Contains similar data for each continent.

## How To Use

- Open an run file in an IDE with a Python interpreter such as Thonny
- Call the function with the CSV file name as the argument. Example:
    - dict_country, dict_continent = main('Covid-data-for-project-2-sample.csv')

## Output

- The output returned are dictionaries where some data is presented below:

>>> dict_country['afghanistan']
[[1963, 1, 174, 1952, 13081, 16020, 1681, 1494, 1109, 2157, 4849, 5252], [86, 0, 4, 60, 194, 482, 188, 119, 57, 78, 257, 396], [1, 1, 7, 11, 13, 16, 8, 11, 10, 14, 15, 18], [3, 0, 4, 12, 11, 13, 8, 15, 14, 15, 15, 16]]

>>> dict_country['italy']
[[169327, 1126, 104664, 99671, 27534, 7729, 6959, 21677, 45647, 364569, 922124, 505612], [4596, 29, 12399, 15539, 5448, 1383, 374, 342, 411, 2724, 16958, 18583], [8, 8, 17, 15, 11, 15, 14, 12, 15, 11, 15, 15], [6, 6, 14, 17, 12, 16, 13, 4, 15, 12, 15, 15]]

>>> dict_continent.keys()
dict_keys(['asia', 'europe', 'africa', 'north america', 'south america', 'oceania', ''])

>>> dict_continent['oceania']
[[198, 16, 4534, 2207, 436, 718, 9360, 8539, 1277, 499, 317, 513], [0, 0, 18, 75, 10, 2, 97, 456, 231, 19, 1, 1], [10, 5, 10, 11, 13, 9, 14, 13, 13, 13, 10, 12], [0, 0, 12, 14, 10, 2, 11, 14, 10, 10, 1, 1]]

