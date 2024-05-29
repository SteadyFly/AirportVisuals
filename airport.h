#ifndef AIRPORT_H
#define AIRPORT_H

typedef struct {
    char icao_code[5];
    char iata_code[4];
    char full_name[256];
    double datum_lat;
    double datum_lon;
    char rwy_nums[32][2][48];
    double rwy_coords[32][4];
    int rwy_count;
} Airport;

#endif // AIRPORT_H
