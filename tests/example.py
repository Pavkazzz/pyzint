from io import BytesIO

from pyzint.cyzint import ZBarcode, BARCODE_RSS_EXP
from PIL import Image


if __name__ == '__main__':
    z = ZBarcode("[255]11111111111222", BARCODE_RSS_EXP, scale=1)
    data = z.render_bmp(fgcolor="#aacccc", bgcolor="#000000")
    with open("f.bmp", "wb+") as fp:
        fp.write(data)
        fp.seek(0)
        img = Image.open(fp)
        img.save("fff.png")

