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
#include <signal.h>
#include <netdb.h>


#define MAX_LENGTH 100

typedef struct{
    char name[MAX_LENGTH];
    char party[MAX_LENGTH];
}Voters;

typedef struct{
    char partyName[MAX_LENGTH];
    int count;
}Stats;

pthread_t masterThread;
pthread_mutex_t mutex;
pthread_cond_t bufferEmpty;
pthread_cond_t bufferFull;

int  portnum;
int numWorkerThread;
int bufferSize;
FILE* poll_log;
FILE* poll_stats;
int sock;
int* buffer;
int bufferCounter = 0;
int bufferIn = 0;
int bufferOut = 0;
Voters* voters;
int countVoters = 0;
Stats* stats;
int differentParties = 0; 

int print_to_pollLog(char* str1, char* str2){
    int lenght1 = strlen(str1);
    int lenght2 = strlen(str2);
    char* voterName = malloc(sizeof(char*) * lenght1);
    char* voterParty = malloc(sizeof(char*) * lenght2);
    strcpy(voterName,str1);
    strcpy(voterParty,str2);

    if(voterName[lenght1-2] != '\0'){
        voterName[lenght1-2] = '\0';
    }
    if(voterParty[lenght2-2] != '\0'){
        voterParty[lenght2-2] = '\0';
    }

    if(fprintf(poll_log, "%s %s\n",voterName, voterParty) == -1){
        perror("fprintf");
        free(voterName);
        free(voterParty);
        return 0;
    }
    free(voterName);
    free(voterParty);
    return 1;
};

int print_to_pollStat(char* str, int num){
    int lenght = strlen(str);
    char* statParty = malloc(sizeof(char*) * lenght);
    strcpy(statParty,str);

    if(statParty[lenght-2] != '\0'){
        statParty[lenght-2] = '\0';
    }

    if(fprintf(poll_stats, "%s %d\n",statParty, num) == -1){
        perror("fprintf");
        free(statParty);
        return 0;
    }
    free(statParty);
    return 1;
};

int has_voted(char name[]){
    for(int i=0;i<countVoters;i++){
        if(strcmp(name,voters[i].name) == 0){
            return 1;
        }
    }
    return 0;
}

void signal_handler(int sig){
    for(int i=0;i<differentParties;i++) {
        if (!print_to_pollStat(stats[i].partyName, stats[i].count)) {
            exit(EXIT_FAILURE);
        }
    }

	//closing
    close(sock);
    free(buffer);
    free(voters);
    free(stats);
    fclose(poll_log);
    fclose(poll_stats);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&bufferEmpty);
    pthread_cond_destroy(&bufferFull);

    exit(EXIT_SUCCESS);
}

void* workers_threads(void* arg){

    int clientSocket;

    while(1) {

        pthread_mutex_lock(&mutex);
        char name[MAX_LENGTH] = {'\0'};
        char party[MAX_LENGTH] = {'\0'};
        char response[MAX_LENGTH] = {'\0'};

        //wait if buffer is empty
        while (bufferCounter == 0) {
            pthread_cond_wait(&bufferEmpty, &mutex);
        }

        clientSocket = buffer[bufferOut];
        bufferOut = (bufferOut + 1) % bufferSize;
        bufferCounter--;

        //Communication with client
        strcpy(response, "SEND NAME PLEASE\n");
        send(clientSocket, response, strlen(response) + 1, 0);
        if (recv(clientSocket, name, sizeof(name), 0) == -1) {
            perror("recv");
            close(clientSocket);
            pthread_exit(NULL);
        }

        if (has_voted(name)) {
            strcpy(response, "ALREADY VOTED\n");
            send(clientSocket, response, strlen(response) + 1, 0);
            close(clientSocket);
            pthread_mutex_unlock(&mutex);
            continue;
        }


        strcpy(response, "SEND VOTE PLEASE\n");
        send(clientSocket, response, strlen(response) + 1, 0);
        if (recv(clientSocket, party, sizeof(party), 0) == -1) {
            perror("recv");
            close(clientSocket);
            pthread_exit(NULL);
        }

        //A Voter added to the struct voters
        strcpy(voters[countVoters].name, name);
        strcpy(voters[countVoters].party, party);

        int exist = -1;
        for (int i = 0; i < differentParties; i++) {
            if (strcmp(stats[i].partyName, voters[countVoters].party) == 0) {
                exist = i;
            }
        }
        if (exist == -1) {
            strcpy(stats[differentParties].partyName, voters[countVoters].party);
            stats[differentParties].count = 1;
            differentParties++;
        } else {
            stats[exist].count++;
        }
        countVoters++;

        sprintf(response, "VOTE for Party %s RECORDED\n", voters[countVoters - 1].party);
        send(clientSocket, response, strlen(response), 0);

        //Write to poll_log file
        if (!print_to_pollLog(voters[countVoters - 1].name, voters[countVoters - 1].party)) {
            pthread_exit(NULL);
        }
        close(clientSocket);
        //Buffer is not Full
        pthread_cond_broadcast(&bufferFull);
        pthread_mutex_unlock(&mutex);

    }
    pthread_exit(NULL);
}

