    /* Non viene utilizzato come file "solitario" in quanto è stato inglobato
    come funzione all'interno di hashTable.c */
    
    unsigned long
    hash(unsigned char *str)
    {
        unsigned long hash = 5381;
        int c;

        while (c = *str++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

        return hash;
    }

    void main(void){
        printf("Hash is: %ld", hash("Ciao a tutti quanti."));
        printf("Hash is: %ld", hash("CIAO A TUTTI quanti."));
    }