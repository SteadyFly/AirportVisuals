//
//  main.c
//  ND_DISPLAY
//
//  Created by  on 1/31/23.
//

#include <gtk/gtk.h>
#include <cairo.h>
#include <glib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

static void
rounded_rectangle_wstroke(cairo_t *cr, double x, double y, double width, double height, double radius,
                          double red, double green, double blue, double sred, double sgreen, double sblue, double strokewidth)
{
    double degrees = M_PI / 180;
    cairo_new_sub_path(cr);
    cairo_arc(cr, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
    cairo_arc(cr, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
    cairo_arc(cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
    cairo_arc(cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
    cairo_close_path(cr);
    cairo_set_source_rgb(cr, red, green, blue);
    cairo_fill_preserve(cr);
    cairo_set_source_rgb(cr, sred, sgreen, sblue);
    cairo_set_line_width(cr, strokewidth);
    cairo_stroke(cr);
}

static void
rounded_rectangle_wstroke_nocolor(cairo_t *cr, double x, double y, double width, double height, double radius, double strokewidth)
{
    double degrees = M_PI / 180;
    cairo_new_sub_path(cr);
    cairo_arc(cr, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
    cairo_arc(cr, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
    cairo_arc(cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
    cairo_arc(cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
    cairo_close_path(cr);
    
    cairo_fill_preserve(cr);
    
    cairo_set_line_width(cr, strokewidth);
    cairo_stroke(cr);
}

static void
better_rectangle(cairo_t *cr, double x, double y, double width, double height, double r, double g, double b)
{
    cairo_set_source_rgb(cr, r, g, b);
    cairo_rectangle(cr, x, y, width, height);
    cairo_fill(cr);
}

static void
better_rectangle_no_rgb(cairo_t *cr, double x, double y, double width, double height)
{
    cairo_rectangle(cr, x, y, width, height);
    cairo_fill(cr);
}

static void
MFD_low_txtbox(cairo_t *cr, double x, double y)
{
    rounded_rectangle_wstroke(cr, x, y, 230, 70, 5, 0.33, 0.29, 0.34, 1, 1, 1, 2.5);
}

static void
draw_text(cairo_t *cr, const char *text, double x, double y, int align)
{
    cairo_text_extents_t extents;
    cairo_text_extents(cr, text, &extents);
    
    switch (align)
    {
        case 0:
            cairo_move_to(cr, x, y);
            // 1: Left
            break;
        case 1:
            cairo_move_to(cr, x - extents.width / 2, y);
            
            // Center
            break;
        case 2:
            cairo_move_to(cr, x - extents.width, y);
            // Right
            break;
            
        default:
            break;
    }
    
    cairo_show_text(cr, text);
}

long double
toRadians(const long double degree)
{
    // cmath library in C++
    // defines the constant
    // M_PI as the value of
    // pi accurate to 1e-30
    long double one_deg = (M_PI) / 180;
    return (one_deg * degree);
}

long double
distance(long double lat1, long double long1,
         long double lat2, long double long2)
{
    // Convert the latitudes
    // and longitudes
    // from degree to radians.
    lat1 = toRadians(lat1);
    long1 = toRadians(long1);
    lat2 = toRadians(lat2);
    long2 = toRadians(long2);
    
    // Haversine Formula
    long double dlong = long2 - long1;
    long double dlat = lat2 - lat1;
    
    long double ans = pow(sin(dlat / 2), 2) +
    cos(lat1) * cos(lat2) *
    pow(sin(dlong / 2), 2);
    
    ans = 2 * asin(sqrt(ans));
    
    // Radius of Earth in
    // Kilometers, R = 6371
    // Use R = 3956 for miles
    long double R = (3956 / 1.151);
    
    // Calculate the result
    ans = ans * R;
    
    return ans;
}




//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------UTIL FUNCTIONS END!!--------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------





int line_number = 0;
int line_number_apt_occurrence = 0;
char line[256];
char line2[256];
char aptRaw[256];
char rwyList[32][128];
int rwyCount = 0;
char iata_code[256];
char datum_lat[256];
char datum_lon[256];

char rwyNumsList[32][2][48];
int runwayCount = 0;

char rwyCoordsListChar[10][4][48];

double latLonList[512][8192][5];

char *line_end;
char *line_start;

const int chunk_size = 8000000;
char buffer[chunk_size];

static void
parseRWYList()
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
                strncpy(rwyNumsList[runwayCount][0], token, 3);
                rwyNumsList[runwayCount][0][3] = '\0';
            }
            else if (field == 17)
            {
                strncpy(rwyNumsList[runwayCount][1], token, 3);
                rwyNumsList[runwayCount][1][3] = '\0';
            }
            else if (field == 9)
            {
                
                strncpy(rwyCoordsListChar[runwayCount][0], token, 11);
                rwyCoordsListChar[runwayCount][0][11] = '\0';
            }
            else if (field == 10)
            {
                strncpy(rwyCoordsListChar[runwayCount][1], token, 11);
                rwyCoordsListChar[runwayCount][1][11] = '\0';
            }
            else if (field == 18)
            {
                strncpy(rwyCoordsListChar[runwayCount][2], token, 11);
                rwyCoordsListChar[runwayCount][2][11] = '\0';
            }
            else if (field == 19)
            {
                strncpy(rwyCoordsListChar[runwayCount][3], token, 11);
                rwyCoordsListChar[runwayCount][3][11] = '\0';
                runwayCount++;
            }
            
            token = strtok(NULL, " ");
            field++;
        }
    }
}


static void
parseIataCode()
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
        
        // cairo_show_text(cr, iata_code);
    }
}

static void
parseDatumLatLon()
{
    // Parse file for datum_lat
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
        
        // cairo_show_text(cr, datum_lat);
    }
    
    // Parse file for datum_lon
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
        
        // cairo_show_text(cr, datum_lon);
    }
    
}

