for i in $(seq -w 15)
do 
    touch ./test_result/result$i.out
    ./bin/splc ./project2_testcases/github_test/test_2_r$i.spl>./test_result/result$i.out 2>&1
done
for i in $(seq -w 15)
do 
    echo $i
    diff ./test_result/result$i.out ./project2_testcases/github_test/test_2_r$i.out   
done