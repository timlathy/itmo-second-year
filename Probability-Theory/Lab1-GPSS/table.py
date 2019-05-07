import re
import json
import math
from tabulate import tabulate

def num(n):
    return str(round(n, 4))

def numcells(cells):
    return ' & '.join(map(num, cells))

rowend = '\\\\\\hline\n'
partrowend = '\\\\\\cline{2-13}\n'

def gen_table1(rn1, rn2):
    with open('result.json') as f:
        data = json.load(f)

    mean_exp = 500
    stddev_exp = 1000 / (math.sqrt(3) * 2)
    variability_exp = stddev_exp / mean_exp

    mean = [r['ran']['mean'] for r in data['reports']] +\
        [r['exp']['mean'] for r in data['reports']]
    mean_diff = [(m - mean_exp) / mean_exp for m in mean]

    stddev = [r['ran']['stddev'] for r in data['reports']] +\
        [r['exp']['stddev'] for r in data['reports']]
    stddev_diff = [(s - stddev_exp) / s for s in stddev]

    variability = [r['ran']['stddev'] / r['ran']['mean'] for r in data['reports']] +\
        [r['exp']['stddev'] / r['exp']['mean'] for r in data['reports']]
    variability_diff = [(v - variability_exp) / variability_exp for v in variability]

    t = '\\begin{tabular}{|l*{12}{|c}|}\\hline\n'
    t += '\\multirow{2}{*}{Хар-ки и интервалы} & \\multicolumn{6}{c|}{RN ' + rn1 + '} & \\multicolumn{6}{c|}{RN ' + rn2 + '}\\\\\\cline{2-13}\n'
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
        freqs = [r['ran']['freqs'][i] for r in data['reports']] +\
            [r['exp']['freqs'][i] for r in data['reports']]
        t += num(rstart) + '-' + num(rstart + 100) + ' & ' + numcells(freqs) + rowend
    t += '\\end{tabular}'

    with open('table1.tex', 'w') as f:
        f.write(t)

gen_table1('10', '20')