static void parseRWYS()
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
                // If the next line doesn't start with 100, break
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


static void
parseTaxiway()
{
 
    // Search for the prefix "110" and parse lat/long pairs until next "110"
    int latLonCount[512] = {0};
    
    int sectionIndex = 0; // Variable to keep track of the current section index
    
    char *line_start = strstr(line_end, "110 ");
    char *line_start_114 = strstr(line_end, "114 ");
    
    // Use the earliest occurrence of either "110 " or "114 "
    if (line_start_114 != NULL && (line_start == NULL || line_start_114 < line_start))
    {
        line_start = line_start_114;
    }
    
    if (line_start != NULL)
    {
        // Move the pointer after "110 " or "114 " to start parsing "111 " lines
        line_start += 4; // Both "110 " and "114 " have a length of 4
        
        char *next_line = strtok(line_start, "\n");
        
        while (next_line != NULL)
        {
            float type, lat, lon, bezierLat = 0, bezierLon = 0; // Initialize bezier values to 0
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
                // Store the parsed values in the latLonList array for the current section
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
                    // Move to the next section when encountering the next "110" line
                    sectionIndex++;
                    
                    // Check if we exceed the maximum number of sections
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




static void
parseAptdat(char *apt)
{
    /* EXAMPLE FORMAT
     1    392 0 0 LFPG Paris - Charles De Gaulle
     1302 city Paris
     1302 country France
     1302 datum_lat 49.009747222
     1302 datum_lon 2.547819444a
     1302 gui_label 3D
     1302 iata_code CDG
     1302 icao_code LFPG
     1302 region_code LF
     1302 state Île-de-France
     1302 transition_alt 5000
     1302 transition_level 70
     100 60.00 1 0 0.00 1 3 0 09L 49.0247190 2.5248920 0 0 3 2 1 0 27R 49.0266940 2.5616890 0 66 3 2 1 0
     100 45.11 1 0 0.00 1 3 0 09R 49.0206170 2.5130580 0 0 3 2 1 0 27L 49.0236851 2.5703075 599 0 3 2 1 0
     100 45.11 1 0 0.00 1 3 0 08L 48.9956874 2.5527569 0 151 3 2 1 0 26R 48.9986995 2.6091707 528 135 3 2 1 0
     100 60.05 1 0 0.00 1 3 0 08R 48.9929139 2.5656774 0 150 3 2 1 0 26L 48.9948780 2.6024330 0 149 3 2 1 0
     */
    
    const char *filename = "/Users/apt.dat";
    

    FILE *file = fopen(filename, "r");
    
    char *eel[40];
    
    if (file == NULL)
    {
        
        perror("fopen");
    }
    
    while (fread(buffer, chunk_size, 1, file) == 1)
    {
        char *occurrence = strstr(buffer, apt);
        if (occurrence != NULL)
        {
            line_start = occurrence;
            line_end = occurrence;
            
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
                
                // Parse file for iata_code
                parseIataCode();
                
                //Parse file for datum lat & lon
                parseDatumLatLon();
                
                
                parseRWYS();
                
                parseTaxiway();

                
            }
        }
    }
    
    
    
    parseRWYList();
    
    

    fclose(file);
}



int timesParsed = 0;

static void
drawMapRWY(cairo_t *cr, double x, double y, char *apt, int pixelsPerNM)
{
    // 0.5 nm to edge
    // 780px to edge
    
    int ppn = pixelsPerNM;
    int rwyAngle = 310;
    
    if ((!timesParsed) > 0)
    {
        parseAptdat(apt);
        timesParsed++;
    }
    
    cairo_set_source_rgb(cr, 1, 1, 1);
    
    char aptFull[256];
    memcpy(aptFull, &aptRaw[18], 255);
    aptFull[255] = '\0';
    cairo_set_font_size(cr, 35);
    draw_text(cr, aptFull, 1000 - 5, 35, 2);
    
    char aptICAO[5];
    memcpy(aptICAO, &aptRaw[13], 4);
    aptICAO[4] = '\0';
    // printf(aptICAO);
    cairo_set_font_size(cr, 35);
    draw_text(cr, aptICAO, 900, 70, 2);
    
    char aptIATA[4];
    memcpy(aptIATA, &iata_code[10], 3);
    aptIATA[3] = '\0';
    
    cairo_set_font_size(cr, 35);
    draw_text(cr, aptIATA, 1000 - 5, 70, 2);
    
    char datum_lat_raw[256];
    memcpy(datum_lat_raw, datum_lat + 10, sizeof(datum_lat) - 14);
    datum_lat_raw[sizeof(datum_lat) - 14] = '\0';
    double datum_lat_value = atof(datum_lat_raw);
    
    char datum_lon_raw[256];
    memcpy(datum_lon_raw, datum_lon + 10, sizeof(datum_lon) - 14);
    datum_lon_raw[sizeof(datum_lon) - 14] = '\0';
    double datum_lon_value = atof(datum_lon_raw);
    
    double rwyCoordsList[10][4];
    
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            char *ptr;
            rwyCoordsList[i][j] = strtod(rwyCoordsListChar[i][j], &ptr);
        }
    }
    
    char test[256];
    sprintf(test, "%f", rwyCoordsList[0][0]);
    
    double CURR_POS[] = {datum_lat_value, datum_lon_value};
    
    double nmDiffStart_x;
    double nmDiffStart_y;
    
    double nmDiffEnd_x;
    double nmDiffEnd_y;
    
    double pxStart_x;
    double pxStart_y;
    
    double pxEnd_x;
    double pxEnd_y;
    
    double angle;
    double center_y;
    double center_x;
    
    
    cairo_translate(cr, x, y);
    cairo_rotate(cr, -M_PI / 2);
    cairo_translate(cr, -x, -y);
    
    
    
    double point_lat;
    double point_lon;
    
    double point_x;
    double point_y;
    
    double firstx;
    double firsty;
    
    
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    
    cairo_set_source_rgb(cr, 1, 1, 0);
    
    
    
    
    double prev_point_x;
    double prev_point_y;
    
    double first_point_y;
    double first_point_x;
    
    
    srand(time(NULL));
    
   
    cairo_set_line_width(cr, 0.7);
    for (int e = 0; e < 512; e++)
    {
        
         if (latLonList[e][0][0] == 0)
         {
         break;
         }
         
        //cairo_new_path(cr);
   
        
        for (int q = 0; q < 8192; q++)
        {
            
            double lat = latLonList[e][q][1];
             double lon = latLonList[e][q][2];

             if (lat == 0 || lon == 0)
             {
                 break;
             }

             double nmDiff_x = distance(CURR_POS[0], CURR_POS[1], lat, CURR_POS[1]);
             double nmDiff_y = distance(CURR_POS[0], CURR_POS[1], CURR_POS[0], lon);

             double point_x, point_y;

             if (CURR_POS[0] > lat)
             {
                 point_x = x - (nmDiff_x * ppn);
             }
             else
             {
                 point_x = x + (nmDiff_x * ppn);
             }

             if (CURR_POS[1] > lon)
             {
                 point_y = y - (nmDiff_y * ppn);
             }
             else
             {
                 point_y = y + (nmDiff_y * ppn);
             }

             if (latLonList[e][q][0] == 112)
             {
                 // Using bezier control points from 3rd and 4th index
                 double ctrlLat = latLonList[e][q][3];
                 double ctrlLon = latLonList[e][q][4];

                 double ctrlDiff_x = distance(CURR_POS[0], CURR_POS[1], ctrlLat, CURR_POS[1]);
                 double ctrlDiff_y = distance(CURR_POS[0], CURR_POS[1], CURR_POS[0], ctrlLon);

                 double ctrl_point_x = CURR_POS[0] > ctrlLat ? x - (ctrlDiff_x * ppn) : x + (ctrlDiff_x * ppn);
                 double ctrl_point_y = CURR_POS[1] > ctrlLon ? y - (ctrlDiff_y * ppn) : y + (ctrlDiff_y * ppn);

                 if (q == 0)
                 {
                     cairo_move_to(cr, point_x, point_y);
                 }
                 else
                 {
                     cairo_curve_to(cr, ctrl_point_x, ctrl_point_y, point_x, point_y, point_x, point_y);
                 }
             }
             else
             {
                 if (q == 0)
                 {
                     cairo_move_to(cr, point_x, point_y);
                 }
                 else
                 {
                     cairo_line_to(cr, point_x, point_y);
                 }
             }
            
            
            
            
            if (q == 0)
            {
                first_point_x = point_x;
                first_point_y = point_y;
            }
            
            prev_point_x = point_x;
            prev_point_y = point_y;
            
            
        }
        double red = (double)rand() / RAND_MAX;
        double green = (double)rand() / RAND_MAX;
        double blue = (double)rand() / RAND_MAX;
        cairo_set_source_rgb(cr, red, green, blue);
        if (!(e == 0))
        {
            cairo_line_to(cr, first_point_x, first_point_y);
        }
        cairo_fill(cr);
    
        
        
        
        
    }
    
    
    
    for (int f = 0; f <= 10; f++)
    {
        
        double rwy_start_lat = rwyCoordsList[f][0];
        double rwy_start_lon = rwyCoordsList[f][1];
        double rwy_end_lat = rwyCoordsList[f][2];
        double rwy_end_lon = rwyCoordsList[f][3];
        
        // Calculate the distance between the current position and runway start/end
        double nmDiffStart_x = distance(CURR_POS[0], CURR_POS[1], rwy_start_lat, CURR_POS[1]);
        double nmDiffStart_y = distance(CURR_POS[0], CURR_POS[1], CURR_POS[0], rwy_start_lon);
        double nmDiffEnd_x = distance(CURR_POS[0], CURR_POS[1], rwy_end_lat, CURR_POS[1]);
        double nmDiffEnd_y = distance(CURR_POS[0], CURR_POS[1], CURR_POS[0], rwy_end_lon);
        
        // Calculate the projected pxStart_x and pxEnd_x
        
        if (CURR_POS[0] > rwy_start_lat)
        {
            pxStart_x = x - (nmDiffStart_x * ppn);
        }
        else
        {
            pxStart_x = x + (nmDiffStart_x * ppn);
        }
        
        if (CURR_POS[0] > rwy_end_lat)
        {
            pxEnd_x = x - (nmDiffEnd_x * ppn);
        }
        else
        {
            pxEnd_x = x + (nmDiffEnd_x * ppn);
        }
        
        // Calculate the projected pxStart_y and pxEnd_y
        if (CURR_POS[1] > rwy_start_lon)
        {
            pxStart_y = y - (nmDiffStart_y * ppn);
        }
        else
        {
            pxStart_y = y + (nmDiffStart_y * ppn);
        }
        
        if (CURR_POS[1] > rwy_end_lon)
        {
            pxEnd_y = y - (nmDiffEnd_y * ppn);
        }
        else
        {
            pxEnd_y = y + (nmDiffEnd_y * ppn);
        }
        
        cairo_set_source_rgb(cr, 1, 1, 1);
        // cairo_arc(cr, pxStart_x, pxStart_y, 5, 0, 2*M_PI);
        cairo_save(cr);
        
        cairo_set_line_width(cr, ppn/50);
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
        cairo_move_to(cr, pxStart_x, pxStart_y);
        
        // cairo_arc(cr, pxEnd_x, pxEnd_y, 5, 0, 2*M_PI);
        cairo_line_to(cr, pxEnd_x, pxEnd_y);
        cairo_stroke(cr);
        
        cairo_restore(cr);
        
        cairo_set_font_size(cr, 30);
        
        angle = atan2(pxEnd_y - pxStart_y, pxEnd_x - pxStart_x);
        cairo_text_extents_t extents;
        cairo_text_extents(cr, rwyNumsList[f][1], &extents);
        
        cairo_save(cr);
        cairo_translate(cr, 30 * cos(angle), 30 * sin(angle));
        
        // Transformations for the first side of the runway text
        cairo_translate(cr, pxEnd_x - (extents.width / 2 + extents.x_bearing) * cos(angle + M_PI / 2) - (extents.height / 2 + extents.y_bearing) * sin(angle + M_PI / 2), pxEnd_y - (extents.width / 2 + extents.x_bearing) * sin(angle + M_PI / 2) + (extents.height / 2 + extents.y_bearing) * cos(angle + M_PI / 2));
        cairo_rotate(cr, angle + M_PI / 2);
        center_x = extents.x_bearing + extents.width / 2;
        center_y = extents.y_bearing + extents.height / 2;
        cairo_translate(cr, center_x, center_y);
        cairo_rotate(cr, M_PI);
        cairo_translate(cr, -center_x, -center_y);
        
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_move_to(cr, extents.x_bearing, extents.y_bearing + extents.height);
        cairo_line_to(cr, extents.x_bearing, extents.y_bearing);
        cairo_line_to(cr, extents.x_bearing + extents.width / 2, extents.y_bearing - extents.height / 2);
        cairo_line_to(cr, extents.x_bearing + extents.width, extents.y_bearing);
        cairo_line_to(cr, extents.x_bearing + extents.width, extents.y_bearing + extents.height);
        cairo_close_path(cr);
        cairo_fill(cr);
        
        cairo_set_source_rgb(cr, 1, 1, 1);
        
        cairo_show_text(cr, rwyNumsList[f][1]);
        
        cairo_set_line_width(cr, 2.5);
        cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
        cairo_move_to(cr, extents.x_bearing, extents.y_bearing + extents.height);
        cairo_line_to(cr, extents.x_bearing, extents.y_bearing);
        cairo_line_to(cr, extents.x_bearing + extents.width / 2, extents.y_bearing - extents.height / 2);
        cairo_line_to(cr, extents.x_bearing + extents.width, extents.y_bearing);
        cairo_line_to(cr, extents.x_bearing + extents.width, extents.y_bearing + extents.height);
        cairo_stroke(cr);
        
        cairo_set_font_size(cr, 35);
        // second side
        
        cairo_restore(cr);
        
        cairo_save(cr);
        cairo_translate(cr, 30 * cos(angle + M_PI), 30 * sin(angle + M_PI));
        
        cairo_text_extents(cr, rwyNumsList[f][0], &extents);
        
        // Transformations for the second side of the runway text
        cairo_translate(cr, pxStart_x - (extents.width / 2 + extents.x_bearing) * cos(angle + M_PI / 2) - (extents.height / 2 + extents.y_bearing) * sin(angle + M_PI / 2), pxStart_y - (extents.width / 2 + extents.x_bearing) * sin(angle + M_PI / 2) + (extents.height / 2 + extents.y_bearing) * cos(angle + M_PI / 2));
        cairo_rotate(cr, angle + M_PI + M_PI / 2); // Rotate by 180 degrees 
        center_x = extents.x_bearing + extents.width / 2;
        center_y = extents.y_bearing + extents.height / 2;
        cairo_translate(cr, 0, -extents.height * 2);
        cairo_rotate(cr, M_PI);
        
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_move_to(cr, extents.x_bearing, extents.y_bearing + extents.height);
        cairo_line_to(cr, extents.x_bearing, extents.y_bearing);
        cairo_line_to(cr, extents.x_bearing + extents.width / 2, extents.y_bearing - extents.height / 2);
        cairo_line_to(cr, extents.x_bearing + extents.width, extents.y_bearing);
        cairo_line_to(cr, extents.x_bearing + extents.width, extents.y_bearing + extents.height);
        cairo_close_path(cr);
        cairo_fill(cr);
        
        cairo_set_source_rgb(cr, 1, 1, 1);
        
        cairo_show_text(cr, rwyNumsList[f][0]);
        
        cairo_set_line_width(cr, 2.5);
        cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
        
        cairo_move_to(cr, extents.x_bearing, extents.y_bearing + extents.height);
        cairo_line_to(cr, extents.x_bearing, extents.y_bearing);
        cairo_line_to(cr, extents.x_bearing + extents.width / 2, extents.y_bearing - extents.height / 2);
        cairo_line_to(cr, extents.x_bearing + extents.width, extents.y_bearing);
        cairo_line_to(cr, extents.x_bearing + extents.width, extents.y_bearing + extents.height);
        cairo_stroke(cr);
        
        cairo_restore(cr);
        
        // Draw the rwy numbers in the center:
        
        char combinedNums[256];
        
        strcpy(combinedNums, rwyNumsList[f][0]);
        strcat(combinedNums, "-");
        strcat(combinedNums, rwyNumsList[f][1]);
        
        // Calculate the center of the runway
        double centerX = (pxStart_x + pxEnd_x) / 2.0;
        double centerY = (pxStart_y + pxEnd_y) / 2.0;
        
        // Define the width and height of the rectangle
        double rectWidth = 100.0;
        double rectHeight = 20.0;
        
        // Calculate the coordinates of the rectangle
        double rectX = centerX - rectWidth / 2.0;
        double rectY = centerY - rectHeight / 2.0;
        
        // Set the font size for the runway text
        cairo_set_font_size(cr, 20);
        
        // Calculate the text extents
        cairo_text_extents_t textExtents;
        cairo_text_extents(cr, combinedNums, &textExtents);
        
        // Apply transformations for the rectangle and runway text
        cairo_save(cr);
        cairo_translate(cr, centerX, centerY);
        cairo_rotate(cr, angle);
        
        // Draw the rectangle
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_rectangle(cr, -textExtents.width / 2 - 3, -textExtents.height / 2, textExtents.width + 7, textExtents.height);
        cairo_fill(cr);
        
        // Draw the runway text in the middle of the rectangle
        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_move_to(cr, -textExtents.width / 2.0, textExtents.height / 2);
        cairo_show_text(cr, combinedNums);
        
        cairo_restore(cr);
    }
    
 
    
    
    //cairo_restore(cr);
    
    cairo_set_font_size(cr, 20);
    // draw_text(cr, test, 1000, 140, 2);
     
}

