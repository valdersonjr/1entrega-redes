/* ******************************/
/* FGA/Eng. Software/ FRC       */
/* Prof. Fernando W. Cruz       */
/* Codigo: tcpServer.c          */
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

#define MAX_SIZE 80

void atende_cliente(int novo_sd, struct sockaddr_in endCli);

int main(int argc, char *argv[]) {
    struct sockaddr_in ladoServ; /* Contém dados do servidor */
    struct sockaddr_in endCli;   /* Contém dados do cliente */
    int sd, novo_sd;             /* Socket descriptors */
    socklen_t tam_Cli;           /* Tamanho do endereço do cliente */

    /* Confere o número de argumentos passados para o programa */
    if (argc < 2) {
        printf("Uso correto: %s <porta_do_servidor>\n", argv[0]);
        exit(1);
    }

    memset((char *)&ladoServ, 0, sizeof(ladoServ)); /* Limpa estrutura */
    ladoServ.sin_family = AF_INET;                  /* Config. socket para internet */
    ladoServ.sin_addr.s_addr = htonl(INADDR_ANY);   /* Recebe de qualquer interface de rede */
    ladoServ.sin_port = htons(atoi(argv[1]));       /* Porta do servidor */

    /* Cria socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        fprintf(stderr, "Criação do socket falhou!\n");
        exit(1);
    }

    /* Liga o socket à porta e IP definidos */
    if (bind(sd, (struct sockaddr *)&ladoServ, sizeof(ladoServ)) < 0) {
        fprintf(stderr, "Ligação do socket falhou!\n");
        exit(1);
    }

    /* Ouve por conexões */
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
            n = recv(novo_sd, buf, MAX_SIZE, 0); /* Recebe dados ... */
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
            fgets(buf, MAX_SIZE, stdin);    /* Lê dados do teclado */
            send(novo_sd, buf, strlen(buf), 0); /* Envia dados ...  */
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
