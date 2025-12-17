#include <stdio.h>
#include <stdlib.h>
#include "gamelib.h"

typedef struct Giocatore Giocatore;




static void crea_mappa() { // crea 15 zone
    
}

static void inserisci_zona() {
    
}

static void cancella_zona() {
    
}

static void stampa_mappa() {
    
}

static void stampa_zona() {
    
}

static void chiudi_mappa() {
    
}

static int lanciaD20(){
    return rand() %20 + 1;
}

static void sceltaAttributi(Giocatore* giocatore){
    printf("Ora puoi scegliere tra:\n");
    printf("1)Attacco_psichico +3 punti e difesa_psichica -3 punti.\n");
    printf("2)Difesa_psichica +3 punti e attacco_psichico -3 punti.\n");
    printf("3)Stai bene così.\n");

    if (!esisteVirgola) {
        printf("4)Diventa UndiciVirgolaCinque (attacco_psichico e difesa_psichica +4, fortuna -7).\n");
        printf("Attenzione: il tuo nome verra cambiano in UndiciVirgolaCinque e solo un giocatore per partita può fare questa scelta.\n");
    }
    int scelta;
    printf("Fai la tua scelta: ");
    fgets(&scelta, sizeof(scelta), stdin);

    if(scelta < 1 || scelta > 4 || (scelta == 4 && esisteVirgola)) {
        printf("Scelta non valida. Riprova.\n");
        sceltaAttributi(&giocatore); // Ripeti il turno per lo stesso giocatore
    } else {
        switch (scelta) {
            case 1:
                giocatori[i].attacco_psichico += 3;
                giocatori[i].difesa_psichica -= 3;
                break;
            case 2:
                giocatori[i].difesa_psichica += 3;
                giocatori[i].attacco_psichico -= 3;
                break;
            case 3:
                // Nessuna modifica
                break;
            case 4:
                giocatori[i].attacco_psichico += 4;
                giocatori[i].difesa_psichica += 4;
                giocatori[i].fortuna -= 7;
                strcpy(giocatori[i].nome, "UndiciVirgolaCinque");
                esisteVirgola = 1;
                break;
            }
        }
}

void imposta_Gioco() {
    printf("Inserire il numero di giocatori (max 4): ");
    static int esisteVirgola = 0;
    int num_giocatori;
    fgets(&num_giocatori, sizeof(num_giocatori), stdin);
    if (num_giocatori < 1 || num_giocatori > 4){
        printf("Numero di giocatori non valido. Riprova.\n");
        imposta_Gioco();
    }else{
        Giocatore *giocatori = (Giocatore *) calloc(4, sizeof(Giocatore));

        for (int i = 0; i < num_giocatori; i++) {
            printf("Inserire il nome del giocatore %d (Max 50 caratteri): ", i + 1);
            fgets(giocatori[i].nome, sizeof(giocatori[i].nome), stdin);
            giocatori[i].mondo = 0; // Inizialmente nel mondo reale
            giocatori[i].pos_mondoreale = 0; 
            giocatori[i].pos_soprasotto = 0; 
            printf("Le tue statistiche verrano generate casualmente (tirando un  D20 per ogni statistica).\n");
            giocatori[i].attacco_psichico = lanciaD20();
            giocatori[i].difesa_psichica = lanciaD20();
            giocatori[i].fortuna = lanciaD20();

            printf("Il tuo attacco psichico è: %d\n (max 20)", giocatori[i].attacco_psichico);
            printf("La tua difesa psichica è: %d\n (max 20)", giocatori[i].difesa_psichica);
            printf("La tua fortuna è: %d\n (max 20)", giocatori[i].fortuna);

            sceltaAttributi(&giocatori[i]);
            
        }
    }
}