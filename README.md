# Computer Systems Engineering, ITMO University, Second Year

I have started this repository at the beginning of my second year (2018-2019)
of studying for a Bachelor's in Computer Science at ITMO Univesity.

It is essentially a continuation of my
[first year's collection](https://github.com/timlathy/itmo-first-year)
of assignment solutions, code snippets, and notes. I decided against
continuing it to start afresh with a simplified, more sensible repository layout:
directory names reflect course titles, and each course gets a separate directory.

Most of the assignments here are my individual work.
Group projects (labs & coursework) are hosted in
[our study group organization](https://github.com/band-of-four).

If you find any of the notes or sources useful, you are free to copy
and modify them as you see fit. Do note that they are provided
without any kind of warranty.

## Lab Reports

I write my lab reports in LaTeX using a special document class that creates a title page
and adds a couple of style tweaks. You'll need to have this class locally in case you
want to render a report to PDF.

To install it, you can run the following command (provided you have `epstopdf` installed,
which is available in the `texlive-epstopdf` package in Fedora
and as a part of `texlive-font-utils` in Ubuntu and Debian):

```bash
mkdir -p ~/texmf/tex/latex/labreport && \
  curl -s http://www.ifmo.ru/file/news/4246/itmo_logo_rus_vert_bw.eps | epstopdf -f -o=$HOME/texmf/tex/latex/labreport/itmo-ru.pdf && \
  curl -s http://www.ifmo.ru/file/news/4246/itmo_logo_en_vert_bw.eps | epstopdf -f -o=$HOME/texmf/tex/latex/labreport/itmo-en.pdf && \
  curl https://raw.githubusercontent.com/timlathy/itmo-second-year/master/labreport.cls -o ~/texmf/tex/latex/labreport/labreport.cls
```
