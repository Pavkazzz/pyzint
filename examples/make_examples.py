from io import BytesIO

import pyzint
from PIL import Image


EXAMPLES = {
    # Tbarcode 7 codes
    pyzint.Barcode.CODE11: "1234567",
    pyzint.Barcode.C25MATRIX: "1234567",
    pyzint.Barcode.C25INTER: "1234567",
    pyzint.Barcode.C25IATA: "1234567",
    pyzint.Barcode.C25LOGIC: "1234567",
    pyzint.Barcode.C25IND: "1234567",
    pyzint.Barcode.CODE39: "1234567",
    pyzint.Barcode.EXCODE39: "1234567",
    pyzint.Barcode.EANX: "1234567",
    pyzint.Barcode.EANX_CHK: "123456789012",
    pyzint.Barcode.EAN128: "[01]12345678901234",
    pyzint.Barcode.CODABAR: "A40156B",
    pyzint.Barcode.CODE128: "1234567",
    pyzint.Barcode.DPLEIT: "1234567",
    pyzint.Barcode.DPIDENT: "1234567",
    pyzint.Barcode.CODE16K: "1234567",
    pyzint.Barcode.CODE49: "1234567",
    pyzint.Barcode.CODE93: "1234567",
    pyzint.Barcode.FLAT: "1234567",
    pyzint.Barcode.RSS14: "1234567",
    pyzint.Barcode.RSS_LTD: "1234567",
    pyzint.Barcode.RSS_EXP: "[01]12345678901234",
    pyzint.Barcode.TELEPEN: "1234567",
    pyzint.Barcode.UPCA: "1234567",
    pyzint.Barcode.UPCA_CHK: "123456789012",
    pyzint.Barcode.UPCE: "001238",
    pyzint.Barcode.UPCE_CHK: "04252614",
    pyzint.Barcode.POSTNET: "555551237",
    pyzint.Barcode.MSI_PLESSEY: "1234567",
    pyzint.Barcode.FIM: "A",
    pyzint.Barcode.LOGMARS: "1234567",
    pyzint.Barcode.PHARMA: "12345",
    pyzint.Barcode.PZN: "1234567",
    pyzint.Barcode.PHARMA_TWO: "1234567",
    pyzint.Barcode.PDF417: "Hello World",
    pyzint.Barcode.PDF417TRUNC: "1234567",
    pyzint.Barcode.MAXICODE: "1234567",
    pyzint.Barcode.QRCODE: "1234567",
    pyzint.Barcode.CODE128B: "1234567",
    pyzint.Barcode.AUSPOST: "12345678",
    pyzint.Barcode.AUSREPLY: "1234567",
    pyzint.Barcode.AUSROUTE: "1234567",
    pyzint.Barcode.AUSREDIRECT: "1234567",
    pyzint.Barcode.ISBNX: "9781491946008",
    pyzint.Barcode.RM4SCC: "1234567",
    pyzint.Barcode.DATAMATRIX: "1234567",
    pyzint.Barcode.EAN14: "1234567",
    pyzint.Barcode.VIN: "123456789012",
    pyzint.Barcode.CODABLOCKF: "1234567",
    pyzint.Barcode.NVE18: "1234567",
    pyzint.Barcode.JAPANPOST: "1234567",
    pyzint.Barcode.KOREAPOST: "123456",
    pyzint.Barcode.RSS14STACK: "1234567",
    pyzint.Barcode.RSS14STACK_OMNI: "1234567",
    pyzint.Barcode.RSS_EXPSTACK: "[01]12345678901234",
    pyzint.Barcode.PLANET: "1234567890123",
    pyzint.Barcode.MICROPDF417: "1234567",
    pyzint.Barcode.ONECODE: "12345678901234567890",
    pyzint.Barcode.PLESSEY: "1234567",

    # Tbarcode 8 codes
    pyzint.Barcode.TELEPEN_NUM: "12345678",
    pyzint.Barcode.ITF14: "12345678",
    pyzint.Barcode.KIX: "12345678",
    pyzint.Barcode.AZTEC: "12345678",
    pyzint.Barcode.DAFT: "A",
    pyzint.Barcode.MICROQR: "12345678",

    # Tbarcode 9 codes
    pyzint.Barcode.HIBC_128: "123456789",
    pyzint.Barcode.HIBC_39: "123456789",
    pyzint.Barcode.HIBC_DM: "123456789",
    pyzint.Barcode.HIBC_QR: "123456789",
    pyzint.Barcode.HIBC_PDF: "123456789",
    pyzint.Barcode.HIBC_MICPDF: "123456789",
    pyzint.Barcode.HIBC_BLOCKF: "123456789",
    pyzint.Barcode.HIBC_AZTEC: "123456789",

    # Tbarcode 10 codes
    pyzint.Barcode.DOTCODE: "1234567890",
    pyzint.Barcode.HANXIN: "1234567890",

    # Tbarcode 11 codes
    pyzint.Barcode.MAILMARK: "41038422416563762EF61AH8T",

    # Zint specific
    pyzint.Barcode.AZRUNE: "1",
    pyzint.Barcode.CODE32: "32",
    pyzint.Barcode.EANX_CC: ("[21]A12345678", "12345678+123"),
    pyzint.Barcode.RSS14_CC: ("[21]A12345678", "12345678"),
    pyzint.Barcode.RSS_LTD_CC: ("[21]A12345678", "12345678"),
    pyzint.Barcode.UPCA_CC: ("[21]A12345678", "12345678"),
    pyzint.Barcode.UPCE_CC: ("[21]A04252614", "0425261"),
    pyzint.Barcode.RSS14STACK_CC: ("[21]A12345678", "12345678"),
    pyzint.Barcode.RSS14_OMNI_CC: ("[21]A12345678", "12345678"),
    pyzint.Barcode.RSS_EXPSTACK_CC: ("[21]A12345678901234", "[01]12345678901234"),
    pyzint.Barcode.CHANNEL: "123",
    pyzint.Barcode.CODEONE: ("[21]A12345678", "12345678"),
    pyzint.Barcode.GRIDMATRIX: ("[21]A12345678", "12345678"),
    pyzint.Barcode.UPNQR: ("[21]A12345678", "12345678"),
    pyzint.Barcode.RMQR: ("[21]A12345678", "12345678"),
}


