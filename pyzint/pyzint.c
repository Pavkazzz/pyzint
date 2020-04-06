#include <Python.h>
#include "endianness.h"
#include "src/zint/backend/zint.h"
#include "src/zint/backend/common.h"
#include "src/zint/backend/gb18030.h"

extern void make_html_friendly(unsigned char * string, char * html_version);

typedef struct {
    PyObject_HEAD
    PyObject *data;
    char *human_symbology;
    int show_hrt;
    float scale;
    unsigned int symbology;
    int option_1;
    int option_2;
    int option_3;
    int fontsize;
    char *buffer;
    Py_ssize_t length;
} CZINT;


static PyObject *
CZINT_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    CZINT *self;

    self = (CZINT *) type->tp_alloc(type, 0);
    return (PyObject *) self;
}

static void
CZINT_dealloc(CZINT *self) {
    Py_CLEAR(self->data);
    self->buffer = NULL;

    Py_TYPE(self)->tp_free((PyObject *) self);
}

uint8_t octet2char(const unsigned char* src) {
    unsigned char result = 0;
    result |= (src[0]?1:0) << 7;
    result |= (src[1]?1:0) << 6;
    result |= (src[2]?1:0) << 5;
    result |= (src[3]?1:0) << 4;
    result |= (src[4]?1:0) << 3;
    result |= (src[5]?1:0) << 2;
    result |= (src[6]?1:0) << 1;
    result |= (src[7]?1:0);

    return result;
}

