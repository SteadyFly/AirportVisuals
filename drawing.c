// drawing.c
#include "drawing.h"
#include "parser.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>


extern char aptRaw[256];
extern char iata_code[256];
extern char datum_lat[256];
extern char datum_lon[256];
extern char rwyNumsList[32][2][48];
extern double latLonList[512][256][5];
extern char rwyCoordsListChar[10][4][48];
extern int rwyCount;

static void draw_text(cairo_t *cr, const char *text, double x, double y, int align)
{
    cairo_text_extents_t extents;
    cairo_text_extents(cr, text, &extents);
    
    switch (align)
    {
        case 0: cairo_move_to(cr, x, y); break;
        case 1: cairo_move_to(cr, x - extents.width / 2, y); break;
        case 2: cairo_move_to(cr, x - extents.width, y); break;
        default: break;
    }
    cairo_show_text(cr, text);
}

static double distance(double lat1, double lon1, double lat2, double lon2)
{
    double degreesToRadians = M_PI / 180;
    lat1 *= degreesToRadians;
    lon1 *= degreesToRadians;
    lat2 *= degreesToRadians;
    lon2 *= degreesToRadians;

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double a = sin(dlat / 2) * sin(dlat / 2) + cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double R = 6371;

    return R * c;
}

void calculate_quadratic_bezier(double *p0, double *p1, double *p2, double *result, int resolution) {
    for (int i = 0; i <= resolution; i++) {
        double t = (double)i / resolution;
        double u = 1 - t;
        result[2 * i] = u * u * p0[0] + 2 * u * t * p1[0] + t * t * p2[0];
        result[2 * i + 1] = u * u * p0[1] + 2 * u * t * p1[1] + t * t * p2[1];
    }
}

//-------------------------------------------

static void draw_airport_info(cairo_t *cr, double x, double y)
{
    cairo_set_source_rgb(cr, 0, 0, 0);

    char aptFull[256];
    memcpy(aptFull, &aptRaw[18], 255);
    aptFull[255] = '\0';
    cairo_set_font_size(cr, 35);
    draw_text(cr, aptFull, 1000 - 5, 35, 2);

    char aptICAO[5];
    memcpy(aptICAO, &aptRaw[13], 4);
    aptICAO[4] = '\0';
    cairo_set_font_size(cr, 35);
    draw_text(cr, aptICAO, 900, 70, 2);

    char aptIATA[4];
    memcpy(aptIATA, &iata_code[10], 3);
    aptIATA[3] = '\0';
    cairo_set_font_size(cr, 35);
    draw_text(cr, aptIATA, 1000 - 5, 70, 2);
}

static void draw_pavement(cairo_t *cr, double x, double y, double datum_lat_value, double datum_lon_value, int ppn)
{

    
    double CURR_POS[] = {datum_lat_value, datum_lon_value};
    cairo_translate(cr, x, y);
    cairo_rotate(cr, -M_PI / 2);
    cairo_translate(cr, -x, -y);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
    cairo_set_source_rgb(cr, 0,0,0);
    cairo_set_line_width(cr, 1);

    double firstLat = 0.0;
    double firstLon = 0.0;

    for (int e = 0; e < 512; e++)
    {
        if (latLonList[e][0][0] == 0)
        {
            break;
        }

        cairo_new_path(cr);
        cairo_set_source_rgb(cr, 0,0,0);

        for (int q = 0; q < 8192; q++)
        {
            
            firstLat = latLonList[e][0][1];
            firstLon = latLonList[e][0][2];
            
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
        
            if (q == 0)
            {
                cairo_move_to(cr, point_x, point_y);

            }
            else
            {
                cairo_line_to(cr, point_x, point_y);
            }
        }


        //cairo_line_to(cr, first_point_x, first_point_y);
       // cairo_close_path(cr);
        cairo_stroke(cr); 
        
        
    }

    FILE *fp;
    fp = fopen("lat_lon_list.csv", "w");
    if (fp == NULL) {
        printf("Error opening file for writing!\n");
        return;
    }

    // Write header
    fprintf(fp, "Index,Latitude,Longitude\n");

    // Write data
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 8192; j++) {
            if (latLonList[i][j][0] == 0) {
                break;
            }
            fprintf(fp, "%d,%f,%f\n", i, latLonList[i][j][1], latLonList[i][j][2]);
        }
    }

    fclose(fp);

}



