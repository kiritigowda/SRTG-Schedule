for j in `seq 1 10`;
do
	rm -rf output"$j"
	mkdir output"$j"
    for i in `seq 0 99`;
	do
    	echo TestSet "$j" -- set-"$i"
    	./../RTG-Scheduler/bin/RTG-scheduler --j testData/testSet"$j"/testSet"$j"-"$i"-jobs.txt --rt testData/testSet"$j"/testSet"$j"-"$i"-releaseTimes.txt --m 99 >> output"$j"/output.log
	done
	mv RTGS-Summary output"$j"/RTGS-Summary
done

