from io import BytesIO

from cyzint.cyzint import ZBarcode, BARCODE_RSS_EXP
from PIL import Image

z = ZBarcode("[255]11111111111222", BARCODE_RSS_EXP)
data = z.render_bmp()
with open("f.bmp", "wb+") as fp:
    fp.write(data)
    fp.seek(0)
    img = Image.open(fp)
    img.save("fff.png")