static int
CZINT_init(CZINT *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {
      "data", "kind", "scale", "show_text", "option_1", "option_2", "option_3", "fontsize", NULL
    };

    self->show_hrt = 1;
    self->scale = 1.0;
    self->option_1 = -1;
    self->option_2 = 0;
    self->option_3 = 928; // PDF_MAX
    self->fontsize = 8;

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "Ob|fbiiii", kwlist,
            &self->data, &self->symbology, &self->scale, &self->show_hrt, &self->option_1, &self->option_2,
            &self->option_3, &self->fontsize
    )) return -1;

    if (self->scale <= 0) {
        PyErr_SetString(
            PyExc_ValueError,
            "scale must be greater then zero"
        );
        return NULL;
    }


    if (self->scale > 10) {
        PyErr_SetString(
            PyExc_ValueError,
            "scale must be lesser then ten"
        );
        return NULL;
    }

    switch (self->symbology) {
        case (BARCODE_CODE11):
            self->human_symbology = "code11";
            break;
        case (BARCODE_C25MATRIX):
            self->human_symbology = "c25matrix";
            break;
        case (BARCODE_C25INTER):
            self->human_symbology = "c25inter";
            break;
        case (BARCODE_C25IATA):
            self->human_symbology = "c25iata";
            break;
        case (BARCODE_C25LOGIC):
            self->human_symbology = "c25logic";
            break;
        case (BARCODE_C25IND):
            self->human_symbology = "c25ind";
            break;
        case (BARCODE_CODE39):
            self->human_symbology = "code39";
            break;
        case (BARCODE_EXCODE39):
            self->human_symbology = "excode39";
            break;
        case (BARCODE_EANX):
            self->human_symbology = "eanx";
            break;
        case (BARCODE_EANX_CHK):
            self->human_symbology = "eanx_chk";
            break;
        case (BARCODE_EAN128):
            self->human_symbology = "ean128";
            break;
        case (BARCODE_CODABAR):
            self->human_symbology = "codabar";
            break;
        case (BARCODE_CODE128):
            self->human_symbology = "code128";
            break;
        case (BARCODE_DPLEIT):
            self->human_symbology = "dpleit";
            break;
        case (BARCODE_DPIDENT):
            self->human_symbology = "dpident";
            break;
        case (BARCODE_CODE16K):
            self->human_symbology = "code16k";
            break;
        case (BARCODE_CODE49):
            self->human_symbology = "code49";
            break;
        case (BARCODE_CODE93):
            self->human_symbology = "code93";
            break;
        case (BARCODE_FLAT):
            self->human_symbology = "flat";
            break;
        case (BARCODE_RSS14):
            self->human_symbology = "rss14";
            break;
        case (BARCODE_RSS_LTD):
            self->human_symbology = "rss_ltd";
            break;
        case (BARCODE_RSS_EXP):
            self->human_symbology = "rss_exp";
            break;
        case (BARCODE_TELEPEN):
            self->human_symbology = "telepen";
            break;
        case (BARCODE_UPCA):
            self->human_symbology = "upca";
            break;
        case (BARCODE_UPCA_CHK):
            self->human_symbology = "upca_chk";
            break;
        case (BARCODE_UPCE):
            self->human_symbology = "upce";
            break;
        case (BARCODE_UPCE_CHK):
            self->human_symbology = "upce_chk";
            break;
        case (BARCODE_POSTNET):
            self->human_symbology = "postnet";
            break;
        case (BARCODE_MSI_PLESSEY):
            self->human_symbology = "msi_plessey";
            break;
        case (BARCODE_FIM):
            self->human_symbology = "fim";
            break;
        case (BARCODE_LOGMARS):
            self->human_symbology = "logmars";
            break;
        case (BARCODE_PHARMA):
            self->human_symbology = "pharma";
            break;
        case (BARCODE_PZN):
            self->human_symbology = "pzn";
            break;
        case (BARCODE_PHARMA_TWO):
            self->human_symbology = "pharma_two";
            break;
        case (BARCODE_PDF417):
            self->human_symbology = "pdf417";
            break;
        case (BARCODE_PDF417TRUNC):
            self->human_symbology = "pdf417trunc";
            break;
        case (BARCODE_MAXICODE):
            self->human_symbology = "maxicode";
            break;
        case (BARCODE_QRCODE):
            self->human_symbology = "qrcode";
            break;
        case (BARCODE_CODE128B):
            self->human_symbology = "code128b";
            break;
        case (BARCODE_AUSPOST):
            self->human_symbology = "auspost";
            break;
        case (BARCODE_AUSREPLY):
            self->human_symbology = "ausreply";
            break;
        case (BARCODE_AUSROUTE):
            self->human_symbology = "ausroute";
            break;
        case (BARCODE_AUSREDIRECT):
            self->human_symbology = "ausredirect";
            break;
        case (BARCODE_ISBNX):
            self->human_symbology = "isbnx";
            break;
        case (BARCODE_RM4SCC):
            self->human_symbology = "rm4scc";
            break;
        case (BARCODE_DATAMATRIX):
            self->human_symbology = "datamatrix";
            break;
        case (BARCODE_EAN14):
            self->human_symbology = "ean14";
            break;
        case (BARCODE_VIN):
            self->human_symbology = "vin";
            break;
        case (BARCODE_CODABLOCKF):
            self->human_symbology = "codablockf";
            break;
        case (BARCODE_NVE18):
            self->human_symbology = "nve18";
            break;
        case (BARCODE_JAPANPOST):
            self->human_symbology = "japanpost";
            break;
        case (BARCODE_KOREAPOST):
            self->human_symbology = "koreapost";
            break;
        case (BARCODE_RSS14STACK):
            self->human_symbology = "rss14stack";
            break;
        case (BARCODE_RSS14STACK_OMNI):
            self->human_symbology = "rss14stack_omni";
            break;
        case (BARCODE_RSS_EXPSTACK):
            self->human_symbology = "rss_expstack";
            break;
        case (BARCODE_PLANET):
            self->human_symbology = "planet";
            break;
        case (BARCODE_MICROPDF417):
            self->human_symbology = "micropdf417";
            break;
        case (BARCODE_ONECODE):
            self->human_symbology = "onecode";
            break;
        case (BARCODE_PLESSEY):
            self->human_symbology = "plessey";
            break;
        case (BARCODE_TELEPEN_NUM):
            self->human_symbology = "telepen_num";
            break;
        case (BARCODE_ITF14):
            self->human_symbology = "itf14";
            break;
        case (BARCODE_KIX):
            self->human_symbology = "kix";
            break;
        case (BARCODE_AZTEC):
            self->human_symbology = "aztec";
            break;
        case (BARCODE_DAFT):
            self->human_symbology = "daft";
            break;
        case (BARCODE_MICROQR):
            self->human_symbology = "microqr";
            break;
        case (BARCODE_HIBC_128):
            self->human_symbology = "hibc_128";
            break;
        case (BARCODE_HIBC_39):
            self->human_symbology = "hibc_39";
            break;
        case (BARCODE_HIBC_DM):
            self->human_symbology = "hibc_dm";
            break;
        case (BARCODE_HIBC_QR):
            self->human_symbology = "hibc_qr";
            break;
        case (BARCODE_HIBC_PDF):
            self->human_symbology = "hibc_pdf";
            break;
        case (BARCODE_HIBC_MICPDF):
            self->human_symbology = "hibc_micpdf";
            break;
        case (BARCODE_HIBC_BLOCKF):
            self->human_symbology = "hibc_blockf";
            break;
        case (BARCODE_HIBC_AZTEC):
            self->human_symbology = "hibc_aztec";
            break;
        case (BARCODE_DOTCODE):
            self->human_symbology = "dotcode";
            break;
        case (BARCODE_HANXIN):
            self->human_symbology = "hanxin";
            break;
        case (BARCODE_MAILMARK):
            self->human_symbology = "mailmark";
            break;
        case (BARCODE_AZRUNE):
            self->human_symbology = "azrune";
            break;
        case (BARCODE_CODE32):
            self->human_symbology = "code32";
            break;
        case (BARCODE_EANX_CC):
            self->human_symbology = "eanx_cc";
            break;
        case (BARCODE_EAN128_CC):
            self->human_symbology = "ean128_cc";
            break;
        case (BARCODE_RSS14_CC):
            self->human_symbology = "rss14_cc";
            break;
        case (BARCODE_RSS_LTD_CC):
            self->human_symbology = "rss_ltd_cc";
            break;
        case (BARCODE_RSS_EXP_CC):
            self->human_symbology = "rss_exp_cc";
            break;
        case (BARCODE_UPCA_CC):
            self->human_symbology = "upca_cc";
            break;
        case (BARCODE_UPCE_CC):
            self->human_symbology = "upce_cc";
            break;
        case (BARCODE_RSS14STACK_CC):
            self->human_symbology = "rss14stack_cc";
            break;
        case (BARCODE_RSS14_OMNI_CC):
            self->human_symbology = "rss14_omni_cc";
            break;
        case (BARCODE_RSS_EXPSTACK_CC):
            self->human_symbology = "rss_expstack_cc";
            break;
        case (BARCODE_CHANNEL):
            self->human_symbology = "channel";
            break;
        case (BARCODE_CODEONE):
            self->human_symbology = "codeone";
            break;
        case (BARCODE_GRIDMATRIX):
            self->human_symbology = "gridmatrix";
            break;
        case (BARCODE_UPNQR):
            self->human_symbology = "upnqr";
            break;
        case (BARCODE_ULTRA):
            self->human_symbology = "ultra";
            break;
        case (BARCODE_RMQR):
            self->human_symbology = "rmqr";
            break;
        default:
            PyErr_Format(
                PyExc_ValueError,
                "Unknown barcode type %d",
                self->symbology
            );
            return -1;
    }

    if (PyBytes_Check(self->data)) {
        if (PyBytes_AsStringAndSize(self->data, &self->buffer, &self->length) == -1) {
            return -1;
        }
    } else if (PyUnicode_Check(self->data)) {
        self->buffer = (char *)PyUnicode_AsUTF8AndSize(self->data, &self->length);
        if (self->buffer == NULL) {
            return -1;
        }
    } else {
        PyErr_SetNone(PyExc_ValueError);
        return -1;
    }

    Py_INCREF(self->data);
    return 0;
}

