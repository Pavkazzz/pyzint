from pyzint.pyzint import Zint, BARCODE_RSS_EXP


def test_cyzint():
    z = Zint("[255]11111111111222", BARCODE_RSS_EXP)
    with open("f.bmp", "wb") as f:
        f.write(z.render_bmp())
