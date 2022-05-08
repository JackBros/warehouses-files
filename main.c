#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

int main(int argc, char *argv[]){
    int opt = 0, w_option = 0, n_option = 0, e_option = 0, a_option = 0, d_option = 0;
    double latitude_num = 0, longitude_num = 0;
    char *warehouse_name_optarg, *latitude, *longitude;
    char* optstring = "-:w:n:e:ad";

    // CMD arguments
    while ((opt = getopt(argc, argv, optstring)) != -1) {  
        switch (opt) {
            case 'w':  // warehouse name filter
                w_option = 1;
                warehouse_name_optarg = optarg;
                break;
            case 'n':  // GPS filter
                n_option = 1;
                latitude = optarg;
                break;
            case 'e': 
                e_option = 1;
                longitude = optarg;
                break;
            case 'a':  // price filter min. to max.
                a_option = 1;
                break;
            case 'd':  // price filter max. to min.
                d_option = 1;
                break;
        }
    }
    if (n_option == 1 && e_option == 1) {
        latitude_num = atof(latitude);
        longitude_num = atof(longitude);
    }


    // warehouses
    FILE *warehouseF, *itemsF;
    warehouseF = fopen(WAREHOUSE_DB_FILE,  "r");
    WAREHOUSE *db = NULL;
    ITEM items[100];
    int w_load = 0, e = 0, round = 0, nearest = 1000000, nearest_e = -1, dist = 0;
    char item_path[1000];

    // warehouse files
    if (warehouseF == NULL) {
        // folder error
    } else {
        db = calloc(20, sizeof(WAREHOUSE));

        while (fscanf(warehouseF, "%s %lf %lf %d", db[w_load].name, &db[w_load].gps.lat, &db[w_load].gps.lon, &db[w_load].capacity) == 4) {
            printf("%s %.3lf %.3lf %d\n", db[w_load].name, db[w_load].gps.lat, db[w_load].gps.lon, db[w_load].capacity);
            w_load++;
            
            db = realloc(db, sizeof(WAREHOUSE) * (w_load + 1));
        }
    }
    fclose(warehouseF);

    // warehouse filters
    int control[w_load];
    while (e < w_load) {  
        int enable_items = 0, sort_int = 0, enable_sort = 0, name_to_int = 0, i_load = 0;
        char sort_char[100];
        char price_char[1000];
        control[e] = 1;
        round++;

        if (w_option == 1) {  // warehouse name filter
            if (strcmp(db[e].name, warehouse_name_optarg) == 0)
                enable_items = 1;

        } else if (n_option == 1 && e_option == 1) {  // nearest warehouse filter (GPS)
            GPS optarg_gps;
            optarg_gps.lat = fabs(latitude_num);
            optarg_gps.lon = fabs(longitude_num);
            dist = distance(optarg_gps, db[e].gps);
            if (dist < nearest) {
                nearest = dist;
                nearest_e = e;
            }
            if (e == w_load - 2) {
                enable_items = 1;
                e = nearest_e;
            }
        } else if (w_option == 0 && e_option == 0 && n_option == 0)
            enable_items = 1;  // no warehouse filters


        // items
        if (enable_items == 1) {

            // item file to struct
            sprintf(item_path, "%s%s%s.txt", ITEMS_FOLDER, PATH_SEPARATOR, db[e].name);
            itemsF = fopen(item_path, "r");

            i_load = 0;
            if (itemsF == NULL) {  // error situation 1
                fprintf(stderr, "FILE_ERROR %s.txt\n", db[e].name);
                control[e] = 0;
                enable_items = 0;

            } else {
                db[e].items = calloc(20, sizeof(ITEM)); 

                while ((fscanf(itemsF, "%s %s", db[e].items[i_load].name, price_char) == 2)) {
                    db[e].items[i_load].price = atoi(price_char);
                    name_to_int = atoi(db[e].items[i_load].name);
                    if ((db[e].items[i_load].price == 0 || name_to_int != 0) && enable_items == 1) {  // error situation 3
                        fprintf(stderr, "FORMAT_ERROR %s.txt\n", db[e].name);
                        enable_items = 0;
                    }
                    
                    i_load++;
                    db[e].items = realloc(db[e].items, sizeof(ITEM) * (i_load + 1));
                } 

                if (i_load > db[e].capacity) {  // error situation 2
                    fprintf(stderr, "CAPACITY_ERROR %s.txt\n", db[e].name);
                    enable_items = 0;
                } 

                
                // items 
                if (enable_items == 1) {

                    // sorting
                    for (int k=0;k<i_load;k++) { // lexicographical sort
                        for (int l=k+1;l<i_load;l++) {
                            if (a_option == 0 && d_option == 0) { 
                                if (strcmp(db[e].items[k].name, db[e].items[l].name) > 0) {
                                    strcpy(sort_char, db[e].items[k].name);
                                    sort_int = db[e].items[k].price;
                                    strcpy(db[e].items[k].name, db[e].items[l].name);
                                    db[e].items[k].price = db[e].items[l].price;
                                    strcpy(db[e].items[l].name, sort_char);
                                    db[e].items[l].price = sort_int;
                                }
                            } else if (a_option == 1) {  // ascending price sort
                                if (db[e].items[k].price > db[e].items[l].price) {
                                    strcpy(sort_char, db[e].items[k].name);
                                    sort_int = db[e].items[k].price;
                                    strcpy(db[e].items[k].name, db[e].items[l].name);
                                    db[e].items[k].price = db[e].items[l].price;
                                    strcpy(db[e].items[l].name, sort_char);
                                    db[e].items[l].price = sort_int;
                                }
                            } else if (d_option == 1) {  // descending price sort
                                if (db[e].items[k].price < db[e].items[l].price) {
                                    strcpy(sort_char, db[e].items[k].name);
                                    sort_int = db[e].items[k].price;
                                    strcpy(db[e].items[k].name, db[e].items[l].name);
                                    db[e].items[k].price = db[e].items[l].price;
                                    strcpy(db[e].items[l].name, sort_char);
                                    db[e].items[l].price = sort_int;
                                }
                            }
                        }
                    }
                        
                    // printing
                    printf("%s %.3lf %.3lf %d :\n", db[e].name, db[e].gps.lat, db[e].gps.lon, db[e].capacity); 
                    for (int j = 0; j < i_load; j++)
                        printf("%d. %s %d\n", j + 1, db[e].items[j].name, db[e].items[j].price);
                }
                fclose(itemsF);
            }
        }
        if (round == 51 && nearest_e != -1) {  // because of GPS filter
            e = w_load + 1;
        }
        e++;
    }
    for (int d = 0; d < w_load; d++) {  // free alloc
        if (control[d] == 1)
            free(db[d].items);
    }
    free(db);

    return 0;
}
