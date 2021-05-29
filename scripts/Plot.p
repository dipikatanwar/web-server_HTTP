set terminal png size 600,400
set output './result/sizeVsHTTPVer.png'
set xlabel 'File Size(bytes)'
set ylabel 'Time'
plot    './data/sizeVsHTTPVer.txt' using 1:2 with lines title 'HTTP/1.0', \
        './data/sizeVsHTTPVer.txt' using 1:3 with lines title 'HTTP/1.1'
