import re
import json
import math
from tabulate import tabulate

TABLE1 = [('rexp176.gps.json', 176), ('rexp924.gps.json', 924)]

def num(n):
    return str(round(n, 4))

def numcells(cells):
    return ' & '.join(map(num, cells))

rowend = '\\\\\\hline\n'
partrowend = '\\\\\\cline{2-13}\n'

def gen_table1():
    file1, rn1 = TABLE1[0]
    file2, rn2 = TABLE1[1]

    with open(file1) as f:
        data1 = json.load(f)
    with open(file2) as f:
        data2 = json.load(f)

    reports = [r['T_RAN'] for r in data1['reports']] + [r['T_RAN'] for r in data2['reports']]

    mean_exp = 500
    stddev_exp = 1000 / (math.sqrt(3) * 2)
    variability_exp = stddev_exp / mean_exp

    mean = [r['mean'] for r in reports]
    mean_diff = [(m - mean_exp) / mean_exp for m in mean]

    stddev = [r['stddev'] for r in reports]
    stddev_diff = [(s - stddev_exp) / s for s in stddev]

    variability = [r['stddev'] / r['mean'] for r in reports]
    variability_diff = [(v - variability_exp) / variability_exp for v in variability]

    t = '\\begin{tabular}{|l*{12}{|c}|}\\hline\n'
    t += '\\multirow{2}{*}{Хар-ки и интервалы} & \\multicolumn{6}{c|}{RN ' + num(rn1) + '} & \\multicolumn{6}{c|}{RN ' + num(rn2) + '}\\\\\\cline{2-13}\n'
    t += '& 10 & 100 & 1000 & 5000 & 10000 & 20000 & 10 & 100 & 1000 & 5000 & 10000 & 20000' + rowend
    # mean
    t += '\\multirow{2}{*}{Мат. ож = ' + num(mean_exp) + '} & ' + numcells(mean) + partrowend
    t += '& ' + numcells(mean_diff) + rowend
    # stddev
    t += '\\multirow{2}{*}{С.к.о. = ' + num(stddev_exp) + '} & ' + numcells(stddev) + partrowend
    t += '& ' + numcells(stddev_diff) + rowend
    # variability
    t += '\\multirow{2}{*}{К-т вар. = ' + num(variability_exp) + '} & ' + numcells(variability) + partrowend
    t += '& ' + numcells(variability_diff) + rowend
    for i, rstart in enumerate(range(0, 1000, 100)):
        freqs = [r['freqs'][i] for r in reports]
        t += num(rstart) + '-' + num(rstart + 100) + ' & ' + numcells(freqs) + rowend
    t += '\\end{tabular}'

    with open('table1.tex', 'w') as f:
        f.write(t)

gen_table1()
