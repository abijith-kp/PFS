void addFiles(void *arg) {
        FILE *fp2;
        char key[100], val[100];
        char *dt, command[100];
        int t=0;
        
        dt = (char *)arg;
        
        memset(key, 0, sizeof(key));
        sprintf(key, "PROCESSING: %s\n", dt);
        logPFS(key);

        if(!strcmp(dt, TSPath)) {
                pthread_mutex_lock(&TSum);
                memset(command, 0, sizeof(command));
                sprintf(command, "%s 1", runScript);
                system(command);
                headDTS = killList(headDTS);
                t = 1;
        }
        else {
                pthread_mutex_lock(&CSum);
                system(runScript);
                headDCS = killList(headDCS);
        }
        
        fp2 = fopen(dt, "r");
        while(!feof(fp2)) {
                memset(key, 0, sizeof(key));
                memset(val, 0, sizeof(val));
                fscanf(fp2, "%s  %s\n", key, val);                
                if(t == 0) {
                        addDict(&headDCS, key, val);
                        lenDCS++;
                }
                else {
                        addDict(&headDTS, key, val);
                        lenDTS++;
                }
        }
        fclose(fp2);

        if(t == 1) {
                pthread_mutex_unlock(&TSum);
        }
        else {
                pthread_mutex_unlock(&CSum);
        }
}

/**
 * for syncing removed files
 **/
void removeFile(char *addr) {
        char *pt = rindex(addr, '/');
        char cmdPath[100];
        sprintf(cmdPath, "rm -f %s%s", root, pt);
        system(cmdPath);
}

/**
 * something like string matching should be implemented here to compare the two
 * linked lists for removing the nodes having same members
 **/
void compareFiles(struct dict **chkSumDict, struct dict **tmpSumDict) {
        struct dict *tmpDTS, *tmpDCS, *prevDTS = NULL, *prevDCS = NULL;
        struct dict *newHead;
        int flag = 0;

        addList = killList(addList);
        rmList = killList(rmList);
        modList = killList(modList);
        unchList = killList(unchList);

        makeNewDict(*tmpSumDict, &newHead);
        
        memset(&tmpDCS, 0, sizeof(struct dict));
        memset(&tmpDTS, 0, sizeof(struct dict));

        tmpDCS = *chkSumDict;
        tmpDTS = *tmpSumDict;

        while(tmpDCS) {
                tmpDTS = *tmpSumDict;
                prevDTS = NULL;
                while(tmpDTS && tmpDCS) { 
                        /**
                         * for removing unchanged files from both the lists
                         **/
                        flag = 0;
                        if(!strcmp(tmpDCS->value, tmpDTS->value) && (!strcmp(tmpDCS->key, tmpDTS->key))) {
                                addDict(&unchList, tmpDCS->key, tmpDCS->value);
                                flag = 1;
                        }
                        /**
                         * for adding modified files to modList
                         **/
                        else if(!strcmp(tmpDCS->value, tmpDTS->value) && (strcmp(tmpDCS->key, tmpDTS->key))) {
                                addDict(&modList, tmpDCS->key, tmpDCS->value);
                                flag = 1;
                        }

                        if(flag) {
                                if(!prevDTS) {
                                        *tmpSumDict = (*tmpSumDict)->next;
                                        tmpDTS = *tmpSumDict;
                                }
                                else {
                                        prevDTS->next = tmpDTS->next;
                                        free(tmpDTS);
                                        tmpDTS = prevDTS->next;
                                }
                                if(!prevDCS) {
                                        *chkSumDict = (*chkSumDict)->next;
                                        tmpDCS = *chkSumDict;
                                }
                                else {
                                        prevDCS->next = tmpDCS->next;
                                        free(tmpDCS);
                                        tmpDCS = prevDCS->next;
                                }
                                continue;
                        }
                        prevDTS = tmpDTS;
                        tmpDTS = tmpDTS->next;
                }
                prevDCS = tmpDCS;
                if(!tmpDCS)
                        break;
                tmpDCS = tmpDCS->next;
        }
        /**
         * >> *tmpSumDict contains added list of files
         * >> *chkSumDict contains removed list of files
         **/
        makeNewDict(*tmpSumDict, &addList);
        makeNewDict(*chkSumDict, &rmList);

        *chkSumDict = killList(*chkSumDict);
        *tmpSumDict = killList(*tmpSumDict);
        *chkSumDict = newHead;
}

void *checkFiles(void *arg) {
        struct timeval t;
        int rep = 0;

        while(1) {
                logPFS("FILECHECK: checkFiles\n");
                t.tv_sec = 10;
                t.tv_usec = 0;
                select(rep, NULL, NULL, NULL, &t);
                system(runScript);
                /**
                 * make the temporary dictionary and compare it with the
                 * original one and do the corresponding actions
                 * there would be 4 different cases to check for
                 **/
                
                addFiles((void *)TSPath);
                
                /**
                 * keeps the details about the files in root/ directory updated
                 * check-update command gives the update information
                 * if update command is issued then all files in all the other
                 * computers
                 **/
                compareFiles(&headDCS, &headDTS);
                syncFiles();
        }
}
