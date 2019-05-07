import re
import json
import math
from tabulate import tabulate

# %load_ext autoreload
# %autoreload 2

def latex_table(data, headers):
    s = tabulate(data, headers=headers, tablefmt='latex')
    # https://github.com/gregbanks/python-tabulate/issues/5
    s = re.sub(r'(\^\{\})', "^", s); s = re.sub(r'\\([\$\_\{\}\^])', r'\1', s); s = re.sub(r'(\\textbackslash{})', r'\\', s)
    return s

def num(n):
    return str(round(n, 4))

def gen_table1(rn1):
    with open('result.json') as f:
        data = json.load(f)
    r10, r100, r1000, r5000, r10000, r20000 = data['reports']

    rowend = '\\\\\\hline\n'
    partrowend = '\\\\\\cline{2-7}\n'

    mean_exp = 500
    mean_row = ' & '.join(num(r['ran']['mean']) for r in data['reports'])
    mean_diff_row = ' & '.join(num((r['ran']['mean'] - mean_exp) / mean_exp) for r in data['reports'])

    stddev_exp = 1000 / (math.sqrt(3) * 2)
    stddev_row = ' & '.join(str(r['ran']['stddev']) for r in data['reports'])
    stddev_diff_row = ' & '.join(num((r['ran']['stddev'] - stddev_exp) / stddev_exp) for r in data['reports'])

    variability_exp = stddev_exp / mean_exp
    variability = [r['ran']['stddev'] / r['ran']['mean'] for r in data['reports']] 
    variability_row = ' & '.join(num(v) for v in variability)
    variability_diff_row = ' & '.join(num((v - variability_exp) / variability_exp) for v in variability)

    t = '\\begin{tabular}{|l*{6}{|c}|}\\hline\n'
    t += '\\multirow{2}{*}{Хар-ки и интервалы} & \\multicolumn{6}{c|}{RN ' + rn1 + '}\\\\\\cline{2-7}\n'
    t += '& 10 & 100 & 1000 & 5000 & 10000 & 20000' + rowend
    # mean
    t += '\\multirow{2}{*}{Мат. ож = ' + str(mean_exp) + '} & ' + mean_row + partrowend
    t += '& ' + mean_diff_row + rowend
    # stddev
    t += '\\multirow{2}{*}{С.к.о. = ' + num(stddev_exp) + '} & ' + stddev_row + partrowend
    t += '& ' + stddev_diff_row + rowend
    # variability
    t += '\\multirow{2}{*}{К-т вар. = ' + num(variability_exp) + '} & ' + variability_row + partrowend
    t += '& ' + variability_diff_row + rowend
    for i, rstart in enumerate(range(0, 1000, 100)):
        t += f'{rstart}-{rstart + 100} ' + ' '.join('& ' + str(r['ran']['freqs'][i]) for r in data['reports']) + rowend
    t += '\\end{tabular}'

    with open('table1.tex', 'w') as f:
        f.write(t)

gen_table1('10')
