for i in $(seq -w 5)
do 
    ./bin/splc ./test/test_1_$i.spl>./test/test_1_$i.out 2>&1
done