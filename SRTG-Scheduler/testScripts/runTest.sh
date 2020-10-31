for i in {0..99};
do 
	echo set-"$i";
	$1 --j $2/$2-"$i"-jobs.txt --rt $2/$2-"$i"-releaseTimes.txt --m 99 >> $3/output.log
done
mv RTGS-Summary $3/
