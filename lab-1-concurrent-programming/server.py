import socket
from _thread import *
import threading
import time
from datetime import datetime, timedelta
import sys, errno  
import csv 

NUMBER_OF_CLASSROOMS = 5
LENGTH_OF_CLASS_IN_HOURS = 1.5
STARTING_H_M = [8, 0]
ENDING_H_M = [20, 0]

#define return codes
OK = 0
ALREADY_BOOKED = -1
CANCEL_WITHIN_COOL_DOWN = -2
INVALID_REQUEST = -3


#define request types
BOOK = 'BOOK'
CANCEL = 'CANCEL'
GET = 'GET'


class MyScheduler:
    def __init__(self):
        self.log = []

    def log_operation(self, requestCount, operation_type, room, timeslot, status, message):
        log_entry = [requestCount, operation_type, room, timeslot, status, message]
        self.log.append(log_entry)
        return

    def write_log_to_csv(self, filename):
        with open(filename, mode='w', newline='') as file:
            writer = csv.writer(file)
            # Write the header
            writer.writerow(['Request Time', 'Type', 'Room', 'Timeslot', 'Status'])
            # Write the log entries
            writer.writerows(self.log)


class ClassroomBooking:
    def __init__(self, numberOfClasses, starting_h_m, ending_h_m , classDuration, scheduler:MyScheduler):
        self.scheduler = scheduler
        self.requestCount = 0
        self.classes = numberOfClasses
        self.startTime = starting_h_m
        self.endTime = ending_h_m
        self.classDuration = classDuration
        self.classrooms = {}
        self.lock = threading.Lock()
        self.startTime = datetime.now().replace(hour=STARTING_H_M[0], minute=STARTING_H_M[1])
        self.endTime = datetime.now().replace(hour=ENDING_H_M[0], minute=ENDING_H_M[1])
        self.time_list = []
        current_time = self.startTime
        while current_time < self.endTime:
            self.time_list.append(current_time.strftime('%H:%M'))
            current_time += timedelta(hours=self.classDuration)
        self.time_list = self.time_list[:-1]

    def initialize_classroom(self, classroom_id):
        print("I am here without a lock\n\n")
        apple = self.lock.acquire()
        print("this is apple", apple)
        try:
            print("I have a lock to play with\n\n")
            if classroom_id not in self.classrooms:
                self.classrooms[classroom_id] = {i: None for i in self.time_list}
                print(self.classrooms[2])

        finally:
            self.lock.release()
            print("I am done with the lock\n\n")    
    def book_classroom(self, classroom_id, start_time) -> int:
        print("Here 2\n\n")
        with self.lock:
            self.requestCount += 1
            end_time = self.get_end_time(start_time)
            timeslot = f"{start_time}-{end_time}"

            if classroom_id not in self.classrooms:
                # self.scheduler.log_operation(self.requestCount,BOOK, classroom_id, timeslot, INVALID_REQUEST,'')
                self.initialize_classroom(classroom_id)
                print("Initilizing \n\n")

            if start_time not in self.classrooms[classroom_id]:
                # self.scheduler.log_operation(self.requestCount,BOOK, classroom_id, timeslot, INVALID_REQUEST,'')
                print("Start time not in classroom\n\n")
                return -3
                
            if self.classrooms[classroom_id][start_time] != None:
                # self.scheduler.log_operation(self.requestCount,BOOK, classroom_id, timeslot, ALREADY_BOOKED,'')
                print("Already booked\n\n")
                return -1
            
            #if no problem, then book the class as follows
            self.classrooms[classroom_id][start_time] = self.requestCount
            self.scheduler.log_operation(self.requestCount,BOOK, classroom_id, timeslot, OK,'')
            print(self.classrooms)
            return 0
        
    def cancel_classroom(self, classroom_id, start_time) -> int:
        with self.lock:
            self.requestCount += 1
            end_time = self.get_end_time(start_time)
            timeslot = f"{start_time}-{end_time}"
            if classroom_id not in self.classrooms:
                self.scheduler.log_operation(self.requestCount,CANCEL, classroom_id, timeslot, INVALID_REQUEST,'')
                return -3

            if start_time not in self.classrooms[classroom_id]:
                self.scheduler.log_operation(self.requestCount,CANCEL, classroom_id, timeslot, INVALID_REQUEST,'')
                return -3

            if self.classrooms[classroom_id][start_time] is None:
                self.scheduler.log_operation(self.requestCount,CANCEL, classroom_id, timeslot, INVALID_REQUEST,'')
                return -3

            #if no problem, then cancel the class as follows
            #check if twenty requests have been made since the booking
            if self.classrooms[classroom_id][start_time] != None:
                if (self.classrooms[classroom_id][start_time] + 20) >= self.requestCount:
                    self.scheduler.log_operation(self.requestCount,CANCEL, classroom_id, timeslot, CANCEL_WITHIN_COOL_DOWN,'')
                    return -2
                
                self.classrooms[classroom_id][start_time] = None
                self.scheduler.log_operation(self.requestCount,CANCEL, classroom_id, timeslot, OK,'')
                return 0

            return -1
        
    def status_classroom(self):
        with self.lock:
            self.requestCount += 1
            booked_classrooms = [(classroom_id, timeslot) for classroom_id, schedule in self.classrooms.items() for timeslot, booking_time in schedule.items() if booking_time is not None]
            sorted_booked_classrooms = sorted(booked_classrooms, key=lambda x: booked_classrooms.count(x), reverse=True)
            self.scheduler.log_operation(self.requestCount,GET,'' ,'' , OK,sorted_booked_classrooms)
            return sorted_booked_classrooms

    def format_booked_classrooms(self, booked_classrooms):
        formatted_result = [f"('{classroom}', '{timeslot}-{self.calculate_end_time(timeslot)}')" for classroom, timeslot in booked_classrooms]
        return "{" + ', '.join(formatted_result) + "}"

    def calculate_end_time(self, start_time):
        start_time_obj = time.strptime(start_time, '%H:%M')
        end_time_obj = time.gmtime(time.mktime(start_time_obj) + 5400)  # 5400 seconds = 1.5 hours
        return time.strftime('%H:%M', end_time_obj)
    
    def get_end_time(self, start_time):
        time_object = datetime.strptime(start_time, '%H:%M')
        time_object += timedelta(hours=self.classDuration)
        return time_object.strftime('%H:%M')


            


