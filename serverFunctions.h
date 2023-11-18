#ifndef systemFunctions
#define systemFunctions

#include <stdbool.h>

#define MSG_SIZE 250
#define MAX_CLIENTS 30

int checkUser(char *username);
char *removeEnterLine(char *credentials);
void startGame();
void setRandomShips();
void checkCoords(char letra, int numero, int i);
char findCharacterSection(const char *cadena, char spliter, int letterJumps, char number);
void writeCharacterSection(char *cadena, char spliter, int letterJumps, char number);
int checkSunken(char *cadena, char spliter, int letterJumps, char number);

    /*---------------------------------------------------- 
        Variables genericas para funciones               
    -----------------------------------------------------*/

    extern fd_set readfds, auxfds;
	extern char buffer[MSG_SIZE];
    extern char usernameFinalist[MSG_SIZE];
	extern char letra;
   	extern int arrayClientes[MAX_CLIENTS];
    extern int numClientes;
    extern int i,j,k;
    extern int on, ret;
    extern int numero;
    extern int shootCounter;
    int arrayRequests[MAX_CLIENTS];
    int num_game_requests = 0;
    int letterJumps = 0;
    int counterTouchedA = 0;
    int counterTouchedB = 0;
    int counterShootedA = 0;
    int counterShootedB = 0;
    char gridA[MSG_SIZE];
    char gridB[MSG_SIZE];
    char gridBTouched[MSG_SIZE] = "A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A.";
    char gridATouched[MSG_SIZE] = "A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A;A,A,A,A,A,A,A,A,A,A.";
    char tempBuffer[MSG_SIZE];
    char shootCounterChain[2];
    char result;
    char spliter;
    bool turn = false;   
    
    /*---------------------------------------------------- 
        Funciones del sistema             
    -----------------------------------------------------*/

int makeRegister()
{
    char *username = NULL;
    char *password = NULL;
    char *token = strtok(buffer, " ");

    while (token != NULL) 
    {
        if (strcmp(token, "REGISTRO") == 0) 
        {
            token = strtok(NULL, " ");
            if (token && strcmp(token, "-u") == 0) 
            {
                token = strtok(NULL, " ");
                if (token) 
                {
                     username = token;
                }
                token = strtok(NULL, " ");
                if (token && strcmp(token, "-p") == 0)
                {
                    token = strtok(NULL, " ");
                    if (token)
                    {
                        password = token;
                    }
                }
            }
        }
        token = strtok(NULL, " ");
    }
    if (username && password)
    {
        if(checkUser(username) == 0 )
        {
            FILE *userFile = fopen("user.txt", "a");
            if (userFile)
            {
                fprintf(userFile, "%s,%s", username, password);
                fclose(userFile);
                return 1;
            }
        }
    }
    return 0;    
}


int checkUser(char *username)
{
    char linea[256];
    char usuarios[256][256]; // Almacena los usuarios encontrados
    int numUsuarios = 0;

    FILE *userFile = fopen("user.txt", "r+");
        if (userFile)
        {
            // Recorrer el archivo para extraer los usuarios
            while (fgets(linea, sizeof(linea), userFile)) 
            {
                char *token = strtok(linea, ",");
                if (token != NULL) 
                {
                    // Guarda el usuario en el array
                    strcpy(usuarios[numUsuarios], token);
                    numUsuarios++;
                }
        
                for (int i = 0; i < numUsuarios; i++) 
                {
                    if (strcmp(usuarios[i], username) == 0) 
                    {
                        printf("El usuario %s está en la base de datos.\n", username); 
                        return 1;  
                    }
                }
            }
        }
    fclose(userFile);
    return 0;
}


