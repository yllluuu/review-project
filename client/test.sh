#!/bin/bash 
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/iot24/yanglu/review-project/client/lib 
for((i=1;i<=100;i=i+1))
do
	./client -i 127.0.0.1 -p 78965 -t 2 &
done
