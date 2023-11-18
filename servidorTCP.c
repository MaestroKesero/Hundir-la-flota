#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <regex.h>
#include <ctype.h>
#include <sys/select.h>
#include <time.h>
#include "serverFunctions.h"

#define MSG_SIZE 250
#define MAX_CLIENTS 30

void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]);

/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
        
    socklen_t from_len;
    fd_set readfds, auxfds;
    struct sockaddr_in sockname, from;
    char buffer[MSG_SIZE];
    char tempBuffer[MSG_SIZE];
    char usernameFinalist[MSG_SIZE];
    char letra;
    int salida;
    int arrayClientes[MAX_CLIENTS];
    int numClientes = 0;
    int sd, new_sd;
    int i,j,k;
    int recibidos = -1;
    int on, ret;
    int shootCounter = 0;
    int numero;
    bool playingGame = false;  

int main ()
{
    
	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/

  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("–ERR. No se puede abrir el socket cliente\n");
    		exit (1);	
	}
        //Permitimos activar los socket para reutilizar puertos (configuracion opciones del socket) (si no habria que esperar)
    	on=1;
    	ret = setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    /* ------------------------------------------------------------------
		Se rellenan los campos de la estructura con la IP del 
		servidor y el puerto del servicio que solicitamos
	-------------------------------------------------------------------*/
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2065);
	sockname.sin_addr.s_addr =  INADDR_ANY;

	if (bind (sd, (struct sockaddr *) &sockname, sizeof (sockname)) == -1)
	{
		perror("–ERR. No se ha podido realizar la operación bind");
		exit(1);
	}
	
	from_len = sizeof (from);

	if(listen(sd,1) == -1){
		perror("–ERR. No se ha podido realizar la operación listen");
		exit(1);
	}

    printf("El servidor está esperando conexiones...\n");	//Inicializar los conjuntos fd_set
	
	FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(sd,&readfds);
    FD_SET(0,&readfds);
   	
    //Capturamos la señal SIGINT (Ctrl+c)
    signal(SIGINT,manejador);
    
	/*-----------------------------------------------------------------------
		El servidor acepta una petición
	------------------------------------------------------------------------ */
	while(1)
    {
        //Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya conectados)   
        auxfds = readfds;
        
        //Monitoria el socket en busca de actividad de lectura
        salida = select(FD_SETSIZE,&auxfds,NULL,NULL,NULL);

        if(salida > 0)
        {
            for(i=0; i<FD_SETSIZE; i++)
            {
                //Buscamos el socket por el que se ha establecido la comunicación (Listo opcion lectura)
                if(FD_ISSET(i, &auxfds)) 
                {
                    //Se ha detectado nueva conexion entrante
                    if(i == sd)
                    {   
                        if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1){
                                perror("–ERR. No se han podido aceptar peticiones");
                        }
                        else
                        {
                            if(numClientes < MAX_CLIENTS)
                            {
                                arrayClientes[numClientes] = new_sd;
                                numClientes++;
                                FD_SET(new_sd,&readfds);
                                
                                strcpy(buffer, "+Ok. Usuario conectado\n\n");
                                send(new_sd,buffer,sizeof(buffer),0);
                            }
                            else
                            {
                                bzero(buffer,sizeof(buffer));
                                strcpy(buffer, "–ERR. Demasiados clientes conectados\n");
                                send(new_sd,buffer,sizeof(buffer),0);
                                close(new_sd);
                            }
                        }
                    }
                    else if (i == 0)
                    {
                        //Se ha introducido información de teclado
                        bzero(buffer, sizeof(buffer));
                        fgets(buffer, sizeof(buffer),stdin);
            
                        //Controlar si se ha introducido "SALIR" por parte del servidor
                        if(strcmp(buffer,"SALIR\n") == 0)
                        { 
                            for (i=0; i<numClientes; i++){
                            salirCliente(i,&readfds,&numClientes,arrayClientes);
                            }
                            manejador(SIGINT);
                        }
                    } 
                    else
                    {
                        bzero(buffer,sizeof(buffer));
                        recibidos = recv(i,buffer,sizeof(buffer),0);
                            
                        if(recibidos > 0)
                        {     
                            //Aqui introducimos los paquetes a desarrollar
                            if(strcmp(buffer,"SALIR\n") == 0)
                            {
                                strcpy(buffer, "Desconectado Cliente");
                                send(i, buffer, sizeof(buffer), 0);
                                salirCliente(i,&readfds,&numClientes,arrayClientes);

                                if (playingGame == true && shootCounter != 0)
                                {
                                    strcpy(buffer, "\n+Ok. Tu oponente ha terminado la partida\n");
                                    send(arrayClientes[0], buffer, sizeof(buffer), 0);
                                    send(arrayClientes[1], buffer, sizeof(buffer), 0);
                                }
                                    
                            }

                            else if (strncmp(buffer, "REGISTRO -u ", strlen("REGISTRO -u ")) == 0) {

                                if(makeRegister())
                                {
                                    strcpy(buffer, "\n+Ok Usuario Registrado\n");
                                    send(i, buffer, sizeof(buffer), 0);

                                }else
                                {
                                    strcpy(buffer, "\n-Err El usuario introducido ya existe\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                            }

                            else if (strncmp(buffer, "USUARIO ", strlen("USUARIO ")) == 0) 
                            {
                                char* username = buffer + strlen("USUARIO ");
                                username = removeEnterLine(username);
                                char* usernamecopy = strdup(username);  //Evitamos conflictos con los punteros
                        
                                if(checkUser(username))
                                {
                                    strcpy(buffer, "+Ok. Usuario correcto\n");
                                    send(i, buffer, sizeof(buffer), 0);

                                    recv(i,buffer,sizeof(buffer),0);

                                    if((strncmp(buffer, "PASSWORD ", strlen("PASSWORD ")) == 0) && checkPassword(usernamecopy, buffer))
                                    {
                                        strcpy(buffer, "\n+Ok. Usuario validado\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                    else
                                    {
                                        strcpy(buffer, "\n-Err. Error en la validacion\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                    strcpy(usernameFinalist, usernamecopy);
                                }
                                else
                                {
                                    strcpy(buffer, "\n-Err. Usuario incorrecto\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                            }

                            else if(strcmp(buffer,"INICIAR-PARTIDA\n") == 0)
                            {
                                pairConexion(i);
                                playingGame = true;
                            
                            }

                            else if (sscanf(buffer, "DISPARO %c,%d", &letra, &numero) == 2) 
                            {
                                shootCounter++;

                                strcpy(tempBuffer, "+OK ");
                                strcat(tempBuffer, buffer);
                                strcpy(buffer, tempBuffer);

                                for(j=0; j<2; j++){
                                    
                                    if(arrayClientes[j] != i)
                                        send(arrayClientes[j],buffer,sizeof(buffer),0);
                                }
                                checkCoords(letra, numero, i);
                            }

                            else if(strcmp(buffer,"SALIR") == 0 && playingGame == true )
                            {
                                strcpy(buffer, "\n+Ok. Tu oponente ha terminado la partida\n");
                                
                                send(arrayClientes[0], buffer, sizeof(buffer), 0);
                                send(arrayClientes[1], buffer, sizeof(buffer), 0);
                            }
                            else
                            {
                                strcpy(buffer, "-ERR\n");
                                send(i, buffer, sizeof(buffer), 0);
                            }   
                        }
                    }   
                    //Si el cliente introdujo ctrl+c
                    if(recibidos == 0)
                    {
                        printf("El socket %d, ha introducido ctrl+c\n", i);
                        //Eliminar ese socket
                        salirCliente(i,&readfds,&numClientes,arrayClientes);

                        if (playingGame == true)
                        {
                            strcpy(buffer, "\n+Ok. Tu oponente ha terminado la partida\n");
                                
                            send(arrayClientes[0], buffer, sizeof(buffer), 0);
                            send(arrayClientes[1], buffer, sizeof(buffer), 0);
                        }
                    }
                    
                }
            }
        }
    }
    close(sd);
	return 0;
}

void manejador (int signum){

    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT,manejador);

    for (j = 0; j < numClientes; j++){
    
		bzero(buffer, sizeof(buffer));
		strcpy(buffer,"Desconexión servidor\n"); 
        send(arrayClientes[j],buffer , sizeof(buffer),0);
        close(arrayClientes[j]);
        FD_CLR(arrayClientes[j],&readfds);
    }
    close(sd);
    exit(-1);
}