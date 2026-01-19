#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gamelib.h"

typedef struct Giocatore Giocatore;
typedef struct Zona_mondoreale Mondoreale;
typedef struct Zona_soprasotto Soprasotto;
typedef enum Tipo_zona zona;
typedef enum Tipo_nemico nemico;
typedef enum Tipo_oggetto oggetto;

static int esisteVirgola;
static int fine = 0;
static int esisteDemotorzone = 0;
static Mondoreale *prima_zona_mondoreale;
static Soprasotto *prima_zona_soprasotto;
static int numZone = 0;
static int tipiNemici = 4;
static const int tipiZone = 10;
static const int tipiOggetti = 5;
static int numGiocatori;
static int numTurno = 0;
static int *ordineTurno;
static Giocatore *giocatori;

static void scelta_mappa();
static void cancella_mappa();
static char *tipo_zona_to_string(zona tipo);
static char *nemico_to_string(nemico nemico);
static char *oggetto_to_string(oggetto oggetto);

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
/**
 * se zona_reale->avanti è == a prima_zona_mondoreale quindi
 * zona_reale è l'ultima zona_reale
 */
static int lanciaD20()
{
    return rand() % 20 + 1;
}
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
        printf("%d) %s\n", c + 1, tipo_zona_to_string(c));
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
    } while (tolower(esisteNemicoReale[0]) != 'y' && tolower(esisteNemicoReale[0]) != 'n');

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
    } while (tolower(esisteNemicoSoprasotto[0]) != 'y' && tolower(esisteNemicoSoprasotto[0]) != 'n');

    // Scelta oggetto
    printf("Che oggetto ci sarà nella zona del mondo reale?\n");
    for (int c = 0; c < tipiOggetti; c++)
    {
        printf("%d) %s\n", c + 1, oggetto_to_string(c));
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

    if (posto == 1)
    {
        prima_zona_mondoreale = nuova_zona_reale;
        prima_zona_soprasotto = nuova_zona_soprasotto;
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
static char *tipo_zona_to_string(zona tipo)
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
static char *nemico_to_string(nemico nemico)
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
static char *oggetto_to_string(oggetto oggetto)
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
            printf("Tipo: %s\n", tipo_zona_to_string(pScan_reale->tipo));
            printf("Nemico: %s\n", nemico_to_string(pScan_reale->nemico));
            printf("Oggetto: %s\n", oggetto_to_string(pScan_reale->oggetto));
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
            printf("Tipo: %s\n", tipo_zona_to_string(pScan_sotto->tipo));
            printf("Nemico: %s\n", nemico_to_string(pScan_sotto->nemico));
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
    printf("Tipo: %s\n", tipo_zona_to_string(pScan_reale->tipo));
    printf("Nemico: %s\n", nemico_to_string(pScan_reale->nemico));
    printf("Oggetto: %s\n", oggetto_to_string(pScan_reale->oggetto));
    printf("Soprasotto\n");
    printf("Tipo: %s\n", tipo_zona_to_string(pScan_sotto->tipo));
    printf("Nemico: %s\n", nemico_to_string(pScan_sotto->nemico));

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
        if (giocatore->attacco_psichico < 0)
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
    } while (sceltaInt != 6 && esisteDemotorzone && numZone >= 15);
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
    printf("Inserire il numero di giocatori (max 4): ");

    char num_giocatoriChar[4];
    do
    {
        fgets(num_giocatoriChar, sizeof(num_giocatoriChar), stdin);
        numGiocatori = (int)strtol(num_giocatoriChar, NULL, 10);

        if (numGiocatori < 1 || numGiocatori > 4)
        {
            printf("Numero di giocatori non valido. Riprova.\n");
        }
    } while (numGiocatori < 1 || numGiocatori > 4);

    for (int i = 0; i < numGiocatori; i++)
    {
        printf("Inserire il nome del giocatore %d (Max 50 caratteri): ", i + 1);
        fgets(giocatori[i].nome, sizeof(giocatori[i].nome), stdin);
        printf("Le tue statistiche verrano generate casualmente (tirando un  D20 per ogni statistica).\n");
        giocatori[i].attacco_psichico = lanciaD20();
        giocatori[i].difesa_psichica = lanciaD20();
        giocatori[i].fortuna = lanciaD20();

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
 * evasione: 20% + fortuna
 **/
static void genera_ordine_turno()
{
    ordineTurno = calloc(numGiocatori, sizeof(int));
    ordineTurno[0] = (rand() % numGiocatori) + 1;
    int esiste;
    for (int c = 1; c < numGiocatori; c++)
    {

        do
        {
            esiste = 0;
            ordineTurno[c] = (rand() % numGiocatori) + 1;
            for (int i = 0; i < c; i++)
            {
                if (ordineTurno[i] == ordineTurno[c])
                {
                    esiste = 1;
                }
            }
        } while (esiste);
    }
    numTurno++;
}

static char *numeroGiocatore_toString(int numeroGiocatore)
{
    return giocatori[numeroGiocatore - 1].nome;
}

void gioca()
{
    printf("Benvenunti in Occhinz \n");
    printf("L'unico vincitore sarà colui che riuscirà a sconfiggere il Demotorzone!\n");
    for (int c = 0; c < numGiocatori; c++)
    {
        giocatori[c].pos_mondoreale = prima_zona_mondoreale;
        giocatori[c].pos_soprasotto = prima_zona_soprasotto;
        giocatori[c].mondo = 0; // Tutti spawnano nel mondo reale
    }

    do
    {
        genera_ordine_turno();
        printf("Turno %d\n", numTurno);
        printf("Ordine: \n");
        for (int i = 0; i < numGiocatori; i++)
        {
            printf("-> %s", numeroGiocatore_toString(ordineTurno[i]));
            if (i >= numGiocatori)
            {
                printf("\n");
            }
        }

        for (int c = 0; c < numGiocatori; c++)
        {
            printf("Giocatore %s cosa desisderi fare?\n", numeroGiocatore_toString(ordineTurno[c]));
            printf("(Puoi scegliere solo una volta per turno se avanzare, indietreggiare o cambiare mondo)");
            printf("1)Avanza");
            printf("2)Indietreggia");
            printf("3)Cambia mondo");
            printf("4)Combatti");
            printf("5)Statistiche personali");
            printf("6)Stampa zona corrente");
            printf("7)Raccogli oggetto");
            printf("8)Utilizza oggetto");
            printf("9)Passa il turno");

            char sceltaGioco[4];
            int sceltaGiocoInt;
            do
            {
                printf("Inserire il valore dell'azione che si vuole fare: ");
                fgets(sceltaGioco, sizeof(sceltaGioco), stdin);
                sceltaGiocoInt = (int) strtol(sceltaGioco, NULL, 10);

                if (sceltaGiocoInt < 1 && sceltaGiocoInt > 9)
                {
                    printf("Inserire un valore valido, per favore.");
                }
                
            } while (sceltaGiocoInt < 1 && sceltaGiocoInt > 9);
            
            // switch (sceltaGiocoInt)
            // {
            // case constant expression:
            //     /* code */
            //     break;
            
            // default:
            //     break;
            // }
        }
        
    } while (!fine);
    
}