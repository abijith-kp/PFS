struct sndStruct {
        int port;
        char *ip;
        char *name;
};

/**
 * port used for removing files is the default port(portNo).
 **/
void *syncFilesRemove(void *arg) {
        char packet2[100];
        struct dict *tmpRm = rmList;
        struct userNode *tmpUN = headUN;
        struct sockaddr_in dest;
        int rmSock, len;


        while(tmpRm) {
                memset(packet2, 0, sizeof(packet2));
                sprintf(packet2, "2 %s %s rm %d", username, tmpRm->value, (int)strlen(tmpRm->value));
                tmpUN = headUN;

                while(tmpUN) {
                        memset(&dest, 0, sizeof(struct sockaddr));
                        dest.sin_family = AF_INET;
                        dest.sin_addr.s_addr = inet_addr(tmpUN->ip);
                        dest.sin_port = htons(portNo);

                        rmSock = socket(AF_INET, SOCK_STREAM, 0);
                        if(connect(rmSock, (struct sockaddr *)&dest, sizeof(struct sockaddr)) == -1) {
                                logPFS("ERROR: SYNC: Files remove connecting problem \n");
                        }

                        len = send(rmSock, packet2, strlen(packet2), 0);
                        if(len < 0) {
                                logPFS("ERROR: SYNC: Files remove sending problem \n");
                        }
                        close(rmSock);
                        tmpUN = tmpUN->next;
                }
                tmpRm = tmpRm->next;
        }
        pthread_exit(NULL);
}

int checkPacket(char *packet, char *name) {
        int type;
        char ip[50], cond[10];

        sscanf(packet, "%d %s %s", &type, ip, cond);
        if(!strcmp(cond, "YES") && (type == 3) && !strcmp(ip, name))
                return 1;

        return 0;
}

/**
 * NEW FUNCTION: find username using ip
 **/
void *sndFile(void *arg) {
        char packet2[100], buffer[100], packet3[100], packet4[100], packet5[100];
        char packet[MAXSIZE+100];
        struct sndStruct *tmp;
        struct dict *tmpAdd = addList;
        tmp = (struct sndStruct *)arg;
        int fileSock, sndSock, len;
        struct sockaddr_in dest, serv;
        FILE *fp;

        memset(&dest, 0, sizeof(struct sockaddr));
        dest.sin_family = AF_INET;
        dest.sin_addr.s_addr = inet_addr(tmp->ip);
        dest.sin_port = htons(portNo);

        sndSock = socket(AF_INET, SOCK_STREAM, 0);

        if(connect(sndSock, (struct sockaddr *)&dest, sizeof(struct sockaddr)) == -1) {
                logPFS("ERROR: SYNC: Files remove connecting problem \n");
                pthread_exit(NULL);
        }

        memset(&serv, 0, sizeof(struct sockaddr));
        serv.sin_family = AF_INET;
        serv.sin_addr.s_addr = inet_addr(tmp->ip);
        serv.sin_port = htons(tmp->port);

        fileSock = socket(AF_INET, SOCK_STREAM, 0);
        bind(fileSock, (struct sockaddr *)&serv, sizeof(struct sockaddr));

        if(connect(fileSock, (struct sockaddr *)&serv, sizeof(struct sockaddr)) == -1) {
                logPFS("ERROR: SYNC: Files remove connecting problem \n");
                pthread_exit(NULL);
        }

        memset(packet5, 0, sizeof(packet5));
        sprintf(packet5, "5 %s _ALL end %d", username, 0);
        /**
         * packet 3 : 3 username YES
         * this is just an acknoledgement for the setting up of a listerner at the given port
         * packet 4 is to send a message that it is the end of a file packet 5 is to give the 
         * indication od the end of syncronisation
         **/

        while(tmpAdd) {
                memset(packet2, 0, sizeof(packet2));
                memset(packet3, 0, sizeof(packet3));
                memset(packet4, 0, sizeof(packet4));
                sprintf(packet2, "2 %s %s add %d %d", username, tmpAdd->value, (int)strlen(tmpAdd->value), tmp->port);
                sprintf(packet4, "2 %s %s end %d", username, tmpAdd->value, (int)strlen(tmpAdd->value));
                        
                len = send(sndSock, packet2, strlen(packet2), 0);
                if(len < 0) {
                        logPFS("ERROR: SYNC: Files remove sending problem \n");
                }

                while(!pthread_mutex_trylock(&fileManage)) ;
                printf("\n POPQJJKNIOONN \n");
                        
                fp = fopen(tmpAdd->value, "r");
                while(!feof(fp)) {
                        fgets(buffer, MAXSIZE, fp);
                        sprintf("10 %s %s %s", username, buffer, tmpAdd->value);
                        len = send(fileSock, buffer, strlen(buffer), 0);
                        if(len < 0) {
                                logPFS("ERROR: SYNC: Files remove sending problem \n");
                        }
                }
                fclose(fp);
                        
                len = send(fileSock, packet4, strlen(packet4), 0);
                if(len < 0) {
                        logPFS("ERROR: SYNC: Files remove sending problem \n");
                }

                pthread_mutex_lock(&fileManage);
                tmpAdd = tmpAdd->next;
        }
        
        len = send(fileSock, packet5, strlen(packet5), 0);
        if(len < 0) {
                logPFS("ERROR: SYNC: Files remove sending problem \n");
        }
  
        close(sndSock);
        close(fileSock);
        pthread_exit(NULL);
}

void *syncFilesAdd(void *arg) {
        struct userNode *tmpUN = headUN;
        int i = 0, rslt, startPort = portNo+1;

        while(tmpUN) {
                pthread_t id;
                struct sndStruct *tmpSS;
                tmpSS = (struct sndStruct *)malloc(sizeof(struct sndStruct));
                memset(tmpSS, 0, sizeof(struct sndStruct));

                tmpSS->port = startPort++;
                tmpSS->ip = strdup(tmpUN->ip);
                // tmpSS->name = strdup(tmpUN->user);

                rslt = pthread_create(&id, NULL, sndFile, (void *)tmpSS);
                if(rslt < 0) {
                        printf("\n ERROR \n");
                }
                tmpUN = tmpUN->next;
        }
        pthread_exit(NULL);
}

void syncFiles() {
        pthread_t idRm, idAdd;
        int rsltRm, rsltAdd;

        logPFS("SYNC: Syncing temporary file list with current file list\n");

        if(rmList) {
                rsltRm = pthread_create(&idRm, NULL, syncFilesRemove, NULL);
                if(rsltRm < 0) {
                        logPFS("ERROR: SYNC: pthread creation error \n");
                }
        }
        
        if(addList) {
                rsltAdd = pthread_create(&idAdd, NULL, syncFilesAdd, NULL);
                if(rsltAdd < 0) {
                        logPFS("ERROR: SYNC: pthread creation error \n");
                }
        }
}
