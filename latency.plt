set term png
set output "latency.png"

set xlabel "Latency (ms)"
set ylabel "Percentage of latency at least x"
set title "Latency CDF"


plot "latencies.dat" with lines
