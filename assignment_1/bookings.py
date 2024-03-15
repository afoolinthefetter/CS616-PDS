import threading
import csv
import sys
import time


# Lock for controlling access to the shared variable
lock = threading.Lock()

number_of_courts = 4
genders = ['M','F']
preferences = ['S','D','b','B']

preference_suitable_for_single = ['S','b','B']
preference_suitable_for_double = ['D','B','b']

male_single_time = 10
male_double_time = 15
female_single_time = 5
female_double_time = 10

output_file = 'output.csv'

def append_to_csv(data):
    with open(output_file, 'a', newline='') as csvfile:
        csv_writer = csv.writer(csvfile)
        csv_writer.writerow(data)

def initialize_csv(header):
    with open(output_file, 'w', newline='') as csvfile:
        csv_writer = csv.writer(csvfile)
        csv_writer.writerow(header)

header = ['Start-Time', 'End-Time', 'Players', 'Court-Number']
initialize_csv(header)

#preparing the available courts
court = {
    "court_number": None,
    "start_time": None,
    "end_time": None,
    "player_ids": [],
    "occupied": False
}

courts = [court.copy() for _ in range(number_of_courts)]
for i in range(number_of_courts):
    courts[i]["court_number"] = i + 1

def is_there_empty_court():
    global courts
    for court in courts:
        if court["occupied"] == False:
            return court["court_number"]
    return 0
        
def is_there_empty_court_for_me(current_time:int):
    for court in courts:
        if court["occupied"] == True and court["end_time"] < current_time - 1:
            return True
    return False


#getting the requests from the csv file with a function
request = {
    "player_id": None,
    "arrival_time": None,
    "gender": None,
    "preference": None
}
requests = []
waiting_list = []
matched_list = []

def read_csv(file_path):
    global requests
    with open(file_path, 'r') as csv_file:
        csv_reader = csv.reader(csv_file)
        next(csv_reader)  # Skip the header row

        # Appending data to requests array
        for row in csv_reader:
            player_id = int(row[0])
            arrival_time = int(row[1])
            gender = row[2]
            preference = row[3]
            if gender not in genders or preference not in preferences:
                continue
            request["player_id"] = player_id
            request["arrival_time"] = arrival_time
            request["gender"] = gender
            request["preference"] = preference
            requests.append(request.copy())

    return



def add_to_waiting_list(request: request):
    global waiting_list
    global matched_list
    waiting_list.append(request)
    allocate_court()
    #prune the waiting list
    waiting_list = [value for index, value in enumerate(waiting_list) if index not in matched_list]
    matched_list.clear()
    return

def mark_court_occupied(ids: list):
    global courts
    global waiting_list
    players_getting_court = []
    for i in ids:
        players_getting_court.append(waiting_list[i])
    arrival_times = [player['arrival_time'] for player in players_getting_court]
    
    # Find the best court
    max_arrival_time = max(arrival_times)
    min_time_difference = 99999
    court_number = 0
    for court in courts:
        if court["occupied"] == False:
            court_number = court["court_number"]
            break
        if court["occupied"] == True and court["end_time"] < max_arrival_time:
            time_difference = max_arrival_time - court["end_time"]
            if time_difference < min_time_difference:
                min_time_difference = time_difference
                court_number = court["court_number"]
    if court_number == 0:
        return False
    
    # Check count and unique genders
    count = len(arrival_times)
    unique_genders = set(player['gender'] for player in players_getting_court)
    player_ids = [player['player_id'] for player in players_getting_court]
    print(players_getting_court)
    if len(unique_genders) == 1:
        single_gender = unique_genders.pop()  # Retrieve the single gender
        if single_gender == 'M':
            if count == 2:
                duration = male_single_time
            duration = male_double_time
        else:
            if count == 2:
                duration = female_single_time
            duration = female_single_time
    else:
        if count == 2:
            duration = male_single_time
        duration = male_double_time

    for court in courts:
        if court["court_number"] == court_number:
            court["occupied"] = True
            court["start_time"] = max_arrival_time
            court["end_time"] = max_arrival_time + duration
            court["player_ids"] = player_ids
            break

    # Convert list to a comma-separated string
    player_ids_str = ",".join(map(str, player_ids))
    append_to_csv([max_arrival_time, max_arrival_time + duration, player_ids_str, court_number])
            
    return True

def match_for_single(player_id:int):
    global waiting_list
    global matched_list
    if len(waiting_list) < 2:
        return
    checked_list = []
    checked_list.append(player_id)
    for i, request in enumerate(waiting_list):
        if  i in checked_list or i in matched_list:
            continue
        if request['preference'] in preference_suitable_for_single:
            checked_list.append(i)
            #update the court
            marked = mark_court_occupied(checked_list)
            if marked:
                return checked_list
            return None

def match_for_double(player_id:int):
    global waiting_list
    global matched_list
    if len(waiting_list) < 4:
        return
    checked_list = []
    checked_list.append(player_id)
    for i, request in enumerate(waiting_list):
        if i in checked_list or i in matched_list:
            continue
        elif request['preference'] in preference_suitable_for_double:
            checked_list.append(i)
            if len(checked_list) == 4:
                marked = mark_court_occupied(checked_list)
                if marked:
                    return checked_list
                return None
            continue         

def allocate_court():
    global courts
    global waiting_list
    global matched_list
    #sort waitinglist by arrival time
    waiting_list = sorted(waiting_list, key=lambda k: k['arrival_time'])

    for i in range(len(waiting_list)):
        if i in matched_list:
            continue
        #can i obtain court for this request?
        if is_there_empty_court() == 0:
            can_i_get_empty_court = is_there_empty_court_for_me(waiting_list[i]['arrival_time'])
            if can_i_get_empty_court == False:
                continue
        preference = waiting_list[i]['preference']
        ids_to_remove = []

        if preference == 'S':
            ids_to_remove = match_for_single(i)
        elif preference == 'D':
            ids_to_remove = match_for_double(i)
        elif preference == 'b':
            ids_to_remove = match_for_single(i)
            if ids_to_remove == None:
                ids_to_remove = match_for_double(i)
        else:
            ids_to_remove = match_for_double(i)
            if ids_to_remove == None:
                ids_to_remove = match_for_single(i)

        #ERROR this was messing up the waiting list
        #remove the ids to remove from waiting list
        # if ids_to_remove != None:
        #     waiting_list = [value for index, value in enumerate(waiting_list) if index not in ids_to_remove]
        #     if(len(waiting_list)==0):
        #         return
                
        #INSTEAD
        if ids_to_remove != None:
            matched_list = matched_list + ids_to_remove
    return
            
   
def process_element(thread_index, array):
    for i in range(thread_index, len(array), 2):
        with lock:
            request = array[i]
            add_to_waiting_list(request)
        

def main():
    if len(sys.argv) != 2:
        print("Usage: python script.py <file_name.csv>")
        sys.exit(1)

    csv_file_path = sys.argv[1]
    # csv_file_path = '/Users/amangupta/Desktop/CS616-PDS/assignment_1/input.csv'
    read_csv(csv_file_path)

    # Create and start eight threads
    threads = []
    for i in range(2):
        thread = threading.Thread(target=process_element, args=(i, requests))
        threads.append(thread)
        thread.start()

    # Wait for all threads to finish
    for thread in threads:
        thread.join()

    # for request in requests:
    #     add_to_waiting_list(request)


if __name__ == "__main__":
    main()