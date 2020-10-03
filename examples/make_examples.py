from io import BytesIO

import pyzint
from PIL import Image

NAMES = {
    # Tbarcode 7 codes
    pyzint.BARCODE_CODE11: "CODE11",
    pyzint.BARCODE_C25MATRIX: "C25MATRIX",
    pyzint.BARCODE_C25INTER: "C25INTER",
    pyzint.BARCODE_C25IATA: "C25IATA",
    pyzint.BARCODE_C25LOGIC: "C25LOGIC",
    pyzint.BARCODE_C25IND: "C25IND",
    pyzint.BARCODE_CODE39: "CODE39",
    pyzint.BARCODE_EXCODE39: "EXCODE39",
    pyzint.BARCODE_EANX: "EANX",
    pyzint.BARCODE_EANX_CHK: "EANX_CHK",
    pyzint.BARCODE_EAN128: "EAN128",
    pyzint.BARCODE_CODABAR: "CODABAR",
    pyzint.BARCODE_CODE128: "CODE128",
    pyzint.BARCODE_DPLEIT: "DPLEIT",
    pyzint.BARCODE_DPIDENT: "DPIDENT",
    pyzint.BARCODE_CODE16K: "CODE16K",
    pyzint.BARCODE_CODE49: "CODE49",
    pyzint.BARCODE_CODE93: "CODE93",
    pyzint.BARCODE_FLAT: "FLAT",
    pyzint.BARCODE_RSS14: "RSS14",
    pyzint.BARCODE_RSS_LTD: "RSS_LTD",
    pyzint.BARCODE_RSS_EXP: "RSS_EXP",
    pyzint.BARCODE_TELEPEN: "TELEPEN",
    pyzint.BARCODE_UPCA: "UPCA",
    pyzint.BARCODE_UPCA_CHK: "UPCA_CHK",
    pyzint.BARCODE_UPCE: "UPCE",
    pyzint.BARCODE_UPCE_CHK: "UPCE_CHK",
    pyzint.BARCODE_POSTNET: "POSTNET",
    pyzint.BARCODE_MSI_PLESSEY: "MSI_PLESSEY",
    pyzint.BARCODE_FIM: "FIM",
    pyzint.BARCODE_LOGMARS: "LOGMARS",
    pyzint.BARCODE_PHARMA: "PHARMA",
    pyzint.BARCODE_PZN: "PZN",
    pyzint.BARCODE_PHARMA_TWO: "PHARMA_TWO",
    pyzint.BARCODE_PDF417: "PDF417",
    pyzint.BARCODE_PDF417TRUNC: "PDF417TRUNC",
    pyzint.BARCODE_MAXICODE: "MAXICODE",
    pyzint.BARCODE_QRCODE: "QRCODE",
    pyzint.BARCODE_CODE128B: "CODE128B",
    pyzint.BARCODE_AUSPOST: "AUSPOST",
    pyzint.BARCODE_AUSREPLY: "AUSREPLY",
    pyzint.BARCODE_AUSROUTE: "AUSROUTE",
    pyzint.BARCODE_AUSREDIRECT: "AUSREDIRECT",
    pyzint.BARCODE_ISBNX: "ISBNX",
    pyzint.BARCODE_RM4SCC: "RM4SCC",
    pyzint.BARCODE_DATAMATRIX: "DATAMATRIX",
    pyzint.BARCODE_EAN14: "EAN14",
    pyzint.BARCODE_VIN: "VIN",
    pyzint.BARCODE_CODABLOCKF: "CODABLOCKF",
    pyzint.BARCODE_NVE18: "NVE18",
    pyzint.BARCODE_JAPANPOST: "JAPANPOST",
    pyzint.BARCODE_KOREAPOST: "KOREAPOST",
    pyzint.BARCODE_RSS14STACK: "RSS14STACK",
    pyzint.BARCODE_RSS14STACK_OMNI: "RSS14STACK_OMNI",
    pyzint.BARCODE_RSS_EXPSTACK: "RSS_EXPSTACK",
    pyzint.BARCODE_PLANET: "PLANET",
    pyzint.BARCODE_MICROPDF417: "MICROPDF417",
    pyzint.BARCODE_ONECODE: "ONECODE",
    pyzint.BARCODE_PLESSEY: "PLESSEY",

    # Tbarcode 8 codes
    pyzint.BARCODE_TELEPEN_NUM: "TELEPEN_NUM",
    pyzint.BARCODE_ITF14: "ITF14",
    pyzint.BARCODE_KIX: "KIX",
    pyzint.BARCODE_AZTEC: "AZTEC",
    pyzint.BARCODE_DAFT: "DAFT",
    pyzint.BARCODE_MICROQR: "MICROQR",

    # Tbarcode 9 codes
    pyzint.BARCODE_HIBC_128: "HIBC_128",
    pyzint.BARCODE_HIBC_39: "HIBC_39",
    pyzint.BARCODE_HIBC_DM: "HIBC_DM",
    pyzint.BARCODE_HIBC_QR: "HIBC_QR",
    pyzint.BARCODE_HIBC_PDF: "HIBC_PDF",
    pyzint.BARCODE_HIBC_MICPDF: "HIBC_MICPDF",
    pyzint.BARCODE_HIBC_BLOCKF: "HIBC_BLOCKF",
    pyzint.BARCODE_HIBC_AZTEC: "HIBC_AZTEC",

    # Tbarcode 10 codes
    pyzint.BARCODE_DOTCODE: "DOTCODE",
    pyzint.BARCODE_HANXIN: "HANXIN",

    # Tbarcode 11 codes
    pyzint.BARCODE_MAILMARK: "MAILMARK",

    # Zint specific
    pyzint.BARCODE_AZRUNE: "AZRUNE",
    pyzint.BARCODE_CODE32: "CODE32",
    pyzint.BARCODE_EANX_CC: "EANX_CC",
    pyzint.BARCODE_EAN128_CC: "EAN128_CC",
    pyzint.BARCODE_RSS14_CC: "RSS14_CC",
    pyzint.BARCODE_RSS_LTD_CC: "RSS_LTD_CC",
    pyzint.BARCODE_RSS_EXP_CC: "RSS_EXP_CC",
    pyzint.BARCODE_UPCA_CC: "UPCA_CC",
    pyzint.BARCODE_UPCE_CC: "UPCE_CC",
    pyzint.BARCODE_RSS14STACK_CC: "RSS14STACK_CC",
    pyzint.BARCODE_RSS14_OMNI_CC: "RSS14_OMNI_CC",
    pyzint.BARCODE_RSS_EXPSTACK_CC: "RSS_EXPSTACK_CC",
    pyzint.BARCODE_CHANNEL: "CHANNEL",
    pyzint.BARCODE_CODEONE: "CODEONE",
    pyzint.BARCODE_GRIDMATRIX: "GRIDMATRIX",
    pyzint.BARCODE_UPNQR: "UPNQR",
    pyzint.BARCODE_ULTRA: "ULTRA",
    pyzint.BARCODE_RMQR: "RMQR",
}

