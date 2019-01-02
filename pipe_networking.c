#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(char * pid) {
  printf("[Server] Creating wkp...\n" );
  int r = mkfifo("wkp", 0644);
  if (r < 0) printf("Error %d: %s\n", errno, strerror(errno));

  printf("[Server] Receiving from client...\n" );
  int from_client = open("wkp", O_RDONLY);
  read(from_client, pid, sizeof(pid)); // reading client's pid/FIFO name
  printf("[Server] Message received: %s\n", pid);

  printf("[Server] Removing wkp...\n" );
  remove("wkp");


  return from_client;
}

int subserver_handshake(int * to_client, char * pid, int from_client) {
  printf("Writing ack to private pipe...\n" );
  *to_client = open(pid, O_WRONLY);
  write(*to_client, ACK, sizeof(ACK));

  printf("Receiving from client...\n" );
  char clbuf[HANDSHAKE_BUFFER_SIZE];
  read(from_client, clbuf, sizeof(clbuf));
  printf("Message received: %s\n", clbuf);

  return from_client;
}

/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  printf("[Client] Creating private pipe...\n" );
  char pid[HANDSHAKE_BUFFER_SIZE];
  sprintf(pid, "%d", getpid());
  int r = mkfifo(pid, 0644 | O_CREAT);
  if (r < 0) printf("Error %d: %s\n", errno, strerror(errno));

  printf("[Client] Writing PID to wkp...\n" );
  *to_server = open("wkp", O_WRONLY);
  write(*to_server, pid, sizeof(pid));

  printf("[Client] Receiving from private pipe...\n" );
  int prvfd = open(pid, O_RDONLY);
  char serverbuf[HANDSHAKE_BUFFER_SIZE];
  read(prvfd, serverbuf, sizeof(serverbuf));
  printf("[Client] Message received: %s\n", serverbuf);

  printf("[Client] Removing private pipe...\n" );
  remove(pid);

  printf("[Client] Sending message to server...\n" );
  write(*to_server, ACK, sizeof(ACK));

  return prvfd;
}
