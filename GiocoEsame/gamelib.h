enum Tipo_oggetto {nessun_oggetto, bicicletta, maglietta_fuocoinferno, bussola, schitarrata_metallica};
/** Caratteristica oggetti
 * bicicletta: puoi muoverti di una zona indipendetemente dai nemici
 * maglietta_fuocoinferno: 
 */
enum Tipo_nemico {nessun_nemico, billi, democane, demotorzone};
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
    short* pos_mondoreale;
    short* pos_soprasotto;
    int attacco_psichico;
    int difesa_psichica;
    int fortuna;
    enum Tipo_oggetto zaino[3];
};