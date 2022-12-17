for i in $(seq 4)
do 
    ./bin/splc ./extra/test_3_b'0'$i.spl>./extra/test_3_b{'0'$i}.ir 2>&1
done