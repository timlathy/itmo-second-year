import re
import json
import math
from tabulate import tabulate

RESULTS = [('rexp176.gps.json', 176), ('rexp924.gps.json', 924)]

def num(n):
    return str(round(n, 4))

def numcells(cells):
    return ' & '.join(map(num, cells))

rowend = '\\\\\\hline\n'
partrowend = '\\\\\\cline{2-13}\n'

def result_table(rn1, rn2, reports, mean_pred, stddev_pred, variability_pred, ranges):
    mean = [r['mean'] for r in reports]
    mean_diff = [(m - mean_pred) / mean_pred for m in mean]

    stddev = [r['stddev'] for r in reports]
    stddev_diff = [(s - stddev_pred) / s for s in stddev]

    variability = [r['stddev'] / r['mean'] for r in reports]
    variability_diff = [(v - variability_pred) / variability_pred for v in variability]

    t = '\\begin{tabular}{|l*{12}{|c}|}\\hline\n'
    t += '\\multirow{2}{*}{Хар-ки и интервалы} & \\multicolumn{6}{c|}{RN ' + num(rn1) + '} & \\multicolumn{6}{c|}{RN ' + num(rn2) + '}\\\\\\cline{2-13}\n'
    t += '& 10 & 100 & 1000 & 5000 & 10000 & 20000 & 10 & 100 & 1000 & 5000 & 10000 & 20000' + rowend
    # mean
    t += '\\multirow{2}{*}{Мат. ож = ' + num(mean_pred) + '} & ' + numcells(mean) + partrowend
    t += '& ' + numcells(mean_diff) + rowend
    # stddev
    t += '\\multirow{2}{*}{С.к.о. = ' + num(stddev_pred) + '} & ' + numcells(stddev) + partrowend
    t += '& ' + numcells(stddev_diff) + rowend
    # variability
    t += '\\multirow{2}{*}{К-т вар. = ' + num(variability_pred) + '} & ' + numcells(variability) + partrowend
    t += '& ' + numcells(variability_diff) + rowend
    for i, rstart in enumerate(ranges):
        freqs = [r['freqs'][i] for r in reports]
        t += num(rstart) + '-' + num(rstart + 100) + ' & ' + numcells(freqs) + rowend
    t += '\\end{tabular}'

    return t

def gen_tables():
    (file1, rn1), (file2, rn2) = RESULTS

    with open(file1) as f:
        data1 = json.load(f)
    with open(file2) as f:
        data2 = json.load(f)

    reports_ran = [r['T_RAN'] for r in data1['reports']] + [r['T_RAN'] for r in data2['reports']]
    ranges_ran = range(0, 1000, 100)
    reports_exp = [r['T_EXP'] for r in data1['reports']] + [r['T_EXP'] for r in data2['reports']]
    ranges_exp = range(0, 2000, 100)

    mean_ran_pred = 500
    stddev_ran_pred = 1000 / (math.sqrt(3) * 2)
    variability_ran_pred = stddev_ran_pred / mean_ran_pred

    table_ran = result_table(rn1, rn2,
        reports_ran, mean_ran_pred, stddev_ran_pred, variability_ran_pred, ranges_ran)

    mean_exp_pred = 500
    stddev_exp_pred = mean_exp_pred
    variability_exp_pred = stddev_exp_pred / mean_exp_pred

    table_exp = result_table(rn1, rn2,
        reports_exp, mean_exp_pred, stddev_exp_pred, variability_exp_pred, ranges_exp)

    with open('table1.tex', 'w') as f:
        f.write(table_ran)

    with open('table2.tex', 'w') as f:
        f.write(table_exp)

gen_tables()
