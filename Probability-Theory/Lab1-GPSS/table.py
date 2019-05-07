import re
import json
from tabulate import tabulate

# %load_ext autoreload
# %autoreload 2

def latex_table(data, headers):
    s = tabulate(data, headers=headers, tablefmt='latex')
    # https://github.com/gregbanks/python-tabulate/issues/5
    s = re.sub(r'(\^\{\})', "^", s); s = re.sub(r'\\([\$\_\{\}\^])', r'\1', s); s = re.sub(r'(\\textbackslash{})', r'\\', s)
    return s

def gen_table1(rn1):
    with open('result.json') as f:
        data = json.load(f)
    r10, r100, r1000, r5000, r10000, r20000 = data['reports']

    rowend = '\\\\\\hline\n'
    partrowend = '\\\\\\cline{2-7}\n'

    t = '\\begin{tabular}{|l*{6}{|c}|}\\hline\n'
    t += '\\multirow{2}{*}{Хар-ки и интервалы} & \\multicolumn{6}{c|}{RN ' + rn1 + '}\\\\\\cline{2-7}\n'
    t += '& 10 & 100 & 1000 & 5000 & 10000 & 20000' + rowend
    # mean
    t += '\\multirow{2}{*}{Мат. ож =} ' + ' '.join('& ' + str(r['ran']['mean']) for r in data['reports']) + partrowend
    t += ' '.join('& ' + '???' for r in data['reports']) + rowend
    # stddev
    t += '\\multirow{2}{*}{С.к.о. =} ' + ' '.join('& ' + str(r['ran']['stddev']) for r in data['reports']) + partrowend
    t += ' '.join('& ' + '???' for r in data['reports']) + rowend
    # variability
    t += '\\multirow{2}{*}{К-т вар. =} ' + ' '.join('& ' + '???' for r in data['reports']) + partrowend
    t += ' '.join('& ' + '???' for r in data['reports']) + rowend
    for i, rstart in enumerate(range(0, 1000, 100)):
        t += f'{rstart}-{rstart + 100} ' + ' '.join('& ' + str(r['ran']['freqs'][i]) for r in data['reports']) + rowend
    t += '\\end{tabular}'

    with open('table1.tex', 'w') as f:
        f.write(t)

gen_table1('10')
