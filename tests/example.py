from PIL import Image

from pyzint.pyzint import Zint, BARCODE_RSS_EXP


def png():
    z = Zint("[255]11111111111222", BARCODE_RSS_EXP, scale=3)
    data = z.render_bmp()
    with open("f.bmp", "wb+") as fp:
        fp.write(data)
        fp.seek(0)
        img = Image.open(fp)
        img.save("fff.png")


def svg():
    z = Zint("[255]11111111111222", BARCODE_RSS_EXP, scale=3)
    with open("fff.svg", "w+") as fp:
        fp.write(z.render_svg().decode())


if __name__ == '__main__':
    svg()
    png()
