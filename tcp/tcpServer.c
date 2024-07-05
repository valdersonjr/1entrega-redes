#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_SIZE 80

void atende_cliente(int novo_sd, struct sockaddr_in endCli);

int main(int argc, char *argv[]) {
    struct sockaddr_in ladoServ; 
    struct sockaddr_in endCli;   
    int sd, novo_sd;            
    socklen_t tam_Cli;         

    if (argc < 2) {
        printf("Uso correto: %s <porta_do_servidor>\n", argv[0]);
        exit(1);
    }

    memset((char *)&ladoServ, 0, sizeof(ladoServ)); 
    ladoServ.sin_family = AF_INET;                  
    ladoServ.sin_addr.s_addr = htonl(INADDR_ANY);   
    ladoServ.sin_port = htons(atoi(argv[1]));       

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        fprintf(stderr, "Criação do socket falhou!\n");
        exit(1);
    }

    if (bind(sd, (struct sockaddr *)&ladoServ, sizeof(ladoServ)) < 0) {
        fprintf(stderr, "Ligação do socket falhou!\n");
        exit(1);
    }

    if (listen(sd, 5) < 0) {
        fprintf(stderr, "A escuta por conexões falhou!\n");
        exit(1);
    }

    while (1) {
        tam_Cli = sizeof(endCli);
        novo_sd = accept(sd, (struct sockaddr *)&endCli, &tam_Cli);
        if (novo_sd < 0) {
            fprintf(stderr, "Aceitação da conexão falhou!\n");
            exit(1);
        }
        atende_cliente(novo_sd, endCli);
    }

    return 0;
}

void atende_cliente(int novo_sd, struct sockaddr_in endCli) {
    char buf[MAX_SIZE];
    int n;

    while (1) {
        while (1) {
            n = recv(novo_sd, buf, MAX_SIZE, 0);
            if (n > 0) {
                buf[n] = '\0';
                printf("Cliente: %s", buf);
                if (strncmp(buf, "FIM", 3) == 0) {
                    break;
                }
                if (strncmp(buf, "ENDTURN", 7) == 0) {
                    break;
                }
            }
        }

        if (strncmp(buf, "FIM", 3) == 0) {
            break;
        }

        while (1) {
            printf("> ");
            fgets(buf, MAX_SIZE, stdin);   
            send(novo_sd, buf, strlen(buf), 0);
            if (strncmp(buf, "ENDTURN", 7) == 0) {
                break;
            }
            if (strncmp(buf, "FIM", 3) == 0) {
                break;
            }
        }

        if (strncmp(buf, "FIM", 3) == 0) {
            break;
        }
    }

    printf("------- encerrando conexão com o cliente -----\n");
    close(novo_sd);
}
