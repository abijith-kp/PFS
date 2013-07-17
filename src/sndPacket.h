/**
 * packet1 : 1 len(username) username
 **/
void *sndPacketFunct(void *arg) {
        char logger[100], packet[50]; /* +1 so we can add null terminator */
        int len, max=0, tmpSock;
        struct sockaddr_in dest; 
        struct timeval t;
        struct userNode *tmp;

        sprintf(packet, "1 %s", username);
        
        while(1) {
                /**
                 * time interval between sending of active packet(packet 1)
                 * time = 5 sec
                 **/
                t.tv_sec = 5;
                t.tv_usec = 0;

                select(max, NULL, NULL, NULL, &t);
                tmp = headUN;
                
                while(tmp) {
                        memset(&dest, 0, sizeof(dest));                 /* zero the struct */
                        dest.sin_family = AF_INET;
                        dest.sin_addr.s_addr = inet_addr(tmp->ip);      /* set destination IP number */ 
                        dest.sin_port = htons(portNo);                  /* set destination port number */
                        
                        tmpSock = socket(AF_INET, SOCK_STREAM, 0);
                        sprintf(logger, "LOOPING: activateFunc - %s %s\n", tmp->ip, tmp->user);
                        logPFS(logger);

                        if(connect(tmpSock, (struct sockaddr *)&dest, sizeof(struct sockaddr)) == -1) {
                                logPFS("ERROR: activateFunc - connection error\n");
                                /**
                                 * removeUser and addUser functions should be
                                 * united together
                                 **/
                                tmp = removeUser(tmp->user);
                                continue;
                        }

                        len = send(tmpSock, packet, strlen(packet), 0); 
                        if(len < 0) {
                                logPFS("ERROR: activateFunc - sending error\n");
                        }
                        close(tmpSock);
                        tmp = tmp->next;
                }
        }
}
