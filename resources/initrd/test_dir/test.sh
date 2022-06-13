for i in {1..2048}
do
   touch $i.txt
   echo "This file name is $i.txt!" > $i.txt
done