EXAMPLES = {
    # Tbarcode 7 codes
    pyzint.BARCODE_CODE11: "1234567",
    pyzint.BARCODE_C25MATRIX: "1234567",
    pyzint.BARCODE_C25INTER: "1234567",
    pyzint.BARCODE_C25IATA: "1234567",
    pyzint.BARCODE_C25LOGIC: "1234567",
    pyzint.BARCODE_C25IND: "1234567",
    pyzint.BARCODE_CODE39: "1234567",
    pyzint.BARCODE_EXCODE39: "1234567",
    pyzint.BARCODE_EANX: "1234567",
    pyzint.BARCODE_EANX_CHK: "123456789012",
    pyzint.BARCODE_EAN128: "[01]12345678901234",
    pyzint.BARCODE_CODABAR: "A40156B",
    pyzint.BARCODE_CODE128: "1234567",
    pyzint.BARCODE_DPLEIT: "1234567",
    pyzint.BARCODE_DPIDENT: "1234567",
    pyzint.BARCODE_CODE16K: "1234567",
    pyzint.BARCODE_CODE49: "1234567",
    pyzint.BARCODE_CODE93: "1234567",
    pyzint.BARCODE_FLAT: "1234567",
    pyzint.BARCODE_RSS14: "1234567",
    pyzint.BARCODE_RSS_LTD: "1234567",
    pyzint.BARCODE_RSS_EXP: "[01]12345678901234",
    pyzint.BARCODE_TELEPEN: "1234567",
    pyzint.BARCODE_UPCA: "1234567",
    pyzint.BARCODE_UPCA_CHK: "123456789012",
    pyzint.BARCODE_UPCE: "001238",
    pyzint.BARCODE_UPCE_CHK: "04252614",
    pyzint.BARCODE_POSTNET: "555551237",
    pyzint.BARCODE_MSI_PLESSEY: "1234567",
    pyzint.BARCODE_FIM: "A",
    pyzint.BARCODE_LOGMARS: "1234567",
    pyzint.BARCODE_PHARMA: "12345",
    pyzint.BARCODE_PZN: "1234567",
    pyzint.BARCODE_PHARMA_TWO: "1234567",
    pyzint.BARCODE_PDF417: "Hello World",
    pyzint.BARCODE_PDF417TRUNC: "1234567",
    pyzint.BARCODE_MAXICODE: "1234567",
    pyzint.BARCODE_QRCODE: "1234567",
    pyzint.BARCODE_CODE128B: "1234567",
    pyzint.BARCODE_AUSPOST: "12345678",
    pyzint.BARCODE_AUSREPLY: "1234567",
    pyzint.BARCODE_AUSROUTE: "1234567",
    pyzint.BARCODE_AUSREDIRECT: "1234567",
    pyzint.BARCODE_ISBNX: "9781491946008",
    pyzint.BARCODE_RM4SCC: "1234567",
    pyzint.BARCODE_DATAMATRIX: "1234567",
    pyzint.BARCODE_EAN14: "1234567",
    pyzint.BARCODE_VIN: "123456789012",
    pyzint.BARCODE_CODABLOCKF: "1234567",
    pyzint.BARCODE_NVE18: "1234567",
    pyzint.BARCODE_JAPANPOST: "1234567",
    pyzint.BARCODE_KOREAPOST: "123456",
    pyzint.BARCODE_RSS14STACK: "1234567",
    pyzint.BARCODE_RSS14STACK_OMNI: "1234567",
    pyzint.BARCODE_RSS_EXPSTACK: "[01]12345678901234",
    pyzint.BARCODE_PLANET: "1234567890123",
    pyzint.BARCODE_MICROPDF417: "1234567",
    pyzint.BARCODE_ONECODE: "12345678901234567890",
    pyzint.BARCODE_PLESSEY: "1234567",

    # Tbarcode 8 codes
    pyzint.BARCODE_TELEPEN_NUM: "12345678",
    pyzint.BARCODE_ITF14: "12345678",
    pyzint.BARCODE_KIX: "12345678",
    pyzint.BARCODE_AZTEC: "12345678",
    pyzint.BARCODE_DAFT: "A",
    pyzint.BARCODE_MICROQR: "12345678",

    # Tbarcode 9 codes
    pyzint.BARCODE_HIBC_128: "123456789",
    pyzint.BARCODE_HIBC_39: "123456789",
    pyzint.BARCODE_HIBC_DM: "123456789",
    pyzint.BARCODE_HIBC_QR: "123456789",
    pyzint.BARCODE_HIBC_PDF: "123456789",
    pyzint.BARCODE_HIBC_MICPDF: "123456789",
    pyzint.BARCODE_HIBC_BLOCKF: "123456789",
    pyzint.BARCODE_HIBC_AZTEC: "123456789",

    # Tbarcode 10 codes
    pyzint.BARCODE_DOTCODE: "1234567890",
    pyzint.BARCODE_HANXIN: "1234567890",

    # Tbarcode 11 codes
    pyzint.BARCODE_MAILMARK: "41038422416563762EF61AH8T",

    # Zint specific
    pyzint.BARCODE_AZRUNE: "1",
    pyzint.BARCODE_CODE32: "32",
    pyzint.BARCODE_EANX_CC: ("[21]A12345678", "12345678+123"),
    pyzint.BARCODE_RSS14_CC: ("[21]A12345678", "12345678"),
    pyzint.BARCODE_RSS_LTD_CC: ("[21]A12345678", "12345678"),
    pyzint.BARCODE_UPCA_CC: ("[21]A12345678", "12345678"),
    pyzint.BARCODE_UPCE_CC: ("[21]A04252614", "0425261"),
    pyzint.BARCODE_RSS14STACK_CC: ("[21]A12345678", "12345678"),
    pyzint.BARCODE_RSS14_OMNI_CC: ("[21]A12345678", "12345678"),
    pyzint.BARCODE_RSS_EXPSTACK_CC: ("[21]A12345678901234", "[01]12345678901234"),
    pyzint.BARCODE_CHANNEL: "123",
    pyzint.BARCODE_CODEONE: ("[21]A12345678", "12345678"),
    pyzint.BARCODE_GRIDMATRIX: ("[21]A12345678", "12345678"),
    pyzint.BARCODE_UPNQR: ("[21]A12345678", "12345678"),
    pyzint.BARCODE_RMQR: ("[21]A12345678", "12345678"),
}


