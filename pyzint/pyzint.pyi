from typing import Union

# Tbarcode 7 codes
BARCODE_CODE11: int
BARCODE_C25MATRIX: int
BARCODE_C25INTER: int
BARCODE_C25IATA: int
BARCODE_C25LOGIC: int
BARCODE_C25IND: int
BARCODE_CODE39: int
BARCODE_EXCODE39: int
BARCODE_EANX: int
BARCODE_EANX_CHK: int
BARCODE_EAN128: int
BARCODE_CODABAR: int
BARCODE_CODE128: int
BARCODE_DPLEIT: int
BARCODE_DPIDENT: int
BARCODE_CODE16K: int
BARCODE_CODE49: int
BARCODE_CODE93: int
BARCODE_FLAT: int
BARCODE_RSS14: int
BARCODE_RSS_LTD: int
BARCODE_RSS_EXP: int
BARCODE_TELEPEN: int
BARCODE_UPCA: int
BARCODE_UPCA_CHK: int
BARCODE_UPCE: int
BARCODE_UPCE_CHK: int
BARCODE_POSTNET: int
BARCODE_MSI_PLESSEY: int
BARCODE_FIM: int
BARCODE_LOGMARS: int
BARCODE_PHARMA: int
BARCODE_PZN: int
BARCODE_PHARMA_TWO: int
BARCODE_PDF417: int
BARCODE_PDF417TRUNC: int
BARCODE_MAXICODE: int
BARCODE_QRCODE: int
BARCODE_CODE128B: int
BARCODE_AUSPOST: int
BARCODE_AUSREPLY: int
BARCODE_AUSROUTE: int
BARCODE_AUSREDIRECT: int
BARCODE_ISBNX: int
BARCODE_RM4SCC: int
BARCODE_DATAMATRIX: int
BARCODE_EAN14: int
BARCODE_VIN: int
BARCODE_CODABLOCKF: int
BARCODE_NVE18: int
BARCODE_JAPANPOST: int
BARCODE_KOREAPOST: int
BARCODE_RSS14STACK: int
BARCODE_RSS14STACK_OMNI: int
BARCODE_RSS_EXPSTACK: int
BARCODE_PLANET: int
BARCODE_MICROPDF417: int
BARCODE_ONECODE: int
BARCODE_PLESSEY: int

# Tbarcode 8 codes
BARCODE_TELEPEN_NUM: int
BARCODE_ITF14: int
BARCODE_KIX: int
BARCODE_AZTEC: int
BARCODE_DAFT: int
BARCODE_MICROQR: int

# Tbarcode 9 codes
BARCODE_HIBC_128: int
BARCODE_HIBC_39: int
BARCODE_HIBC_DM: int
BARCODE_HIBC_QR: int
BARCODE_HIBC_PDF: int
BARCODE_HIBC_MICPDF: int
BARCODE_HIBC_BLOCKF: int
BARCODE_HIBC_AZTEC: int

# Tbarcode 10 codes
BARCODE_DOTCODE: int
BARCODE_HANXIN: int

# Tbarcode 11 codes
BARCODE_MAILMARK: int

# Zint specific
BARCODE_AZRUNE: int
BARCODE_CODE32: int
BARCODE_EANX_CC: int
BARCODE_EAN128_CC: int
BARCODE_RSS14_CC: int
BARCODE_RSS_LTD_CC: int
BARCODE_RSS_EXP_CC: int
BARCODE_UPCA_CC: int
BARCODE_UPCE_CC: int
BARCODE_RSS14STACK_CC: int
BARCODE_RSS14_OMNI_CC: int
BARCODE_RSS_EXPSTACK_CC: int
BARCODE_CHANNEL: int
BARCODE_CODEONE: int
BARCODE_GRIDMATRIX: int
BARCODE_UPNQR: int
BARCODE_ULTRA: int
BARCODE_RMQR: int


# noinspection PyPropertyDefinition
class Zint:
    def __init__(
        self,
        data: Union[str, bytes],
        kind: int,
        scale: float = 1.0,
        show_text: bool = True,
        option_1: int = -1,
        option_2: int = 0,
        option_3: int = 928,
        fontsize: int = 8,
        primary: Union[str, bytes] = None
    ): ...

    def render_bmp(
        self, angle: int = 0, bgcolor="#FFFFFF", fgcolor="#000000"
    ) -> bytes:
        ...

    def render_svg(
        self, angle: int = 0, bgcolor="#FFFFFF", fgcolor="#000000"
    ) -> bytes:
        ...

    def human_symbology(self) -> str:
        ...
