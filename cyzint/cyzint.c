#include <Python.h>
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

PyDoc_STRVAR(CZINT_render_bmp_docstring,
    "Render bmp.\n\n"
    "    ZBarcode('data').render_bmp(angle: int = 0) -> bytes"
);
static PyObject* CZINT_render_bmp(
    CZINT *self, PyObject *args, PyObject *kwds
) {
    static char *kwlist[] = {"angle", NULL};

    int angle = 0;
    if (!PyArg_ParseTupleAndKeywords(
        args, kwds, "|i", kwlist,
        &angle
    )) return NULL;

    int res = 0;
    unsigned int size = 0;
    char *bmp = NULL;

    Py_BEGIN_ALLOW_THREADS

    res = ZBarcode_Encode_and_Buffer(self->symbol, (unsigned char *)self->buffer, self->length, angle);
///////////// 14x5
    char ch[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    self->symbol->bitmap = ch;
////////////
    unsigned int width = self->symbol->bitmap_width = 14;
    unsigned int height = self->symbol->bitmap_height = 5;
    unsigned int bitmap_size = width * height;
    printf("bsize: %s\n", bitmap_size);
    size = bitmap_size + 62;

    static const unsigned char bmp_template[62] = {
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
      0x00, 0x00, 0x00, 0x00, // red channel
      0xff, 0xff, 0xff, 0xff  // green channel
    };


    if (res == 0) {
        bmp = calloc(size, sizeof(char *));

        memcpy(bmp, &bmp_template, 62);

        bmp[2] = (unsigned char)(size);
        bmp[3] = (unsigned char)(size >> 8);
        bmp[4] = (unsigned char)(size >> 16);
        bmp[5] = (unsigned char)(size >> 24);

        bmp[18] = (unsigned char)(width);
        bmp[19] = (unsigned char)(width >> 8);
        bmp[20] = (unsigned char)(width >> 16);
        bmp[21] = (unsigned char)(width >> 24);

        bmp[22] = (unsigned char)(height);
        bmp[23] = (unsigned char)(height >> 8);
        bmp[24] = (unsigned char)(height >> 16);
        bmp[25] = (unsigned char)(height >> 24);

        char *pixels = &bmp[62];

        unsigned int point;
        unsigned int offset = 0;

//        for(int y=height - 1; y >= 0; y--)
//        {
//            for(int x=0; x < width; x++)
//            {
//                point = self->symbol->bitmap[y*width+x];
//                printf("b: %s\n", point);
//                if (point > 128)
//                {
//                    pixels[offset/8] |= (1 << (7 -(offset % 8)));
//                }
//                offset++;
//            }
//        }
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

    PyObject *result = PyBytes_FromStringAndSize(bmp, size);
    free(bmp);
    return result;
}


static PyMethodDef CZINT_methods[] = {
    {
        "render_bmp",
        (PyCFunction) CZINT_render_bmp, METH_VARARGS | METH_KEYWORDS,
        CZINT_render_bmp_docstring
    },
    {NULL}  /* Sentinel */
//    {
//        "render_svg",
//        (PyCFunction) CZINT_render_svg, METH_VARARGS | METH_KEYWORDS,
//        CZINT_render_svg_docstring
//    }
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
//    .tp_members = CZINT_members,
    .tp_methods = CZINT_methods,
    .tp_repr = (reprfunc) CZINT_repr
};


static PyModuleDef cyzint_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "cyzint",
    .m_doc = "Cyzint binding",
    .m_size = -1,
};

PyMODINIT_FUNC PyInit_cyzint(void) {
    static PyTypeObject* ZINTTypeP = &ZINTType;
    PyEval_InitThreads();

    PyObject *m;

    m = PyModule_Create(&cyzint_module);

    if (m == NULL) return NULL;

    if (PyType_Ready(ZINTTypeP) < 0) return NULL;

    Py_INCREF(ZINTTypeP);

    if (PyModule_AddObject(m, "ZBarcode", (PyObject *) ZINTTypeP) < 0) {
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