void* master_thread(void* arg){
    int clientSocket;
    int socketDescriptor = *(int*)arg;
    pthread_t workersThread[numWorkerThread];

    if (listen(socketDescriptor, bufferSize) == -1) {
        perror("Failed to listen for connections");
        free(arg);
        pthread_exit(NULL);
    }

    //create numWorkerThreads
    for (int i=0;i<numWorkerThread;i++) {
        pthread_create(&workersThread[i], NULL, workers_threads, NULL);
    }
    for (int i = 0; i < numWorkerThread; i++) {
        pthread_detach(workersThread[i]);
    }


    while (1) {
    //accept connection from client
        struct sockaddr_in clientAddress; //client address during connection

        socklen_t clientAddressSize = sizeof(clientAddress);

        clientSocket = accept(socketDescriptor, (struct sockaddr *) &clientAddress, &clientAddressSize);
        if (clientSocket == -1) {
            perror("Failed to accept");
            free(arg);
            continue;
        }

        pthread_mutex_lock(&mutex);


        //wait if the buffer is full
        while (bufferCounter == bufferSize) {
            pthread_cond_wait(&bufferFull, &mutex);
        }

        //add connection to the buffer
        buffer[bufferIn] = clientSocket;
        bufferIn = (bufferIn + 1) % bufferSize;
        bufferCounter++;


        //Buffer is not empty
        pthread_cond_broadcast(&bufferEmpty);

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}


int main(int argc, char* argv[]){
    if(argc != 6){
        printf("Wrong number of args\n");
        return 1;

    }
    portnum = atoi(argv[1]);
    numWorkerThread = atoi(argv[2]);
    if(numWorkerThread <= 0){
        printf("Num of workers threads need to be > 0\n");
        return 1;
    }
    bufferSize = atoi(argv[3]);
    if(bufferSize <= 0){
        printf("Buffer size need to be > 0\n");
        return 1;
    }
    buffer = malloc(sizeof(int)*bufferSize);
    voters = malloc(200 * sizeof(Voters));
    stats = malloc(200 * sizeof(Stats));

    //open poll and stat files
    poll_log = fopen(argv[4], "w");
    if(poll_log == NULL){
        perror("Failed to open");
        return 1;
    }
    poll_stats = fopen(argv[5], "w");
    if(poll_stats == NULL){
        perror("Failed to open");
        return 1;
    }

	//initialaization
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&bufferEmpty,NULL);
    pthread_cond_init(&bufferFull,NULL);

    signal(SIGINT, signal_handler);

    while(1) {

        //Create socket:AF_INET for IPv4 and SOCK_STREAM for TCP
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            perror("Failed to create socket\n");
            return 1;
        }
        struct sockaddr_in serverAddress;
        //internet domain IPv4
        serverAddress.sin_family = AF_INET;
        //assignment of the server's IP address
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        //assign the port number to the sin_port field
        serverAddress.sin_port = htons(portnum);

        //bind serverAddress with sock
        if (bind(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
            perror("Failed to bind to port\n");
            return 1;
        }

        //create master thread
        if (pthread_create(&masterThread, NULL, master_thread, (void *) &sock) != 0) {
            perror("Failed to create master thread");
            return 1;
        }

        //wait master thread to finish
        pthread_join(masterThread, NULL);

    }

    return 0;
}




