for i in $(seq -w 10)
do 
    touch ./test/test_3_r$i.ir
    ./bin/splc ./test/test_3_r$i.spl>./test/test_3_r$i.ir 2>&1
done