int checkPassword(char *username, char* buffer)
{
    char linea[256];
    char usuarios[256][256]; // Almacena los usuarios encontrados
    int numUsuarios = 0;

    char* password = buffer + strlen("PASSWORD ");
    password = removeEnterLine(password);
    
    FILE *userFile = fopen("user.txt", "r+");
    if (userFile)
    {
        // Recorrer el archivo para extraer los usuarios
        while (fgets(linea, sizeof(linea), userFile)) 
        {
            char *token = strtok(linea, ",");
            if (token != NULL) 
            {
                // Guarda el usuario en el array
                strcpy(usuarios[numUsuarios], token);
                numUsuarios++;
            }
        
            for (int i = 0; i < numUsuarios; i++) 
            {
                if (strcmp(usuarios[i], username) == 0) 
                {
                    token = strtok(NULL, ",");
                    if (token != NULL)
                    {
                        token = removeEnterLine(token);
                          
                        if (strcmp(token, password) == 0) 
                        {
                           return 1;                         
                        }
                    }
                }                        
            }
        }
    }
    fclose(userFile);
    return 0;
}


char *removeEnterLine(char *credentials)
{
    size_t len = strcspn(credentials, "\n");
    if (len < strlen(credentials)) 
    {
        credentials[len] = '\0';
    }
    return credentials;

}


void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[])
{
    char buffer[250];
    int j;
    
    close(socket);
    FD_CLR(socket,readfds);
    
    //Re-estructurar el array de clientes
    for (j = 0; j < (*numClientes) - 1; j++)
        if (arrayClientes[j] == socket)
            break;
    for (; j < (*numClientes) - 1; j++)
        (arrayClientes[j] = arrayClientes[j+1]);
    
    (*numClientes)--;
    
    bzero(buffer,sizeof(buffer));
    sprintf(buffer,"Desconexión del cliente <%d>",socket);
    
    for(j=0; j<(*numClientes); j++)
        if(arrayClientes[j] != socket)
            send(arrayClientes[j],buffer,sizeof(buffer),0);
}


void pairConexion(int i)
{   
    for (int k = 0; k < numClientes; k++){

        arrayRequests[k] = arrayClientes[k];
    }

    if (numClientes < MAX_CLIENTS)
    {
        num_game_requests++;
    }

    switch (num_game_requests) 
    {
        //Solo esta implementado de forma compacta 2 clientes
        //Para abarcara las 10 partidas (20 clientes), necesitamos establecer cases %2 hasta gestionar 20 clientes.
        //Implementar en todos los send switches con num_game_requests para gestionar los 20 clientes
        case 2:
            
            bzero(buffer,sizeof(buffer));
            send(arrayRequests[0], "+Ok. Empieza la partida.", sizeof("+Ok. Empieza la partida."), 0);
            bzero(buffer,sizeof(buffer));
            send(arrayRequests[1], "+Ok. Empieza la partida.", sizeof("+Ok. Empieza la partida."), 0);

            startGame();
            break;

        case 4:
            bzero(buffer,sizeof(buffer));
            send(arrayRequests[2], "+Ok. Empieza la partida.", sizeof("+Ok. Empieza la partida."), 0);
            bzero(buffer,sizeof(buffer));
            send(arrayRequests[3], "+Ok. Empieza la partida.", sizeof("+Ok. Empieza la partida."), 0);

            startGame();
            break;

        //case ...

        default:
            bzero(buffer,sizeof(buffer));
            strcpy(buffer, "\n+Ok. Esperando jugadores. Quedamos a la espera de más jugadores\n");
            send(i, buffer, sizeof(buffer), 0);
            
            break;
    }
}


void startGame()
{
    setRandomShips();
}


void setRandomShips()
{
    sleep(1);   //Necesario para que se envie bien los mensajes
    bzero(buffer,sizeof(buffer));

    FILE *file;
    int lineCount = 0;

    srand(time(NULL));

    file = fopen("matches.txt", "r+");
    if (file == NULL) {
        perror("Error al abrir el archivo");
    }

    while (fgets(buffer, sizeof(buffer), file)) {
        lineCount++;
    }

    fseek(file, 0, SEEK_SET); //El puntero apunta al comienzo

    int randomLineA = rand() % lineCount + 1;
    int randomLineB = rand() % lineCount + 1;

    for (int i = 0; i <= randomLineA; i++) {
        if (fgets(gridA, sizeof(gridA), file) == NULL) 
        {
            //perror("Error al leer el archivo");
        }   
    }

    for (int i = 0; i <= randomLineB; i++) {
        if (fgets(gridB, sizeof(gridB), file) == NULL) 
        {
            //perror("Error al leer el archivo");
        }
    }

    fclose(file);

    strcpy(buffer,gridA);
    send(arrayRequests[0], buffer, sizeof(buffer), 0);

    strcpy(buffer,gridB);
    send(arrayRequests[1], buffer, sizeof(buffer), 0);
}


