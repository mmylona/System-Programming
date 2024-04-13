#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_LENGTH 100
#define MAX_VOTERS 200

typedef struct{
    char name[MAX_LENGTH];
    char party[MAX_LENGTH];
}Voters;

pthread_t thread;
pthread_mutex_t mutex;

char* serverName;
int portNum;
Voters voters[MAX_VOTERS];
int countVoters = 0;


void* sending_party(void* arg){

    pthread_mutex_lock(&mutex);

    struct sockaddr_in serverAddress;
    struct sockaddr *serverptr = (struct sockaddr*) &serverAddress;
    struct hostent *rem;

    //create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Failed to create socket\n");
        pthread_exit(NULL);
    }

    //Find server address
    rem = gethostbyname(serverName);
    if (rem == NULL) {
        perror("gethostbyname");
        pthread_exit(NULL);
    }

    //internet domain IPv4
    serverAddress.sin_family = AF_INET;
    //copy ip address h_addr to sin_addr in the format needed
    memcpy(&serverAddress.sin_addr, rem->h_addr, rem->h_length);
    //assign the port number to the sin_port field
    serverAddress.sin_port = htons(portNum);



    if(connect(sock,serverptr,sizeof(serverAddress)) < 0){
        perror("connect");
        pthread_exit(NULL);
    }

    //communication with server
    char message[MAX_LENGTH] = {'\0'};
    char response[MAX_LENGTH] = {'\0'};

    if (recv(sock, message, sizeof(message), 0) == -1) {
        perror("recv");
        close(sock);
        pthread_exit(NULL);
    }

	//send name
    strcpy(response, voters[countVoters - 1].name);
    if(send(sock, response, strlen(response), 0) == -1){
        perror("send");
        pthread_exit(NULL);
    }

    if (recv(sock, message, sizeof(message), 0) == -1) {
        perror("recv");
        close(sock);
        pthread_exit(NULL);
    }

	//send party
    strcpy(response, voters[countVoters - 1].party);
    strcat(response, " ");
    if(send(sock, response, strlen(response), 0) == -1){
        perror("send");
        pthread_exit(NULL);
    }

    if (recv(sock, message, sizeof(message), 0) == -1) {
        perror("recv");
        close(sock);
        pthread_exit(NULL);
    }

    pthread_mutex_unlock(&mutex);

    close(sock);
    pthread_exit(NULL);

}

int main(int argc, char* argv[]){
    if(argc != 4){
        printf("Wrong number of args\n");
        return 1;

    }
    serverName = malloc(sizeof(char*) * strlen(argv[1]));
    strcpy(serverName,argv[1]);
    if(serverName == NULL){
        perror("strcpy");
        return 1;
    }

    portNum = atoi(argv[2]);

    FILE *inputFile = fopen(argv[3], "r");
    if (inputFile == NULL) {
        perror("Error opening file");
        pthread_exit(NULL);
    }

    char line[MAX_LENGTH];
    while (fgets(line, MAX_LENGTH, inputFile) != NULL) {
        int lineSize = strlen(line) - 1;
        line[lineSize] = '\0';
        char* tokens[3];

		//split the line
        tokens[0] = strtok(line, " ");

        int i=0;
        while(tokens[i] != NULL){
            while (tokens[i] != NULL) {
                i++;
                tokens[i] = strtok(NULL, " ");
            }
        }

		//put 2 first tokens to name
        strcpy(voters[countVoters].name,tokens[0]);
        strcat(voters[countVoters].name, " ");
        strcat(voters[countVoters].name, tokens[1]);
        strcat(voters[countVoters].name, " ");
        strcat(voters[countVoters].name, " ");
        
        strcpy(voters[countVoters].party,tokens[2]);
        voters[countVoters].party[strlen(voters[countVoters].party)] = ' ';

        countVoters++;

        pthread_create(&thread, NULL, sending_party, NULL);

        pthread_join(thread, NULL);
    }

    fclose(inputFile);
    free(serverName);

    return 0;
}
