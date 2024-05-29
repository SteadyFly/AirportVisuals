// parser.c
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char aptRaw[256];
char iata_code[256];
char datum_lat[256];
char datum_lon[256];
char rwyNumsList[32][2][48];
char rwyCoordsListChar[10][4][48]; // Add this declaration
double latLonList[512][8192][5];
char buffer[8000000];
int rwyCount = 0;
char rwyList[32][128]; // Declare this missing variable
char line[256]; // Declare the missing 'line' variable

static void parseIataCode(char *line_end)
{
    char *iata_start = strstr(line_end, "1302 iata_code");
    if (iata_start != NULL)
    {
        iata_start += 5;
        char *iata_end = iata_start;
        while (*iata_end != '\n' && *iata_end != '\0')
        {
            iata_end++;
        }
        int iata_length = iata_end - iata_start;
        strncpy(iata_code, iata_start, iata_length);
        iata_code[iata_length] = '\0';
    }
}

static void parseDatumLatLon(char *line_end)
{
    char *datum_lat_start = strstr(line_end, "1302 datum_lat");
    if (datum_lat_start != NULL)
    {
        datum_lat_start += 5;
        char *datum_lat_end = datum_lat_start;
        while (*datum_lat_end != '\n' && *datum_lat_end != '\0')
        {
            datum_lat_end++;
        }
        int datum_lat_length = datum_lat_end - datum_lat_start;
        strncpy(datum_lat, datum_lat_start, datum_lat_length);
        datum_lat[datum_lat_length] = '\0';
    }

    char *datum_lon_start = strstr(line_end, "1302 datum_lon");
    if (datum_lon_start != NULL)
    {
        datum_lon_start += 5;
        char *datum_lon_end = datum_lon_start;
        while (*datum_lon_end != '\n' && *datum_lon_end != '\0')
        {
            datum_lon_end++;
        }
        int datum_lon_length = datum_lon_end - datum_lon_start;
        strncpy(datum_lon, datum_lon_start, datum_lon_length);
        datum_lon[datum_lon_length] = '\0';
    }
}

static void parseRWYList()
{
    for (int i = 0; i < rwyCount; i++)
    {
        char line[128];
        strcpy(line, rwyList[i]);

        char *token = strtok(line, " ");
        int field = 0;

        while (token != NULL)
        {
            if (field == 8)
            {
                strncpy(rwyNumsList[rwyCount][0], token, 3);
                rwyNumsList[rwyCount][0][3] = '\0';
            }
            else if (field == 17)
            {
                strncpy(rwyNumsList[rwyCount][1], token, 3);
                rwyNumsList[rwyCount][1][3] = '\0';
            }
            else if (field == 9)
            {
                strncpy(rwyCoordsListChar[rwyCount][0], token, 11);
                rwyCoordsListChar[rwyCount][0][11] = '\0';
            }
            else if (field == 10)
            {
                strncpy(rwyCoordsListChar[rwyCount][1], token, 11);
                rwyCoordsListChar[rwyCount][1][11] = '\0';
            }
            else if (field == 18)
            {
                strncpy(rwyCoordsListChar[rwyCount][2], token, 11);
                rwyCoordsListChar[rwyCount][2][11] = '\0';
            }
            else if (field == 19)
            {
                strncpy(rwyCoordsListChar[rwyCount][3], token, 11);
                rwyCoordsListChar[rwyCount][3][11] = '\0';
                rwyCount++;
            }

            token = strtok(NULL, " ");
            field++;
        }
    }
}

