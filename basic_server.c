
#include "pipe_networking.h"

static void sighandler(int signo) {
  if (signo == SIGINT) {
    remove("wkp");
    exit(0);
  }
}

int main() {
  signal(SIGINT, sighandler);

  int to_client;
  int from_client;
  char msg[BUFFER_SIZE];
  int i;
  char pid[HANDSHAKE_BUFFER_SIZE];


  while (1) {
    from_client = server_handshake( pid );
    // printf("PID of client: %s\n", pid);
    int f = fork();
    if (!f) {
      from_client = subserver_handshake(&to_client, pid, from_client);

      // entering client
      while (read(from_client, msg, sizeof(msg))) {
        printf("[Server] received: %s\n", msg);

        //process msg
        for (i = 0; i < strlen(msg); i++) {
          msg[i]++;
        }

        write(to_client, msg, sizeof(msg));
      }
    }
    else {
      
    }

  }
}
