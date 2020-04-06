import pytest
from pyzint.pyzint import Zint, BARCODE_QRCODE


def test_border_width():
    z = Zint("Barcode", BARCODE_QRCODE)
    assert z.border_width == 0


def test_data():
    z = Zint("Barcode", BARCODE_QRCODE)
    assert z.data == 'Barcode'


def test_dot_size():
    z = Zint("Barcode", BARCODE_QRCODE)
    assert z.dot_size == pytest.approx(0.8)


def test_eci():
    z = Zint("Barcode", BARCODE_QRCODE)
    assert z.eci == 0


def test_fontsize():
    z = Zint("Barcode", BARCODE_QRCODE)
    assert z.fontsize == 8


def test_height():
    z = Zint("Barcode", BARCODE_QRCODE)
    assert z.height == 50


def test_option_1():
    z = Zint("Barcode", BARCODE_QRCODE)
    assert z.option_1 == 0


def test_option_2():
    z = Zint("Barcode", BARCODE_QRCODE)
    assert z.option_2 == 0


def test_option_3():
    z = Zint("Barcode", BARCODE_QRCODE)
    assert z.option_3 == 0


def test_primary():
    z = Zint("Barcode", BARCODE_QRCODE)
    assert z.primary is None


def test_scale():
    z = Zint("Barcode", BARCODE_QRCODE)
    assert z.scale == 1.0


def test_show_text():
    z = Zint("Barcode", BARCODE_QRCODE)
    assert z.show_text is True


def test_symbology():
    z = Zint("Barcode", BARCODE_QRCODE)
    assert z.symbology == BARCODE_QRCODE


def test_symbology_name():
    z = Zint("Barcode", BARCODE_QRCODE)
    assert z.symbology_name == 'qrcode'


def test_text():
    z = Zint("Barcode", BARCODE_QRCODE)
    assert z.text is None