void checkCoords(char letra, int number, int i)
{
    //Implementar asignacion dinamica
    switch (letra) 
    {
        case 'A':
            letterJumps = 1;
            break;
        case 'B':
            letterJumps = 2;
            break;
        case 'C':
            letterJumps = 3;
            break;
        case 'D':
            letterJumps = 4;
            break;
        case 'E':
            letterJumps = 5;
            break;
        case 'F':
            letterJumps = 6;
            break;
        case 'G':
            letterJumps = 7;
            break;
        case 'H':
            letterJumps = 8;
            break;
        case 'I':
            letterJumps = 9;
            break;
        case 'J':
            letterJumps = 10;
            break;
        default:
            strcpy(buffer, "-ERR. Formato de disparo no valido\n");
            send(i,buffer,sizeof(buffer),0);
    }

    spliter = ';';

    //para manipular secciones posteriormente
    letterJumps = letterJumps -1;   
    
    if ((letterJumps > 10) || (number > 10))
    {
        printf("-ERR. Formato de disparo no valido\n");
    }
    
    //para manipular posiciones de caracteres
    number = (number * 2) - 1 ;

    //Turno false -> jugador A, Turno true -> jugador B 
    if (turn == true)
    {
        counterShootedB++;
        writeCharacterSection(gridBTouched, spliter, letterJumps, number);
        result = findCharacterSection(gridA, spliter, letterJumps, number);
    }

    if (turn == false)
    {
        counterShootedA++;
        writeCharacterSection(gridATouched, spliter, letterJumps, number);
        result = findCharacterSection(gridB, spliter, letterJumps, number);
        
    }

    if (result != '\0') 
    {
        //printf("El carácter buscado en la sección %d es: %c\n", letterJumps, result);
    } else 
    {
        //printf("No se encontró el carácter buscado en la sección %d.\n", letterJumps);
    }
    if (result == 'B')
    {
        /* Funcion de Comprobar Hundido en mantenimiento por fallos en la logica de checkSunken

        if (turn == false) 
        {
            if(checkSunken(gridB, spliter, letterJumps, number) == 1)
            {
                    strcpy(buffer,"+OK. HUNDIDO\n");
                    send(i, buffer, sizeof(buffer), 0);
            }

        } else if (turn == true) 
        {
            if(checkSunken(gridB, spliter, letterJumps, number) == 1)
            {
                strcpy(buffer,"+OK. HUNDIDO\n");
                send(i, buffer, sizeof(buffer), 0);
            }

        }*/
            
        strcpy(buffer,"+OK. TOCADO\n");
        send(i, buffer, sizeof(buffer), 0);

        if(turn == false)
        {
            counterTouchedA++;
        }else
        {
            counterTouchedB++;
        }

        if (counterTouchedB == 14 || counterTouchedA == 14)
        {
            strcpy(tempBuffer, "+OK. ");
            strcat(tempBuffer, usernameFinalist);
            strcat(tempBuffer, " ha ganado, numero de disparos ");

            if (turn == false)
            {
                sprintf(shootCounterChain, "%d", counterShootedA);
            }else 
            {
                sprintf(shootCounterChain, "%d", counterShootedB);
            }
                
            strcat(tempBuffer, shootCounterChain);
            strcpy(buffer, tempBuffer);

            send(arrayClientes[0], buffer, sizeof(buffer), 0);
            send(arrayClientes[1], buffer, sizeof(buffer), 0);

            strcpy(buffer, "\n+Ok. Partida finalizada.");
            send(arrayClientes[0], buffer, sizeof(buffer), 0);
            send(arrayClientes[1], buffer, sizeof(buffer), 0);
        }
            //turn = turn;
    }
        
    if (result == 'A')
    {
        strcpy(buffer,"+OK. AGUA\n");

        send(arrayClientes[0], buffer, sizeof(buffer), 0);
        send(arrayClientes[1], buffer, sizeof(buffer), 0);
           
        turn = !turn;
    }
}


