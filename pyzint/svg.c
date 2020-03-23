#include <string.h>
#include "common.h"

extern void make_html_friendly(unsigned char * string, char * html_version);

int svg_plot(struct zint_symbol *symbol) {
    int error_number = 0;
    const char *locale = NULL;
    float ax, ay, bx, by, cx, cy, dx, dy, ex, ey, fx, fy;
    float radius;
    int i;

    struct zint_vector_rect *rect;
    struct zint_vector_hexagon *hex;
    struct zint_vector_circle *circle;
    struct zint_vector_string *string;

    int html_len = strlen((char *)symbol->text) + 1;

    for (i = 0; i < strlen((char *)symbol->text); i++) {
        switch(symbol->text[i]) {
            case '>':
            case '<':
            case '"':
            case '&':
            case '\'':
                html_len += 6;
                break;
        }
    }

    char html_string[html_len];

    /* Check for no created vector set */
    /* E-Mail Christian Schmitz 2019-09-10: reason unknown  Ticket #164*/
    if (symbol->vector == NULL) {
        return ZINT_ERROR_INVALID_DATA;
    }

    locale = setlocale(LC_ALL, "C");

    /* Start writing the header */
    fprintf(fsvg, "<?xml version=\"1.0\" standalone=\"no\"?>\n");
    fprintf(fsvg, "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n");
    fprintf(fsvg, "   \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n");
    fprintf(fsvg, "<svg width=\"%d\" height=\"%d\" version=\"1.1\"\n", (int) ceil(symbol->vector->width), (int) ceil(symbol->vector->height));
    fprintf(fsvg, "   xmlns=\"http://www.w3.org/2000/svg\">\n");
    fprintf(fsvg, "   <desc>Zint Generated Symbol\n");
    fprintf(fsvg, "   </desc>\n");
    fprintf(fsvg, "\n   <g id=\"barcode\" fill=\"#%s\">\n", symbol->fgcolour);

    fprintf(fsvg, "      <rect x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" fill=\"#%s\" />\n", (int) ceil(symbol->vector->width), (int) ceil(symbol->vector->height), symbol->bgcolour);

    rect = symbol->vector->rectangles;
    while (rect) {
        fprintf(fsvg, "      <rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" />\n", rect->x, rect->y, rect->width, rect->height);
        rect = rect->next;
    }

    hex = symbol->vector->hexagons;
    while (hex) {
        radius = hex->diameter / 2.0;
        ay = hex->y + (1.0 * radius);
        by = hex->y + (0.5 * radius);
        cy = hex->y - (0.5 * radius);
        dy = hex->y - (1.0 * radius);
        ey = hex->y - (0.5 * radius);
        fy = hex->y + (0.5 * radius);
        ax = hex->x;
        bx = hex->x + (0.86 * radius);
        cx = hex->x + (0.86 * radius);
        dx = hex->x;
        ex = hex->x - (0.86 * radius);
        fx = hex->x - (0.86 * radius);
        fprintf(fsvg, "      <path d=\"M %.2f %.2f L %.2f %.2f L %.2f %.2f L %.2f %.2f L %.2f %.2f L %.2f %.2f Z\" />\n", ax, ay, bx, by, cx, cy, dx, dy, ex, ey, fx, fy);
        hex = hex->next;
    }

    circle = symbol->vector->circles;
    while (circle) {
        if (circle->colour) {
            fprintf(fsvg, "      <circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" fill=\"#%s\" />\n", circle->x, circle->y, circle->diameter / 2.0, symbol->bgcolour);
        } else {
            fprintf(fsvg, "      <circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" fill=\"#%s\" />\n", circle->x, circle->y, circle->diameter / 2.0, symbol->fgcolour);
        }
        circle = circle->next;
    }

    string = symbol->vector->strings;
    while (string) {
        fprintf(fsvg, "      <text x=\"%.2f\" y=\"%.2f\" text-anchor=\"middle\"\n", string->x, string->y);
        fprintf(fsvg, "         font-family=\"Helvetica\" font-size=\"%.1f\" fill=\"#%s\" >\n", string->fsize, symbol->fgcolour);
        make_html_friendly(string->text, html_string);
        fprintf(fsvg, "         %s\n", html_string);
        fprintf(fsvg, "      </text>\n");
        string = string->next;
    }

    fprintf(fsvg, "   </g>\n");
    fprintf(fsvg, "</svg>\n");

    if (symbol->output_options & BARCODE_STDOUT) {
        fflush(fsvg);
    } else {
        fclose(fsvg);
    }

    if (locale)
        setlocale(LC_ALL, locale);

    return error_number;
}
