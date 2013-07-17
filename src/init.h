void *temp(void *arg) {
        struct userNode *tmp;
        char user[100];
        int i = 0;

        tmp = headUN;
        while(tmp) {
                sprintf(user, "USER%d: %s %s\n", ++i, tmp->user, tmp->ip);
                logPFS(user);
                tmp = tmp->next;
        }

        pthread_exit(NULL);
}

void initialize() {
        logPFS("\n[PFS log file]\n");
        
        char user[100], addr[100];
        struct userNode *tmp;
        pthread_t id;
        FILE *fp1;
        
        addFiles((void *)CSPath); 
        
        /**
         * to create the userNode list from the file data/users
         **/
        headUN = NULL;
        
        fp1 = fopen(UPath, "r");
        
        logPFS("INIT: creating user data structure\n");
        while(!feof(fp1)) {
                tmp = (struct userNode *)malloc(sizeof(struct userNode));
                fscanf(fp1, "%s %s\n", user, addr);
                tmp->user = strdup(user);
                tmp->ip = strdup(addr);
                tmp->sock = -1;
                tmp->next = headUN;
                headUN = tmp;
                lenHUN++;
        }
        fclose(fp1);

        id = pthread_create(&id, NULL, temp, NULL);

        logPFS("INIT: Finishing init\n");
}
