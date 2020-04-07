import pytest

from pyzint import zint


def test_all():
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
    primary = "foo"
    text = "bar"
    dot_size = 5

    z = zint.Zint(
        "[255]11111111111222",
        zint.BARCODE_RSS_EXP,
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
    assert z.text == text
    assert z.primary == primary
    assert z.dot_size == dot_size


def test_border_width():
    z = zint.Zint("Barcode", zint.BARCODE_QRCODE)
    assert z.border_width == 0


def test_data():
    z = zint.Zint("Barcode", zint.BARCODE_QRCODE)
    assert z.data == "Barcode"


def test_dot_size():
    z = zint.Zint("Barcode", zint.BARCODE_QRCODE)
    assert z.dot_size == pytest.approx(0.8)


def test_eci():
    z = zint.Zint("Barcode", zint.BARCODE_QRCODE)
    assert z.eci == 0


def test_fontsize():
    z = zint.Zint("Barcode", zint.BARCODE_QRCODE)
    assert z.fontsize == 8


def test_height():
    z = zint.Zint("Barcode", zint.BARCODE_QRCODE)
    assert z.height == 50


def test_option_1():
    z = zint.Zint("Barcode", zint.BARCODE_QRCODE)
    assert z.option_1 == 0


def test_option_2():
    z = zint.Zint("Barcode", zint.BARCODE_QRCODE)
    assert z.option_2 == 0


def test_option_3():
    z = zint.Zint("Barcode", zint.BARCODE_QRCODE)
    assert z.option_3 == 0


def test_primary():
    z = zint.Zint("Barcode", zint.BARCODE_QRCODE)
    assert z.primary is None


def test_scale():
    z = zint.Zint("Barcode", zint.BARCODE_QRCODE)
    assert z.scale == 1.0


def test_show_text():
    z = zint.Zint("Barcode", zint.BARCODE_QRCODE)
    assert z.show_text is True


def test_symbology():
    z = zint.Zint("Barcode", zint.BARCODE_QRCODE)
    assert z.symbology == zint.BARCODE_QRCODE


def test_symbology_name():
    z = zint.Zint("Barcode", zint.BARCODE_QRCODE)
    assert z.symbology_name == "qrcode"


def test_text():
    z = zint.Zint("Barcode", zint.BARCODE_QRCODE)
    assert z.text is None