static PyObject* CZINT_repr(CZINT *self) {
    return PyUnicode_FromFormat(
        "<%s as %p: kind=%s (%d) buffer=%s (%d) option-1=%d option-2=%d option-3=%d>",
        Py_TYPE(self)->tp_name, self, self->human_symbology,
        self->symbology, self->buffer, self->length,
        self->option_1, self->option_2, self->option_3
    );
}

int parse_color_hex(const char *str, unsigned int *target) {
    if (str == NULL) {
        return 0;
    }
    if (str[0] != '#') {
        PyErr_Format(
            PyExc_ValueError,
            "Invalid color: %s. Color must be started with '#'",
            str
        );
        return -1;
    }

    int res = sscanf(&str[1],"%2x%2x%2x", &target[0], &target[1], &target[2]);
    if (res < 0) {
        PyErr_SetString(
            PyExc_ValueError,
            "Invalid color format. Color must be in '#ffffff'"
        );
        return -1;
    }

    return 0;
}

int parse_color_str(const char *str, char *target) {
    if (str == NULL) {
        return 0;
    }
    if (str[0] != '#') {
        PyErr_Format(
            PyExc_ValueError,
            "Invalid color: %s. Color must be started with '#'",
            str
        );
        return -1;
    }
    memcpy(target, &str[1], 6);
    return 0;
}