static void draw_runways(cairo_t *cr, double x, double y, double datum_lat_value, double datum_lon_value, int ppn)
{

    cairo_set_source_rgb(cr, 0, 0, 0);
    double rwyCoordsList[10][4];
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            char *ptr;
            rwyCoordsList[i][j] = strtod(rwyCoordsListChar[i][j], &ptr);
        }
    }

    double CURR_POS[] = {datum_lat_value, datum_lon_value};

    for (int f = 0; f <= 10; f++)
    {
        double rwy_start_lat = rwyCoordsList[f][0];
        double rwy_start_lon = rwyCoordsList[f][1];
        double rwy_end_lat = rwyCoordsList[f][2];
        double rwy_end_lon = rwyCoordsList[f][3];

        double nmDiffStart_x = distance(CURR_POS[0], CURR_POS[1], rwy_start_lat, CURR_POS[1]);
        double nmDiffStart_y = distance(CURR_POS[0], CURR_POS[1], CURR_POS[0], rwy_start_lon);
        double nmDiffEnd_x = distance(CURR_POS[0], CURR_POS[1], rwy_end_lat, CURR_POS[1]);
        double nmDiffEnd_y = distance(CURR_POS[0], CURR_POS[1], CURR_POS[0], rwy_end_lon);

        double pxStart_x = (CURR_POS[0] > rwy_start_lat) ? x - (nmDiffStart_x * ppn) : x + (nmDiffStart_x * ppn);
        double pxStart_y = (CURR_POS[1] > rwy_start_lon) ? y - (nmDiffStart_y * ppn) : y + (nmDiffStart_y * ppn);
        double pxEnd_x = (CURR_POS[0] > rwy_end_lat) ? x - (nmDiffEnd_x * ppn) : x + (nmDiffEnd_x * ppn);
        double pxEnd_y = (CURR_POS[1] > rwy_end_lon) ? y - (nmDiffEnd_y * ppn) : y + (nmDiffEnd_y * ppn);
        double angle = atan2(pxEnd_y - pxStart_y, pxEnd_x - pxStart_x);

        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, 10);
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
        cairo_move_to(cr, pxStart_x, pxStart_y);
        cairo_line_to(cr, pxEnd_x, pxEnd_y);
        cairo_stroke(cr);
   //     /*
        cairo_set_font_size(cr, 30);
        
        cairo_text_extents_t extents;
        cairo_text_extents(cr, rwyNumsList[f][1], &extents);

        cairo_save(cr);
        cairo_translate(cr, 30 * cos(angle), 30 * sin(angle));
        cairo_translate(cr, pxEnd_x - (extents.width / 2 + extents.x_bearing) * cos(angle + M_PI / 2) - (extents.height / 2 + extents.y_bearing) * sin(angle + M_PI / 2), pxEnd_y - (extents.width / 2 + extents.x_bearing) * sin(angle + M_PI / 2) + (extents.height / 2 + extents.y_bearing) * cos(angle + M_PI / 2));
        cairo_rotate(cr, angle + M_PI / 2);
        double center_x = extents.x_bearing + extents.width / 2;
        double center_y = extents.y_bearing + extents.height / 2;
        cairo_translate(cr, center_x, center_y);
        cairo_rotate(cr, M_PI);
        cairo_translate(cr, -center_x, -center_y);

        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_move_to(cr, extents.x_bearing, extents.y_bearing + extents.height);
        cairo_line_to(cr, extents.x_bearing, extents.y_bearing);
        cairo_line_to(cr, extents.x_bearing + extents.width / 2, extents.y_bearing - extents.height / 2);
        cairo_line_to(cr, extents.x_bearing + extents.width, extents.y_bearing);
        cairo_line_to(cr, extents.x_bearing + extents.width, extents.y_bearing + extents.height);
        cairo_close_path(cr);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_show_text(cr, rwyNumsList[f][1]);

        cairo_set_line_width(cr, 2.5);
        cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
        cairo_move_to(cr, extents.x_bearing, extents.y_bearing + extents.height);
        cairo_line_to(cr, extents.x_bearing, extents.y_bearing);
        cairo_line_to(cr, extents.x_bearing + extents.width / 2, extents.y_bearing - extents.height / 2);
        cairo_line_to(cr, extents.x_bearing + extents.width, extents.y_bearing);
        cairo_line_to(cr, extents.x_bearing + extents.width, extents.y_bearing + extents.height);
        cairo_stroke(cr);
        cairo_restore(cr);

        cairo_save(cr);
        cairo_translate(cr, 30 * cos(angle + M_PI), 30 * sin(angle + M_PI));
        cairo_text_extents(cr, rwyNumsList[f][0], &extents);
        cairo_translate(cr, pxStart_x - (extents.width / 2 + extents.x_bearing) * cos(angle + M_PI / 2) - (extents.height / 2 + extents.y_bearing) * sin(angle + M_PI / 2), pxStart_y - (extents.width / 2 + extents.x_bearing) * sin(angle + M_PI / 2) + (extents.height / 2 + extents.y_bearing) * cos(angle + M_PI / 2));
        cairo_rotate(cr, angle + M_PI + M_PI / 2);
        center_x = extents.x_bearing + extents.width / 2;
        center_y = extents.y_bearing + extents.height / 2;
        cairo_translate(cr, 0, -extents.height * 2);
        cairo_rotate(cr, M_PI);

        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_move_to(cr, extents.x_bearing, extents.y_bearing + extents.height);
        cairo_line_to(cr, extents.x_bearing, extents.y_bearing);
        cairo_line_to(cr, extents.x_bearing + extents.width / 2, extents.y_bearing - extents.height / 2);
        cairo_line_to(cr, extents.x_bearing + extents.width, extents.y_bearing);
        cairo_line_to(cr, extents.x_bearing + extents.width, extents.y_bearing + extents.height);
        cairo_close_path(cr);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, 0, 0, 0);
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
//*/
        char combinedNums[256];
        strcpy(combinedNums, rwyNumsList[f][0]);
        strcat(combinedNums, "-");
        strcat(combinedNums, rwyNumsList[f][1]);

        double centerX = (pxStart_x + pxEnd_x) / 2.0;
        double centerY = (pxStart_y + pxEnd_y) / 2.0;

        double rectWidth = 100.0;
        double rectHeight = 20.0;

        double rectX = centerX - rectWidth / 2.0;
        double rectY = centerY - rectHeight / 2.0;

        cairo_set_font_size(cr, 20);
        cairo_text_extents_t textExtents;
        cairo_text_extents(cr, combinedNums, &textExtents);

        cairo_save(cr);
        cairo_translate(cr, centerX, centerY);
        cairo_rotate(cr, angle);

        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_rectangle(cr, -textExtents.width / 2 - 3, -textExtents.height / 2, textExtents.width + 7, textExtents.height);
        cairo_fill(cr);

        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_move_to(cr, -textExtents.width / 2.0, textExtents.height / 2);
        cairo_show_text(cr, combinedNums);

        cairo_restore(cr);
    }
}

void drawMap(cairo_t *cr, double x, double y, const char *apt, int pixelsPerNM)
{
    int ppn = pixelsPerNM;

    draw_airport_info(cr, x, y);

    char datum_lat_raw[256];
    memcpy(datum_lat_raw, datum_lat + 10, sizeof(datum_lat) - 14);
    datum_lat_raw[sizeof(datum_lat) - 14] = '\0';
    double datum_lat_value = atof(datum_lat_raw);

    char datum_lon_raw[256];
    memcpy(datum_lon_raw, datum_lon + 10, sizeof(datum_lon) - 14);
    datum_lon_raw[sizeof(datum_lon) - 14] = '\0';
    double datum_lon_value = atof(datum_lon_raw);

    draw_pavement(cr, x, y, datum_lat_value, datum_lon_value, ppn);
    draw_runways(cr, x, y, datum_lat_value, datum_lon_value, ppn);
}



void exportLatLonListToCSV() {
   
}