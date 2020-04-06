import xml.etree.ElementTree as ET
from io import BytesIO

import pytest
from PIL import Image

from pyzint.zint import BARCODE_RSS_EXP, Zint


def test_params_bytes():
    option_1 = 0
    option_2 = 0
    option_3 = 0
    scale = 9.76
    show_text = False
    fontsize = 10
    height = 74
    whitespace_width = 2
    border_width = 3
    eci = 4
    primary = b"foo"
    text = b"bar"
    dot_size = 5

    z = Zint(
        "[255]11111111111222",
        BARCODE_RSS_EXP,
        option_1,
        option_2,
        option_3,
        scale=scale,
        show_text=show_text,
        fontsize=fontsize,
        height=height,
        whitespace_width=whitespace_width,
        border_width=border_width,
        eci=eci,
        text=text,
        primary=primary,
        dot_size=dot_size,
    )

    assert z.data == "[255]11111111111222"
    assert z.option_1 == option_1
    assert z.option_2 == option_2
    assert z.option_3 == option_3

    assert z.scale == pytest.approx(scale)
    assert z.fontsize == fontsize
    assert z.height == height
    assert z.whitespace_width == whitespace_width
    assert z.border_width == border_width
    assert z.show_text == show_text
    assert z.eci == eci
    assert z.text == text.decode()
    assert z.primary == primary.decode()
    assert z.dot_size == dot_size


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
    assert int(xml.get("width")) == 366
    assert int(xml.get("height")) == 86
