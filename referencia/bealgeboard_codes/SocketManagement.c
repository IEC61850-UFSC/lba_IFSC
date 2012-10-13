#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>


#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "include/MemoryManagement.h"
#include "include/SocketManagement.h"
#include "include/FileOperations.h"



static pthread_t monitor_captura_thread; //Variavel que aponta a thread.
char foto_name[140];                      //Nome da foto alterado dinamicamente.
int indicie;                      //Numero de fotos capturadas.
SEM semaforo_ocr;
char string_time[30];                //capture time

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
  \return O resultado da conexão: 0 sucesso / negativo erro.
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

//Thread que lee o socket e caso uma imagem seja capturada a salva
//na pasta especificada pelo parametro arg->destination_path+indicie.
int socket_monitor_task(void *arg){

    FILE *f_frame;                      //Pontero que salva o nome da foto.
    int n;                              //Variaveis auxiliares.
    unsigned char *frame;               //Imagem original.
    struct thread_arg *a = (struct thread_arg *)arg; /*Structura onde estão os argumentos
                                                     da função.*/
    time_t rawtime;
    struct tm * timeinfo;
    ITS_CAM_PROTOCOL_HEADER_IO_RQST header;

	for(;;){

        printf("---waiting for trigger---\n");

        n = recv(a->fd, &header, sizeof(header), MSG_WAITALL);
        if (n != sizeof(header)){
            printf("n=%d\n",n);
            perror("nao bate");
            exit(0);
        }
        //printf("size = %d\n",header.img_size);

        #if 0
        //************Cria o nome do arquivo dinamicamente ***************
        //O nome vai até 9999.
        if (indicie < 10)
            sprintf(foto_name,"%s000%d.BMP",a->destination_path,indicie);
        else if (indicie < 100)
            sprintf(foto_name,"%s00%d.BMP",a->destination_path,indicie);
        else if (indicie < 1000)
            sprintf(foto_name,"%s0%d.BMP",a->destination_path,indicie);
        else if (indicie < 10000){
            sprintf(foto_name,"%s%d.BMP",a->destination_path,indicie);
            if (indicie == 9999)
                indicie = 0;
        }

        //************************ abre o arquivo ************************
        f_frame = fopen (foto_name, "w+b") ;
        if (f_frame == NULL){
            printf ( "Cannot open target file" ) ;
            return -1;
        }//*/
        //****************************************************************
        #endif

        //Lendo os proximos 718146 bytes trazidos pela camera correspondentes a imagem.
        frame = malloc(header.img_size);
        n = recv(a->fd, frame, header.img_size, MSG_WAITALL);
        if (n != header.img_size){
            printf("n=%d\n",n);
            perror("nao bate");
            exit(0);
        }
        //printf("size = %d\n",header.img_size);
        //fwrite(frame,1,header.img_size,f_frame);

        printf("---photo saved---\n");

        //GETTING THE CAPTURE TIME
        memset(string_time,0,sizeof(string_time));
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );

        strftime (string_time,30,"%Y.%m.%d-%H:%M:%S",timeinfo);
        //Fecha o arquivo.
        //fflush(f_frame);
        //fclose ( f_frame );
        free(frame);
        //Aumenta o numero de fotos capturadas.
        SemSend(semaforo_ocr,1);
		indicie++;
	}
	return 0;
}

//Função de inicialização da thread.
int socketThreadStart(unsigned int socket_file_descriptor, char *des_path){
    struct thread_arg *arguments;

    arguments = calloc(1, sizeof(struct thread_arg));
    arguments[0].fd = socket_file_descriptor;
    arguments[0].destination_path = des_path;

    // Cria uma thread para monitorar a conexão
	pthread_create(&monitor_captura_thread, NULL,
						  (void * (*)(void *))socket_monitor_task,
						  &arguments[0]);

    return 0;
}