PyDoc_STRVAR(CZINT_render_bmp_docstring,
    "Render bmp barcode. Image will 1bit color depth "
    "and user defined palette.\n\n"
    "    Zint('data', BARCODE_QRCODE).render_bmp(angle: int = 0, fgcolor: str = '#FFFFFF', bgcolor: str = '#000000') -> bytes"
);
static PyObject* CZINT_render_bmp(
    CZINT *self, PyObject *args, PyObject *kwds
) {
    static char *kwlist[] = {"angle", "fgcolor", "bgcolor", NULL};

    int angle = 0;
    unsigned int fgcolor[3] = {0, 0, 0};
    unsigned int bgcolor[3] = {255, 255, 255};

    char *fgcolor_str = NULL;
    char *bgcolor_str = NULL;


    if (!PyArg_ParseTupleAndKeywords(
        args, kwds, "|iss", kwlist,
        &angle, &fgcolor_str, &bgcolor_str
    )) return NULL;

    if (parse_color_hex(fgcolor_str, (unsigned int *)&fgcolor)) return NULL;
    if (parse_color_hex(bgcolor_str, (unsigned int *)&bgcolor)) return NULL;

    int res = 0;
    char *bmp = NULL;
    int bmp_1bit_size = 0;
    unsigned int offset = 0;

    static const unsigned int header_size = 62;
    static const unsigned char bmp_template[] = {
        0x42, 0x4d,
        0x00, 0x00, 0x00, 0x00, // size
        0x00, 0x00, 0x00, 0x00, // padding (zero)
        0x3e, 0x00, 0x00, 0x00, // 62
        0x28, 0x00, 0x00, 0x00, // 40
        0x00, 0x00, 0x00, 0x00, // width
        0x00, 0x00, 0x00, 0x00, // height
        0x01, 0x00, 0x01, 0x00, // planes and bpp
        0x00, 0x00, 0x00, 0x00, // compression
        0x00, 0x00, 0x00, 0x00, // size
        0xc4, 0x0e, 0x00, 0x00, // x pxls per meter
        0xc4, 0x0e, 0x00, 0x00, // y pxls per meter
        0x02, 0x00, 0x00, 0x00, // colors in table
        0x02, 0x00, 0x00, 0x00, // important color in table
        0x00, 0x00, 0x00, 0x00, // red channel - fgcolor
        0xff, 0xff, 0xff, 0xff  // green channel - bgcolor
    };

    struct zint_symbol *symbol = ZBarcode_Create();

    if (symbol == NULL) {
        PyErr_Format(
            PyExc_RuntimeError,
            "Symbol initialization failed"
        );
        return NULL;
    }

    Py_BEGIN_ALLOW_THREADS

    symbol->symbology = self->symbology;
    symbol->scale = self->scale;
    symbol->show_hrt = self->show_hrt;
    symbol->option_1 = self->option_1;
    symbol->option_2 = self->option_2;
    symbol->option_3 = self->option_3;
    symbol->fontsize = self->fontsize;

    res = ZBarcode_Encode_and_Buffer(
        symbol,
        (unsigned char *)self->buffer,
        self->length, angle
    );

    unsigned int width = symbol->bitmap_width;
    unsigned int height = symbol->bitmap_height;

    unsigned char bitmap[height][width + 8];
    if (bitmap == NULL) {
        return NULL;
    }
    memset(&bitmap, 0, height * (width + 8));

    {
        unsigned int length = height * width;
        for(unsigned int i=0; i<length; i++) {
            bitmap[i/width][i%width] = symbol->bitmap[i * 3];
        }
    }

    const int bmp_1bit_with_bytes = (width / 8 + (width % 8 == 0?0:1));

    const int padding = (bmp_1bit_with_bytes * 3) % 4;
    bmp_1bit_size = (
        header_size + bmp_1bit_with_bytes * height + (height * padding)
    );

    if (res == 0) {
        bmp = calloc(bmp_1bit_size * 1.1, sizeof(char *));

        memcpy(bmp, &bmp_template, header_size);

        unsigned int be_value = hton32(bmp_1bit_size);
        bmp[5] = (unsigned char)(be_value);
        bmp[4] = (unsigned char)(be_value >> 8);
        bmp[3] = (unsigned char)(be_value >> 16);
        bmp[2] = (unsigned char)(be_value >> 24);

        be_value = hton32(width);
        bmp[21] = (unsigned char)(be_value);
        bmp[20] = (unsigned char)(be_value >> 8);
        bmp[19] = (unsigned char)(be_value >> 16);
        bmp[18] = (unsigned char)(be_value >> 24);

        be_value = hton32(height);
        bmp[25] = (unsigned char)(be_value);
        bmp[24] = (unsigned char)(be_value >> 8);
        bmp[23] = (unsigned char)(be_value >> 16);
        bmp[22] = (unsigned char)(be_value >> 24);

        bmp[54] = (unsigned char)fgcolor[0];
        bmp[55] = (unsigned char)fgcolor[1];
        bmp[56] = (unsigned char)fgcolor[2];

        bmp[58] = (unsigned char)bgcolor[0];
        bmp[59] = (unsigned char)bgcolor[1];
        bmp[60] = (unsigned char)bgcolor[2];

        char *pixels = &bmp[header_size];

        {
            for(int y=height-1; y >= 0; y--) {
                for(int x=0; x < width; x+=8) {
                    pixels[offset] = octet2char(&bitmap[y][x]);
                    offset++;
                }
                offset += padding;
            }
        }
    }

    if (res == 0) {
        ZBarcode_Clear(symbol);
        ZBarcode_Delete(symbol);
    }

    Py_END_ALLOW_THREADS

    if (res > 0) {
        PyErr_Format(
            PyExc_RuntimeError,
            "Error while rendering: %s",
            symbol->errtxt
        );
        ZBarcode_Clear(symbol);
        ZBarcode_Delete(symbol);
        return NULL;
    }

    PyObject *result = PyBytes_FromStringAndSize(bmp, bmp_1bit_size);
    free(bmp);
    return result;
}

