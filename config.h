#include <math.h>

// MAKRA
#define WAREHOUSE_DB_FILE "warehouse_db.txt"
#define ITEMS_FOLDER "items"
#define MAX_NAME 100 // max. dlzka pre nazvy
#define RADIUS_EARTH_KM 6372 // polomer zeme
#define deg2rad(X) ((X)*M_PI/180) // makro na konverziu stupnov na radiany

// SEPARATOR ADRESAROV
#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

// DATOVE TYPY

// GPS poloha
typedef struct gps {
    double lat; // latitude
    double lon; // longitude
} GPS;

// polozka na sklade (tovar)
typedef struct item {
    char name[MAX_NAME + 1]; // nazov
    int price; // cena
} ITEM;

// sklad
typedef struct warehouse {
    char name[MAX_NAME + 1]; // nazov
    GPS gps; // poloha
    int capacity; // kapacita skladu
    int n; // aktualny pocet poloziek na sklade
    ITEM *items; // databaza poloziek na sklade (dynamicke pole)
} WAREHOUSE;

// DEKLARACIE FUNKCII
double distance(GPS gps1, GPS gps2);
