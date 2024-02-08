#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
// #include <omp.h>

#define NUMBER_OF_COURTS 4
#define GENDER_COUNT 2
#define PREFERENCE_COUNT 4
#define NUMBER_OF_REQUESTS 300

char genders[GENDER_COUNT] = {'M', 'F'};
char preferences[PREFERENCE_COUNT] = {'S', 'D', 'b', 'B'};
char game_types[] = {'S', 'D'};
char preference_suitable_for_single[] = {'S', 'b', 'B'};
char preference_suitable_for_double[] = {'D', 'B', 'b'};
int iterations_for_match = 3;

int male_single_time = 10;
int male_double_time = 15;
int female_single_time = 5;
int female_double_time = 10;


typedef struct {
    int player_id;
    int arrival_time;
    char gender;
    char preference;
} Request;

typedef struct {
    int court_number;
    int start_time;
    int end_time;
    int* player_ids;
    int occupied;
} Court;

//struct for matched data
typedef struct {
    int court_number;
    int start_time;
    int end_time;
    int* player_ids;
    int number_of_players;
    char game_type;
}Match;

//initialize the list needed for below
Court courts[NUMBER_OF_COURTS];
Match matched_list[NUMBER_OF_REQUESTS];
Request waiting_list[NUMBER_OF_REQUESTS];
Request checked_list[NUMBER_OF_REQUESTS];
Request temp_checked_list[NUMBER_OF_REQUESTS];


//indecies for the list st we can loop through
int waiting_list_index = 0;
int matched_list_index = 0;
int checked_list_index = 0;
int temp_checked_list_index = 0;

//initialize the courts
void initialize_courts() {
    for (int i = 0; i < NUMBER_OF_COURTS; ++i) {
        courts[i].court_number = i + 1;
        courts[i].start_time = 0;
        courts[i].end_time = 0;
        courts[i].player_ids = NULL;
        courts[i].occupied = 0;
    }
}

//if there is an empty court, return the court number, else return 0
int is_there_empty_court(){
    for (int i = 0; i < NUMBER_OF_COURTS; ++i) {
        if(courts[i].occupied == 0){
            return courts[i].court_number;
        }
    }
    return 0;
}

//returns court number if the player can get court, else returns 0
//dont use the court number here, just check if >0 or ==0
int empty_court_ask(int current_time){
    if (is_there_empty_court() != 0){
        return is_there_empty_court();
    }
    else{
        for (int i = 0; i < NUMBER_OF_COURTS; ++i) {
            if(courts[i].occupied == 1 && courts[i].end_time <= current_time){
                return courts[i].court_number;
            }
        }
    }
    return 0;
}

int append_to_waiting_list(Request request){
    waiting_list[waiting_list_index] = request;
    waiting_list_index++;
    make_match();
    return 1;
    //this function will be responsible for a level of matching to be performed on the waiting list before returning 
    //implement a function to do eveything before return
}

//check if an index is in the checked list
int is_in_checked_list(int player_id){
    for (int i = 0; i < checked_list_index; ++i) {
        if(checked_list[i].player_id == player_id){
            return 1;
        }
    }
    return 0;
}


int compareArrivalTime(const void* a, const void* b) {
        return ((Request*)a)->arrival_time - ((Request*)b)->arrival_time;
    }

void make_match(){
    //check for possible matches and append the match list
    qsort(waiting_list, waiting_list_index, sizeof(Request), 
          (int (*)(const void*, const void*))compareArrivalTime);
    // Function to compare arrival times for qsort
    

    for(int i = 0; i < waiting_list_index; i++){
        if (is_in_checked_list(waiting_list[i].player_id) == 1){
            continue;
        }
        
        // if (check_empty_court(waiting_list[i].arrival_time) == 0){
        //     continue;
        // }
        printf("matching for player %d\n", waiting_list[i].player_id);

        switch (waiting_list[i].preference)
        {
        case 'S':
            match_for_single(waiting_list[i]);
            break;
        case 'D':
            match_for_double(waiting_list[i]);
            break;
        case 'b':
            match_for_single(waiting_list[i]);
            if (temp_checked_list_index == 0)
                match_for_double(waiting_list[i]);
            break;
        default:
            match_for_double(waiting_list[i]);
            if (temp_checked_list_index == 0)
                match_for_single(waiting_list[i]);
            break;
        }

        if (temp_checked_list_index > 0){
        printf("there has been some match");
            for (int j = 0; j < temp_checked_list_index; ++j) {
                checked_list[checked_list_index] = temp_checked_list[j];
                checked_list_index++;
            }
            temp_checked_list_index = 0;
        }
    }
    return;
}

