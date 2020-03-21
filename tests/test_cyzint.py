from cyzint.cyzint import ZBarcode, BARCODE_RSS_EXP


def test_cyzint():
    z = ZBarcode("[255]11111111111222", BARCODE_RSS_EXP)
    with open("f.bmp", "wb") as f:
        f.write(z.render_png())