def threaded(c, addr, classroom_booking: ClassroomBooking, ):
     try:  
        data = c.recv(1024)
        request = data.decode('utf-8').split(',')
        print(request)
        requestLabel = request[1]
        if requestLabel == 'BOOK':
            print("Here\n\n")
            start_time = request[3].split('-')[0]
            classroom_id = int(request[2])
            message = classroom_booking.book_classroom(classroom_id, start_time)
        if requestLabel == 'CANCEL':
            start_time = request[3].split('-')[0]
            classroom_id = int(request[2])
            message = classroom_booking.cancel_classroom(classroom_id, start_time)
        if requestLabel == 'GET':
            message = classroom_booking.status_classroom()
     except IOError as e:  
        if e.errno == errno.EPIPE:  
            c.close()
            print("Pipe Error Address:", addr)
            return

     print(message)
     try:  
         c.send(message)
     except IOError as e:  
        if e.errno == errno.EPIPE:  
            c.close()
            print("Pipe Error Address:", addr)
            return

    # connection closed
     c.close()
     print("Connection corresponding to address closed:", addr)


def Main():
    host = ""
    port = 12345
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((host, port))
    print("socket bound to port", port)

    #initialize the classroom booking system
    scheduler = MyScheduler()
    classrooms = ClassroomBooking(NUMBER_OF_CLASSROOMS, STARTING_H_M, ENDING_H_M, LENGTH_OF_CLASS_IN_HOURS, scheduler)
        
    s.listen(5)
    print("socket is listening")
    while True:
        c, addr = s.accept()
 
        # lock acquired by client
        #print_lock.acquire()
        print('Connected to :', addr[0], ':', addr[1])
        
        
        
        # Start a new thread and return its identifier
        start_new_thread(threaded, (c,addr,classrooms,))

    s.close()


if __name__ == '__main__':
    Main()