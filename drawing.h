// drawing.h
#ifndef DRAWING_H
#define DRAWING_H

#include <cairo.h>

void drawMap(cairo_t *cr, double x, double y, const char *apt, int pixelsPerNM);
void exportLatLonListToCSV();

#endif // DRAWING_H