with open("README.md", "w+") as fp:
    fp.write("Barcode Examples\n")
    fp.write("================\n")
    fp.write("Auto generated examples. "
             "See `make_examples.py` for details.\n\n")

    for kind, payload in EXAMPLES.items():
        print("Creating example for", kind.name, "with payload", payload)

        primary = None
        if isinstance(payload, tuple):
            payload, primary = payload

        symbol = kind(payload, scale=2, primary=primary)

        with BytesIO(symbol.render_bmp()) as bmp:
            img = Image.open(bmp)
            fname = "images/{}.png".format(kind.name.lower())
            img.save(fname)

        fp.write("## {}\n\n".format(kind.name))

        fp.write("Example `{}` barcode with content `{}`".format(
            kind.name,
            payload
        ))

        if primary:
            fp.write(" and primary `{}`".format(primary))

        fp.write("\n\n")

        fp.write("![{} barcode example]({})\n\n".format(kind.name, fname))
        fp.write("Code example:\n")
        fp.write("```python\n")
        fp.write("import pyzint\n\n")
        fp.write("symbol = pyzint.Barcode.{}({!r}".format(kind.name, payload))
        if primary:
            fp.write(", primary={!r}".format(primary))
        fp.write(")\n\n")
        fp.write("with open('{!s}.bmp', \"wb\") as bmp:\n".format(kind.name))
        fp.write("    bmp.write(symbol.render_bmp())\n".format(kind.name))
        fp.write("```\n")
