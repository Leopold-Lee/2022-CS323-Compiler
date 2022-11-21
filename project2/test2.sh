for i in $(seq -w 18)
do 
    touch ./test_result/result2$i.out
    ./bin/splc ./test-ex/test_SID_$i.spl>./test_result/result2$i.out 2>&1
done
for i in $(seq -w 18)
do 
    echo $i
    diff ./test_result/result2$i.out ./test-ex/test_SID_$i.out   
done