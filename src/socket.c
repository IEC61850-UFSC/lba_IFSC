#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>

#include "include/socket.h"

/*! \fn int socketConfig(const char *server_name, int port_number)
  \brief Conexão via socket.

  Configura a criação do socket IP.
  Se o server estiver habilitado então a conexão é estabelecida.

  \param server_name: uma string com o nome ou enederço do server.
  \param port_number: um inteiro com a porta onde sera estabelecido o socket.
  \return O resultado da conexão: o entero do file descriptor / negativo erro.
  \sa main (int argc, char *argv[]).
*/
int socketStart(const char* host_name, unsigned int port){

    /*! \section funcvar_sec Variaveis:
    *   \brief struct sockaddr_in serv_addr : Estrutura com a informação do server.
    *
    *   \brief hostent *server: Estrutura com o nome e enderço do server
    *
    *   \brief int sockfd:  Client file descriptor utilizado como retorno desta função
    *
    *   \section functask_sec Tarefas da função:
    */


    unsigned int socket_file_descriptor;
    struct sockaddr_in serv_addr;//Sever information structure
    struct hostent *server;//Server host name setructure

    /** * Cria o socket para utilizar TCP */
    /*! \code */
    if ((socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("ERROR opening socket\n");
        return -1;
    }
    /*! \endcode */


    /** * Obtem a informação do endereço do server*/
    /*! \code */
    if ((server = gethostbyname(host_name)) == NULL){
        printf("ERROR, no such host\n");
        return -2;
    }
    /*! \endcode */

    // Configura para estabelecer un socket TCP
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // Configura a porta para estabelecer o socket
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(port);

    /** * Conecta o socket ao server*/
    /*! \code */
    if (connect(socket_file_descriptor,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        printf("CONNECTION ERROR\n");
        return -3;
    }
    /*! \endcode */

    return socket_file_descriptor;
}

int read_socket(int fd, char *buffer){
    char chr_rcv[1];
    int n = 0;
    do{
        read(fd,chr_rcv,sizeof(chr_rcv));
        buffer[n] = chr_rcv[0];
        n++;
    }while(strcmp(chr_rcv,"\n") != 0);
    buffer[n]='\0';
    printf("%s\n",buffer);
    return n;
}