PyDoc_STRVAR(CZINT_render_svg_docstring,
    "Render svg barcode.\n\n"
    "    Zint('data', BARCODE_QRCODE).render_svg(angle: int = 0, fgcolor: str = '#FFFFFF', bgcolor: str = '#000000') -> bytes"
);
static PyObject* CZINT_render_svg(
    CZINT *self, PyObject *args, PyObject *kwds
) {
    static char *kwlist[] = {"angle", "scale", "fgcolor", "bgcolor", NULL};

    int angle = 0;
    char *fgcolor_str = "#000000";
    char *bgcolor_str = "#FFFFFF";


    if (!PyArg_ParseTupleAndKeywords(
        args, kwds, "|iss", kwlist,
        &angle, &fgcolor_str, &bgcolor_str
    )) return NULL;


    struct zint_symbol *symbol = ZBarcode_Create();

    if (symbol == NULL) {
        PyErr_Format(
            PyExc_RuntimeError,
            "Symbol initialization failed"
        );
        return NULL;
    }

    if (parse_color_str(fgcolor_str, (char *)&symbol->fgcolour)) return NULL;
    if (parse_color_str(bgcolor_str, (char *)&symbol->bgcolour)) return NULL;

    symbol->symbology = self->symbology;
    symbol->scale = self->scale;
    symbol->show_hrt = self->show_hrt;
    symbol->option_1 = self->option_1;
    symbol->option_2 = self->option_2;
    symbol->option_3 = self->option_3;
    symbol->fontsize = self->fontsize;

    int res = 0;
    char *fsvg = NULL;
    int len_fsvg = 0;
    int max_len = 4 * 1024 * 1024;
    struct zint_vector_rect *rect;
    struct zint_vector_hexagon *hex;
    struct zint_vector_circle *circle;
    struct zint_vector_string *string;
    float ax, ay, bx, by, cx, cy, dx, dy, ex, ey, fx, fy;
    float radius;


    Py_BEGIN_ALLOW_THREADS

    res = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char *)self->buffer, self->length, angle);

    if (res == 0) {

        int html_len = strlen((char *)symbol->text) + 1;

        {
            unsigned int text_length = strlen((char *)symbol->text);
            for(unsigned int i = 0; i < text_length; i++) {
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
        }
        char html_string[html_len];

        fsvg = calloc(max_len, sizeof(char *));

        /* Start writing the header */
        len_fsvg += snprintf(&fsvg[len_fsvg], max_len, "<?xml version=\"1.0\" standalone=\"no\"?>\n");

        len_fsvg += snprintf(&fsvg[len_fsvg], max_len, "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n");
        len_fsvg += snprintf(&fsvg[len_fsvg], max_len, "<svg width=\"%d\" height=\"%d\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">\n", (int) ceil(symbol->vector->width), (int) ceil(symbol->vector->height));
        len_fsvg += snprintf(&fsvg[len_fsvg], max_len, "<desc>Zint Generated Symbol via pyzint</desc>\n");
        len_fsvg += snprintf(&fsvg[len_fsvg], max_len, "<g id=\"barcode\" fill=\"#%s\">\n", symbol->fgcolour);
        len_fsvg += snprintf(&fsvg[len_fsvg], max_len, "<rect x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" fill=\"#%s\" />\n", (int) ceil(symbol->vector->width), (int) ceil(symbol->vector->height), symbol->bgcolour);
        rect = symbol->vector->rectangles;
        while (rect) {
            len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "<rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" />\n", rect->x, rect->y, rect->width, rect->height);
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
            len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "<path d=\"M %.2f %.2f L %.2f %.2f L %.2f %.2f L %.2f %.2f L %.2f %.2f L %.2f %.2f Z\" \n/>", ax, ay, bx, by, cx, cy, dx, dy, ex, ey, fx, fy);
            hex = hex->next;
        }

        circle = symbol->vector->circles;
        while (circle) {
            if (circle->colour) {
                len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "<circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" fill=\"#%s\" \n/>", circle->x, circle->y, circle->diameter / 2.0, symbol->bgcolour);
            } else {
                len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "<circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" fill=\"#%s\" \n/>", circle->x, circle->y, circle->diameter / 2.0, symbol->fgcolour);
            }
            circle = circle->next;
        }

        string = symbol->vector->strings;
        while (string) {
            len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "<text x=\"%.2f\" y=\"%.2f\" text-anchor=\"middle\" ", string->x, string->y);
            len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "font-family=\"Helvetica\" font-size=\"%.1f\" fill=\"#%s\">", string->fsize, symbol->fgcolour);
            make_html_friendly(string->text, html_string);
            len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, " %s ", html_string);
            len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "</text>");
            string = string->next;
        }

        len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "</g>");
        len_fsvg += snprintf(&fsvg[len_fsvg], max_len-len_fsvg, "</svg>");

        ZBarcode_Clear(symbol);
        ZBarcode_Delete(symbol);
    }

    Py_END_ALLOW_THREADS

    if (res > 0) {
        PyErr_Format(
            PyExc_RuntimeError,
            "Error while rendering: %s",
            symbol->errtxt
        );
        ZBarcode_Clear(symbol);
        ZBarcode_Delete(symbol);
        return NULL;
    }

    PyObject *result = PyBytes_FromStringAndSize(fsvg, len_fsvg);
    free(fsvg);

    if (result == NULL) {
        return NULL;
    }

    return result;
}


