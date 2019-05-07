# GPSS World Basics

This lab work requires performing a sequence of manual actions in
the [GPSS World](http://www.minutemansoftware.com/downloads.asp) software package.

I decided to take a stab at automating this with [Pywinauto](https://pywinauto.github.io/).

## Prerequisites

* a Windows system (I couldn't get Wine to work with this :/)
* [GPSS World](http://www.minutemansoftware.com/downloads.asp)
* Python 3.7+
* [pywin32](https://github.com/mhammond/pywin32/releases) (make sure to choose the `.exe` that matches your Python interpreter version)

```sh
pip install --user Pillow pywinauto
```

## Generating `result.json`

1. Place `main.py` alongside `rexp.gps` and `erlang.gps`
2. Open `main.py` in an editor and substitute the value of `GPSS_PATH` with the path to your GPSS World installation
3. Run `python3 main.py`

## Creating the report

This can be done on Linux (`pywin32` and `pywinauto` packages are not needed).

1. Place `table.py` alongside your `result.json`
2. Run `python3 table.py`
3. Create a TeX file to wrap the generated `table1.tex` (refer to `report.tex` as an example)
4. Render it (`xelatex report.tex`)

