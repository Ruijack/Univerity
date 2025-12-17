#include <stdio.h>


int main (){
    //printf("Benvenuto giocatore!\n");
    //printf("Sarai pronto per affrontare l'avventura in Occhinz?\n");
    do {
        printf("      Cosestrane\n");
        printf("1)Imposta gioco\n");
        printf("2)Gioca\n");
        printf("3)Esci\n");
        printf("4)Crediti\n");
        printf("Inserisci il comando: ");

        int scelta, giocoImpostato = 0;
        fgets(&scelta, sizeof(scelta), stdin);

        switch (scelta) {
            case 1:
                imposta_Gioco();
                giocoImpostato = 1;
                break;
            case 2:
                if (!giocoImpostato) {
                    printf("Devi prima impostare il gioco!\n");
                }else {
                    gioca();
                }
                break;
            case 3:
                
                return 0;
            case 4:
                
                break;
            default:
                printf("Scelta non valida, inserire un comando valido.\n");
                break;
        }
    } while (scelta != 3);

    return 0;
}