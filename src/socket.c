#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

//#include <stdint.h>

//#include <string.h>

//#include <errno.h>
//#include <sys/types.h>
//#include <sys/stat.h>

//#include <signal.h>
//#include <fcntl.h>
//#include <sys/wait.h>

//#include "include/MemoryManagement.h"
#include "include/socket.h"
//#include "include/FileOperations.h"

static pthread_t socket_monitor_thread; //Variavel que aponta a thread.

#if 0
char foto_name[140];                      //Nome da foto alterado dinamicamente.
int indicie;                      //Numero de fotos capturadas.
SEM semaforo_ocr;
char string_time[30];                //capture time
#endif

#define MAX_MSG_LENGHT 196

struct thread_arg {    /* strutura de argumentos para a thread */
           int      fd;                 /* file descriptor do socket */
           char*    destination_path;   /* pasta onde vão ser salvadas as imagens */
};



/*! \fn int socketConfig(const char *server_name, int port_number)
  \brief Conexão via socket.

  Configura a criação do socket IP.
  Se o server estiver habilitado então a conexão é estabelecida.

  \param server_name: uma string com o nome ou enederço do server.
  \param port_number: um inteiro com a porta onde sera estabelecido o socket.
  \return O resultado da conexão: o entero do file descriptor / negativo erro.
  \sa main (int argc, char *argv[]).
*/
int socketConfig(const char* host_name, unsigned int port){

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

#if 1
//Thread que lee o socket e caso uma imagem seja capturada a salva
//na pasta especificada pelo parametro arg->destination_path+indicie.
int socket_monitor_task(void *arg){

    int n;                                                //Variaveis auxiliares.
    //char *buffer;
    char *w_buffer;
    char teste[1];
    char teste_2[3];
    //char *id_msg;
    struct thread_arg *args = (struct thread_arg *)arg;  //Structura onde estão os argumentos da função.
    GPSD_PROTOCOL_VERSION_HEADER version_header;
    GPSD_PROTOCOL_DEVICE_HEADER device_header;

    //Habilita visualizar os dados do gpsd
    //w_buffer = "?WATCH={\"enable\":true,\"json\":true}";
	//write(args->fd,w_buffer,strlen(w_buffer));

	//reading hello from gpsd
	//id_msg = malloc(MAX_MSG_LENGHT);
    //n = recv(args->fd, id_msg, MAX_MSG_LENGHT, MSG_WAITALL);

    n = recv(args->fd, &version_header, sizeof(version_header), MSG_WAITALL);

    printf("%c\n",version_header.ini);
    version_header.tag[sizeof(version_header.tag)-1] = '\0';
    printf("%s\n",version_header.tag);
    version_header.header_class[sizeof(version_header.header_class)-1] = '\0';
    printf("%s\n",version_header.header_class);
    version_header.rest[sizeof(version_header.rest)-1] = '\0';
    printf("%s\n",version_header.rest);
    printf("des_path =%s\n",args->destination_path);
    //free(id_msg);

    //Enable to visualize gpsd data
    w_buffer = "?WATCH={\"enable\":true,\"json\":true}";
	write(args->fd,w_buffer,strlen(w_buffer));

	n = recv(args->fd, &device_header, sizeof(device_header), MSG_WAITALL);

    printf("%c\n",device_header.ini);
    device_header.tag[sizeof(device_header.tag)-1] = '\0';
    printf("%s\n",device_header.tag);
    device_header.header_class[sizeof(device_header.header_class)-1] = '\0';
    printf("%s\n",device_header.header_class);
    device_header.rest[sizeof(device_header.rest)-1] = '\0';
    printf("%s\n",device_header.rest);
    printf("des_path =%s\n",args->destination_path);

    n = read(args->fd,teste,sizeof(teste));
    if(teste[0] == '"'){
        printf("teste = %s\n",teste);
        do{
        n = read(args->fd,teste_2,sizeof(teste_2));
        printf("%s",teste_2);
        }while(strcmp(teste_2,"}]}") != 0);

        n = read(args->fd,teste,sizeof(teste));
        printf("%s",teste);
        n = read(args->fd,teste,sizeof(teste));
        printf("%s",teste);
    }
    else{
        do{
        n = read(args->fd,teste,sizeof(teste));
        printf("%s",teste);
        }while(strcmp(teste,"}") != 0);

        n = read(args->fd,teste,sizeof(teste));
        printf("%s",teste);

        n = read(args->fd,teste,sizeof(teste));
        printf("%s",teste);

    }

    printf("\nfinish\n");

    n = recv(args->fd, &device_header, sizeof(device_header), MSG_WAITALL);

    printf("%c\n",device_header.ini);
    device_header.tag[sizeof(device_header.tag)-1] = '\0';
    printf("%s\n",device_header.tag);
    device_header.header_class[sizeof(device_header.header_class)-1] = '\0';
    printf("%s\n",device_header.header_class);
    device_header.rest[sizeof(device_header.rest)-1] = '\0';
    printf("%s\n",device_header.rest);







    //Disable to visualize gpsd data
    w_buffer = "?WATCH={\"enable\":false}";
	write(args->fd,w_buffer,strlen(w_buffer));


	#if 0
	for(;;){

        printf("---waiting for message---\n");

        //n = recv(args->fd, &header, sizeof(header), MSG_WAITALL);
        //printf("Class Tag = %s\n",header.msg_class_tag);
        //printf("Class = %s\n",header.msg_class);

        buffer = malloc(MAX_MSG_LENGHT);
        n = recv(args->fd, buffer, MAX_MSG_LENGHT, MSG_WAITALL);
        if (n != MAX_MSG_LENGHT){
            printf("n=%d\n",n);
            perror("nao bate");
            exit(0);
        }

        printf("received = %s\n",buffer);
        printf("des_path =%s\n",args->destination_path);

        free(buffer);
	}
	#endif
	return 0;
}
#endif

//Função de inicialização da thread.
int socketStart(const char* server_name, int server_port, char *des_path){
    int socket_fd;
    struct thread_arg *arguments;

    //Open Socket
    socket_fd = socketConfig(server_name,server_port);
    printf("socket_fd = %d \n",socket_fd);


    arguments = calloc(1, sizeof(struct thread_arg));
    arguments[0].fd = socket_fd;
    arguments[0].destination_path = des_path;

    // Cria uma thread para monitorar a conexão
	pthread_create(&socket_monitor_thread, NULL,
                   (void * (*)(void *))socket_monitor_task,
                   &arguments[0]);

    return 0;
}

