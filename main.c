// main.c
// ND_DISPLAY
// Created by on 1/31/23.

#include <gtk/gtk.h>
#include "drawing.h"
#include "parser.h"

int timesParsed = 0;
double rwy_x = 250;
double rwy_y = 250;
double click_x = 0;
double click_y = 0;
int ppn = 110;
char airport[4];

// Path to your icon file
const char *icon_path = "./icon.png";

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
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_rectangle(cr, 0, 0, 10000, 10000);
    cairo_fill(cr);
    
    if (!timesParsed)
    {
        parseAptdat("LFPG");
        timesParsed++;
    }
    drawMap(cr, rwy_x, rwy_y, "LFPG", ppn);
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    do_drawing(cr, widget);
    return FALSE;
}

int main(int argc, char *argv[])
{
    exportLatLonListToCSV();


    GtkWidget *window;
    GtkWidget *darea;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    darea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), darea);

    g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), NULL);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(darea), "button-press-event", G_CALLBACK(on_button_press_event), NULL);
    g_signal_connect(G_OBJECT(darea), "motion-notify-event", G_CALLBACK(on_motion_notify_event), NULL);
    g_signal_connect(G_OBJECT(darea), "scroll-event", G_CALLBACK(on_scroll_event), NULL);

    gtk_widget_set_events(darea, gtk_widget_get_events(darea) | GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK | GDK_SCROLL_MASK);

    gtk_window_set_default_size(GTK_WINDOW(window), 900, 500);
    gtk_window_set_title(GTK_WINDOW(window), "APTVISUALS");
    gtk_window_set_icon_from_file(GTK_WINDOW(window), icon_path, NULL);

   

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
