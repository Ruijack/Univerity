enum Tipo_oggetto {nessun_oggetto, bicicletta, maglietta_fuocoinferno, bussola, schitarrata_metallica};
/** Caratteristica oggetti
 * bicicletta: passiva = puoi muoverti indipendetemente dalla presenza di nemici
 * bicicletta: uso in combattimento = evadi un attacco, viene distrutta
 * maglietta_fuocoinferno: aumenta la difesa psichica di 10 punti 
 * bussola: ti dice in quale zona si trova il demotorzone
 * schitarrata_metallica: aumenta l'attacco psichico di 5 punti 
 */
enum Tipo_nemico {nessun_nemico, billi, democane, demotorzone};
/**
 * Statistiche nemici
 * Billi: attacco psichico 3, difesa psichica 20
 * Democane: attacco psichico 5, difesa psichica 30
 * Demotorzone: attacco psichico 8, difesa psichica 60
 */
enum Tipo_zona {bosco, scuola, laboratorio, caverna, strada, giardino, supermercato, centrale_elettrica, deposito_abbandonato, stazione_polizia};

struct Zona_soprasotto {
    enum Tipo_zona tipo;
    enum Tipo_nemico nemico;
    struct Zona_soprasotto* avanti;
    struct Zona_soprasotto* indietro;
    struct Zona_mondoreale* link_mondoreale;
};

struct Zona_mondoreale {
    enum Tipo_zona tipo;
    enum Tipo_nemico nemico;
    enum Tipo_oggetto oggetto;
    struct Zona_mondoreale* avanti;
    struct Zona_mondoreale* indietro;
    struct Zona_soprasotto* link_soprasotto;
};

struct Giocatore {
    char nome[50];
    short mondo; // 0 = mondo reale, 1 = soprasotto
    struct Zona_mondoreale* pos_mondoreale;
    struct Zona_soprasotto* pos_soprasotto;
    int attacco_psichico;   //punti attacco del giocatore/nemico
    int difesa_psichica;    //punti vita del giocatore/nemico
    int fortuna;            //indica la possibilità di evadere un attacco
    enum Tipo_oggetto zaino[3];
};

struct Vincitore {
    char nome[50];
    int attacco_psichico;
    int difesa_psichica;
    int fortuna;
    int numTurno;
    char data_vittoria[20];
};

void imposta_gioco();
void gioca();
void termina_gioco();
void crediti();