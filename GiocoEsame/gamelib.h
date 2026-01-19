enum Tipo_oggetto {nessun_oggetto, bicicletta, maglietta_fuocoinferno, bussola, schitarrata_metallica};
/** Caratteristica oggetti
 * bicicletta: passiva = puoi muoverti indipendetemente dalla presenza di nemici
 * bicicletta: uso in combattimento = evadi un attacco ma viene distrutta
 * maglietta_fuocoinferno: aumenta la difesa psichica di 5 punti per due attacchi che subisci
 * bussola: ti dice in quale zona si trova il demotorzone
 * schitarrata_metallica: aumenta l'attacco psichico di 5 punti per due attacchi che effettui
 */
enum Tipo_nemico {nessun_nemico, billi, democane, demotorzone};
/**
 * Statistiche nemici
 * Billi: attacco psichico 5, difesa psichica 8, fortuna 5
 * Democane: attacco psichico 7, difesa psichica 12, fortuna 8
 * Demotorzone: attacco psichico 8, difesa psichica 16, fortuna 5
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
    short mondo; // 0 = mondoreale, 1 = soprasotto
    struct Zona_mondoreale* pos_mondoreale;
    struct Zona_soprasotto* pos_soprasotto;
    int attacco_psichico;
    int difesa_psichica;
    int fortuna;
    enum Tipo_oggetto zaino[3];
};

void imposta_gioco();
void gioca();
void crediti();