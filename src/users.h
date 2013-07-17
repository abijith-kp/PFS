struct userNode {
        char *ip;
        char *user;
        struct sockaddr_in conIp;
        int sock;
        struct userNode *next;
} *headUN;

int lenHUN = 0;

/**
 * arguement will be ip of the user
 **/
void addUser(char *ip, char *name) {
        logPFS("USERADD: replyPacket\n");
        struct userNode *tmp; 
        
        tmp = headUN;
        while(tmp) {
                if(!strcmp(ip, tmp->ip)) {
                        return;
                }
                tmp = tmp->next;
        }

        tmp = (struct userNode *)malloc(sizeof(struct userNode));
        tmp->ip = strdup(ip);
        tmp->user = strdup(name);
        tmp->next = headUN;
        headUN = tmp;
        lenHUN++;
}

char *findUser(char *ip) {
        struct userNode *head = headUN;

        while(head) {
                if(!strcmp(head->ip, ip)) {
                        return strdup(head->user);
                }
                head = head->next;
        }

        return NULL;
}

/**
 * removeUser uses userNode structure
 **/
struct userNode *removeUser(char *user) {
        struct userNode *tmp, *prev;

        if(!headUN)
                return NULL;

        if(!strcmp(headUN->user, user)) {
                headUN = headUN->next;
                lenHUN--;
                return headUN;
        }

        tmp = headUN->next;
        prev = headUN;
        while(tmp) {
                if(!strcmp(tmp->user, user)) {
                        prev->next = tmp->next;
                        lenHUN--;
                        return prev->next;
                }
                else {
                        prev = tmp;
                        tmp = tmp->next;
                }
        }
        return NULL;
}
