/**
 * all help can be included in this function by differentiating using
 * arguements
 * 0  :  introduction about the program and the author
 **/
void introduce(int opt) {
        switch(opt) {
                case 0:
                        printf(" \
                                        Author  : Abijith K P \n \
                                        Email   : abijithkp@yahoo.co.in \n \
                                        Username: %s \n \
                                        Port No : %d \n \
                                        License : [] \n \
                                        For help type \"help\" (without the quotes) \n ", username, portNo);
                }
}
