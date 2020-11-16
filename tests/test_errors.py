import pytest

from pyzint.zint import (
    BARCODE_EAN14,
    BARCODE_ISBNX,
    BARCODE_ITF14,
    BARCODE_QRCODE,
    BARCODE_UPCA,
    BARCODE_UPCE,
    Zint,
)

bad_barcodes = pytest.mark.parametrize(
    "type,value",
    [
        (BARCODE_EAN14, "97802013796251111111"),
        (BARCODE_UPCE, "aaaaaa"),
        (BARCODE_UPCA, "aaaaaa"),
        (BARCODE_ITF14, "aaaaa"),
        (BARCODE_ISBNX, "22222"),
    ],
)

types = pytest.mark.parametrize("callback_ext", [
    Zint.render_svg,
    Zint.render_bmp,
])


@types
@bad_barcodes
def test_bmp_errors(type, value, callback_ext):
    z = Zint(value, type)
    with pytest.raises(RuntimeError):
        callback_ext(z)


@types
def test_png_too_long(callback_ext):
    z = Zint('a' * 100000, BARCODE_QRCODE)
    with pytest.raises(RuntimeError) as e:
        callback_ext(z)
    code, text = e.value.args
    assert code == 5
