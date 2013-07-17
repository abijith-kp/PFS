pthread_mutex_t logMutex;

/**
 * for logging errors and working flow of the program
 **/
void logPFS(char *s) {
        FILE *fp;
        int len;

        pthread_mutex_lock(&logMutex);
        fp = fopen(LPath, "a");
        len = strlen(s);
        write(fileno(fp), s, len);
        fclose(fp);
        pthread_mutex_unlock(&logMutex);
}
