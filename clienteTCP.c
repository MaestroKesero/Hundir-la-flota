#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <sys/select.h>

void printGrid(char *buffer);

int main (int argc, char const *argv[])
{
	if (argc < 2)
	{
      printf("Tienes que llamar al programa de la siguiente manera:\n");
      printf("./serverTCP <Ip del Servidor>\n");
      return -1;
	}

	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd;
	struct sockaddr_in sockname;
	char buffer[250];
	socklen_t len_sockname;
    fd_set readfds, auxfds;
    int salida;
    int fin = 0;
	int turn = 0;
	bool user = false;
	bool password = false;
	bool playing = false;
	bool myTurn = false;
	
	/* ------------------------------------------------------------------
		Se abre el socket con socket(Dominio, Tipo Conexion TCP)
	-------------------------------------------------------------------*/

  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("–ERR. No se puede abrir el socket cliente\n");
    		exit (1);	
	}

	/* ------------------------------------------------------------------
		Se rellenan los campos de la estructura con la IP del 
		servidor y el puerto del servicio que solicitamos
	-------------------------------------------------------------------*/
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2065);
	sockname.sin_addr.s_addr =  inet_addr(argv[1]);

	/* ------------------------------------------------------------------
		Se solicita la conexión con el servidor 
	-------------------------------------------------------------------*/
	len_sockname = sizeof(sockname);
	
	if (connect(sd, (struct sockaddr *)&sockname, len_sockname) == -1)
   {
      perror("–ERR. Al establecer la conexión");
      exit(1);
   }
    
    //Inicializamos las estructuras
    FD_ZERO(&auxfds);
    FD_ZERO(&readfds);
    
    FD_SET(0,&readfds);
    FD_SET(sd,&readfds);

    printf("=====================================================================================\n");
	printf("Bienvenido a hundir la flota\n");
	printf(" Introduce una accion:\n\n");
	printf(" REGISTRO -u <usuario> -p <contraseña> \t - Para registrarse en la aplicación. \n");
	printf(" USUARIO <usuario> \t\t\t - Para iniciar sesión \n");
	printf("=====================================================================================\n\n");

	/* ------------------------------------------------------------------
		Se transmite la información 
	-------------------------------------------------------------------*/

	do
	{
		//Apuntan a la misma direccion y con select monitoreamos el descriptor. 
        auxfds = readfds;
        salida = select(sd+1,&auxfds,NULL,NULL,NULL);
        
        //Tengo mensaje desde el servidor lo leemos e interpretamos.
        if (FD_ISSET(sd, &auxfds))
      	{

         	bzero(buffer, sizeof(buffer));
         	recv(sd, buffer, sizeof(buffer), 0);

        	printf("%s", buffer);

         	if (strcmp(buffer, "–ERR. Demasiados clientes conectados\n") == 0)
            	fin = 1;

         	if (strcmp(buffer, "–ERR. Desconectado por el servidor\n") == 0)
            	fin = 1;
			
			if (strcmp(buffer, "-ERR. Formato de disparo no valido\n") == 0)
            	myTurn = true;
			
			if (strcmp(buffer, "+Ok. Usuario correcto\n") == 0)
			{
				user = true;
				printf("Introduzca su contraseña: \n");
				printf("PASSWORD <password> \n\n");
			}

			if (strcmp(buffer, "\n+Ok. Usuario validado\n") == 0)
			{
				password = true;
				printf("================================================================\n");
				printf("Introduce una accion:\n\n");
				printf("INICIAR-PARTIDA \t Para empezar a jugar\n");
				printf("SALIR \t\t\t Para desconectar del servidor.\n");
				printf("================================================================\n\n");
			}

			if (strcmp(buffer, "\n+Ok. Esperando jugadores. Quedamos a la espera de más jugadores\n") == 0)
			{
				printf("Si lo deseas en cualquier momento puedes salir con SALIR\n");
				myTurn = true;
			}

			if (strncmp(buffer, "+Ok. Empieza la partida.", strlen("+Ok. Empieza la partida.")) == 0)
			{
				playing = true;
				printf("%s", buffer);
				printf("En su turno podrá:\n");
				printf("================================================================\n");
				printf("DISPARO (letra,numero) \t Para dispara a una casilla\n");
				printf("SALIR \t\t\t Para salir de la partida:\n");
				printf("================================================================\n\n\n\n");

				if (myTurn == true)
				{
					printf("+Ok. Turno de partida\n");
				}

				else if (myTurn == false)
				{
					printf("+Ok. Turno del otro jugador\n");
				}
			}

			//Nos llega la posicion de las casillas.
			if (buffer[1] == ',') 
			{
            	//printf("%s\n\n",buffer);
				printGrid(buffer);
            }

			if ((strstr(buffer, "+OK. AGUA\n") != NULL) && (myTurn == true))
			{
				printf("+Ok. Turno del otro jugador\n");
				turn = 1;
				myTurn = false;
            }

			if ((strstr(buffer, "+OK. AGUA\n") != NULL) && (myTurn == false) && turn == 0)
			{
				printf("+Ok. Turno de PARTIDA\n");
				myTurn = true;
            }

			if (strstr(buffer, "+OK. TOCADO\n") != NULL) 
			{
				printf("+Ok. Turno de partida\n");
				myTurn = true;
            }

			if (strstr(buffer, "+OK. HUNDIDO") != NULL) 
			{
				printf("+Ok. Turno de partida\n");
				myTurn = true;
            }

			if (strcmp(buffer, "+Ok. Turno del otro jugador\n") == 0)
				myTurn = false;
			
			if (strcmp(buffer, "+Ok. Ha salido el otro jugador. Finaliza la partida\n") == 0)
				playing = false;

			if (strcmp(buffer, "+Ok. Desconexión procesada.\n") == 0)
				fin = 1;

			if (strcmp(buffer, "+Ok. Turno del otro jugador\n") == 0)
				myTurn = false;

			if (strcmp(buffer, "+Ok. Turno de partida\n") == 0)
				myTurn = true;

			if (strcmp(buffer, "\n+Ok. Partida finalizada.") == 0) 
			{
				playing = false;
				myTurn = false;
				fin = 1;
			}

			if (strcmp(buffer, "\n+Ok. Tu oponente ha terminado la partida\n") == 0) 
			{
				playing = false;
				myTurn = false;
				fin = 1;
			}

			turn = 0;	//Necesario para la gestion de turnos.
		}
		else
        {
         	// Control de errores
         	if (FD_ISSET(0, &auxfds))
         	{
				bzero(buffer, sizeof(buffer));
				fgets(buffer, sizeof(buffer), stdin);

				if ((strncmp(buffer, "PASSWORD", strlen("PASSWORD")) == 0) && (user == false))
				{
				printf("-Err. No puede introducir la contraseña antes que el nombre de usuario\n");
				}

				else if ((strncmp(buffer, "PASSWORD", strlen("PASSWORD")) == 0) && (password == true))
				{
				printf("-Err. Ya ha iniciado sesión\n");
				}

				else if ((strncmp(buffer, "REGISTRO", strlen("REGISTRO")) == 0) && (user == true))
				{
				printf("-Err. Ya está registrado\n");
				}

				else if ((strncmp(buffer, "USUARIO ", strlen("USUARIO ")) == 0) && (user == true))
				{
				printf("-Err. Ya ha iniciado sesión\n");
				}

				else if ((strcmp(buffer, "INICIAR-PARTIDA\n") == 0) && (password == false))
				{
				printf("-Err. No puede iniciar partida antes de iniciar sesión\n");
				}

				else if ((strcmp(buffer, "INICIAR-PARTIDA\n") == 0) && (playing == true))
				{
				printf("-Err. No puede volver a iniciar partida\n");
				}

				else if ((strncmp(buffer, "DISPARO ", strlen("DISPARO ")) == 0) && (myTurn == false))
				{
				printf("-Err. Debe esperar su turno\n");
				}
				else
				{
				send(sd, buffer, sizeof(buffer), 0);
				}
        	}
    	}	
    }while(fin == 0);
		
    close(sd);
    return 0;
}


void printGrid(char *buffer) {
    char grid[10][11];  
    int row = 0;  
    int col = 0;  

    printf(" Sus barcos son los siguientes\n\n");
    printf("   1  2  3  4  5  6  7  8  9 10\n\n"); 

    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == ';' || buffer[i] == '.') {
            printf("%c ", 'A' + row);  // Imprimir el identificador de fila como letra (A-J)

            for (int j = 0; j < 10; j++) {
                printf("%2c ", grid[j][row]);  // Imprimir contenido de la columna
            }
            printf("\n");
            row++;
            col = 0;
        } else if (buffer[i] != ',') {
            grid[col][row] = buffer[i];  // Almacenar el carácter en la matriz con el intercambio de fila y columna
            col++;
        }
    }
}