% vim: syntax=tex
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{labreport}

% Options:
% * listings — use the `listings` package
\newif\ifListings
\Listingsfalse
\DeclareOption{listings}{\Listingstrue}
% * en — use the English version of the title page
\newif\ifEnglish
\Englishfalse
\DeclareOption{en}{\Englishtrue}

\ProcessOptions\relax

\LoadClass[12pt, a4paper]{article}

\RequirePackage[a4paper, includeheadfoot, mag=1000,
  headsep=0.8cm, footskip=0.8cm,
  left=2cm, right=1.5cm, top=1.5cm, bottom=1.5cm]{geometry}

% Set up fonts & language support
\RequirePackage{fontspec, unicode-math}
\setmainfont[Ligatures=TeX]{CMU Serif}
\setmonofont{CMU Typewriter Text}
\RequirePackage[english, russian]{babel}

% Indent first paragraph
\RequirePackage{indentfirst}
\setlength{\parskip}{5pt}

% Logo
\RequirePackage{graphicx}

\ifListings
  \RequirePackage[dvipsnames]{xcolor}
  \RequirePackage{listings}
\fi

\newcommand{\organization}[1]{\renewcommand{\titleorganization}{#1}}
\newcommand{\departmentsubject}[2]{
  \renewcommand{\titledepartment}{#1}
  \renewcommand{\titlesubject}{\uppercase{#2}}
}
\newcommand{\titleparts}[2]{
  \renewcommand{\titlemain}{#1}
  \renewcommand{\titlesub}{#2}
}
\newcommand{\students}[1]{\renewcommand{\titlestudents}{#1}}
\newcommand{\group}[1]{\renewcommand{\titlegroup}{#1}}

\ifEnglish
  \newcommand{\titlelogo}{
    \includegraphics[trim=0 -0.85cm 0 2.6cm, width=9cm]{itmo-en.pdf}}
  \newcommand{\titleorganization}{National Research University ITMO}
  \newcommand{\titledepartment}{Department of Computational Technologies}
  \newcommand{\titlesubject}{\uppercase{Subject name}}
  \newcommand{\titlemain}{Lab Work \#1}
  \newcommand{\titlesub}{Lab topic}
  \newcommand{\titlestudents}{First Name Last Name}
  \newcommand{\titlegroup}{Group P3202}
  \newcommand{\titlecity}{Saint Petersburg}
\else
  \newcommand{\titlelogo}{
    \includegraphics[trim=0 0 0 3cm, width=10cm]{itmo-ru.pdf}}
  \newcommand{\titleorganization}{ФГАОУ ВО «Санкт-Петербургский национальный исследовательский университет информационных технологий, механики и оптики»}
  \newcommand{\titledepartment}{Кафедра вычислительной техники}
  \newcommand{\titlesubject}{\uppercase{Название предмета}}
  \newcommand{\titlemain}{Лабораторная работа №1}
  \newcommand{\titlesub}{Вариант 1010}
  \newcommand{\titlestudents}{Фамилия Имя Отчество}
  \newcommand{\titlegroup}{Группа P3202}
  \newcommand{\titlecity}{Санкт-Петербург}
\fi

% Title page template
\newcommand{\maketitlepage}[0]{
  \begin{titlepage}
  \begin{center}

  \titlelogo\\
  \textsc{
    \titleorganization\\[4mm]
    \titledepartment\\[1.8cm]
  }

  \noindent\rule{\textwidth}{0.8pt}\\[8mm]

  \textbf{\titlesubject}\\[8mm]
  \textbf{\titlemain}\\[2mm]
  \titlesub\\[6mm]

  \noindent\rule{\textwidth}{0.8pt}\\[1.8cm]

  \titlestudents\\[2mm]
  \titlegroup

  \vfill
  \titlecity\\[2mm]
  \the\year

  \end{center}
  \end{titlepage}
}