char findCharacterSection(const char *cadena, char spliter, int letterJumps, char number) 
{
    int letterJumpsCurrent = 0;
    int charactersCounter = 0;
    const char *ptr = cadena;

    // Buscar la sección deseada
    while (letterJumpsCurrent < letterJumps && *ptr) {
        if (*ptr == spliter) {
            letterJumpsCurrent++;
        }
        ptr++;
    }

    // Verificar si se encontró la sección deseada
    if (letterJumpsCurrent < letterJumps) {
        return '\0'; // No se encontró la sección deseada
    }

    // Buscar el carácter deseado en la sección
    while (*ptr) {
        if (*ptr != number) {
            charactersCounter++;
        }
        if (charactersCounter == number) {
            return *ptr;
        }
        ptr++;
    }

    return '\0'; // No se encontró el carácter deseado
}

void writeCharacterSection(char *cadena, char spliter, int letterJumps, char number) 
{
    int letterJumpsCurrent = 0;
    int charactersCounter = 0;
    int position = 0;
    char localArray[MSG_SIZE];

    strcpy(localArray, cadena);

    char *ptr = localArray;

    // Buscar la sección deseada
    while (letterJumpsCurrent < letterJumps && *ptr) {
        if (*ptr == spliter) {
            letterJumpsCurrent++;
        }
        ptr++;
    }

    // Verificar si se encontró la sección deseada
    if (letterJumpsCurrent < letterJumps) {
        //return '\0';
    }

    // Buscar el carácter deseado en la sección
    while (*ptr) {
        if (*ptr != number) {
            charactersCounter++;
        }
        if (charactersCounter == number) {

            position = 20 * letterJumpsCurrent + charactersCounter -1;

            if((turn == false && gridB[position] == 'B') || (turn == true && gridA[position] == 'B'))
            {
                localArray[position] = 'T';
                strcpy(cadena, localArray);
            }
             else
            {
                localArray[position] = 'x';
                strcpy(cadena, localArray);
            }

            strcpy(buffer, cadena);
            send(i, buffer, sizeof(buffer), 0);
        }
        ptr++;
    }
}


//Funcion a implementar la logica de forma correcta
int checkSunken(char *cadena, char spliter, int letterJumps, char number)
{
    int letterJumpsCurrent = 0;
    int charactersCounter = 0;
    int position = 0;
    char localArray[MSG_SIZE];

    strcpy(localArray, cadena);

    char *ptr = localArray;

    // Buscar la sección deseada
    while (letterJumpsCurrent < letterJumps && *ptr) {
        if (*ptr == spliter) {
            letterJumpsCurrent++;
        }
        ptr++;
    }

    // Verificar si se encontró la sección deseada
    if (letterJumpsCurrent < letterJumps) {
        //return '\0'; // No se encontró la sección deseada
    }

    // Buscar el carácter deseado en la sección
    while (*ptr) {
        if (*ptr != number) {
            charactersCounter++;
        }
        if (charactersCounter == number) 
        {
            //Actual
            position = 20 * letterJumpsCurrent + charactersCounter -1;

            //Derecha
            position = 20 * letterJumpsCurrent + charactersCounter +1;

            if((turn == false && gridA[position] == 'B') || (turn == true && gridB[position] == 'B'))
            {
                return 1;
            }


            //Izquierda
            position = 20 * letterJumpsCurrent + charactersCounter -3;

            if((turn == false && gridA[position] == 'B') || (turn == true && gridB[position] == 'B'))
            {
                return 1;
            }
            //Arriba
            position = 20 * (letterJumpsCurrent-1) + charactersCounter -1;

            if((turn == false && gridA[position] == 'B') || (turn == true && gridB[position] == 'B'))
            {
                return 1;
            }
            //Abajo
            position = 20 * (letterJumpsCurrent+1) + charactersCounter -1;

            if((turn == false && gridA[position] == 'B') || (turn == true && gridB[position] == 'B'))
            {
                return 1;
            }
            //return 0;
        }
        ptr++;
    }
}

#endif