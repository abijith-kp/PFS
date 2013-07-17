#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <error.h>

#define MAXRCVLEN 500
#define MAXSIZE 500

int portNo = 0, transferPort = 0;
char *username;

pthread_mutex_t CSum, TSum;
pthread_mutex_t fileManage;

#include "path.h"
#include "log.h"
#include "dict.h"
#include "users.h"
#include "transfer.h"
#include "files.h"
#include "init.h"
#include "server.h"
#include "sndPacket.h"
#include "help.h"

int main(int argc, char *argv[]) {
        pthread_t id1, id2, id3;
        char command[100], log[100];
        
        /**
         * argv[1] = portNo
         * argv[2] = username;
         **/
        if(argc < 3) {
                printf("ERROR: Few number of arguements\n usage: %s port username\n", argv[0]);
                memset(log, 0, sizeof(log));
                sprintf(log, "ERROR: main - Few number of arguements\n usage: %s port username\n", argv[0]);
                logPFS(log);
                exit(1);
        }

        portNo = atoi(argv[1]);
        transferPort = portNo+1;
        username = strdup(argv[2]);

        memset(log, 0, sizeof(log));
        sprintf(log, "username: %s ;; port no: %d\n", username, portNo);
        logPFS(log);

        introduce(0);

        /**
         * initialize the file to linked list 
         * file format: username ip
         **/
        initialize();

        /**
         *Function that sends the message that this process is still active.
         **/
        pthread_create(&id1, NULL, sndPacketFunct, NULL);
        pthread_detach(id1);
        sched_yield();
        
        pthread_create(&id2, NULL, serverFunct, NULL);
        pthread_detach(id2);
        sched_yield();
        
        /**
         * this should start only after initialisation is over
         * even when working with thread
         **/
        pthread_create(&id3, NULL, checkFiles, NULL);
        pthread_detach(id3);
        sched_yield();
        
        /**
         * command line for file management
         **/
        while(1) {
                fflush(stdout);
                printf("\n>> ");
                memset(command, 0, sizeof(command));
                scanf("%s", command);
                
                memset(log, 0, sizeof(log));
                sprintf(log, "COMMAND: %s\n", command);
                logPFS(log);

                if(!strcmp(command, "ls")) {
                        memset(command, 0, sizeof(command));
                        sprintf(command, "ls %s", root);
                        system(command);
                }
                else if(!strcmp(command, "exit")) {
                /**
                 * cancel all the threads craeted earlier before the program
                 * exists
                 **/
                        pthread_cancel(id1);
                        pthread_cancel(id2);
                        pthread_cancel(id3);
                        break;
                }
                else if(!strcmp(command, "list")) {
                        struct userNode *tmp = headUN;
                        while(tmp) {
                                printf("\n%s  %s", tmp->ip, tmp->user);
                                tmp = tmp->next;
                        }
                }
                else if(!strcmp(command, "show")) {
                        struct dict *tmp = addList;
                        printf("\nadd list: \n");
                        while(tmp) {
                                printf("%s\n", tmp->value);
                                tmp = tmp->next;
                        }
                        tmp = rmList;
                        printf("\nrm list: \n");
                        while(tmp) {
                                printf("%s\n", tmp->value);
                                tmp = tmp->next;
                        }
                        tmp = modList;
                        printf("\nmod list: \n");
                        while(tmp) {
                                printf("%s\n", tmp->value);
                                tmp = tmp->next;
                        }
                        tmp = unchList;
                        printf("\nunchanged list: \n");
                        while(tmp) {
                                printf("%s\n", tmp->value);
                                tmp = tmp->next;
                        }
                }
                else {
                        logPFS("ERROR: command not found\n");
                        printf("ERROR: %s - command not found\n", command);
                }
        }

        logPFS("EXIT: PFS terminating\n");
        return EXIT_SUCCESS;
}
