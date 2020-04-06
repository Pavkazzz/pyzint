import xml.etree.ElementTree as ET
from io import BytesIO

import pytest
from PIL import Image
from pyzint.pyzint import Zint, BARCODE_QRCODE

options = pytest.mark.parametrize("option_1, option_2, size", [
    (0, 0, 42),
    (0, 2, 50),
    (1, 0, 42),
    (1, 2, 50),
    (2, 0, 42),
    (2, 2, 50),
    (3, 0, 50),
    (3, 4, 66),
    (4, 0, 50),
    (4, 4, 66),
])


@options
def test_bmp_exp_cyzint(option_1, option_2, size):
    z = Zint(
        "Barcode QRCode", BARCODE_QRCODE,
        option_1, option_2
    )
    barcode = z.render_bmp()
    with BytesIO(barcode) as fp:
        img = Image.open(fp)

        assert img.verify() is None
        assert img.height == size
        assert img.width == size


@options
def test_svg_exp_cyzint(option_1, option_2, size):
    z = Zint(
        "Barcode QRCode", BARCODE_QRCODE,
        option_1, option_2
    )
    barcode = z.render_svg()
    xml = ET.fromstring(barcode.decode())
    assert int(xml.get('width')) == size
    assert int(xml.get('height')) == size
