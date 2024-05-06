#23072751
#CITS1401 Project2
#Semester 2, 2021
#Yang, Helen

def main(csvfile):
    count = {}          #dictionary to store the averages, number of days for each country/month of covid cases/deaths for countries
    count2 = {}         #dictionary to store the averages for each country/month of covid/cases/deaths for continents
    dict_country = {}   #dictionary storing the covid cases/deaths/days above average for countries
    dict_continent = {} #dictonary storing the covid cases/deaths/days above average for continents
    
    #0'th index for each list to average the total cases for each continent
    #1st index used as the total days in each month to calculate the average 
    mon_iter = [[0,31],[1,29],[2,31],[3,30],[4,31],[5,30],[6,31],[7,31],[8,30],[9,31],[10,30],[11,31]] 
    
    try: #error exception for a csvfile not found 
        open_file = open(csvfile, "r")

    except FileNotFoundError:
        print("[Errno 2] No such file or directory:", "'",csvfile,"'")
        return None, None
    
    header = open_file.readline() #to find if the requried columns are in the csvfile
    header = header.split(",")
    header = [i.strip() for i in header]
    header = [i.lower() for i in header]
 
    try:
         #finding the index number for each required column
        country = header.index("location")
        case_num = header.index("new_cases")
        death_num = header.index("new_deaths")
        date = header.index("date")
        continent = header.index("continent")
        
    except ValueError:                                #if the index number is not present, the program exits gracefully
        print("One or more required columns is missing from file.")
        return None, None
        
    for line in open_file: 
        line_split = line.split(",")                         #each line is split by ","
        date_split = line_split[date].split("/")             #the date is split by "/"
        
        """if date is not in correct dd/mm/yy format, the line is header row (identified by presence of 'iso_code" column
        the program will skip the line and start the loop from the beginning"""
        if line_split[case_num] == "new_cases" or len(date_split) != 3 or len(date_split[0]) > 2 or len(date_split[1]) > 2: 
            continue
        
        if line_split[death_num].isdigit() == False: #changes death count and case number cells in excel file to 0 if data is negative, absent or a string. 
            line_split[death_num] = 0
        if line_split[case_num].isdigit() == False:
            line_split[case_num] = 0
        if date_split[1] == "02" and int(date_split[2])%4 == 0:  #changes no. days in feb to 29 if the year is a leap year
            mon_iter[1][1] = 29
        
        
        continent = line_split[header.index("continent")].lower() #lowers the continent index in each line and saves it as a variable
        continent = continent.strip()
        country = line_split[header.index("location")].lower()    #lowers the country index in each line and saves it as a variable
        country = country.strip()
        month = int(date_split[1]) - 1          #converts month index from each date into an index from a string
        cases = int(line_split[case_num])       #converts case number index from each line into an integer from a string
        deaths = int(line_split[death_num])     #convers death number indexs from each line into an integer from a string

        if country in dict_country:              #if country on the line exists in the dictionary already:           
            count[country][0][month] = count[country][0][month] + 1    #adds 1 to the count for the days of the month for the country for that line to count dictionary
            dict_country[country][0][month] = dict_country[country][0][month] + cases  #adds to the cases for the month for the country for that line to country dictionary
            dict_country[country][1][month] = dict_country[country][1][month] + deaths  #same here but for death numbers
            
        else:                                    #if the country on the line does not exist in the dict_country dictionary:
            dict_country[country] = [[0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0]]   #dictionary is created for country
            count[country] = [[0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0]]      #count dictionary created for country
            dict_country[country][0][month] = cases   #adds first of the cases for the month for the country for that line to dict_country
            dict_country[country][1][month] = deaths  #same for deaths
            count[country][0][month] = 1   #adds 1 to the count for the days of that month for that country
            
        if continent in dict_continent:  #if continent is in dictionary then case/death numbers for that line added to correct dictionary
            dict_continent[continent][0][month] = dict_continent[continent][0][month] + cases         
            dict_continent[continent][1][month] = dict_continent[continent][1][month] + deaths
            
        else: #creates lists for continent and continent count for new continents
            dict_continent[continent] = [[0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0]]
            count2[continent] = [[0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0]]
            dict_continent[continent][0][month] = cases
            dict_continent[continent][1][month] = deaths
  
    for i in list(dict_country.keys()):        #calculates averages for each month of each country and puts into count dictionary
        for x in range(0,12):
            if count[i][0][x] > 0:
                average = dict_country[i][0][x] / count[i][0][x]
                average2 = dict_country[i][1][x] / count[i][0][x]
                count[i][0][x] = round(average,4)
                count[i][2][x] = round(average2,4)
    
    for continent in list(dict_continent.keys()):    #calculates averages for each month of each country and puts into count2 dictionary
        for x,y in mon_iter:
            count2[continent][0][x] = round(dict_continent[continent][0][x]/y,2)
            count2[continent][1][x] = round(dict_continent[continent][1][x]/y,2)
                
    """code below reads through every line in the csvfile again to count the number of days with death/case number counts over the average
    for each month for each country/continent to create lists 3 and 4"""
    
    open_file = open(csvfile, "r")       
    for line in open_file:
        line_split = line.split(",")     
        date_split = line_split[header.index("date")].split("/")
        if line_split[case_num] == "new_cases" or len(date_split) != 3 or len(date_split[0]) > 2 or len(date_split[1]) > 2: 
            continue
        if line_split[death_num].isdigit() == False:
            line_split[death_num] = 0
        if line_split[case_num].isdigit() == False:
            line_split[case_num] = 0

        country = line_split[header.index("location")].lower()
        country = country.strip()
        continent = line_split[header.index("continent")].lower()
        continent = continent.strip()
        month = int(date_split[1]) - 1
        cases = int(line_split[case_num])
        deaths = int(line_split[death_num])
        
        if cases > count[country][0][month]: 
            count[country][1][month] = count[country][1][month] + 1
            dict_country[country][2][month] = count[country][1][month]
            
        if deaths > count[country][2][month]:
            count[country][3][month] = count[country][3][month] + 1
            dict_country[country][3][month] = count[country][3][month]
            
        if cases > count2[continent][0][month]:
            dict_continent[continent][2][month] = dict_continent[continent][2][month] + 1
            
        if deaths > count2[continent][1][month]:
            dict_continent[continent][3][month] = dict_continent[continent][3][month] + 1
            
    return dict_country, dict_continent

