for i in $(seq -w 12)
do 
    ./bin/splc ./phase1/test/test_1_r$i.spl>./phase1/result/result$i.out 2>&1
done
for i in $(seq -w 12)
do 
    echo $i
    diff ./phase1/result/result$i.out ./phase1/test/test_1_r$i.out   
done