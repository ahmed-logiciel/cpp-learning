# Chat Application Approach

- Create a server class
    - This class creates two threads.
    - Parent thread is for taking input from user who ran server. User can give commands to view active clients, all registered clients, view logs for a specific client and exit. Upon exit, state of server will be saved in a file.
    - Child thread creates an acceptor class which continuously waits for a connection from a client. This happens in a while loop. Once an active client request is received, it creates an object of service class which creates a new thread for handling the session with the client. This handler thread performs all types of authentication or registeration and communication with the client.
    - Non-threaded approach is to make server asynchronous.

- Create a client class
    - There are two possible ways to implement this.
    - First is creating two sockets for every client in two different thread. One socket is for reading and other socket is for writing only.
    - Second approach is to read and write asynchronously.
    - In either approach, client should first perform authentication for login or regsiteration. Then it can send commands to server and receive messages.
    - Possible commands are these. Request all clients list. Request server to send message to either one or many clients. Request server to share sockets of active clients to whom client can share files with. Delete client account. Change client password.