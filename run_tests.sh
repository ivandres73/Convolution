#!/bin/bash

#path substrings
folder="imgs/"
exten=".jpg"
img_path=""

#Creating empty new file
TIME_LOG_FILE=times.txt

rm -f $TIME_LOG_FILE
touch $TIME_LOG_FILE

#executing the program 100 times
for i in {1..100}
do
    img_path=$folder$i$exten
    echo ./a.out $img_path 4
    ./a.out $img_path 4 >> $TIME_LOG_FILE
done

exit 0