g++ -o cg1.bin cg1.cpp -framework OpenGL -framework GLUT 

./cg1.bin

brew install texlive

cd SG2
pdflatex -interaction=nonstopmode -halt-on-error sg2.tex

brew install gnuplot

cd SG3
lualatex --shell-escape -interaction=nonstopmode -halt-on-error sg3.tex