PyDoc_STRVAR(CZINT_human_symbology_docstring,
    "Return human readable barcode kind.\n\n"
    "    Zint('data', BARCODE_QRCODE).human_symbology() -> str"
);
static int
CZINT_human_symbology(CZINT *self, PyObject *args, PyObject *kwds)
{
    if (self->human_symbology == NULL) {
        PyErr_SetString(
            PyExc_ValueError,
            "symbology is empty"
        );
        return NULL;
    }
    return PyUnicode_FromString(self->human_symbology);
}

static PyMethodDef CZINT_methods[] = {
    {
        "render_bmp",
        (PyCFunction) CZINT_render_bmp, METH_VARARGS | METH_KEYWORDS,
        CZINT_render_bmp_docstring
    },
    {
        "render_svg",
        (PyCFunction) CZINT_render_svg, METH_VARARGS | METH_KEYWORDS,
        CZINT_render_svg_docstring
    },
    {
        "human_symbology",
        (PyCFunction) CZINT_human_symbology, METH_NOARGS,
        CZINT_human_symbology_docstring
    },

    {NULL}  /* Sentinel */

};



static PyTypeObject
ZINTType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Zint",
    .tp_doc = "Pyzint - c-binding to zint",
    .tp_basicsize = sizeof(CZINT),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = CZINT_new,
    .tp_init = (initproc) CZINT_init,
    .tp_dealloc = (destructor) CZINT_dealloc,
    .tp_methods = CZINT_methods,
    .tp_repr = (reprfunc) CZINT_repr
};


static PyModuleDef pyzint_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "pyzint",
    .m_doc = "Pyzint binding",
    .m_size = -1,
};

