from pywinauto.application import Application
from pywinauto.keyboard import send_keys
from time import sleep
import json
import re

GPSS_PATH = 'C:\Program Files (x86)\GPSS World\GPSS World Student.exe'
LAB_FILES = [
    ('rexp176.gps', ['T_RAN', 'T_EXP']),
    ('rexp924.gps', ['T_RAN', 'T_EXP']),
    ('erlang176_k2.gps', ['TA5']),
    ('erlang176_k4.gps', ['TA5']),
    ('erlang176_k8.gps', ['TA5']),
    ('erlang924_k2.gps', ['TA5']),
    ('erlang924_k4.gps', ['TA5']),
    ('erlang924_k8.gps', ['TA5'])
]

def extract_report_data(raw, report_sections):
    def parse_freq_lines(num_ranges, lines):
        freqs = [0 for i in range(num_ranges)]
        for l in lines:
            _, start, _, _, freq, _ = l.split(' ')
            if start == '_':
                start_idx = 0
            else:
                start_idx = int(int(start.split('.')[0]) / 100)
            freqs[start_idx] = int(freq)
        return freqs
    def parse_freq_section(section_name, all_lines):
        start_idx = next(i for i, l in enumerate(lines) if l.startswith(f' {section_name}'))
        line_count = next(i for i, l in enumerate(lines[start_idx+1:]) if len(l) < 2 or l[1] == 'T')
        section_lines = [re.sub(r"\s\s+", ' ', l) for l in lines[start_idx:start_idx+line_count+1]]
        mean, stddev = section_lines[0].split(' ')[2:4]
        freqs = parse_freq_lines(20, section_lines[1:])
        return {'mean': float(mean), 'stddev': float(stddev), 'freqs': freqs}

    lines = raw.split('\r')
    return {k: parse_freq_section(k, lines) for k in report_sections}

def simulation_report(gpss, workspace, num_values, report_sections):
    send_keys('%ct') # Alt+C = Command, +T = START
    start_command_window = gpss.top_window().child_window(title="Start Command", control_type="Window")
    start_command_window.Edit.set_edit_text(f'START {num_values}')
    send_keys('{ENTER}')
    
    report_window = workspace.Dialog
    report_text = report_window.child_window(control_type="Edit").get_value()
    report_window.close()
    send_keys('{TAB}{ENTER}') # save report? no - yes - cancel, tab selects "yes", enter presses it (send_keys is faster than looking up the button)

    return extract_report_data(report_text, report_sections)

for file, sections in LAB_FILES:
    gpss = Application(backend='uia').start(GPSS_PATH + ' ' + file)
    workspace = gpss.top_window().child_window(title="Workspace", control_type="Pane")

    send_keys('^%s') # Ctrl+Alt+S = Create Simulation

    reports = [simulation_report(gpss, workspace, num_values, sections)
               for num_values in [10, 90, 900, 4000, 5000, 10000]]
    journal = workspace.Dialog.Edit.get_value().replace('\r', '\n')

    with open(file + '.json', 'w') as f:
        json.dump({'reports': reports, 'journal': journal}, f)

    gpss.kill()
