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
  int sd, rc, n, tam_Cli;
  struct sockaddr_in endCli;
  struct sockaddr_in endServ; 
  char msg[MAX_MSG];

  if (argc<3) {
    printf("Digite IP e Porta para este servidor\n");
    exit(1);
  }

  sd=socket(AF_INET, SOCK_DGRAM, 0);
  if(sd<0) {
    printf("%s: nao pode abrir o socket \n",argv[0]);
    exit(1);
  }

  endServ.sin_family = AF_INET;
  endServ.sin_addr.s_addr = inet_addr(argv[1]); 
  endServ.sin_port = htons(atoi(argv[2]));

  rc = bind (sd, (struct sockaddr *) &endServ, sizeof(endServ));
  if(rc<0) {
    printf("%s: nao pode fazer bind na porta %s \n", argv[0], argv[2]);
    exit(1);
  }

  printf("%s: esperando por dados no IP: %s, porta UDP numero: %s\n", argv[0], argv[1], argv[2]);

  while(1) {
    memset(msg, 0x0, MAX_MSG);
    tam_Cli = sizeof(endCli);

    n = recvfrom(sd, msg, MAX_MSG, 0, (struct sockaddr *) &endCli, &tam_Cli);
    if(n<0) {
      printf("%s: nao pode receber dados \n", argv[0]);
      continue;
    } 

    printf("{UDP, IP_L: %s, Porta_L: %u", inet_ntoa(endServ.sin_addr), ntohs(endServ.sin_port));
    printf(" IP_R: %s, Porta_R: %u} => %s\n", inet_ntoa(endCli.sin_addr), ntohs(endCli.sin_port), msg);

    if (strncmp(msg, END_FLAG, strlen(END_FLAG)) == 0) {
      printf("Recebida mensagem de término. Encerrando servidor.\n");
      break;
    }

    printf("Digite uma mensagem para enviar ao cliente: ");
    fgets(msg, MAX_MSG, stdin);
    msg[strcspn(msg, "\n")] = '\0';

    rc = sendto(sd, msg, strlen(msg), 0, (struct sockaddr *) &endCli, sizeof(endCli));
    if(rc<0) {
      printf("%s: nao pode enviar dados\n", argv[0]);
      close(sd);
      exit(1);
    }

    if (strncmp(msg, END_FLAG, strlen(END_FLAG)) == 0) {
      printf("Mensagem de término enviada. Encerrando servidor.\n");
      break;
    }
  }
  
  close(sd);
  return 0;
}