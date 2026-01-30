#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamelib.h"

int main()
{
    time_t t;
    srand((unsigned)time(&t));
    // printf("Benvenuto giocatore!\n");
    // printf("Sarai pronto per affrontare l'avventura in Occhinz?\n");
    char scelta[4];
    int sceltaInt;
    int giocoImpostato = 0;
    do
    {
        printf("      CoseStrane\n");
        printf("1)Imposta gioco\n");
        printf("2)Gioca\n");
        printf("3)Esci\n");
        printf("4)Crediti\n");
        
        printf("Inserisci il comando: ");
        fgets(scelta, sizeof(scelta), stdin);

        sceltaInt = (int) strtol(scelta, NULL, 10);
        switch (sceltaInt)
        {
        case 1:
            imposta_gioco();
            giocoImpostato = 1;
            printf("Gioco impostato con successo!\n");
            break;
        case 2:
            if (!giocoImpostato)
            {
                printf("Devi prima impostare il gioco!\n");
            }
            else
            {
                printf("Inizio del gioco!\n");
                gioca();
                giocoImpostato = 0;
            }
            break;
        case 3:
            termina_gioco();
            break;
        case 4:
            crediti();
            break;
        default:
            printf("Scelta non valida, inserire un comando valido.\n");
            break;
        }
    } while (sceltaInt != 3);

    return 0;
}