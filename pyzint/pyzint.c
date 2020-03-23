#include <Python.h>
#include "endianness.h"
#include "src/zint/backend/zint.h"

typedef struct {
    PyObject_HEAD
    struct zint_symbol *symbol;
    PyObject *data;
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
    if (self->symbol != NULL) {
        ZBarcode_Delete(self->symbol);
        self->symbol = NULL;
    }
    Py_CLEAR(self->data);
    self->buffer = NULL;

    Py_TYPE(self)->tp_free((PyObject *) self);
}

uint8_t octet2char(const char* src) {
    unsigned char result = 0;
    unsigned char t;

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
    static char *kwlist[] = {"data", "kind", "scale", NULL};

    self->symbol = ZBarcode_Create();

    if (self->symbol == NULL) {
        PyErr_Format(
            PyExc_RuntimeError,
            "Symbol initialization failed"
        );
        return -1;
    }

    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "Ob|f", kwlist,
            &self->data, &self->symbol->symbology, &self->symbol->scale
    )) return -1;

    if (PyBytes_Check(self->data)) {
        if (PyBytes_AsStringAndSize(self->data, &self->buffer, &self->length) == -1) {
            return -1;
        }
    } else if (PyUnicode_Check(self->data)) {
        self->buffer = PyUnicode_AsUTF8AndSize((const char *)self->data, &self->length);
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
    if (self->symbol == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Pool not initialized");
        return NULL;
    }
    return PyUnicode_FromFormat(
        "<%s as %p: kind=%d buffer=%s (%d)>",
        Py_TYPE(self)->tp_name, self, self->symbol->symbology, self->buffer, self->length
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
        PyErr_Format(
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
    memcpy(&str[1], target, 6);
}


PyDoc_STRVAR(CZINT_render_bmp_docstring,
    "Render bmp.\n\n"
    "    ZBarcode('data').render_bmp(angle: int = 0) -> bytes"
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

    if (parse_color_hex(fgcolor_str, &fgcolor)) return NULL;
    if (parse_color_hex(bgcolor_str, &bgcolor)) return NULL;

    int res = 0;
    unsigned int bmp_size = 0;
    char *bmp = NULL;
    int bmp_1bit_size = 0;
    Py_BEGIN_ALLOW_THREADS

    res = ZBarcode_Encode_and_Buffer(
        self->symbol,
        (unsigned char *)self->buffer,
        self->length, angle
    );

    unsigned int width = self->symbol->bitmap_width;
    unsigned int height = self->symbol->bitmap_height;
    const unsigned int bitmap_channel_size = width * height;
    const unsigned int bitmap_size = bitmap_channel_size * 3;

    unsigned char bitmap[height][width + 8];
    memset(&bitmap, 0, height * (width + 8));

    for (int i=0; i<height*width; i++) {
        bitmap[i/width][i%width] = self->symbol->bitmap[i * 3];
    }

    static const unsigned int header_size = 62;
    const int bmp_1bit_with_bytes = (width / 8 + (width % 8 == 0?0:1));
    const int padding = ((4 - (width * 3) % 4) % 4);
    bmp_1bit_size = (
        header_size + bmp_1bit_with_bytes * height + (height * padding)
    );

    static const unsigned char bmp_template[header_size] = {
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

    if (res == 0) {
        bmp = calloc(bmp_1bit_size, sizeof(char *));

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

        unsigned char point;
        unsigned int offset = 0;

        for(unsigned int y=height-1; y > 0; --y) {
            for(unsigned int x=0; x < width; x+=8) {
                pixels[offset] = octet2char(&bitmap[y][x]);
                offset++;
            }
            offset += padding;
        }
        printf("\n");
    }

    Py_END_ALLOW_THREADS

    if (res > 0) {
        PyErr_Format(
            PyExc_RuntimeError,
            "Error while rendering: %s",
            self->symbol->errtxt
        );
        return NULL;
    }
    PyObject *result = PyBytes_FromStringAndSize(bmp, bmp_1bit_size);
    if (result == NULL) {
        free(bmp);
        return NULL;
    }
    free(bmp);
    return result;
}

PyDoc_STRVAR(CZINT_render_svg_docstring,
    "Render bmp.\n\n"
    "    ZBarcode('data').render_svg(angle: int = 0) -> bytes"
);
static PyObject* CZINT_render_svg(
    CZINT *self, PyObject *args, PyObject *kwds
) {
    static char *kwlist[] = {"angle", "fgcolor", "bgcolor", NULL};

    int angle = 0;
    char *fgcolor_str = "000000";
    char *bgcolor_str = "FFFFFF";


    if (!PyArg_ParseTupleAndKeywords(
        args, kwds, "|iss", kwlist,
        &angle, &fgcolor_str, &bgcolor_str
    )) return NULL;


    if (parse_color_str(fgcolor_str, &self->symbol->fgcolour)) return NULL;
    if (parse_color_str(bgcolor_str, &self->symbol->bgcolour)) return NULL;

    int res = 0;
    unsigned int size = 0;
    char *bmp = NULL;
    int bmp_1bit_size = 0;
    Py_BEGIN_ALLOW_THREADS

    res = ZBarcode_Encode_and_Buffer_Vector(self->symbol, (unsigned char *)self->buffer, self->length, angle);

    Py_END_ALLOW_THREADS

    if (res > 0) {
        PyErr_Format(
            PyExc_RuntimeError,
            "Error while rendering: %s",
            self->symbol->errtxt
        );
        return NULL;
    }
    PyObject *result = PyBytes_FromStringAndSize(bmp, bmp_1bit_size);
    if (result == NULL) {
        free(bmp);
        return NULL;
    }
    free(bmp);
    return result;
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
    {NULL}  /* Sentinel */

};



static PyTypeObject
ZINTType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ZBarcode",
    .tp_doc = "ZBarcode c-binding to zint",
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
