from PIL import Image, ImageFile

from pyzint import Barcode


def png():
    z = Barcode.UPCA("33123456")
    data = z.render_bmp()
    with open("f.bmp", "wb+") as fp:
        fp.write(data)
        fp.seek(0)

        ImageFile.LOAD_TRUNCATED_IMAGES = True
        img = Image.open(fp)
        img.save("fff.png")


def svg():
    z = Barcode.RSS_EXP("[255]11111111111222", scale=2, show_text=False)
    with open("fff.svg", "w+") as fp:
        fp.write(z.render_svg().decode())


if __name__ == "__main__":
    svg()
    png()
