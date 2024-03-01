import json
import socket
import time
from mpi4py import MPI


comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()
signal = 0
rank_player_id_map_file = "map.json"

my_game_size = 0
my_winner = 0
my_winner_rank = 0
player_id = 0
number_of_players = 0
status = MPI.Status()

congratulations_text = "Congratulations on winning the game!"
reply_text = "Thank You!"

# def wish_winner(send_to_rank: int):
#     comm.send(congratulations_text, dest=send_to_rank, tag=1)
#     print(f"Wish From {rank} To {send_to_rank}")
#     msg = comm.recv(source=send_to_rank, tag=2)
#     print(f"Reply received By {rank} from {send_to_rank}")

# def receive_wish(number_of_players: int):
#     global status
#     if number_of_players == 4:
#         for i in range(2):
    


def send_message_to_server(message):
    # Create a TCP socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Configure the server address
    server_addr = ('127.0.0.1', 8080)

    # Connect to the server
    client_socket.connect(server_addr)

    # Send a message to the server
    client_socket.sendall(message.encode())

    # Receive and print the echoed message from the server
    buffer_size = 1024
    received_data = client_socket.recv(buffer_size)
    print(f"Rank: {rank} Server Reply: {received_data.decode()}")

    client_socket.close()

    # if received_data.decode() != "Request Timed-out":
    #     #Player Id: 127, Court Number: 3, Start Time: 84, End Time: 99, Winner: 147 ,Number of Players: 4, Game Type: D
    #     #get the winner's id from this reply
    #     global my_winner
    #     my_winner = received_data.decode().split(',')[4]
    #     my_winner = int(my_winner.split(' ')[2])
    #     number_of_players = received_data.decode().split(',')[5]
    #     number_of_players = int(number_of_players.split(' ')[3])

    #     #open json file
    #     with open(rank_player_id_map_file, 'r') as file:
    #         data = json.load(file)
    #         for item in data:
    #             if int(item[1]) == my_winner:
    #                 my_winner_rank = item[0]
    #                 break

    #     if my_winner == player_id:
    #         receive_wish(number_of_players)
    #     else:
    #         wish_winner(my_winner_rank)
            

    # Close the client socket when done
    

if __name__ == "__main__":
    arrival_time = 0
    message = ""
    if rank != 0:
        with open('input.csv', 'r') as file:
            lines = file.readlines()
            message = lines[rank]
            player_id = int(lines[rank].split(',')[0])
            arrival_time = int(lines[rank].split(',')[1])
        
    local_information = (rank, player_id, arrival_time)

    if rank == 0:
        gathered_information = [None] * size
    else:
        gathered_information = None

    gathered_information = comm.gather(local_information, root=0)
    
    if rank ==0:
        print(gathered_information[len(gathered_information)-1])
        sorted_information = sorted(gathered_information, key=lambda x: x[2])

        with open(rank_player_id_map_file, 'w') as file:
            json.dump(sorted_information, file)

        for info in sorted_information:
            comm.send(1, dest=info[0], tag=0)
            time.sleep(0.2)
    
    if rank != 0:
        signal = comm.recv(source=0, tag=0)
        if signal:
            send_message_to_server(message)

    MPI.Finalize()
        
    # Call the function to send the message to the server
    # send_message_to_server(sys.argv[1])
