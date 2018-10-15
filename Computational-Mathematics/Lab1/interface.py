from IPython.display import display
import ipywidgets as widgets
from _nonlineq import bisect_solve_interval, newton_solve_interval, newton_solve_interval_manual

def run():
  _ = widgets.interact(data_input, mode=widgets.RadioButtons(
                         description='Режим:',
                         options=['чтение из файла', 'интерактивный ввод']))


# === Widgets


def data_input(mode):
  if mode == 'чтение из файла':
    display_file_input(lambda path: load_file(path),
                       description='Загрузить:',
                       placeholder='Путь к файлу с a, b, Δ, разделенными пробелом',
                       button_text="Открыть")
  else:
    int_start = widgets.FloatText(description='a', value=0.0, step=0.1)
    int_end = widgets.FloatText(description='b', value=0.0, step=0.1)
    delta = widgets.FloatText(description='Δ', value=0.01, step=0.000001)
    widgets.interact(find_root, int_start=int_start, int_end=int_end, delta=delta)


def display_file_input(file_chosen_cb, description, placeholder, button_text):
  file_input = widgets.Text(description=description, placeholder=placeholder,
                            layout=widgets.Layout(width='400px'))
  file_input_button = widgets.Button(description=button_text)
  file_input_button.on_click(lambda _: file_chosen_cb(file_input.value))
  display(widgets.HBox([file_input, file_input_button]))


def save_result(res):
  def write_to_file(path):
    with open(path, 'w') as f: f.write(res)
    print('Файл записан')

  display_file_input(write_to_file,
                     description='Сохранить:',
                     placeholder='Путь к файлу для записи x, f(x), числа итераций',
                     button_text="Записать")


# === Computation routines


def load_file(path):
  try:
    with open(path, 'r') as f:
      int_start, int_end, delta = [float(v) for v in f.read().split(' ')]
    find_root(int_start, int_end, delta)
  except OSError:
    print(f'Невозможно открыть файл {path}')
  except ValueError:
    print(f'Файл должен содержать начало интервала, конец интервала, погрешность, записанные через пробел')


def find_root(int_start, int_end, delta):
  try:
    print('Метод половинного деления:')
    (x, f_x, iter_num) = bisect_solve_interval(
      1, 2.28, -1.934, -3.907, int_start, int_end, delta)
    print(f'  число итераций = {iter_num}, x = {x}, f(x) = {f_x}')
    print('Метод Ньютона:')
    (x_n, f_x_n, iter_num_n) = newton_solve_interval(
      1, 2.28, -1.934, -3.907, int_start, int_end, delta)
    print(f'  число итераций = {iter_num_n}, x = {x_n}, f(x) = {f_x_n}')
    save_result(f'{x} {f_x} {iter_num}\n{x_n} {f_x_n} {iter_num_n}\n')
  except ValueError as e:
    if str(e) == 'Expected an isolating interval':
      print('  Необходим интервал изоляции корня: функция должна принимать разные знаки на концах интервала')
    if str(e) == 'Cannot determine the initial guess':
      print('  Невозможно определить начальное приближение: ни на одном из концов не совпадают знаки функции и второй производной')