void match_for_single(Request request){
    int check;
    int paired = 0;
    if (waiting_list_index < 2){
        return;
    }
    temp_checked_list[temp_checked_list_index] = request;
    temp_checked_list_index++;
    for (int i = 0; i < waiting_list_index; ++i) {
        check = is_in_checked_list(waiting_list[i].player_id);
        if (check != 1){
            continue;
        }
        char preference = waiting_list[i].preference;
        for(int j=0; j < iterations_for_match; j++){
            if (preference == preference_suitable_for_single[j]){
                printf("found a match");
                temp_checked_list[temp_checked_list_index] = waiting_list[i];
                temp_checked_list_index++;
                paired = 1;
                break;
            }
        }
        if (paired == 1){
            break;
        }
    }
    //to clarify that there was no match for the caller function
    if (paired == 0){
        temp_checked_list_index = 0;
    }
    //else thing to enter the matched into the matched_list

    return;
}

int is_in_temp_checked_list(int player_id){
    for (int i = 0; i < temp_checked_list_index; ++i) {
        if(temp_checked_list[i].player_id == player_id){
            return 1;
        }
    }
    return 0;
}

void match_for_double(Request request){
    int check1, check2;
    int paired = 0;
    if (waiting_list_index < 4){
        printf("waiting list is less than 4\n");
        return;
    }
    temp_checked_list[temp_checked_list_index] = request;
    temp_checked_list_index++;
    for (int i = 0; i < waiting_list_index; ++i) {
        printf("checking for player %d\n", waiting_list[i].player_id);
        check1 = is_in_checked_list(waiting_list[i].player_id);
        check2 = is_in_temp_checked_list(waiting_list[i].player_id);
        if (check1 != 0 || check2 != 0){
            printf("player already checked");
            continue;
        }
        char preference = waiting_list[i].preference;
        for(int j=0; j < iterations_for_match; j++){
            if (preference == preference_suitable_for_double[j]){
                temp_checked_list[temp_checked_list_index] = waiting_list[i];
                temp_checked_list_index++;
                break;
            }
        }
        if (temp_checked_list_index == 4){
            paired = 1;
            break;
        }
    }
    if (paired == 0){
        temp_checked_list_index = 0;
        return;
    }

    //here will be the else condition to enter the players into a matched column
    //also a determining factor to occupy the court
    add_to_matched_list();
    return;    
}

void add_to_matched_list(){

    int late_arriving_player = 0;
    int late_arriving_player_index = 0;
    for(int i = 0; i < temp_checked_list_index; i++){
        if (temp_checked_list[i].arrival_time > late_arriving_player){
            late_arriving_player = temp_checked_list[i].arrival_time;
            late_arriving_player_index = i;
        }
    }
    printf("late arriving player is %d\n", late_arriving_player_index);

    //from the latest arriving player, we can determine the game start time
    //initially pick a court that is empty
    //if not empty, i will have to choose a court that is not empty...
    //but the game there will end before the player arriving last arrives
    //this property was guarenteed initialy before we started to match for it
    int court_number = is_there_empty_court();
    if (court_number == 0){
        int minimum_difference = 0;
        int difference = 0;
        for (int i = 0; i < NUMBER_OF_COURTS; ++i) {
            if(courts[i].occupied == 1 && courts[i].end_time <= late_arriving_player){
                difference = temp_checked_list[late_arriving_player_index].arrival_time - courts[i].end_time;
                if (difference < minimum_difference){
                    minimum_difference = difference;
                    court_number = courts[i].court_number;
                }
            }
        }
    }

    //we have the court number of where to add the player
    //we have the start time of the game -> the arrival time of the lateeeest arrving player
    int start_time = temp_checked_list[late_arriving_player_index].arrival_time;
    int end_time;

    //check among all the player if there are Males only, or Females only or mixed
    //if mixed we take the time of that for males
    //if not mixed, we take the time of each gender
    int male_count = 0;
    int female_count = 0;
    for(int i = 0; i < temp_checked_list_index; i++){
        if (temp_checked_list[i].gender == 'M'){
            male_count++;
            continue;
        }
        female_count++;
    }
    if (temp_checked_list_index == 2){
    //means the game is that of doubles
        if (male_count > 0){
            end_time = start_time + male_single_time;
        }
        else{
            end_time = start_time + female_single_time;
        }
    }
    //else means this is a doubles game
    else{
        if(male_count > 0){
            end_time = start_time + male_double_time;
        }
        else{
        end_time = start_time + female_double_time;
        }
    }


    //we have the start time, end time, court number and the players
    //now we can add the players to the matched list
    int* player_ids;

    //have a doubt that if this will add the pointer to that lists
    //and that if there are too many mallocs what will happen
    player_ids = (int *)(malloc(temp_checked_list_index * sizeof(int)));

    for(int i = 0; i < temp_checked_list_index; i++){
        player_ids[i] = temp_checked_list[i].player_id;
    }

    for(int i = 0; i < temp_checked_list_index; i++){
        printf("Player ID: %d\n", player_ids[i]);
    }

    //insert once on the matched list
    matched_list[matched_list_index].court_number = court_number;
    matched_list[matched_list_index].start_time = start_time;
    matched_list[matched_list_index].end_time = end_time;
    matched_list[matched_list_index].player_ids = player_ids;
    matched_list[matched_list_index].number_of_players = temp_checked_list_index;
    if (temp_checked_list_index == 2){
        matched_list[matched_list_index].game_type = 'S';
    }
    else{
        matched_list[matched_list_index].game_type = 'D';
    }
    matched_list_index++;
    printf("added to matched list, now we have %d matches", matched_list_index);
    
    //We also havr to occupy the court
    for (int i = 0; i < NUMBER_OF_COURTS; ++i) {
        if(courts[i].court_number == court_number){
            courts[i].occupied = 1;
            courts[i].start_time = start_time;
            courts[i].end_time = end_time;
            courts[i].player_ids = player_ids;
            break;
        }
    }

    //now we dont need the temp_checked_list anymore
    //but we need to add it to the checked list in the make_match function
    // temp_checked_list_index = 0;
    return;
}

