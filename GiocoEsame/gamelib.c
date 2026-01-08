#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gamelib.h"

typedef struct Giocatore Giocatore;
typedef struct Zona_mondoreale Mondoreale;
typedef struct Zona_soprasotto Soprasotto;
typedef enum Tipo_zona zona;
typedef enum Tipo_nemico nemico;
typedef enum Tipo_oggetto oggetto;

static int esisteVirgola = 0;
static int esisteDemotorzone = 0;
static Mondoreale *prima_zona_mondoreale;
static Soprasotto *prima_zona_soprasotto;
static int numZone = 0;
static int tipi_nemici = 4;

// ritorna un numero casuale tra 0 e 4 eccetto l'eccezione
static int rand_exept(int eccezione)
{
    int r;
    do
    {
        r = rand() % tipi_nemici;
    } while (r == eccezione);

    if (r == 3)
    {
        esisteDemotorzone = 1;
        tipi_nemici = 3;
    }
    return r;
}
/**
 * se zona_reale->avanti è == a prima_zona_mondoreale quindi
 * zona_reale è l'ultima zona_reale
 */
static void crea_zona_fine()
{
    Mondoreale *nuova_zona_reale = (Mondoreale *)malloc(sizeof(Mondoreale));
    Soprasotto *nuova_zona_soprasotto = (Soprasotto *)malloc(sizeof(Soprasotto));

    Mondoreale *ultima_zona_reale = prima_zona_mondoreale->indietro;
    Soprasotto *ultima_zona_soprasotto = prima_zona_soprasotto->indietro;

    nuova_zona_reale->nemico = rand_exept(3);
    nuova_zona_reale->oggetto = rand() % 5;
    nuova_zona_reale->tipo = rand() % 10;
    nuova_zona_reale->link_soprasotto = nuova_zona_soprasotto;
    nuova_zona_reale->avanti = prima_zona_mondoreale;
    nuova_zona_reale->indietro = ultima_zona_reale;

    nuova_zona_soprasotto->tipo = nuova_zona_reale->tipo;
    nuova_zona_soprasotto->link_mondoreale = nuova_zona_reale;
    nuova_zona_reale->nemico = rand_exept(1);
    nuova_zona_soprasotto->avanti = prima_zona_soprasotto;
    nuova_zona_soprasotto->indietro = ultima_zona_soprasotto;

    ultima_zona_reale->avanti = nuova_zona_reale;
    ultima_zona_soprasotto->avanti = nuova_zona_soprasotto;

    prima_zona_mondoreale->indietro = nuova_zona_reale;
    prima_zona_soprasotto->indietro = nuova_zona_soprasotto;

    numZone++;
}

static void crea_mappa()
{ // crea 15 zone
    prima_zona_mondoreale = (Mondoreale *)malloc(sizeof(Mondoreale));
    prima_zona_soprasotto = (Soprasotto *)malloc(sizeof(Soprasotto));

    Mondoreale *zona_reale = prima_zona_mondoreale;
    Soprasotto *zona_soprasotto = prima_zona_soprasotto;

    // creazione prima zona
    zona_reale->link_soprasotto = zona_soprasotto;
    zona_reale->tipo = rand() % 10;
    zona_reale->oggetto = rand() % 5;
    zona_reale->nemico = rand_exept(3);
    zona_reale->avanti = zona_reale;
    zona_reale->indietro = zona_reale;

    zona_soprasotto->link_mondoreale = zona_reale;
    zona_soprasotto->tipo = zona_reale->tipo;
    zona_soprasotto->nemico = rand_exept(1);
    zona_soprasotto->avanti = zona_soprasotto;
    zona_soprasotto->indietro = zona_soprasotto;

    numZone++;
    for (int i = 0; i < 14; i++)
    {
        crea_zona_fine();
    }
}

static void inserisci_zona(int posto)
{
}

static void cancella_zona(int posto)
{
}

static void stampa_mappa()
{
    char scelta[2];
    printf("Quale mappa, mondo reale(1) o soprasotto(2)?");
    fgets(&scelta, sizeof(scelta), stdin);
    if (scelta[1] != 1 || scelta[1] != 2)
    {
        printf("Scelta non valida. Riprova.\n");
        stampa_mappa();
    }
    else
    {
        switch (scelta[1])
        {
        case 1:
            int i = 0;
            printf("Mappa del mondo reale");
            Mondoreale *pScan = prima_zona_mondoreale;
            do
            {
                printf("Zona %d", i);
                printf("Tipo: %d\n", pScan->tipo);
                printf("Nemico: %d\n", pScan->nemico);
                printf("Oggetto: %d\n", pScan->oggetto);
                pScan = pScan->avanti;
                i++;
            } while (pScan != prima_zona_mondoreale);
            break;

        case 2:
            int i = 0;
            printf("Mappa del soprasotto");
            Soprasotto *pScan = prima_zona_soprasotto;
            do
            {
                printf("Zona %d", i);
                printf("Tipo: %d\n", pScan->tipo);
                printf("Nemico: %d\n", pScan->nemico);
                pScan = pScan->avanti;
                i++;
            } while (pScan != prima_zona_soprasotto);
            break;
        }
    }
}

