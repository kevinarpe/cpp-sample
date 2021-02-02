Generated using these Bash shell commands:

$ for (( b = 1; b <= 100; ++b )) ; do for (( c = (b - 1) * 100 - 10 ; c <= (b - 1) * 100 + 90 ; ++c )) ; do printf -- 'b%d,c%d,%d\n' $b $c $(( 1 + (RANDOM % 100) )) ; done ; done > data/test/throughput/basket_defs.csv
$ for (( b = 1; b <= 100; ++b )) ; do printf -- 'b%d,0.%d,0.%d\n' $b $(( 25 + (RANDOM % 75) )) $(( 25 + (RANDOM % 75) )) ; done > data/test/throughput/basket_thresholds.csv
