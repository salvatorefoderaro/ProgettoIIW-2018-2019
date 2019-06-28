#include <stdio.h>
#include <string.h>

int main(void){
        char accept[1024] = "image/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8";
        char *restoDelMondo = accept;
        restoDelMondo = strtok(restoDelMondo, ":");
        restoDelMondo = strtok(restoDelMondo, NULL);
		do {
            printf("\nResto del mondo è: %s\n", restoDelMondo);
            char *string = strtok(restoDelMondo,";");
            char *qValue = strtok(NULL, ",");
		    restoDelMondo = strtok(NULL, ",");
            if (strstr(string, "image") != NULL){
                qValue = strtok(qValue, "=");
                qValue = strtok(NULL, ".");
                qValue = strtok(NULL, "\0");
                printf("\nEureka!\n");
                printf("\nAAA s: %d", atoi(qValue)*10);
            }
		    printf("\n%s\n%s\n%s\n", string, restoDelMondo, qValue);
        } while(restoDelMondo != NULL);
        return 0;
}