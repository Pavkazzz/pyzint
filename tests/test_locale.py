import locale

from pyzint import zint


def test_ru_locale_svg():
    locale.setlocale(locale.LC_NUMERIC, "ru_RU.utf8")
    xml = zint.Zint("test", zint.BARCODE_AZTEC, height=200, border_width=4).render_svg().decode()
    assert "," not in xml
