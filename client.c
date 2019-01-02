#include "pipe_networking.h"

int main() {

  int to_server;
  int from_server;
  char msg[BUFFER_SIZE];
  char resp[BUFFER_SIZE];

  from_server = client_handshake( &to_server );

  while (1) {

    printf("[Client] data to send: " );
    fgets(msg, BUFFER_SIZE, stdin);
    msg[strlen(msg)-1] = 0;
    write(to_server, msg, sizeof(msg));
    printf("[Client] sent: %s\n", msg);

    read(from_server, resp, sizeof(resp));
    printf("[Client] received: %s\n", resp);
  }

  return 0;
}
