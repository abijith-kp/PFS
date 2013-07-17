struct dict {
        char *key;
        char *value;
        struct dict *next;
};
struct dict *headDCS, *headDTS, *rmList, *addList, *modList, *unchList;
int lenDCS = 0, lenDTS = 0;

/**
 * >> all new nodes are added at the begining of the list
 * >> can be added in ascending order
 * >> provision for adding in ascending order is made in the code but hav to
 *    check for consistency
 **/
void addDict(struct dict **headRef, char *key, char *value) {
        struct dict *tmp;
        struct dict *tempD = *headRef, *prev = NULL;
        tmp = (struct dict *)malloc(sizeof(struct dict));
        
        tmp->key = strdup(key);
        tmp->value = strdup(value);
        tmp->next = NULL;
        
        while(tempD) {
                if(strcmp(tmp->key, key) > 0) {
                        break; 
                }
                prev = tempD;
                tempD = tempD->next;
        }
        
        tmp->next = tempD;

        if(!prev) {
                *headRef = tmp;
        }
        else {
                prev->next = tmp;
        }
}


void removeDict(struct dict **headRef, char *key, char *val) {
        struct dict *head = *headRef, *tmpDict, *prev;

        if(head) {
                if((!strcmp(head->key, key)) && (!strcmp(head->value, val))) {
                        *headRef = head->next;
                        free(head);
                        return;
                }
        }
        
        tmpDict = head->next;
        prev = head;

        while(tmpDict) {
                if((!strcmp(tmpDict->key, key)) && (!strcmp(tmpDict->value, val))) {
                        prev->next = tmpDict->next;
                        free(tmpDict);
                        return;
                }
                prev = tmpDict;
                tmpDict = tmpDict->next;
        }
        return;
}

char *findValDict(struct dict *head, char *key) {
        while(head) {
                if(!strcmp(head->key, key))
                        return head->value;
                head = head->next;
        }
        return NULL;
}

char *findKeyDict(struct dict *head, char *val) {
        while(head) {
                if(!strcmp(head->value, val))
                        return head->value;
                head = head->next;
        }
        return NULL;
}

/**
 * make a new copy of linked list starting with the head given as arguement
 **/
void makeNewDict(struct dict *headRef, struct dict **newHead) {
        while(headRef) {
                addDict(newHead, headRef->key, headRef->value);
                headRef = headRef->next;
        }
}

struct dict *killList(struct dict *head) {
        struct dict *tmp;
        
        while(head) {
                tmp = head->next;
                free(head);
                head = tmp;
        }
        return NULL;
}