PyMODINIT_FUNC PyInit_pyzint(void) {
    static PyTypeObject* ZINTTypeP = &ZINTType;
    PyEval_InitThreads();

    PyObject *m;

    m = PyModule_Create(&pyzint_module);

    if (m == NULL) return NULL;

    if (PyType_Ready(ZINTTypeP) < 0) return NULL;

    Py_INCREF(ZINTTypeP);

    if (PyModule_AddObject(m, "Zint", (PyObject *) ZINTTypeP) < 0) {
        Py_XDECREF(ZINTTypeP);
        Py_XDECREF(m);
        return NULL;
    }

    PyModule_AddIntConstant(m, "BARCODE_CODE11", BARCODE_CODE11);
    PyModule_AddIntConstant(m, "BARCODE_C25MATRIX", BARCODE_C25MATRIX);
    PyModule_AddIntConstant(m, "BARCODE_C25INTER", BARCODE_C25INTER);
    PyModule_AddIntConstant(m, "BARCODE_C25IATA", BARCODE_C25IATA);
    PyModule_AddIntConstant(m, "BARCODE_C25LOGIC", BARCODE_C25LOGIC);
    PyModule_AddIntConstant(m, "BARCODE_C25IND", BARCODE_C25IND);
    PyModule_AddIntConstant(m, "BARCODE_CODE39", BARCODE_CODE39);
    PyModule_AddIntConstant(m, "BARCODE_EXCODE39", BARCODE_EXCODE39);
    PyModule_AddIntConstant(m, "BARCODE_EANX", BARCODE_EANX);
    PyModule_AddIntConstant(m, "BARCODE_EANX_CHK", BARCODE_EANX_CHK);
    PyModule_AddIntConstant(m, "BARCODE_EAN128", BARCODE_EAN128);
    PyModule_AddIntConstant(m, "BARCODE_CODABAR", BARCODE_CODABAR);
    PyModule_AddIntConstant(m, "BARCODE_CODE128", BARCODE_CODE128);
    PyModule_AddIntConstant(m, "BARCODE_DPLEIT", BARCODE_DPLEIT);
    PyModule_AddIntConstant(m, "BARCODE_DPIDENT", BARCODE_DPIDENT);
    PyModule_AddIntConstant(m, "BARCODE_CODE16K", BARCODE_CODE16K);
    PyModule_AddIntConstant(m, "BARCODE_CODE49", BARCODE_CODE49);
    PyModule_AddIntConstant(m, "BARCODE_CODE93", BARCODE_CODE93);
    PyModule_AddIntConstant(m, "BARCODE_FLAT", BARCODE_FLAT);
    PyModule_AddIntConstant(m, "BARCODE_RSS14", BARCODE_RSS14);
    PyModule_AddIntConstant(m, "BARCODE_RSS_LTD", BARCODE_RSS_LTD);
    PyModule_AddIntConstant(m, "BARCODE_RSS_EXP", BARCODE_RSS_EXP);
    PyModule_AddIntConstant(m, "BARCODE_TELEPEN", BARCODE_TELEPEN);
    PyModule_AddIntConstant(m, "BARCODE_UPCA", BARCODE_UPCA);
    PyModule_AddIntConstant(m, "BARCODE_UPCA_CHK", BARCODE_UPCA_CHK);
    PyModule_AddIntConstant(m, "BARCODE_UPCE", BARCODE_UPCE);
    PyModule_AddIntConstant(m, "BARCODE_UPCE_CHK", BARCODE_UPCE_CHK);
    PyModule_AddIntConstant(m, "BARCODE_POSTNET", BARCODE_POSTNET);
    PyModule_AddIntConstant(m, "BARCODE_MSI_PLESSEY", BARCODE_MSI_PLESSEY);
    PyModule_AddIntConstant(m, "BARCODE_FIM", BARCODE_FIM);
    PyModule_AddIntConstant(m, "BARCODE_LOGMARS", BARCODE_LOGMARS);
    PyModule_AddIntConstant(m, "BARCODE_PHARMA", BARCODE_PHARMA);
    PyModule_AddIntConstant(m, "BARCODE_PZN", BARCODE_PZN);
    PyModule_AddIntConstant(m, "BARCODE_PHARMA_TWO", BARCODE_PHARMA_TWO);
    PyModule_AddIntConstant(m, "BARCODE_PDF417", BARCODE_PDF417);
    PyModule_AddIntConstant(m, "BARCODE_PDF417TRUNC", BARCODE_PDF417TRUNC);
    PyModule_AddIntConstant(m, "BARCODE_MAXICODE", BARCODE_MAXICODE);
    PyModule_AddIntConstant(m, "BARCODE_QRCODE", BARCODE_QRCODE);
    PyModule_AddIntConstant(m, "BARCODE_CODE128B", BARCODE_CODE128B);
    PyModule_AddIntConstant(m, "BARCODE_AUSPOST", BARCODE_AUSPOST);
    PyModule_AddIntConstant(m, "BARCODE_AUSREPLY", BARCODE_AUSREPLY);
    PyModule_AddIntConstant(m, "BARCODE_AUSROUTE", BARCODE_AUSROUTE);
    PyModule_AddIntConstant(m, "BARCODE_AUSREDIRECT", BARCODE_AUSREDIRECT);
    PyModule_AddIntConstant(m, "BARCODE_ISBNX", BARCODE_ISBNX);
    PyModule_AddIntConstant(m, "BARCODE_RM4SCC", BARCODE_RM4SCC);
    PyModule_AddIntConstant(m, "BARCODE_DATAMATRIX", BARCODE_DATAMATRIX);
    PyModule_AddIntConstant(m, "BARCODE_EAN14", BARCODE_EAN14);
    PyModule_AddIntConstant(m, "BARCODE_VIN", BARCODE_VIN);
    PyModule_AddIntConstant(m, "BARCODE_CODABLOCKF", BARCODE_CODABLOCKF);
    PyModule_AddIntConstant(m, "BARCODE_NVE18", BARCODE_NVE18);
    PyModule_AddIntConstant(m, "BARCODE_JAPANPOST", BARCODE_JAPANPOST);
    PyModule_AddIntConstant(m, "BARCODE_KOREAPOST", BARCODE_KOREAPOST);
    PyModule_AddIntConstant(m, "BARCODE_RSS14STACK", BARCODE_RSS14STACK);
    PyModule_AddIntConstant(m, "BARCODE_RSS14STACK_OMNI", BARCODE_RSS14STACK_OMNI);
    PyModule_AddIntConstant(m, "BARCODE_RSS_EXPSTACK", BARCODE_RSS_EXPSTACK);
    PyModule_AddIntConstant(m, "BARCODE_PLANET", BARCODE_PLANET);
    PyModule_AddIntConstant(m, "BARCODE_MICROPDF417", BARCODE_MICROPDF417);
    PyModule_AddIntConstant(m, "BARCODE_ONECODE", BARCODE_ONECODE);
    PyModule_AddIntConstant(m, "BARCODE_PLESSEY", BARCODE_PLESSEY);
    PyModule_AddIntConstant(m, "BARCODE_TELEPEN_NUM", BARCODE_TELEPEN_NUM);
    PyModule_AddIntConstant(m, "BARCODE_ITF14", BARCODE_ITF14);
    PyModule_AddIntConstant(m, "BARCODE_KIX", BARCODE_KIX);
    PyModule_AddIntConstant(m, "BARCODE_AZTEC", BARCODE_AZTEC);
    PyModule_AddIntConstant(m, "BARCODE_DAFT", BARCODE_DAFT);
    PyModule_AddIntConstant(m, "BARCODE_MICROQR", BARCODE_MICROQR);
    PyModule_AddIntConstant(m, "BARCODE_HIBC_128", BARCODE_HIBC_128);
    PyModule_AddIntConstant(m, "BARCODE_HIBC_39", BARCODE_HIBC_39);
    PyModule_AddIntConstant(m, "BARCODE_HIBC_DM", BARCODE_HIBC_DM);
    PyModule_AddIntConstant(m, "BARCODE_HIBC_QR", BARCODE_HIBC_QR);
    PyModule_AddIntConstant(m, "BARCODE_HIBC_PDF", BARCODE_HIBC_PDF);
    PyModule_AddIntConstant(m, "BARCODE_HIBC_MICPDF", BARCODE_HIBC_MICPDF);
    PyModule_AddIntConstant(m, "BARCODE_HIBC_BLOCKF", BARCODE_HIBC_BLOCKF);
    PyModule_AddIntConstant(m, "BARCODE_HIBC_AZTEC", BARCODE_HIBC_AZTEC);
    PyModule_AddIntConstant(m, "BARCODE_DOTCODE", BARCODE_DOTCODE);
    PyModule_AddIntConstant(m, "BARCODE_HANXIN", BARCODE_HANXIN);
    PyModule_AddIntConstant(m, "BARCODE_MAILMARK", BARCODE_MAILMARK);
    PyModule_AddIntConstant(m, "BARCODE_AZRUNE", BARCODE_AZRUNE);
    PyModule_AddIntConstant(m, "BARCODE_CODE32", BARCODE_CODE32);
    PyModule_AddIntConstant(m, "BARCODE_EANX_CC", BARCODE_EANX_CC);
    PyModule_AddIntConstant(m, "BARCODE_EAN128_CC", BARCODE_EAN128_CC);
    PyModule_AddIntConstant(m, "BARCODE_RSS14_CC", BARCODE_RSS14_CC);
    PyModule_AddIntConstant(m, "BARCODE_RSS_LTD_CC", BARCODE_RSS_LTD_CC);
    PyModule_AddIntConstant(m, "BARCODE_RSS_EXP_CC", BARCODE_RSS_EXP_CC);
    PyModule_AddIntConstant(m, "BARCODE_UPCA_CC", BARCODE_UPCA_CC);
    PyModule_AddIntConstant(m, "BARCODE_UPCE_CC", BARCODE_UPCE_CC);
    PyModule_AddIntConstant(m, "BARCODE_RSS14STACK_CC", BARCODE_RSS14STACK_CC);
    PyModule_AddIntConstant(m, "BARCODE_RSS14_OMNI_CC", BARCODE_RSS14_OMNI_CC);
    PyModule_AddIntConstant(m, "BARCODE_RSS_EXPSTACK_CC", BARCODE_RSS_EXPSTACK_CC);
    PyModule_AddIntConstant(m, "BARCODE_CHANNEL", BARCODE_CHANNEL);
    PyModule_AddIntConstant(m, "BARCODE_CODEONE", BARCODE_CODEONE);
    PyModule_AddIntConstant(m, "BARCODE_GRIDMATRIX", BARCODE_GRIDMATRIX);
    PyModule_AddIntConstant(m, "BARCODE_UPNQR", BARCODE_UPNQR);
    PyModule_AddIntConstant(m, "BARCODE_ULTRA", BARCODE_ULTRA);
    PyModule_AddIntConstant(m, "BARCODE_RMQR", BARCODE_RMQR);
    return m;
}
