struct info {
        char *file;
        int port;
        char *ip;
};

void *syncFileServ(void *arg) {
        int addSock, conSock, lenSock, type, i=0, len;
        struct info *tmp;
        struct sockaddr_in dest, temp;
        char buffer[MAXSIZE+100], user[50], fileName[100], cmd[30], data[MAXSIZE];
        FILE *tmpFp;

        tmp = (struct info *)arg;

        /**
         * add the filename to root address
         **/
        tmpFp = fopen(tmp->file, "a");
        memset(&dest, 0, sizeof(struct sockaddr));
        dest.sin_family = AF_INET;                /* set the type of connection to TCP/IP */
        dest.sin_addr.s_addr = inet_addr(tmp->ip);    /* set our address to any interface */
        dest.sin_port = htons(tmp->port);
        
        addSock = socket(AF_INET, SOCK_STREAM, 0);
        listen(addSock, 5);
        bind(addSock, (struct sockaddr *)&temp, sizeof(struct sockaddr));
        lenSock = sizeof(struct sockaddr);

        printf("\n wpojwm23e0329x0x20e392ie,302e32e230ei230ie230 \n");
        while(1) {
                printf("\n INSIDE 999999 \n");
                conSock = accept(addSock, (struct sockaddr *)&dest, &lenSock);
                printf("\n outside 999999 \n");
                
                if (conSock < 0) {
                        logPFS("ERROR: activateFunc - on accept\n");
                        exit(1);
                }

                if((len = recv(conSock, buffer, (MAXSIZE+100), 0)) <= 0) {
                        logPFS("ERROR: activateFunc - Recieving error\n");
                        exit(1);
                }

                sscanf(buffer, "%d %s %s %s", &type, user, data, fileName);

                if((type == 10) && (!strcmp(user, findUser(tmp->ip)) && (!strcmp(fileName, tmp->file)))) {
                        fputs(data, tmpFp);
                }
                else if((type == 2) && (!strcmp(user, findUser(tmp->ip))) && (!strcmp(fileName, tmp->file))) {
                        fclose(tmpFp);
                        break;
                }
        }
        close(addSock);
}

void *serverFunct(void *arg) {
        int sndSock, len, consocket, port;
        int type, size, tmpSock;
        char packet3[100], name[20], buffer[MAXRCVLEN + 1], file[100], command[20], log[100];

        sndSock = socket(AF_INET, SOCK_STREAM, 0);
        
        struct sockaddr_in dest;                  /* socket info about the machine connecting to us */
        struct sockaddr_in serv;                  /* socket info about our server */
        socklen_t socksize;                       /* = sizeof(struct sockaddr_in); */
        
        memset(&serv, 0, sizeof(serv));           /* zero the struct before filling the fields */
        serv.sin_family = AF_INET;                /* set the type of connection to TCP/IP */
        serv.sin_addr.s_addr = htonl(INADDR_ANY); /* set our address to any interface */
        serv.sin_port = htons(portNo);            /* set the server port number */    
        
        

        /** 
         * bind serv information to sndSock 
         **/
        bind(sndSock, (struct sockaddr *)&serv, sizeof(struct sockaddr));
        
        /** 
         * start listening, allowing a queue of up to 1 pending connection 
         **/

        listen(sndSock, 5);
        bzero(&dest, sizeof(dest));
        socksize = sizeof(struct sockaddr_in);
                
        while(1) {
                consocket = accept(sndSock, (struct sockaddr *)&dest, &socksize);
                if (consocket < 0) {
                        logPFS("ERROR: activateFunc - on accept\n");
                        exit(1);
                }

                if((len = recv(consocket, buffer, MAXRCVLEN, 0)) <= 0) {
                        logPFS("ERROR: activateFunc - Recieving error\n");
                        exit(1);
                }
                buffer[len] = '\0';
                memset(name, 0, sizeof(name));
                memset(file, 0, sizeof(file));
                memset(command, 0, sizeof(command));
                memset(log, 0, sizeof(log));
                /**
                 * T N F C S P
                 * T ==> type of packet         S ==> size of file or file name length
                 * C ==> command                N ==> username or ip
                 * F ==> file name or username  P ==> port number
                 **/
                sscanf(buffer, "%d %s %s %s %d %d", &type, name, file, command, &size, &port);

                if(type == 1) {
                        /**
                         * function that creates a list of online users.
                         * aguements are: dest and consocket
                         **/
                        addUser(inet_ntoa(dest.sin_addr), name);
                }
                else if(type == 2) {
                        if(!strcmp(command, "rm")) {
                                sprintf(log, "SYNC: removing %s ; CMD from %s \n", file, name);
                                logPFS(log);
                                removeFile(file);
                                /**
                                 * send packet buffer to those computers connected to this computer.
                                 * MODIFICATION: this will be automatically dealt by checkFiles function when time comes.
                                 **/
                        }
                        else if(!strcmp(command, "add")) {
                                /**
                                 * recieve packet 2 ==> send an acknoledgement as packet 3 ==> recieve the new file at the
                                 * port given in the first packet ==> recieve packet 4 as the end of each files ==>
                                 * revieve packet 5 as the end of syncronisation
                                 **/
                                pthread_t id;
                                
                                tmpSock = socket(AF_INET, SOCK_STREAM, 0);
                                dest.sin_family = AF_INET;
                                dest.sin_port = htons(portNo);
                                if(connect(tmpSock, (struct sockaddr *)&dest, sizeof(struct sockaddr)) == -1) {
                                        printf("\n ERROR \n");
                                        continue;
                                }
                                
                                memset(packet3, 0, sizeof(packet3));
                                sprintf(packet3, "3 %s YES", username);

                                printf("\n %s \n", packet3);
                                len = send(tmpSock, packet3, strlen(packet3), 0);
                                if(len < 0) {
                                        printf("\n ERROR \n");
                                }

                                close(tmpSock);
                                struct info *tmp;
                                tmp = (struct info *)malloc(sizeof(struct dict));
                                memset(tmp, 0, sizeof(struct dict));

                                tmp->file = strdup(file);
                                tmp->port = port;
                                tmp->ip = inet_ntoa(dest.sin_addr);
                                pthread_create(&id, NULL, syncFileServ, (void *)tmp);
                        }
                }
                else if(type == 3) {
                        pthread_mutex_unlock(&fileManage);
                        printf("\n ::::::::::::::::: \n");
                }
        }
        close(sndSock);
}