static void parseRWYS(char *line_end)
{
    char *occurrence2 = strstr(line_end, "100 ");
    if (occurrence2 != NULL)
    {
        char *line_start2 = occurrence2;
        char *line_end2 = occurrence2;

        while (line_start2 > buffer && *(line_start2 - 1) != '\n')
        {
            line_start2--;
        }
        while (*line_end2 != '\n' && *line_end2 != '\0')
        {
            line_end2++;
        }
        int line_length2 = line_end2 - line_start2;

        strncpy(rwyList[rwyCount], line_start2, line_length2);
        rwyList[rwyCount][line_length2] = '\0';
        rwyCount++;

        while (rwyCount < 32)
        {
            occurrence2 = strstr(line_end2, "100 ");
            if (occurrence2 == NULL)
            {
                break;
            }

            char *next_line_check = occurrence2;
            while (*next_line_check != '\n' && *next_line_check != '\0')
            {
                next_line_check++;
            }
            if (*next_line_check == '\0')
            {
                break;
            }
            next_line_check++;

            if (strncmp(next_line_check, "100 ", 4) != 0)
            {
                break;
            }

            line_start2 = occurrence2;
            line_end2 = occurrence2;
            while (*line_end2 != '\n' && *line_end2 != '\0')
            {
                line_end2++;
            }
            line_length2 = line_end2 - line_start2;

            strncpy(rwyList[rwyCount], line_start2, line_length2);
            rwyList[rwyCount][line_length2] = '\0';
            rwyCount++;
        }
    }
}

static void parseTaxiway(char *line_end)
{
    int latLonCount[512] = {0};
    int sectionIndex = 0;

    char *line_start = strstr(line_end, "110 ");
    if (line_start != NULL)
    {
        line_start += 4;
        char *next_line = strtok(line_start, "\n");

        while (next_line != NULL)
        {
            float type, lat, lon, bezierLat = 0, bezierLon = 0;
            int num_matches = sscanf(next_line, "%f %f %f", &type, &lat, &lon);

            if (type == 112)
            {
                num_matches = sscanf(next_line, "%f %f %f %f %f", &type, &lat, &lon, &bezierLat, &bezierLon);
            }

            if (type == 110)
            {
                next_line = strtok(NULL, "\n");
                continue;
            }

            if (num_matches >= 2)
            {
                latLonList[sectionIndex][latLonCount[sectionIndex]][0] = type;
                latLonList[sectionIndex][latLonCount[sectionIndex]][1] = lat;
                latLonList[sectionIndex][latLonCount[sectionIndex]][2] = lon;
                latLonList[sectionIndex][latLonCount[sectionIndex]][3] = bezierLat;
                latLonList[sectionIndex][latLonCount[sectionIndex]][4] = bezierLon;
                latLonCount[sectionIndex]++;

                if (latLonCount[sectionIndex] >= 8192)
                {
                    printf("Exceeded maximum allowed latitude and longitude pairs in section %d.\n", sectionIndex);
                    break;
                }

                if (type == 110 || type == 114 || type == 113)
                {
                    sectionIndex++;
                    if (sectionIndex >= 512)
                    {
                        printf("Exceeded maximum allowed number of sections.\n");
                        break;
                    }
                }
            }

            if (type < 111 || type > 116)
            {
                if(type != 110 && type != 112 && type != 1 && type != 2 && type != 3)
                {
                    printf("Encountered line type not in range 111-116. Exiting.\n");
                    break;
                }
            }

            next_line = strtok(NULL, "\n");
        }
    }
}

void parseAptdat(const char *apt)
{
    const char *filename = "/Users/apt.dat";
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        perror("fopen");
    }

    while (fread(buffer, sizeof(buffer), 1, file) == 1)
    {
        char *occurrence = strstr(buffer, apt);
        if (occurrence != NULL)
        {
            char *line_start = occurrence;
            char *line_end = occurrence;

            while (line_start > buffer && *(line_start - 1) != '\n')
            {
                line_start--;
            }
            while (*line_end != '\n' && *line_end != '\0')
            {
                line_end++;
            }
            int line_length = line_end - line_start;
            strncpy(line, line_start, line_length);
            line[line_length] = '\0';

            if (strncmp(line, "1    ", 5) == 0)
            {
                strncpy(aptRaw, line_start, line_length);
                aptRaw[line_length] = '\0';

                parseIataCode(line_end);
                parseDatumLatLon(line_end);
                parseRWYS(line_end);
                parseTaxiway(line_end);
            }
        }
    }

    parseRWYList();
    fclose(file);
}
