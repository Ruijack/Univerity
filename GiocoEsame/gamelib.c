#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "gamelib.h"

typedef struct Giocatore Giocatore;
typedef struct Vincitore Vincitore;
typedef struct Zona_mondoreale Mondoreale;
typedef struct Zona_soprasotto Soprasotto;
typedef enum Tipo_zona zona;
typedef enum Tipo_nemico nemico;
typedef enum Tipo_oggetto oggetto;

static const int tipiZone = 10;
static const int tipiOggetti = 5;
static const int psBilli = 15;
static const int psDemocane = 25;
static const int psDemotorzone = 50;
static const int paBilli = 4;
static const int paDemocane = 5;
static const int paDemotorzone = 8;

static Mondoreale *prima_zona_mondoreale;
static Soprasotto *prima_zona_soprasotto;
static Giocatore *giocatori;
static int capienzaZaino = sizeof(giocatori->zaino) / sizeof(giocatori->zaino[0]);
static nemico nemicoSconfitto = 0;
static int esisteVirgola;
static int esisteDemotorzone = 0;
static int numZone = 0;
static int tipiNemici = 4;
static int numGiocatori;
static int passaTurno = 0;
static int numTurno = 0;
static int *ordineTurno;

static void scelta_mappa();
static void cancella_mappa();
static void salva_vincitore(Giocatore *player);
static char *tipo_zona_toString(zona tipo);
static char *nemico_toString(nemico nemico);
static char *oggetto_toString(oggetto oggetto);
static void rimuovi_oggetto(oggetto zaino[], oggetto itemUsato);
// ritorna un numero casuale tra 0 e 4 eccetto l'intero inserito
// mondo distingue tra mondo reale e soprasotto, 0 = mondoreale, 1 = soprasotto
static int rand_nemico(int mondo)
{
    int r;
    if (mondo == 1)
    {
        do
        {
            if (!esisteDemotorzone && numZone == 14)
            {
                r = 3;
            }
            else
            {
                r = rand() % tipiNemici;
            }
        } while (r == 1);

        if (r == 3)
        {
            esisteDemotorzone = 1;
            tipiNemici = 3;
        }
    }
    else
    {
        if (mondo == 0)
        {
            r = rand() % 2;
        }
    }

    return r;
}
static int giocatore_lancia_D20()
{
    char buffer[20];
    do
    {
        printf("Inserisci qualsiasi cosa per lanciare il d20... ");
        fgets(buffer, sizeof(buffer), stdin);
    } while (isspace(buffer[0]));

    int tiro = rand() % 20 + 1;
    printf("È uscito: %d\n", tiro);
    return tiro;
}
static void trimma(char *testo)
{
    /*conta quante lettere ci sono,
    quindi tiene conto dell'utima cella dell'array*/
    int lettere = 0;
    for (int c = 0; c < strlen(testo); c++)
    {
        if (!isspace(testo[c]))
        {
            testo[lettere] = testo[c];
            lettere++;
        }
    }
    testo[lettere] = '\0';
};
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

    nuova_zona_reale->nemico = rand_nemico(0);
    nuova_zona_reale->oggetto = rand() % 5;
    nuova_zona_reale->tipo = rand() % 10;
    nuova_zona_reale->link_soprasotto = nuova_zona_soprasotto;
    nuova_zona_reale->avanti = prima_zona_mondoreale;
    nuova_zona_reale->indietro = ultima_zona_reale;

    nuova_zona_soprasotto->tipo = nuova_zona_reale->tipo;
    nuova_zona_soprasotto->link_mondoreale = nuova_zona_reale;
    nuova_zona_soprasotto->nemico = rand_nemico(1);
    nuova_zona_soprasotto->avanti = prima_zona_soprasotto;
    nuova_zona_soprasotto->indietro = ultima_zona_soprasotto;

    ultima_zona_reale->avanti = nuova_zona_reale;
    ultima_zona_soprasotto->avanti = nuova_zona_soprasotto;

    prima_zona_mondoreale->indietro = nuova_zona_reale;
    prima_zona_soprasotto->indietro = nuova_zona_soprasotto;

    numZone++;
}
static void crea_mappa() // Fatto
{                        // crea 15 zone
    prima_zona_mondoreale = (Mondoreale *)malloc(sizeof(Mondoreale));
    prima_zona_soprasotto = (Soprasotto *)malloc(sizeof(Soprasotto));

    Mondoreale *zona_reale = prima_zona_mondoreale;
    Soprasotto *zona_soprasotto = prima_zona_soprasotto;

    // creazione prima zona
    zona_reale->link_soprasotto = zona_soprasotto;
    zona_reale->tipo = rand() % 10;
    zona_reale->oggetto = rand() % 5;
    zona_reale->nemico = rand_nemico(0);
    zona_reale->avanti = zona_reale;
    zona_reale->indietro = zona_reale;

    zona_soprasotto->link_mondoreale = zona_reale;
    zona_soprasotto->tipo = zona_reale->tipo;
    zona_soprasotto->nemico = rand_nemico(1);
    zona_soprasotto->avanti = zona_soprasotto;
    zona_soprasotto->indietro = zona_soprasotto;

    numZone++;
    // Assegna ai giocatori le loro posizioni iniziali
    for (int i = 0; i < numGiocatori; i++)
    {
        giocatori[i].pos_mondoreale = prima_zona_mondoreale;
        giocatori[i].pos_soprasotto = prima_zona_soprasotto;
    }

    for (int i = 0; i < 14; i++)
    {
        crea_zona_fine();
    }
    printf("Mappa creata con successo!\n");
    scelta_mappa();
}
static void inserisci_zona(int posto) // Fatto
{
    // Scelta tipo zona
    printf("Che tipo di zona sarà?\n");
    for (int c = 0; c < tipiZone; c++)
    {
        printf("%d) %s\n", c + 1, tipo_zona_toString(c));
    }
    char sceltaTipo[4];
    int sceltaTipoInt;
    do
    {
        printf("Inserisci il numero corrispondente al tipo di zona: ");
        fgets(sceltaTipo, sizeof(sceltaTipo), stdin);
        sceltaTipoInt = (int)(strtol(sceltaTipo, NULL, 10)) - 1;

        if (sceltaTipoInt < 0 || sceltaTipoInt >= tipiZone)
        {
            printf("Scelta non valida. Riprova.\n");
        }
    } while (sceltaTipoInt < 0 || sceltaTipoInt >= tipiZone);

    // Scelta nemico mondo reale
    printf("Ci sarà un nemico nel mondo reale?(y/n)\n");
    char esisteNemicoReale[4];
    int nemicoReale;
    do
    {
        printf("Inserisci la tua scelta: ");
        fgets(esisteNemicoReale, sizeof(esisteNemicoReale), stdin);
        switch (tolower(esisteNemicoReale[0]))
        {
        case 'y':
            nemicoReale = 1;
            break;
        case 'n':
            nemicoReale = 0;
            break;
        default:
            printf("Scelta non valida. Riprova.\n");
            break;
        }
    } while (tolower(esisteNemicoReale[0]) != 'y' || tolower(esisteNemicoReale[0]) != 'n');

    // Scelta nemico soprasotto
    printf("Ci sarà un nemico nel soprasotto?(y/n)\n");
    char esisteNemicoSoprasotto[4];
    int nemicoSoprasotto;
    do
    {
        printf("Inserisci la tua scelta: ");
        fgets(esisteNemicoSoprasotto, sizeof(esisteNemicoSoprasotto), stdin);
        switch (tolower(esisteNemicoSoprasotto[0]))
        {
        case 'y':
            if (esisteDemotorzone)
            {
                printf("Il nemico nel soprasotto sarà un Democane (esiste già un Demotorzone).\n");
                nemicoSoprasotto = 2;
            }
            else
            {
                printf("Che tipo di demone sarà?\n");
                printf("1) Democane\n");
                printf("2) Demotorzone\n");
                char sceltaNemico[4];
                int sceltaNemicoInt;
                do
                {

                    printf("Inserisci il numero corrispondente al tipo di demone: ");
                    fgets(sceltaNemico, sizeof(sceltaNemico), stdin);
                    sceltaNemicoInt = (int)strtol(sceltaNemico, NULL, 10);
                    switch (sceltaNemicoInt)
                    {
                    case 1:
                        nemicoSoprasotto = 2;
                        break;
                    case 2:
                        nemicoSoprasotto = 3;
                        esisteDemotorzone = 1;
                        tipiNemici = 3;
                        break;
                    default:
                        printf("Scelta non valida. Riprova.\n");
                        break;
                    }
                } while (sceltaNemicoInt < 1 || sceltaNemicoInt > 2);
            }
        case 'n':
            nemicoSoprasotto = 0;
            break;
        default:
            printf("Scelta non valida. Riprova.\n");
            break;
        }
    } while (tolower(esisteNemicoSoprasotto[0]) != 'y' || tolower(esisteNemicoSoprasotto[0]) != 'n');

    // Scelta oggetto
    printf("Che oggetto ci sarà nella zona del mondo reale?\n");
    for (int c = 0; c < tipiOggetti; c++)
    {
        printf("%d) %s\n", c + 1, oggetto_toString(c));
    }
    char sceltaOggetto[4];
    int sceltaOggettoInt;
    do
    {
        printf("Inserisci il numero corrispondente all'oggetto: ");
        fgets(sceltaOggetto, sizeof(sceltaOggetto), stdin);
        sceltaOggettoInt = (int)(strtol(sceltaOggetto, NULL, 10)) - 1;
        if (sceltaOggettoInt < 0 || sceltaOggettoInt >= tipiOggetti)
        {
            printf("Scelta non valida. Riprova.\n");
        }
    } while (sceltaOggettoInt < 0 || sceltaOggettoInt >= tipiOggetti);

    // Creazione nuova zona
    Mondoreale *nuova_zona_reale = (Mondoreale *)malloc(sizeof(Mondoreale));
    Soprasotto *nuova_zona_soprasotto = (Soprasotto *)malloc(sizeof(Soprasotto));
    nuova_zona_reale->tipo = sceltaTipoInt;
    nuova_zona_reale->nemico = nemicoReale;
    nuova_zona_reale->oggetto = sceltaOggettoInt;
    nuova_zona_reale->link_soprasotto = nuova_zona_soprasotto;

    nuova_zona_soprasotto->tipo = sceltaTipoInt;
    nuova_zona_soprasotto->nemico = nemicoSoprasotto;
    nuova_zona_soprasotto->link_mondoreale = nuova_zona_reale;

    // Inserimento nuova zona nella mappa
    if (posto == 1)
    {
        prima_zona_mondoreale = nuova_zona_reale;
        prima_zona_mondoreale->avanti = prima_zona_mondoreale;
        prima_zona_mondoreale->indietro = prima_zona_mondoreale;
        prima_zona_soprasotto = nuova_zona_soprasotto;
        prima_zona_soprasotto->avanti = prima_zona_soprasotto;
        prima_zona_soprasotto->indietro = prima_zona_soprasotto;

        // Assegna ai giocatori le loro posizioni iniziali
        for (int i = 0; i < numGiocatori; i++)
        {
            giocatori[i].pos_mondoreale = prima_zona_mondoreale;
            giocatori[i].pos_soprasotto = prima_zona_soprasotto;
        }
    }
    else
    {
        Mondoreale *pScan_reale = prima_zona_mondoreale;
        for (int c = 1; c < posto; c++)
        {
            if (c != posto)
            {
                pScan_reale = pScan_reale->avanti;
            }
        }
        Mondoreale *pPrev_reale = pScan_reale->indietro;
        pPrev_reale->avanti = nuova_zona_reale;
        nuova_zona_reale->indietro = pPrev_reale;
        nuova_zona_reale->avanti = pScan_reale;
        pScan_reale->indietro = nuova_zona_reale;

        Soprasotto *pScan_sotto = pScan_reale->link_soprasotto;
        Soprasotto *pPrev_sotto = pScan_sotto->indietro;
        pPrev_sotto->avanti = pScan_sotto;
        nuova_zona_soprasotto->indietro = pPrev_sotto;
        nuova_zona_soprasotto->avanti = pScan_sotto;
        pScan_sotto->indietro = nuova_zona_soprasotto;
    }
    printf("Zona inserita con successo!\n");
    numZone++;
    scelta_mappa();
}
static void cancella_zona(int posto) // Fatto
{
    Mondoreale *pScan_reale = prima_zona_mondoreale;

    for (int c = 1; c != posto; c++)
    {

        pScan_reale = pScan_reale->avanti;
    }

    Mondoreale *pPrev_reale = pScan_reale->indietro;
    Mondoreale *pNext_reale = pScan_reale->avanti;
    pPrev_reale->avanti = pNext_reale;
    pNext_reale->indietro = pPrev_reale;

    Soprasotto *pScan_sotto = pScan_reale->link_soprasotto;
    Soprasotto *pPrev_sotto = pScan_sotto->indietro;
    Soprasotto *pNext_sotto = pScan_sotto->avanti;
    pPrev_sotto->avanti = pNext_sotto;
    pNext_sotto->indietro = pPrev_sotto;

    if (posto == 1)
    {
        prima_zona_mondoreale = pNext_reale;
        prima_zona_soprasotto = pNext_sotto;
    }

    if (pScan_sotto->nemico == demotorzone)
    {
        esisteDemotorzone = 0;
        tipiNemici = 4;
    }

    pScan_reale = NULL;
    pScan_sotto = NULL;
    free(pScan_reale);
    free(pScan_sotto);

    printf("Zona cancellata con successo!\n");
    numZone--;
    scelta_mappa();
}
static char *tipo_zona_toString(zona tipo)
{
    switch (tipo)
    {
    case bosco:
        return "Bosco";
    case scuola:
        return "Scuola";
    case laboratorio:
        return "Laboratorio";
    case caverna:
        return "Caverna";
    case strada:
        return "Strada";
    case giardino:
        return "Giardino";
    case supermercato:
        return "Supermercato";
    case centrale_elettrica:
        return "Centrale Elettrica";
    case deposito_abbandonato:
        return "Deposito Abbandonato";
    case stazione_polizia:
        return "Stazione Polizia";
    default:
        return "Tipo non valido";
    }
}
static char *nemico_toString(nemico nemico)
{
    switch (nemico)
    {
    case nessun_nemico:
        return "Nessun nemico";
    case billi:
        return "Billi";
    case democane:
        return "Democane";
    case demotorzone:
        return "Demotorzone";
    default:
        return "Nemico non valido";
    }
}
static char *oggetto_toString(oggetto oggetto)
{
    switch (oggetto)
    {
    case nessun_oggetto:
        return "Nessun oggetto";
    case bicicletta:
        return "Bicicletta";
    case maglietta_fuocoinferno:
        return "Maglietta Fuoco Inferno";
    case bussola:
        return "Bussola";
    case schitarrata_metallica:
        return "Schitarrata Metallica";
    default:
        return "Oggetto non valido";
    }
}
static void stampa_mappa() // Fatto
{
    char sceltaMappa[4];
    int sceltaMappaInt;
    do
    {
        printf("Quale mappa, mondo reale(1) o soprasotto(2)?: ");
        fgets(sceltaMappa, sizeof(sceltaMappa), stdin);
        sceltaMappaInt = (int)strtol(sceltaMappa, NULL, 10);
        if (sceltaMappaInt != 1 && sceltaMappaInt != 2)
        {
            printf("Scelta non valida. Riprova.\n");
        }
    } while (sceltaMappaInt != 1 && sceltaMappaInt != 2);

    int i = 1;

    switch (sceltaMappaInt)
    {
    case 1:

        printf("Mappa del mondo reale\n");
        Mondoreale *pScan_reale = prima_zona_mondoreale;
        do
        {
            printf("Zona %d\n", i);
            printf("Tipo: %s\n", tipo_zona_toString(pScan_reale->tipo));
            printf("Nemico: %s\n", nemico_toString(pScan_reale->nemico));
            printf("Oggetto: %s\n", oggetto_toString(pScan_reale->oggetto));
            pScan_reale = pScan_reale->avanti;
            i++;
        } while (pScan_reale != prima_zona_mondoreale);
        break;

    case 2:
        printf("Mappa del soprasotto\n");
        Soprasotto *pScan_sotto = prima_zona_soprasotto;
        do
        {
            printf("Zona %d\n", i);
            printf("Tipo: %s\n", tipo_zona_toString(pScan_sotto->tipo));
            printf("Nemico: %s\n", nemico_toString(pScan_sotto->nemico));
            pScan_sotto = pScan_sotto->avanti;
            i++;
        } while (pScan_sotto != prima_zona_soprasotto);
        break;
    }

    scelta_mappa();
}
static void stampa_zona() // Fatto
{
    char sceltaZona[4];
    int sceltaZonaInt;
    do
    {
        printf("Inserire la posizione della zona che si vuole stampare, prima zona = 1 (zone attuali %d): ", numZone);
        fgets(sceltaZona, sizeof(sceltaZona), stdin);
        sceltaZonaInt = (int)strtol(sceltaZona, NULL, 10);
        if (sceltaZonaInt < 1 || sceltaZonaInt > numZone)
        {
            printf("Valore invalido, riprova.");
        }
    } while (sceltaZonaInt < 1 || sceltaZonaInt > numZone);

    Mondoreale *pScan_reale = prima_zona_mondoreale;
    for (int c = 1; c < sceltaZonaInt; c++)
    {
        if (c != sceltaZonaInt)
        {
            pScan_reale = pScan_reale->avanti;
        }
    }

    Soprasotto *pScan_sotto = pScan_reale->link_soprasotto;

    printf("Zona %d\n", (int)sceltaZonaInt);
    printf("Mondo reale\n");
    printf("Tipo: %s\n", tipo_zona_toString(pScan_reale->tipo));
    printf("Nemico: %s\n", nemico_toString(pScan_reale->nemico));
    printf("Oggetto: %s\n", oggetto_toString(pScan_reale->oggetto));
    printf("Soprasotto\n");
    printf("Tipo: %s\n", tipo_zona_toString(pScan_sotto->tipo));
    printf("Nemico: %s\n", nemico_toString(pScan_sotto->nemico));

    scelta_mappa();
}
static void chiudi_mappa() // Fatto
{
    if (numZone < 15)
    {
        printf("Il gioco non puo' iniziare con meno di 15 zone!!\n");
        scelta_mappa();
    }
    if (!esisteDemotorzone)
    {
        printf("Il gioco non puo' iniziare senza la presenza di un demotorzone!!\n");
        scelta_mappa();
    }
}
static void scelta_attributi(Giocatore *giocatore) // Fatto
{
    char sceltaModificatore[4];
    int sceltaModificatoreInt;
    do
    {
        printf("Fai la tua scelta: ");
        fgets(sceltaModificatore, sizeof(sceltaModificatore), stdin);
        sceltaModificatoreInt = (int)strtol(sceltaModificatore, NULL, 10);
        if (sceltaModificatoreInt < 1 || sceltaModificatoreInt > 4 || (sceltaModificatoreInt == 4 && esisteVirgola))
        {
            printf("Scelta non valida. Riprova.\n");
        }
    } while (sceltaModificatoreInt < 1 || sceltaModificatoreInt > 4 || (sceltaModificatoreInt == 4 && esisteVirgola));

    switch (sceltaModificatoreInt)
    {
    case 1:
        giocatore->attacco_psichico += 3;
        giocatore->difesa_psichica -= 3;
        if (giocatore->attacco_psichico > 20)
        {
            giocatore->attacco_psichico = 20;
        }

        if (giocatore->difesa_psichica < 1)
        {
            giocatore->difesa_psichica = 1;
        }
        break;
    case 2:

        giocatore->difesa_psichica += 3;
        giocatore->attacco_psichico -= 3;
        if (giocatore->attacco_psichico < 1)
        {
            giocatore->attacco_psichico = 1;
        }

        if (giocatore->difesa_psichica > 20)
        {
            giocatore->difesa_psichica = 20;
        }

        break;
    case 3:
        // Nessuna modifica
        break;
    case 4:
        giocatore->attacco_psichico += 4;
        giocatore->difesa_psichica += 4;
        giocatore->fortuna -= 7;

        if (giocatore->attacco_psichico > 20)
        {
            giocatore->attacco_psichico = 20;
        }
        if (giocatore->difesa_psichica > 20)
        {
            giocatore->difesa_psichica = 20;
        }
        if (giocatore->fortuna < 1)
        {
            giocatore->fortuna = 1;
        }

        strcpy(giocatore->nome, "UndiciVirgolaCinque");
        esisteVirgola = 1;
        break;
    }
}
static void scelta_mappa() // Fatto
{
    char scelta[4];
    static int sceltaInt;
    do
    {
        printf("1)Genera mappa in maniera casuale\n");
        printf("2)Inserisci zona, in una posizione a scelta\n");
        printf("3)Cancella zona, in una posizione a scelta\n");
        printf("4)Stampa la mappa\n");
        printf("5)Stampa zona singola\n");
        printf("6)Esci dalla creazione della mappa\n");

        do
        {
            printf("Inserisci la tua scelta: ");
            fgets(scelta, sizeof(scelta), stdin);
            sceltaInt = (int)strtol(scelta, NULL, 10);

            if (sceltaInt < 1 || sceltaInt > 6)
            {
                printf("Scelta non valida. riprova.\n");
            }
        } while (sceltaInt < 1 || sceltaInt > 6);

        switch (sceltaInt)
        {
        case 1:
            cancella_mappa();
            crea_mappa();
            break;
        case 2:
            char posto[4];
            int postoInt;
            do
            {
                printf("Inserisci la posizione in cui vuoi inserire la nuova zona (zone attuali %d): ", numZone);
                fgets(posto, sizeof(posto), stdin);
                postoInt = (int)strtol(posto, NULL, 10);
                if (postoInt < 1 || postoInt > numZone)
                {
                    printf("Posizione non valida. Riprova.\n");
                }
            } while (postoInt < 1 || postoInt > numZone);

            inserisci_zona(postoInt);
            break;
        case 3:
            char postoCancellare[4];
            int postoCancellareInt;
            if (numZone == 0)
            {
                printf("Non ci sono zone da cancellare\n");
            }
            else
            {
                do
                {
                    printf("Inserisci la posizione della zona che vuoi cancellare, prima zona = 1 (zone attuali %d): ", numZone);
                    fgets(postoCancellare, sizeof(postoCancellare), stdin);
                    postoCancellareInt = (int)strtol(postoCancellare, NULL, 10);
                    if (postoCancellareInt < 1 || postoCancellareInt > numZone)
                    {
                        printf("Posizione non valida. Riprova.\n");
                    }
                } while (postoCancellareInt < 1 || postoCancellareInt > numZone);
                cancella_zona(postoCancellareInt);
            }

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
    } while (sceltaInt != 6 && !esisteDemotorzone && numZone <= 15);
}
static void cancella_mappa()
{
    if (prima_zona_mondoreale != NULL)
    {
        Mondoreale *pScan_reale = prima_zona_mondoreale;
        Soprasotto *pScan_sotto = prima_zona_soprasotto;
        for (int c = 1; c < numZone; c++)
        {
            Mondoreale *pTemp_reale = pScan_reale;
            Soprasotto *pTemp_sotto = pScan_sotto;
            pScan_reale = pScan_reale->avanti;
            pScan_sotto = pScan_sotto->avanti;
            pTemp_reale = NULL;
            pTemp_sotto = NULL;
            free(pTemp_reale);
            free(pTemp_sotto);
        }
        prima_zona_mondoreale = NULL;
        prima_zona_soprasotto = NULL;
        free(prima_zona_mondoreale);
        free(prima_zona_soprasotto);
        numZone = 0;
        esisteDemotorzone = 0;
    }
}
void imposta_gioco() // Fatto
{
    esisteVirgola = 0;
    giocatori = NULL;
    giocatori = (Giocatore *)calloc(4, sizeof(Giocatore));

    char num_giocatoriChar[4];
    do
    {
        printf("Inserire il numero di giocatori (max 4): ");
        fgets(num_giocatoriChar, sizeof(num_giocatoriChar), stdin);
        numGiocatori = (int)strtol(num_giocatoriChar, NULL, 10);

        if (numGiocatori < 1 || numGiocatori > 4)
        {
            printf("Numero di giocatori non valido. Riprova.\n");
        }
    } while (numGiocatori < 1 || numGiocatori > 4);

    for (int i = 0; i < numGiocatori; i++)
    {
        do
        {
            printf("Inserire il nome del giocatore %d (Max 50 caratteri): ", i + 1);
            fgets(giocatori[i].nome, sizeof(giocatori[i].nome), stdin);
            if (strlen(giocatori[i].nome) < 1 || strlen(giocatori[i].nome) > 51)
            {
                printf("Nome non valido. Riprova.\n");
            }

        } while (strlen(giocatori[i].nome) < 1 || strlen(giocatori[i].nome) > 51);

        trimma(giocatori[i].nome);

        printf("Le tue statistiche verrano generate casualmente (tirando un  D20 per ogni statistica).\n");
        giocatori[i].attacco_psichico = giocatore_lancia_D20();
        giocatori[i].difesa_psichica = giocatore_lancia_D20();
        giocatori[i].fortuna = giocatore_lancia_D20();

        giocatori[i].mondo = 0; // Tutti spawnano nel mondo reale

        printf("Il tuo attacco psichico è: %d\n", giocatori[i].attacco_psichico);
        printf("La tua difesa psichica è: %d\n", giocatori[i].difesa_psichica);
        printf("La tua fortuna è: %d\n", giocatori[i].fortuna);

        printf("Puoi anche scegliere tra questi modificatori:\n");
        printf("(ATTENZIONE ogni statistica non puo' scendere sotto a 1 o superare 20)\n");
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

    printf("Ora può scegliere come creare la mappa di gioco:\n");
    scelta_mappa();
}

/**
 * difesa psichica: indica la vita del giocatore/nemico
 * fortuna: indica la possibilità di evadere un attacco
 **/
// rimescola il vettore ordineTurno ogni volta che è chiamata
// con interi tra 0 e il numero di giocatori - 1
static void genera_ordine_turno()
{

    ordineTurno[0] = rand() % numGiocatori;
    if (numGiocatori > 1)
    {
        int esiste;
        for (int c = 1; c < numGiocatori; c++)
        {
            do
            {
                esiste = 0;
                ordineTurno[c] = rand() % numGiocatori;
                for (int i = 0; i < c; i++)
                {
                    if (ordineTurno[i] == ordineTurno[c])
                    {
                        esiste = 1;
                    }
                }
            } while (esiste);
        }
    }
    numTurno++;
}

static int pZona_toNumZona(Mondoreale *pZona) // prima zona = 1
{
    Mondoreale *pScan = prima_zona_mondoreale;
    int c;
    for (c = 0; c < numZone; c++)
    {
        if (pScan != pZona)
        {
            pScan = pScan->avanti;
        }
        else
        {
            break;
        }
    }
    return c + 1;
}

static int possiede_bici(oggetto zaino[])
{
    int possiedeBici = 0;
    for (int i = 0; i < capienzaZaino && possiedeBici == 0; i++)
    {
        if (zaino[i] == bicicletta)
        {
            possiedeBici = 1;
        }
    }
    return possiedeBici;
}
static void avanza(Giocatore *player, int *azione) // Fatto
{
    if (pZona_toNumZona(player->pos_mondoreale) == numZone)
    {
        printf("Questa è l'ultima zona di Occhinz\n");
    }
    else
    {
        int esisteNemico = 0;
        if (!possiede_bici(player->zaino))
        {
            if (player->mondo == 0)
            {
                if (player->pos_mondoreale->nemico != nessun_nemico)
                {
                    esisteNemico = 1;
                }
            }
            else
            {
                if (player->pos_soprasotto->nemico != nessun_nemico)
                {
                    esisteNemico = 1;
                }
            }
        }

        if (esisteNemico)
        {
            printf("Non puoi muoverti se è ancora presente un nemico nella zona dove ti trovi\n");
        }
        else
        {
            // Rigenera il nemico
            if (nemicoSconfitto != 0)
            {
                if (player->mondo == 0)
                {
                    player->pos_mondoreale->nemico = nemicoSconfitto;
                }
                else
                {
                    player->pos_soprasotto->nemico = nemicoSconfitto;
                }
                nemicoSconfitto = 0;
            }
            player->pos_mondoreale = player->pos_mondoreale->avanti;
            player->pos_soprasotto = player->pos_mondoreale->link_soprasotto;
            *azione = 0;
            if (player->mondo == 0)
            {
                printf("Ora %s si trova nella zona %d del mondo reale\n", player->nome, pZona_toNumZona(player->pos_mondoreale));
            }
            else
            {
                printf("Ora %s si trova nella zona %d del soprasotto\n", player->nome, pZona_toNumZona(player->pos_mondoreale));
            }
        }
    }
}

static void indietreggia(Giocatore *player) // Fatto
{
    int esisteNemico = 0;
    if (player->mondo == 0)
    {
        if (player->pos_mondoreale->nemico != 0)
        {
            esisteNemico = 1;
        }
    }
    else
    {
        if (player->pos_soprasotto->nemico != 0)
        {
            esisteNemico = 1;
        }
    }

    if (esisteNemico)
    {
        printf("Non puoi muoverti se è ancora presente un nemico\n");
    }
    else
    {
        if (player->pos_mondoreale == prima_zona_mondoreale)
        {
            printf("Non puoi andare più indietro di così\n");
        }
        else
        {
            player->pos_mondoreale = player->pos_mondoreale->indietro;
            player->pos_soprasotto = player->pos_mondoreale->link_soprasotto;
            if (player->mondo == 0)
            {
                printf("Ora %s si trova nella zona %d del mondo reale\n", player->nome, pZona_toNumZona(player->pos_mondoreale));
            }
            else
            {
                printf("Ora %s si trova nella zona %d del soprasotto\n", player->nome, pZona_toNumZona(player->pos_mondoreale));
            }
        }
    }
}

static void stampa_zona_corrente(Giocatore *player) // Fatto
{
    if (player->mondo == 0)
    {
        Mondoreale *zona_corrente = player->pos_mondoreale;
        printf("Zona %d del mondo reale\n", pZona_toNumZona(player->pos_mondoreale));
        printf("Tipo zona: %s\n", tipo_zona_toString(zona_corrente->tipo));
        printf("Nemico: %s\n", nemico_toString(zona_corrente->nemico));
        printf("Oggetto: %s\n", oggetto_toString(zona_corrente->oggetto));
    }
    else
    {
        Soprasotto *zona_corrente = player->pos_soprasotto;
        printf("Zona %d del soprasotto\n", pZona_toNumZona(player->pos_mondoreale));
        printf("Tipo zona: %s\n", tipo_zona_toString(zona_corrente->tipo));
        printf("Nemico: %s\n", nemico_toString(zona_corrente->nemico));
    }
}

static void rimuovi_giocatore(Giocatore *player)
{
    if (numGiocatori - 1 > 0)
    {
        Giocatore *tempGiocatori = realloc(giocatori, (numGiocatori - 1) * sizeof(Giocatore));
        for (int i = 0; i < numGiocatori; i++)
        {
            if (&giocatori[i] == player)
            {
                for (int c = i; c < numGiocatori; c++)
                {
                    tempGiocatori[c] = giocatori[c + 1];
                }
                break;
            }
            else
            {
                tempGiocatori[i] = giocatori[i];
            }
        }

        giocatori = NULL;
        free(giocatori);
        giocatori = tempGiocatori;
    }
    else
    {
        giocatori = NULL;
        free(giocatori);
    }

    numGiocatori--;
}

static void combattimento(Giocatore *player, nemico tipoNemico)
{
    int psNemico, paNemico;

    switch (tipoNemico)
    {
    case billi:
        psNemico = psBilli;
        paNemico = paBilli;
        break;
    case democane:
        psNemico = psDemocane;
        paNemico = paDemocane;
        break;
    case demotorzone:
        psNemico = psDemotorzone;
        paNemico = paDemotorzone;
        break;
    default:
        break;
    }

    do
    {
        // Il giocatore attacca
        printf("Turno del giocatore, per attaccare tira il D20(danno attacco = tiro D20 + attacco psitico)\n");
        int caso = giocatore_lancia_D20();
        printf("Hai infitto %d danni al nemico\n", caso + player->attacco_psichico);
        psNemico = psNemico - (caso + player->attacco_psichico);

        if (psNemico <= 0)
        {
            if (tipoNemico == demotorzone)
            {
                esisteDemotorzone = 0;
                tipiNemici = 4;
                salva_vincitore(player);
                break;
            }
            else
            {
                printf("Il nemico è stato sconfitto! ");
                int nemicoSparisce = rand() % 2;
                if (nemicoSparisce)
                {
                    if (player->mondo == 0)
                    {
                        player->pos_mondoreale->nemico = nessun_nemico;
                    }
                    else
                    {
                        player->pos_soprasotto->nemico = nessun_nemico;
                    }

                    printf("È sparito in un cumulo di fumo bianco\n");
                }
                else
                {
                    // variabile globale che viene utilizzata quando il giocatore passa il turno
                    nemicoSconfitto = tipoNemico;
                    if (player->mondo == 0)
                    {
                        player->pos_mondoreale->nemico = nessun_nemico;
                    }
                    else
                    {
                        player->pos_soprasotto->nemico = nessun_nemico;
                    }
                    printf("Ma quando ti muoverai tornerà\n");
                }
                break;
            }
        }
        else
        {
            printf("Il nemico ha %d punti di difesa psichica rimasti\n", psNemico);

            // Il nemico attacca
            printf("Turno dell'avversario, ha attacco psichico fisso di %d\n", paNemico);
            int schivata, caso = (rand() % 100) + 1;

            if (possiede_bici(player->zaino))
            {
                char sceltaBici[2];
                printf("Possiedi una bici, vorresti usarla perchivare l'attaco nemico (la bici verra consumata)? (y/n)\n");
                do
                {
                    printf("Decidi: ");
                    fgets(sceltaBici, sizeof(sceltaBici), stdin);
                    if (sceltaBici[0] != 'n' && sceltaBici[0] != 'y')
                    {
                        printf("Inserire un valore valido, per favore.\n");
                    }
                } while (sceltaBici[0] != 'n' && sceltaBici[0] != 'y');

                if (sceltaBici[0] == 'y')
                {
                    schivata = 100;
                    printf("La bici è stata distrutta dall'attacco nemico\n");
                    rimuovi_oggetto(player->zaino, bicicletta);
                }
            }
            else
            {
                printf("Puoi ancora salvarti, tira due D20 per provare a schivare.\n");

                int dado1 = giocatore_lancia_D20();
                int dado2 = giocatore_lancia_D20();

                schivata = dado1 + dado2 + player->fortuna;
                printf("La tua probabilità di schivare è %d%%\n", schivata);
                // Se schivata è minore o uguale a caso(1 a 100), il giocatore schiva l'attacco del nemico
            }

            if (caso > schivata)
            {
                printf("Sei stato colpito!\n");
                player->difesa_psichica = player->difesa_psichica - paNemico;
                if (player->difesa_psichica < 1)
                {
                    printf("%s è stato sconfitto!\n", player->nome);
                    passaTurno = 1;
                }
                else
                {
                    printf("%s ha %d punti di difesa psichica rimasti\n", player->nome, player->difesa_psichica);
                }
            }
            else
            {
                printf("Hai evaso l'attacco nemico\n");
                printf("%s ha %d punti di difesa psichica rimasti\n", player->nome, player->difesa_psichica);
            }
        }

    } while (psNemico > 0 && player->difesa_psichica > 0);
}

static void combatti(Giocatore *player)
{
    int esisteNemico = 0;
    nemico tipoNemico;
    if (player->mondo == 0)
    {
        if (player->pos_mondoreale->nemico != nessun_nemico)
        {
            tipoNemico = player->pos_mondoreale->nemico;
            esisteNemico = 1;
        }
    }
    else
    {
        if (player->pos_soprasotto->nemico != nessun_nemico)
        {
            tipoNemico = player->pos_soprasotto->nemico;
            esisteNemico = 1;
        }
    }

    if (esisteNemico)
    {
        switch (tipoNemico)
        {
        case billi:
            printf("Il tuo nemico sarà Billi (attacco psichico %d, difesa psichica %d)\n", paBilli, psBilli);
            break;
        case democane:
            printf("Il tuo nemico sarà Democane (attacco psichico %d, difesa psichica %d)\n", paDemocane, psDemocane);
            break;
        case demotorzone:
            printf("Il tuo nemico sarà il Demotorzone (attacco psichico %d, difesa psichica %d)\n", paDemotorzone, psDemotorzone);
            break;
        default:
            break;
        }
        printf("Vuoi combattere il nemico di questa zona?(y/n)\n");

        char combatti[2];
        do
        {
            printf("Scegli: ");
            fgets(combatti, sizeof(combatti), stdin);
            if (combatti[0] != 'n' && combatti[0] != 'y')
            {
                printf("Inserire un valore valido, per favore.\n");
            }

        } while (combatti[0] != 'n' && combatti[0] != 'y');

        if (combatti[0] == 'y')
        {
            combattimento(player, tipoNemico);
        }
    }
    else
    {
        printf("Non c'è un nemico in questa zona\n");
    }
}

static void cambia_mondo(Giocatore *player, int *azione) // Fatto
{
    int esisteNemico = 0;
    if (player->mondo == 0)
    {
        if (!possiede_bici(player->zaino))
        {
            if (player->pos_mondoreale->nemico != nessun_nemico)
            {
                esisteNemico = 1;
            }
        }

        if (esisteNemico)
        {
            printf("C'è ancora un nemico in questa zona del mondo reale!\n");
        }
        else
        {
            if (nemicoSconfitto != 0)
            {
                if (player->mondo == 0)
                {
                    player->pos_mondoreale->nemico = nemicoSconfitto;
                }
                else
                {
                    player->pos_soprasotto->nemico = nemicoSconfitto;
                }
                nemicoSconfitto = 0;
            }
            player->mondo = 1;
            *azione = 0;
            printf("%s ora si trova nel soprasotto, zona n.%d\n", player->nome, pZona_toNumZona(player->pos_mondoreale));
        }
    }
    else
    {
        if (player->pos_soprasotto->nemico != nessun_nemico || !possiede_bici(player->zaino))
        {
            printf("C'è ancora un nemico in questa zona del soprasotto!\n");
            printf("Per passare al mondo reale dovrai tirare un dado,\n");
            printf("se il risultato è inferiore alla tua fortuna passerai altrimenti sprecherai solo la tua azione\n");

            char scelta[2];
            do
            {
                printf("Vuoi rischiare (La tua fortuna è %d)? (y/n): ", player->fortuna);
                fgets(scelta, sizeof(scelta), stdin);
                if (scelta[0] != 'y' && scelta[0] != 'n')
                {
                    printf("Inserire una scelta valida, per favore\n");
                }

            } while (scelta[0] != 'y' && scelta[0] != 'n');

            if (scelta[0] == 'y')
            {
                int caso = giocatore_lancia_D20();
                *azione = 0;
                if (caso <= player->fortuna)
                {
                    // Prima di muovere il giocatore rigenera il nemico
                    if (nemicoSconfitto != 0)
                    {
                        if (player->mondo == 0)
                        {
                            player->pos_mondoreale->nemico = nemicoSconfitto;
                        }
                        else
                        {
                            player->pos_soprasotto->nemico = nemicoSconfitto;
                        }
                        nemicoSconfitto = 0;
                    }
                    player->mondo = 0;
                    printf("Lancio: %d, ti è andata bene ora sei nel mondo reale, zona %d\n", caso, pZona_toNumZona(player->pos_mondoreale));
                }
                else
                {
                    printf("Lancio: %d, ti è andata male sei ancora nel soprasotto, zona %d\n", caso, pZona_toNumZona(player->pos_mondoreale));
                }
            }
        }
        else
        {
            if (nemicoSconfitto != 0)
            {
                if (player->mondo == 0)
                {
                    player->pos_mondoreale->nemico = nemicoSconfitto;
                }
                else
                {
                    player->pos_soprasotto->nemico = nemicoSconfitto;
                }
                nemicoSconfitto = 0;
            }
            player->mondo = 0;
            *azione = 0;
            printf("%s ora si trova nel mondo reale, zona n.%d\n", player->nome, pZona_toNumZona(player->pos_mondoreale));
        }
    }
}

static void stampa_statistiche(Giocatore *player) // Fatto
{
    printf("Statistiche di %s:\n", player->nome);
    printf("Attacco psichico: %d\n", player->attacco_psichico);
    printf("Difesa psichica: %d\n", player->difesa_psichica);
    printf("Fortuna: %d\n", player->fortuna);
    printf("Zaino: ");
    int zainoLength = sizeof(player->zaino) / sizeof(player->zaino[0]);
    char zaino[50] = "";
    int zainoVuoto = 1;
    for (int i = 0; i < zainoLength; i++)
    {
        if (player->zaino[i] != nessun_oggetto)
        {
            zainoVuoto = 0;
            strcat(zaino, oggetto_toString(player->zaino[i]));
            if (i + 1 == zainoLength || player->zaino[i + 1] != nessun_oggetto)
            {
                strcat(zaino, ", ");
            }
        }
    }

    if (zainoVuoto)
    {
        strcat(zaino, "Nessun oggetto");
    }
    printf("%s\n", zaino);
    printf("Caratteristica oggetti\n");
    printf("Bicicletta: passiva = puoi muoverti indipendetemente dalla presenza di nemici\n");
    printf("Bicicletta: uso in combattimento = evadi un attacco, viene distrutta\n");
    printf("Maglietta fuocoinferno = aumenta la difesa psichica di 10 punti \n");
    printf("Bussola = ti dice in quale zona si trova il demotorzone, non viene consumata\n");
    printf("Schitarrata metallica = aumenta l'attacco psichico di 5 punti \n");
}

static void raccogli_oggetto(Giocatore *player) // Fatto
{
    oggetto item = player->pos_mondoreale->oggetto;
    int postoVuoto = -1;
    for (int c = 0; c < sizeof(player->zaino) && postoVuoto == -1; c++)
    {
        if (player->zaino[c] == nessun_oggetto)
        {
            postoVuoto = c;
        }
    }

    if (postoVuoto == -1)
    {
        printf("Hai lo zaino pieno!\n");
    }
    else
    {
        printf("Hai raccolto un oggetto: %s\n", oggetto_toString(item));
        player->pos_mondoreale->oggetto = nessun_oggetto;
        player->zaino[postoVuoto] = item;
    }
}

static int num_oggetti(oggetto zaino[])
{
    int numOggetti = 0;
    for (int i = 0; i < capienzaZaino; i++)
    {
        if (zaino[i] != nessun_oggetto)
        {
            numOggetti++;
        }
    }
    return numOggetti;
}

static void rimuovi_oggetto(oggetto zaino[], oggetto itemUsato)
{
    for (int i = 0; i < capienzaZaino; i++)
    {
        if (zaino[i] == itemUsato)
        {
            zaino[i] = nessun_oggetto;
            break;
        }
    }
    for (int i = 0; i < capienzaZaino; i++)
    {
        if (zaino[i] == nessun_oggetto && i + 1 < capienzaZaino && zaino[i + 1] != nessun_oggetto)
        {
            zaino[i] = zaino[i + 1];
            zaino[i + 1] = nessun_oggetto;
        }
    }
}

static void utilizza_oggetto(Giocatore *player) // Fatto
{
    int numOggetti = num_oggetti(player->zaino);

    if (numOggetti == 0)
    {
        printf("Non hai nessun oggetto da usare\n");
    }
    else
    {
        printf("Zaino:\n");
        for (int c = 0; c < capienzaZaino && player->zaino[c] != nessun_oggetto; c++)
        {

            if (player->zaino[c] != nessun_oggetto)
            {
                printf("%d)%s\n", c + 1, oggetto_toString(player->zaino[c]));
            }
        }

        char sceltaOggetto[4];
        int sceltaOggettoInt;
        do
        {
            printf("Inserisci il valore dell'oggetto da usare: ");
            fgets(sceltaOggetto, sizeof(sceltaOggetto), stdin);
            sceltaOggettoInt = (int)strtol(sceltaOggetto, NULL, 10);
            if (sceltaOggettoInt < 1 || sceltaOggettoInt > numOggetti)
            {
                printf("Scelta non valida, riprova\n");
            }
        } while (sceltaOggettoInt < 1 || sceltaOggettoInt > numOggetti);

        switch (player->zaino[sceltaOggettoInt - 1])
        {
        case bicicletta:
            printf("Non puoi usare la bicicletta fuori da un combattamento\n");
            break;
        case maglietta_fuocoinferno:
            player->difesa_psichica = player->difesa_psichica + 10;
            printf("Hai usato la maglietta fuoco inferno, ora la tua difesa psichica è: %d\n", player->difesa_psichica);
            rimuovi_oggetto(player->zaino, maglietta_fuocoinferno);
            break;
        case schitarrata_metallica:
            player->attacco_psichico = player->attacco_psichico + 10;
            printf("Hai usato la schitarrata metallica, ora il tuo attacco psichico è: %d\n", player->attacco_psichico);
            rimuovi_oggetto(player->zaino, schitarrata_metallica);
            break;
        case bussola:
            Soprasotto *pScanSottosopra = prima_zona_soprasotto;
            for (int i = 0; i < numZone || pScanSottosopra->nemico != demotorzone; i++)
            {
                if (pScanSottosopra->nemico != demotorzone)
                {
                    pScanSottosopra = pScanSottosopra->avanti;
                }
            }
            printf("Il demotorzone si trova nella zona %d del soprasotto\n", pZona_toNumZona(pScanSottosopra->link_mondoreale));
            rimuovi_oggetto(player->zaino, bussola);
            break;
        default:
            break;
        }
    }
}

static void salva_vincitore(Giocatore *player)
{
    FILE *vincitoriFile = fopen("vincitori.txt", "a");
    if (vincitoriFile == NULL)
    {
        printf("Errore nell'apertura del file dei vincitori\n");
        return;
    }
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char data_ora[100];
    strftime(data_ora, sizeof(data_ora), "%d-%m-%Y %H:%M:%S", t);

    Vincitore *nuovoVincitore = (Vincitore *)malloc(sizeof(Vincitore));
    char sceltaNick[2], nick[50];
    printf("Vuoi inserire un nickname per la classifica? (y/n): ");
    do
    {
        fgets(sceltaNick, sizeof(sceltaNick), stdin);
        if (sceltaNick[0] != 'y' && sceltaNick[0] != 'n')
        {
            printf("Inserire un valore valido, per favore.\n");
        }
    } while (sceltaNick[0] != 'y' && sceltaNick[0] != 'n');
    if (sceltaNick[0] == 'y')
    {
        printf("Inserisci il nickname: ");
        fgets(nick, sizeof(nick), stdin);
        trimma(nick);
        strcpy(nuovoVincitore->nome, nick);
    }
    else
    {
        strcpy(nuovoVincitore->nome, player->nome);
    }
    nuovoVincitore->attacco_psichico = player->attacco_psichico;
    nuovoVincitore->difesa_psichica = player->difesa_psichica;
    nuovoVincitore->fortuna = player->fortuna;
    nuovoVincitore->numTurno = numTurno;
    strcpy(nuovoVincitore->data_vittoria, data_ora);

    fprintf(vincitoriFile, "%s;%d;%d;%d;%d;%s\n",
            nuovoVincitore->nome,
            nuovoVincitore->attacco_psichico,
            nuovoVincitore->difesa_psichica,
            nuovoVincitore->fortuna,
            nuovoVincitore->numTurno,
            nuovoVincitore->data_vittoria);
    fclose(vincitoriFile);
}
void gioca()
{
    printf("Benvenunti in Occhinz \n");
    printf("L'unico vincitore sarà colui che riuscirà a sconfiggere il Demotorzone!\n");
    do
    {
        ordineTurno = calloc(numGiocatori, sizeof(int));
        genera_ordine_turno();
        printf("Turno %d\n", numTurno);
        printf("Ordine: ");
        printf("%s ", giocatori[ordineTurno[0]].nome);
        if (numGiocatori > 1)
        {
            for (int i = 1; i < numGiocatori; i++)
            {
                printf("-> %s ", giocatori[ordineTurno[i]].nome);
            }
        }
        printf("\n");

        for (int c = 0; c < numGiocatori && numGiocatori > 0 && esisteDemotorzone; c++)
        {
            int azione = 1;
            char sceltaGioco[4] = "";
            int sceltaGiocoInt = 0;
            passaTurno = 0;
            do
            {
                printf("Giocatore %s cosa desideri fare?\n", giocatori[ordineTurno[c]].nome);
                printf("(Puoi scegliere solo una volta per turno se avanzare, indietreggiare o cambiare mondo)\n");
                printf("1)Avanza\n");
                printf("2)Indietreggia\n");
                printf("3)Cambia mondo\n");
                printf("4)Combatti\n");
                printf("5)Statistiche personali\n");
                printf("6)Stampa zona corrente\n");
                printf("7)Raccogli oggetto\n");
                printf("8)Utilizza oggetto\n");
                printf("9)Passa il turno\n");

                do
                {
                    printf("Inserire il valore dell'azione che si vuole fare: ");
                    fgets(sceltaGioco, sizeof(sceltaGioco), stdin);
                    sceltaGiocoInt = (int)strtol(sceltaGioco, NULL, 10);

                    if (sceltaGiocoInt < 1 || sceltaGiocoInt > 9)
                    {
                        printf("Inserire un valore valido, per favore.\n");
                    }

                } while (sceltaGiocoInt < 1 || sceltaGiocoInt > 9);

                switch (sceltaGiocoInt)
                {
                case 1:
                    if (azione)
                    {
                        avanza(&giocatori[ordineTurno[c]], &azione);
                    }
                    else
                    {
                        printf("Ti sei già mosso in questo turno\n");
                    }
                    break;
                case 2:
                    indietreggia(&giocatori[ordineTurno[c]]);
                    break;
                case 3:
                    if (azione)
                    {
                        cambia_mondo(&giocatori[ordineTurno[c]], &azione);
                    }
                    else
                    {
                        printf("Ti sei già mosso in questo turno\n");
                    }

                    break;
                case 4:
                    combatti(&giocatori[ordineTurno[c]]);
                    break;
                case 5:
                    stampa_statistiche(&giocatori[ordineTurno[c]]);
                    break;
                case 6:
                    stampa_zona_corrente(&giocatori[ordineTurno[c]]);
                    break;
                case 7:
                    if (giocatori[ordineTurno[c]].mondo == 0)
                    {
                        if (giocatori[ordineTurno[c]].pos_mondoreale->nemico == nessun_nemico)
                        {
                            if (giocatori[ordineTurno[c]].pos_mondoreale->oggetto != nessun_oggetto)
                            {
                                raccogli_oggetto(&giocatori[ordineTurno[c]]);
                            }
                            else
                            {
                                printf("Non c'è nessun oggetto in questa zona\n");
                            }
                        }
                        else
                        {
                            printf("Sconfiggi prima il nemico per raccogliere l'oggeto\n");
                        }
                    }
                    else
                    {
                        printf("Non ci sono oggetti nel soprasotto\n");
                    }

                    break;
                case 8:
                    utilizza_oggetto(&giocatori[ordineTurno[c]]);
                    break;
                case 9:
                    passaTurno = 1;
                    break;
                }

                if (!esisteDemotorzone)
                {
                    printf("Congratulazioni %s, hai sconfitto il Demotorzone! Hai vinto!\n", giocatori[ordineTurno[c]].nome);
                    break;
                }
            } while (!passaTurno && numGiocatori > 0 && esisteDemotorzone);
        }
        for (int i = 0, numGiocatoriIniziali = numGiocatori; i < numGiocatoriIniziali; i++)
        {
            if (giocatori[ordineTurno[i]].difesa_psichica <= 0)
            {
                rimuovi_giocatore(&giocatori[ordineTurno[i]]);
            }
        }
        if (numGiocatori == 0)
        {
            printf("Tutti i giocatori sono stati sconfitti, il caos trionfa!\n");
            break;
        }

    } while (numGiocatori > 0 && esisteDemotorzone);

    giocatori = NULL;
    free(giocatori);
    cancella_mappa();
}

void termina_gioco()
{
    printf("Grazie per aver giocato a Cosestrane!\n");
}
void crediti()
{
    printf("Gioco sviluppato da: Rui jian Hu, \n");
    printf("Per l'esame di Programmazione Procedurale 2025/2026\n");

    FILE *vincitoriFile = fopen("vincitori.txt", "r");
    if (vincitoriFile != NULL)
    {
        Vincitore vincitori[100];
        int n = 0;
        while (fscanf(vincitoriFile, "%49[^;];%d;%d;%d;%d;%19[^\n]\n",
                      vincitori[n].nome,
                      &vincitori[n].attacco_psichico,
                      &vincitori[n].difesa_psichica,
                      &vincitori[n].fortuna,
                      &vincitori[n].numTurno,
                      vincitori[n].data_vittoria) == 6)
        {
            n++;
            if (n >= 100)
            {
                break;
            }
        }

        int inizioIndice = n - 3;
        if (inizioIndice < 0)
        {
            inizioIndice = 0;
        }

        printf("\n--- Ultimi Vincitori ---\n");

        for (int i = inizioIndice; i < n; i++)
        {
            printf("Nome: %s\n", vincitori[i].nome);
            printf("Data: %s\n", vincitori[i].data_vittoria);
            printf("Turno vittoria: %d\n", vincitori[i].numTurno);
            printf("------------------\n");
        }

        fclose(vincitoriFile);
    }
}