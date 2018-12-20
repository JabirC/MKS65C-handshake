#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
        int up;
        mkfifo("srvr", 0644);

        printf("\n$$SERVER$$ Server has created well known pipe, Waiting for client\n");

        up =open("srvr",O_RDONLY);
        char buffer[HANDSHAKE_BUFFER_SIZE];
        read(up, buffer, HANDSHAKE_BUFFER_SIZE);
        printf("$$SERVER$$ Server received private pipe name, sending acknowledgement\n");
        close(up);
        remove("srvr");

        *to_client = open(buffer, O_WRONLY);
        write(*to_client,ACK , HANDSHAKE_BUFFER_SIZE);
        close(*to_client);

        char client_resp[HANDSHAKE_BUFFER_SIZE];
        *to_client = open(buffer, O_RDONLY);
        read(*to_client, client_resp, HANDSHAKE_BUFFER_SIZE);
        printf("$$SERVER$$ Received response from client\n");
        close(*to_client);
        printf("$$SERVER$$ Handshake Complete\n");

        return up;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  mkfifo("client_pvt", 0644);

  *to_server = open("srvr", O_WRONLY);

  char pvt_name[HANDSHAKE_BUFFER_SIZE];
  strcpy(pvt_name, "clnt_pvt");
  write(*to_server, pvt_name, strlen(pvt_name));
  printf("$$CLIENT$$ Client has sent private pipe name: %s\n", pvt_name);
  char buffer_client[BUFFER_SIZE];

  int down = open(pvt_name, O_RDONLY);
  read(down, buffer_client, HANDSHAKE_BUFFER_SIZE);
  printf("$$CLIENT$$ Client received acknowledgement from server, sending response\n");
  close(down);

  down = open(pvt_name, O_WRONLY);
  char last[HANDSHAKE_BUFFER_SIZE] = "Adios!";
  write(down, last, HANDSHAKE_BUFFER_SIZE);
  close(down);
  printf("$$CLIENT$$ Handshake Complete\n");
  remove(pvt_name);
  return down;
}
