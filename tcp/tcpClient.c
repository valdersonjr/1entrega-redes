/* ******************************/
/* FGA/Eng. Software/ FRC       */
/* Prof. Fernando W. Cruz       */
/* Codigo: tcpClient2.c         */
/* ******************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_SIZE    	80

int main(int argc,char * argv[]) {
    struct sockaddr_in ladoServ; /* Contém dados do servidor */
    int sd;          	      /* Socket descriptor */
    int n;                  /* Número de caracteres lidos do servidor */
    char bufout[MAX_SIZE];     /* Buffer de dados enviados */
    char bufin[MAX_SIZE];      /* Buffer de dados recebidos */

    /* Confere o número de argumentos passados para o programa */
    if(argc<3)  {
        printf("Uso correto: %s <ip_do_servidor> <porta_do_servidor>\n", argv[0]);
        exit(1);
    }

    memset((char *)&ladoServ, 0, sizeof(ladoServ)); /* Limpa estrutura */
    memset(bufout, 0, sizeof(bufout));     /* Limpa buffer */
    memset(bufin, 0, sizeof(bufin));       /* Limpa buffer */
    
    ladoServ.sin_family      = AF_INET; /* Config. socket para internet*/
    ladoServ.sin_addr.s_addr = inet_addr(argv[1]);
    ladoServ.sin_port        = htons(atoi(argv[2]));

    /* Cria socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        fprintf(stderr, "Criação do socket falhou!\n");
        exit(1);
    }

    /* Conecta socket ao servidor definido */
    if (connect(sd, (struct sockaddr *)&ladoServ, sizeof(ladoServ)) < 0) {
        fprintf(stderr,"Tentativa de conexão falhou!\n");
        exit(1);
    }

    while (1) {
        while (1) {
            printf("> ");
            fgets(bufout, MAX_SIZE, stdin);    /* Lê dados do teclado */
            send(sd, bufout, strlen(bufout), 0); /* Envia dados ...  */
            
            if (strncmp(bufout, "FIM", 3) == 0) {
                break;
            }
            
            if (strncmp(bufout, "ENDTURN", 7) == 0) {
                break;
            }
        }

        if (strncmp(bufout, "FIM", 3) == 0) {
            break;
        }

        while (1) {
            n = recv(sd, bufin, MAX_SIZE, 0); /* Recebe dados ... */
            if (n > 0) {
                bufin[n] = '\0';
                printf("Servidor: %s\n", bufin);
                if (strncmp(bufin, "ENDTURN", 7) == 0) {
                    break;
                }
            }
        }
    }

    printf("------- encerrando conexão com o servidor -----\n");
    close(sd);
    return 0;
}
