import xml.etree.ElementTree as ET
from io import BytesIO

from PIL import Image
from pyzint.pyzint import Zint, BARCODE_RSS_EXP


def test_bmp_rss_exp_cyzint():
    z = Zint("[255]11111111111222", BARCODE_RSS_EXP)
    barcode = z.render_bmp()
    with BytesIO(barcode) as fp:
        img = Image.open(fp)

        assert img.verify() is None
        assert img.height == 86
        assert img.width == 366


def test_svg_rss_exp_cyzint():
    z = Zint("[255]11111111111222", BARCODE_RSS_EXP)
    barcode = z.render_svg()
    xml = ET.fromstring(barcode.decode())
    assert int(xml.get('width')) == 366
    assert int(xml.get('height')) == 86
