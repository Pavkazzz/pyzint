pyzint
======
.. image:: https://github.com/pavkazzz/pyzint/workflows/Python%20package/badge.svg
    :target: https://github.com/Pavkazzz/pyzint/actions
    :alt: GitHub CI

.. image:: https://img.shields.io/pypi/v/pyzint.svg
    :target: https://pypi.python.org/pypi/pyzint/
    :alt: Latest Version

.. image:: https://img.shields.io/pypi/wheel/pyzint.svg
    :target: https://pypi.python.org/pypi/pyzint/

.. image:: https://img.shields.io/pypi/pyversions/pyzint.svg
    :target: https://pypi.python.org/pypi/pyzint/

.. image:: https://img.shields.io/pypi/l/pyzint.svg
    :target: https://pypi.python.org/pypi/pyzint/


Python c-api binding to zint c library


Example
=======

Create main object zint
.. code-block:: python
z = Zint("[255]1111111111122", BARCODE_RSS_EXP)
..

Generate bmp image
.. code-block:: python
z.render_bmp()
..

Generate svg image
.. code-block:: python
z.render_bmp()
..