with open("README.md", "w+") as fp:
    fp.write("Barcode Examples\n")
    fp.write("================\n")
    fp.write("Auto generated examples. "
             "See `make_examples.py` for details.\n\n")

    for kind, payload in EXAMPLES.items():
        print("Creating example for", NAMES[kind], "with payload", payload)

        kind_name = NAMES[kind]
        primary = None
        if isinstance(payload, tuple):
            payload, primary = payload

        fp.write("## {}\n\n".format(kind_name))

        fp.write("Example `{}` barcode with content `{}`".format(
            kind_name,
            payload
        ))

        if primary:
            fp.write(" and primary `{}`".format(primary))

        fp.write("\n\n")

        barcoder = pyzint.Zint(payload, kind, scale=2, primary=primary)

        with BytesIO(barcoder.render_bmp()) as bmp:
            img = Image.open(bmp)
            fname = "images/{}.png".format(kind_name.lower())
            img.save(fname)

        fp.write("![{} barcode example]({})\n\n".format(kind_name, fname))
        fp.write("Code example:\n")
        fp.write("```python\n")
        fp.write("import pyzint\n\n")
        fp.write("symbol = pyzint.Zint(\n")
        fp.write("    {!r}, pyzint.BARCODE_{}".format(payload, kind_name))
        if primary:
            fp.write(", primary={!r}".format(primary))
        fp.write("\n)\n\n")
        fp.write("with open('{!s}.bmp', \"wb\") as bmp:\n".format(kind_name))
        fp.write("    bmp.write(symbol.render_bmp())\n".format(kind_name))
        fp.write("```\n")
