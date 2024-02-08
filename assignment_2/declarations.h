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
