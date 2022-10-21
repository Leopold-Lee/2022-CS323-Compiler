for i in $(seq -w 12)
do 
    touch result/result$i.out
    chmod 777 result/result$i.out
    ./splc ./phase1/test/test_1_r$i.spl>./result/result$i.out 2>&1
done
for i in $(seq -w 12)
do 
    echo $i
    diff ./result/result$i.out ./phase1/test/test_1_r$i.out   
done