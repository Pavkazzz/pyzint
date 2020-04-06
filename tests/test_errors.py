import pytest

from pyzint.zint import (
    BARCODE_EAN14,
    BARCODE_ISBNX,
    BARCODE_ITF14,
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


@bad_barcodes
def test_bmp_errors(type, value):
    z = Zint(value, type)
    with pytest.raises(RuntimeError):
        z.render_bmp()


@bad_barcodes
def test_svg_errors(type, value):
    z = Zint(value, type)
    with pytest.raises(RuntimeError):
        z.render_svg()
