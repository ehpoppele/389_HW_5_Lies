set term png enhanced giant size 1980,1080
set output 'latency.png'
set yzeroaxis
set boxwidth 0.05 absolute
set style fill solid 1.0 noborder
bin_width=0.1
bin_number(x) = floor(x/bin_width)
rounded(x) = bin_width * ( bin_number(x) + 0.5)

plot 'latencies.dat' using (rounded($1)):(1) smooth frequency with boxes
