rm result.txt
touch result.txt
for i in $(seq -w 10)
do 
    echo ======$i======== >> result.txt
    gcc ./test_3_r$i.c
    ./a.out >> result.txt
done