void handle_connection(int client_socket, int thread_id) {
    char buffer[1024];
    int bytes_received;
    printf("Thread %d is handling the connection\n", thread_id);
    printf("Going to receive data from the client\n");
    // Receive data from the client
    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        // Echo the received data back to the client
        printf("Waiting for 5 seconds");
        sleep(5);
        send(client_socket, buffer, bytes_received, 0);
    }
    printf("Thread %d is done handling the connection\n", thread_id);
    
    // Close the client socket when done
    close(client_socket);
}

// int main() {
//     int server_socket, client_socket;
//     struct sockaddr_in server_addr, client_addr;
//     socklen_t client_len = sizeof(client_addr);

//     // Create a TCP socket
//     server_socket = socket(AF_INET, SOCK_STREAM, 0);

//     // Configure the server address
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr.sin_port = htons(8080);

//     // Bind the socket
//     bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

//     // Listen for incoming connections
//     listen(server_socket, 5);

//     printf("Server listening on port 8080...\n");

//     // Number of threads to create
//     const int num_threads =100;

//     // Accept connections in parallel using OpenMP
//     #pragma omp parallel num_threads(num_threads)
//     {   
//         if (omp_get_thread_num() == 0) {
//             printf("Number of threads: %d\n", omp_get_num_threads());
//         }
//         // Each thread has its own copy of the server socket
//         int private_server_socket = server_socket;

//         // Each thread listens and accepts connections independently
//         #pragma omp for
//         for (int i = 0; i < num_threads; ++i) {
//             // Accept a connection
//             client_socket = accept(private_server_socket, (struct sockaddr*)&client_addr, &client_len);

//             // Handle the connection in a separate function
//             #pragma omp critical
//             handle_connection(client_socket, omp_get_thread_num());
//         }
//     }

//     // Close the server socket when done
//     close(server_socket);

//     return 0;
// }


int main() {
    FILE *file;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // Open the file for reading
    file = fopen("input.csv", "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening the file\n");
        return 1;
    }

    // Ignore the first line
    if ((read = getline(&line, &len, file)) == -1) {
        fprintf(stderr, "Error reading the first line\n");
        fclose(file);
        free(line);
        return 1;
    }
    int not_much = 2;
    // Read and process each line
    while ((read = getline(&line, &len, file)) != -1) {
    // while(not_much > 0){
        Request request;
        sscanf(line, "%d,%d,%c,%c", &request.player_id, &request.arrival_time, &request.gender, &request.preference);

        // Do something with the Request struct (e.g., print the values)
        printf("Player ID: %d, Arrival Time: %d, Gender: %c, Preference: %c\n", request.player_id, request.arrival_time, request.gender, request.preference);
        append_to_waiting_list(request);
        //print all the indices
        printf("waiting_list_index: %d\n", waiting_list_index);
        printf("matched_list_index: %d\n", matched_list_index);
        printf("checked_list_index: %d\n", checked_list_index);
        printf("temp_checked_list_index: %d\n", temp_checked_list_index);
    }
    
    // print waiting list
    // for (int i = 0; i < waiting_list_index; ++i) {
    //     printf("Player ID: %d, Arrival Time: %d, Gender: %c, Preference: %c\n", waiting_list[i].player_id, waiting_list[i].arrival_time , waiting_list[i].gender, waiting_list[i].preference);  
    // }

    //print the matched list
    for (int i = 0; i < matched_list_index; ++i) {
        printf("Court Number: %d, Start Time: %d, End Time: %d, Number of Players: %d, Game Type: %c\n", matched_list[i].court_number, matched_list[i].start_time, matched_list[i].end_time, matched_list[i].number_of_players, matched_list[i].game_type);
        for (int j = 0; j < matched_list[i].number_of_players; ++j) {
            printf("Player ID: %d\n", matched_list[i].player_ids[j]);
        }
    }
    // Close the file and free allocated memory
    fclose(file);
    free(line);

    return 0;
}