static void stampa_zona()
{
}

static void chiudi_mappa()
{
}

static int lanciaD20()
{
    return rand() % 20 + 1;
}

static void scelta_attributi(Giocatore *giocatore)
{
    char scelta[2];
    printf("Fai la tua scelta (ATTENZIONE le statistiche non possono superare 20 o diminuire più di 1): ");
    fgets(scelta, sizeof(scelta), stdin);

    if (scelta[1] < 1 || scelta[1] > 4 || (scelta[1] == 4 && esisteVirgola))
    {
        printf("Scelta non valida. Riprova.\n");
        scelta_attributi(giocatore); // Ripeti il turno per lo stesso giocatore
    }
    else
    {
        switch (scelta[1])
        {
        case 1:
            if (giocatore->attacco_psichico + 3 > 20 || giocatore->difesa_psichica - 3 < 1)
            {
                printf("Scelta non valida. riprova.\n");
                sceltaAttributi(giocatore);
            }
            else
            {
                giocatore->attacco_psichico += 3;
                giocatore->difesa_psichica -= 3;
            }

            break;
        case 2:
            if (giocatore->attacco_psichico - 3 < 0 || giocatore->difesa_psichica + 3 > 20)
            {
                printf("Scelta non valida. riprova.\n");
                sceltaAttributi(giocatore);
            }
            else
            {
                giocatore->difesa_psichica += 3;
                giocatore->attacco_psichico -= 3;
            }
            break;
        case 3:
            // Nessuna modifica
            break;
        case 4:
            if (giocatore->attacco_psichico + 4 > 20 || giocatore->difesa_psichica)
                giocatore->attacco_psichico += 4;
            giocatore->difesa_psichica += 4;
            giocatore->fortuna -= 7;
            strcpy(giocatore->nome, "UndiciVirgolaCinque");
            esisteVirgola = 1;
            break;
        }
    }
}

static void scelta_mappa()
{
    printf("Inserisci la tua scelta: ");
    char scelta[2];
    fgets(&scelta, sizeof(scelta), stdin);

    if (scelta[1] < 1 || scelta[1] > 6)
    {
        printf("Scelta non valida. riprova.\n");
        scelta_mappa();
    }

    switch (scelta[1])
    {
    case 1:
        crea_mappa();
        break;
    case 2:
        inserisci_zona(0); // da implementare

        break;
    case 3:
        cancella_zona(0); // da implementare
        break;
    case 4:
        stampa_mappa();
        break;
    case 5:
        stampa_zona();
        break;
    case 6:
        chiudi_mappa();
        break;
    }
}

void imposta_Gioco()
{
    printf("Inserire il numero di giocatori (max 4): ");

    char num_giocatori[2];
    fgets(num_giocatori, sizeof(num_giocatori), stdin);
    if (num_giocatori[1] < 1 || num_giocatori[1] > 4)
    {
        printf("Numero di giocatori non valido. Riprova.\n");
        imposta_Gioco();
    }
    else
    {
        Giocatore *giocatori = (Giocatore *)calloc(4, sizeof(Giocatore));

        for (int i = 0; i < num_giocatori[1]; i++)
        {
            printf("Inserire il nome del giocatore %d (Max 50 caratteri): ", i + 1);
            fgets(giocatori[i].nome, sizeof(giocatori[i].nome), stdin);
            giocatori[i].mondo = 0; // Inizialmente nel mondo reale
            giocatori[i].pos_mondoreale = 0;
            giocatori[i].pos_soprasotto = 0;
            printf("Le tue statistiche verrano generate casualmente (tirando un  D20 per ogni statistica).\n");
            giocatori[i].attacco_psichico = lanciaD20();
            giocatori[i].difesa_psichica = lanciaD20();
            giocatori[i].fortuna = lanciaD20();

            printf("Il tuo attacco psichico è: %d\n", giocatori[i].attacco_psichico);
            printf("La tua difesa psichica è: %d\n", giocatori[i].difesa_psichica);
            printf("La tua fortuna è: %d\n)", giocatori[i].fortuna);

            printf("Puoi anche scegliere tra:\n");
            printf("1)Attacco_psichico +3 punti e difesa_psichica -3 punti.\n");
            printf("2)Difesa_psichica +3 punti e attacco_psichico -3 punti.\n");
            printf("3)Sto bene così.\n");

            if (!esisteVirgola)
            {
                printf("4)Diventa UndiciVirgolaCinque (attacco_psichico e difesa_psichica +4, fortuna -7).\n");
                printf("Attenzione: il tuo nome verra cambiano in UndiciVirgolaCinque e solo un giocatore per partita può fare questa scelta.\n");
            }
            scelta_attributi(&(giocatori[i]));
        }
    }

    printf("Ora può scegliere come creare la mappa di gioco:\n");
    printf("1)Genera mappa in maniera casuale\n");
    printf("2)Inserisci zona, in una posizione dettata del game master\n");
    printf("3)Cancella zona, in una posizione dettata del game master\n");
    printf("4)Stampa la mappa\n");
    printf("5)Stampa zona singola (sia del mondo reale che soprasotto), a piacere del game master\n");
    printf("6)Esci dalla creazione della mappa\n");

    scelta_mappa();
}