static void
mapND(cairo_t *cr, double x, double y)
{
    
    const char *degreesList[] = {"N", "3", "6", "E", "12", "15", "S", "21", "24", "W", "30", "33"};
    
    double r = 375;
    double spike_len = 10;
    int spike_count = 72;
    double angle_increment = 2 * M_PI / spike_count;
    
    cairo_save(cr);
    cairo_translate(cr, x, y);
    cairo_rotate(cr, -M_PI / 2);
    cairo_translate(cr, -x, -y);
    
    for (int i = 0; i < spike_count; ++i)
    {
        cairo_set_source_rgb(cr, 1, 1, 1);
        double angle = i * angle_increment;
        cairo_move_to(cr, x + r * cos(angle), y + r * sin(angle));
        if (i % 2 != 0)
        {
            cairo_set_source_rgb(cr, 1, 1, 1);
            cairo_line_to(cr, x + (r + spike_len) * cos(angle), y + (r + spike_len) * sin(angle));
            cairo_stroke(cr);
        }
        else
        {
            cairo_set_source_rgb(cr, 1, 1, 1);
            cairo_line_to(cr, x + (r + (spike_len * 2)) * cos(angle), y + (r + (spike_len * 2)) * sin(angle));
            cairo_stroke(cr);
        }
        
        if (i % 6 == 0)
        {
            /*
             cairo_set_font_size(cr, 35);
             
             draw_text(cr, degreesList[i/6],x + (r + (spike_len*2)) * cos(angle), y + (r + (spike_len*2)) * sin(angle), 1);
             */
            
            cairo_set_font_size(cr, 35);
            cairo_save(cr);
            double text_angle = angle + M_PI_2;
            cairo_text_extents_t extents;
            cairo_text_extents(cr, degreesList[i / 6], &extents);
            
            cairo_translate(cr, x + (r + (spike_len * 2)) * cos(angle), y + (r + (spike_len * 2)) * sin(angle));
            
            cairo_rotate(cr, text_angle);
            cairo_set_source_rgba(cr, 0, 0, 0, 0);
            cairo_rectangle(cr, -extents.width / 2, -extents.height / 2, extents.width, extents.height);
            cairo_stroke(cr);
            
            cairo_set_source_rgb(cr, 1, 1, 1);
            
            cairo_translate(cr, -extents.width / 2, extents.height / 20);
            
            cairo_show_text(cr, degreesList[i / 6]);
            cairo_restore(cr);
            cairo_set_source_rgb(cr, 1, 1, 1);
        }
    }
    
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_arc(cr, x, y, r, 0, 2 * M_PI);
    cairo_stroke(cr);
    
    static const double dashed3[] = {9.5, 51.0, 9.5};
    
    cairo_set_dash(cr, dashed3, 1, 0);
    cairo_arc(cr, x, y, r / 2, 0, 2 * M_PI);
    cairo_stroke(cr);
    
    cairo_restore(cr);
    
    static const double undash[] = {1.0};
    
    cairo_set_dash(cr, undash, 0, 0);
    
    // DRAW PLANE -------------------------------------------->
    cairo_save(cr);
    cairo_set_source_rgb(cr, 1, 0, 1);
    cairo_set_line_width(cr, 4.0);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_move_to(cr, x, y - 15);
    cairo_line_to(cr, x, y + 40);
    
    cairo_move_to(cr, x - 35, y);
    cairo_line_to(cr, x + 35, y);
    
    cairo_move_to(cr, x - 10, y + 35);
    cairo_line_to(cr, x + 10, y + 35);
    
    // 375
    
    cairo_stroke(cr);
    cairo_restore(cr);
    // DRAW PLANE -------------------------------------------->
}

