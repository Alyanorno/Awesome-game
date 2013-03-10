"P:\Program Files (x86)\Git\cmd\git.exe" log --author="Rickard Fridvall" --pretty=format:"%%cd" --date-order --date=short --numstat --reverse > log.log
python graph.py
"P:\Program Files (x86)\gnuplot\bin\wgnuplot.exe" plot.cfg
graph.png
