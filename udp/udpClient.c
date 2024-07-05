#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h> 
#include <stdlib.h>

#define MAX_MSG 100
#define END_FLAG "FIM"

int main(int argc, char *argv[]) {
  int sd, rc, n;
  struct sockaddr_in ladoCli;  
  struct sockaddr_in ladoServ;
  char msg[MAX_MSG]; 

  if(argc < 4) {
    printf("uso correto: %s <ip_do_servidor> <porta_do_servidor> <mensagem_inicial>\n", argv[0]);
    exit(1);
  }

  ladoServ.sin_family = AF_INET;
  ladoServ.sin_addr.s_addr = inet_addr(argv[1]);
  ladoServ.sin_port = htons(atoi(argv[2]));

  ladoCli.sin_family = AF_INET;
  ladoCli.sin_addr.s_addr = htonl(INADDR_ANY);
  ladoCli.sin_port = htons(0); 

  sd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sd < 0) {
    printf("%s: nao pode abrir o socket\n", argv[0]);
    exit(1);
  }

  rc = bind(sd, (struct sockaddr *) &ladoCli, sizeof(ladoCli));
  if(rc < 0) {
    printf("%s: nao pode fazer um bind da porta\n", argv[0]);
    exit(1);
  }
  printf("{UDP, IP_Cli: %s, Porta_Cli: %u, IP_R: %s, Porta_R: %s}\n", inet_ntoa(ladoCli.sin_addr), ntohs(ladoCli.sin_port), argv[1], argv[2]);

  strncpy(msg, argv[3], MAX_MSG);
  rc = sendto(sd, msg, strlen(msg), 0, (struct sockaddr *) &ladoServ, sizeof(ladoServ));
  if(rc < 0) {
    printf("%s: nao pode enviar dados\n", argv[0]);
    close(sd);
    exit(1);
  }
  printf("Enviando primeira mensagem: %s\n", msg);

  while(1) {
    memset(msg, 0x0, MAX_MSG);

    n = recvfrom(sd, msg, MAX_MSG, 0, NULL, NULL);
    if(n < 0) {
      printf("%s: nao pode receber dados\n", argv[0]);
      continue;
    }

    printf("Recebido: %s\n", msg);

    if (strncmp(msg, END_FLAG, strlen(END_FLAG)) == 0) {
      printf("Recebida mensagem de término. Encerrando cliente.\n");
      break;
    }

    printf("Digite uma mensagem para enviar ao servidor: ");
    fgets(msg, MAX_MSG, stdin);
    msg[strcspn(msg, "\n")] = '\0';

    rc = sendto(sd, msg, strlen(msg), 0, (struct sockaddr *) &ladoServ, sizeof(ladoServ));
    if(rc < 0) {
      printf("%s: nao pode enviar dados\n", argv[0]);
      close(sd);
      exit(1);
    }

    if (strncmp(msg, END_FLAG, strlen(END_FLAG)) == 0) {
      printf("Mensagem de término enviada. Encerrando cliente.\n");
      break;
    }
  }
  
  close(sd);
  return 0;
}
