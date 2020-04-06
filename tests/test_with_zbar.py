from io import BytesIO

import pytest
from PIL import Image, ImageFile

try:
    from zbar import Scanner
except ImportError:
    Scanner = None

from pyzint import (
    BARCODE_EANX, BARCODE_CODE128, BARCODE_UPCE, BARCODE_UPCA,
    BARCODE_ISBNX, BARCODE_C25INTER, BARCODE_QRCODE,
    Zint
)


# Cannot decode barcodes:
# (BARCODE_CODE39, "ABC-1234", '', 'ZBAR_CODE39', 'CODE-39'),
# (BARCODE_PDF417, "This is a PDF417", '', 'ZBAR_PDF417', 'PDF-417'),


@pytest.mark.skipif(Scanner is None, reason="zbar is not installed")
@pytest.mark.parametrize("type,value, checksum, zbar_type, exp_type", [
    (BARCODE_EANX, "00090311017", '2', 'ZBAR_UPCA', 'UPC-A'),
    (BARCODE_EANX, "978020137962", '4', 'ZBAR_EAN13', 'EAN-13'),
    (BARCODE_CODE128, "ABC-abc-1234", '', 'ZBAR_CODE128', "CODE-128"),
    (BARCODE_UPCE, "0123456", '5', 'ZBAR_UPCE', 'UPC-E'),
    (BARCODE_UPCA, "725272730706", '', 'ZBAR_UPCA', 'UPC-A'),
    (BARCODE_ISBNX, "0123456789", '', 'ZBAR_ISBN10', 'ISBN-10'),
    (BARCODE_ISBNX, "9781234567897", '', 'ZBAR_ISBN13', 'ISBN-13'),
    (BARCODE_C25INTER, "1234567890", '', 'ZBAR_I25', 'I2/5'),
    (BARCODE_QRCODE, "This is a QRCODE", '', 'ZBAR_QRCODE', 'QR-Code'),
])
def test_with_zbar(type, value, checksum, zbar_type, exp_type):
    scanner = Scanner([(zbar_type, 'ZBAR_CFG_ENABLE', 1)])
    z = Zint(value, type)
    barcode = z.render_bmp()

    with BytesIO(barcode) as fp:
        ImageFile.LOAD_TRUNCATED_IMAGES = True
        img = Image.open(fp)
        img = img.convert("L")
        scanned = scanner.scan(img)
        if exp_type:
            assert len(scanned) == 1
            assert scanned[0].type == exp_type
            assert scanned[0].data.decode() == value + checksum
