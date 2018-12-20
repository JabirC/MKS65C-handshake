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
        if(up == -1){
          printf("%s \n",strerror(errno));
        }
        else{
          char buffer[HANDSHAKE_BUFFER_SIZE];
          read(up, buffer, HANDSHAKE_BUFFER_SIZE);
          printf("$$SERVER$$ Server received private pipe name %s, sending acknowledgement\n", buffer);
          close(up);
          remove("srvr");

          *to_client = open(buffer, O_WRONLY);
          if(*to_client == -1){
            printf("%s \n",strerror(errno));
          }
          else{
            write(*to_client,ACK , HANDSHAKE_BUFFER_SIZE);
            close(*to_client);

            char client_resp[HANDSHAKE_BUFFER_SIZE];
            *to_client = open(buffer, O_RDONLY);
            read(*to_client, client_resp, HANDSHAKE_BUFFER_SIZE);
            printf("$$SERVER$$ Received response from client: %s\n", client_resp);
            close(*to_client);
            printf("$$SERVER$$ Handshake Complete\n");

            return up;
          }
        }
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  mkfifo("privi", 0644);

  *to_server = open("srvr", O_WRONLY);
  if(*to_server== -1){
    printf("%s, Well-known pipe not found \n",strerror(errno));
  }
  else{
    char pvt_name[HANDSHAKE_BUFFER_SIZE];
    strcpy(pvt_name, "privi");
    write(*to_server, pvt_name, HANDSHAKE_BUFFER_SIZE);
    printf("$$CLIENT$$ Client has sent private pipe name: %s\n", pvt_name);
    char buffer_client[BUFFER_SIZE];

    int down = open(pvt_name, O_RDONLY);
    if (down == -1){
      printf("%s\n",strerror(errno));
    }
    else{
      read(down, buffer_client, HANDSHAKE_BUFFER_SIZE);
      printf("$$CLIENT$$ Client received acknowledgement from server: '%s', sending response\n", buffer_client);
      close(down);


      down = open(pvt_name, O_WRONLY);
      char last[HANDSHAKE_BUFFER_SIZE] = "Adios!";
      write(down, last, HANDSHAKE_BUFFER_SIZE);
      close(down);
      printf("$$CLIENT$$ Handshake Complete\n");
      remove(pvt_name);
      return down;
    }
  }
}
