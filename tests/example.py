from PIL import Image, ImageFile

from pyzint.pyzint import Zint, BARCODE_RSS_EXP, BARCODE_C25INTER, \
    BARCODE_PDF417


def png():
    # data = Zint("[255]1111111111122", BARCODE_RSS_EXP).render_bmp()
    # data = Zint("1234567890", BARCODE_PDF417, show_text=False).render_bmp()
    # data = Zint("This is a PDF417", BARCODE_PDF417).render_bmp()
    data = Zint("1234567890", BARCODE_C25INTER).render_bmp()
    with open("f.bmp", "wb+") as fp:
        fp.write(data)
        fp.seek(0)

        ImageFile.LOAD_TRUNCATED_IMAGES = True
        img = Image.open(fp)
        img.save(f"fff.png")


def svg():
    z = Zint("[255]11111111111222", BARCODE_RSS_EXP, show_text=False)
    with open("fff.svg", "w+") as fp:
        fp.write(z.render_svg(scale=2).decode())


if __name__ == '__main__':
    svg()
    png()