// Global position variables
double rwy_x = 500;
double rwy_y = 500;

double click_x = 0;
double click_y = 0;

int ppn = 260;

static gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    
    if (event->button == GDK_BUTTON_PRIMARY) // left mouse button
    {
        click_x = event->x - rwy_x;
        click_y = event->y - rwy_y;
    }
    
    return TRUE;
}

static gboolean on_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    
    if (event->state & GDK_BUTTON1_MASK) // left mouse button is pressed
    {
        rwy_x = event->x - click_x;
        rwy_y = event->y - click_y;
        
        gtk_widget_queue_draw(widget);
    }
    
    return TRUE;
}

static gboolean on_scroll_event(GtkWidget *widget, GdkEventScroll *event, gpointer data)
{
    if (event->direction == GDK_SCROLL_UP)
    {
        ppn += 20; // zoom in, adjust the value to suit your needs
    }
    else if (event->direction == GDK_SCROLL_DOWN)
    {
        ppn -= 20; // zoom out, adjust the value to suit your needs
        if (ppn < 0)
            ppn = 0; // ensure it doesn't go below 0
    }
    
    gtk_widget_queue_draw(widget);
    
    return TRUE;
}

static void do_drawing(cairo_t *cr, GtkWidget *widget)
{
    GtkWidget *win = gtk_widget_get_toplevel(widget);
    
    int width, height;
    gtk_window_get_size(GTK_WINDOW(win), &width, &height);
    
    cairo_select_font_face(cr, "BoeingEICAS", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr, 0, 0, 10000, 10000);
    cairo_fill(cr);
    
    drawMapRWY(cr, rwy_x, rwy_y, "KDFW", ppn);
    // KSFO NOT DRAWING
    // KLAX NOT DRAWING
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr,
                              gpointer user_data)
{
    do_drawing(cr, widget);
    
    return FALSE;
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *darea;
    
    gtk_init(&argc, &argv);
    
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    
    darea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), darea);
    
    g_signal_connect(G_OBJECT(darea), "draw",
                     G_CALLBACK(on_draw_event), NULL);
    
    g_signal_connect(G_OBJECT(window), "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(darea), "button-press-event",
                     G_CALLBACK(on_button_press_event), NULL);
    g_signal_connect(G_OBJECT(darea), "motion-notify-event",
                     G_CALLBACK(on_motion_notify_event), NULL);
    
    g_signal_connect(G_OBJECT(darea), "scroll-event",
                     G_CALLBACK(on_scroll_event), NULL);
    gtk_widget_set_events(darea, gtk_widget_get_events(darea) |
                          GDK_BUTTON_PRESS_MASK |
                          GDK_POINTER_MOTION_MASK |
                          GDK_SCROLL_MASK); // add scroll mask
    
    // You must set the widget to receive these events.
    gtk_widget_set_events(darea, gtk_widget_get_events(darea) |
                          GDK_BUTTON_PRESS_MASK |
                          GDK_POINTER_MOTION_MASK);
    
    // gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS);
    
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 1000);
    
    gtk_window_set_title(GTK_WINDOW(window), "APTVISUALS");
    
    gtk_widget_show_all(window);
    
    gtk_main();
    
    return 0;
}
