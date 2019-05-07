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

## Generating `.json` results

1. Place `main.py` alongside `rexp.gps` and `erlang.gps`
2. Create copies of `rexp.gps` for your variants (e.g. `rexp100.gps` and `rexp200.gps`).
Replace `RN1` with the parameter you're given for `T_RAN`, and `Exponential(1,` with the same parameter for `T_EXP`.
3. Create copies of `erlang.gps` for each of your variants and `k` parameters (`erlang100_k2.gps`, `erlang100_k4.gps`, ...)
4. Open `main.py` in an editor and substitute the value of `GPSS_PATH` with the path to your GPSS World installation,
and the value of `LAB_FILES` with the copies you've just created.
5. Run `python3 main.py`

## Creating the report

This can be done on Linux (`pywin32` and `pywinauto` packages are not needed, but you do need `tabulate`).

1. Place `rexp_tables.py` alongside your result files
2. Open `rexp_tables.py` in an editor and substitute the value of `RESULTS` with the result files
3. Run `python3 rexp_tables.py`
4. Create a TeX file to wrap the generated `table1.tex` and `table2.tex` (refer to `report.tex` as an example)
5. Render it (`xelatex report.tex`)

