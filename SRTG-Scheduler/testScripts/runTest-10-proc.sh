mkdir output1
for i in `seq 0 99`;
do
    echo TestSet 1 -- set-"$i"
    ./../SRTG-Scheduler/bin/SRTG-Scheduler --j testData/testSet1/testSet1-"$i"-jobs.txt --rt testData/testSet1/testSet1-"$i"-releaseTimes.txt --m 99 --p 2 --d 1 >> output1/output.log
done
mv RTGS-Summary output1/RTGS-Summary

mkdir output2
for i in `seq 0 99`;
do
    echo TestSet 2 -- set-"$i"
    ./../SRTG-Scheduler/bin/SRTG-Scheduler --j testData/testSet2/testSet2-"$i"-jobs.txt --rt testData/testSet2/testSet2-"$i"-releaseTimes.txt --m 99 --p 4 --d 3 >> output2/output.log
done
mv RTGS-Summary output2/RTGS-Summary

mkdir output3
for i in `seq 0 99`;
do
    echo TestSet 3 -- set-"$i"
    ./../SRTG-Scheduler/bin/SRTG-Scheduler --j testData/testSet3/testSet3-"$i"-jobs.txt --rt testData/testSet3/testSet3-"$i"-releaseTimes.txt --m 99 --p 8 --d 6 >> output3/output.log
done
mv RTGS-Summary output3/RTGS-Summary

mkdir output4
for i in `seq 0 99`;
do
    echo TestSet 4 -- set-"$i"
    ./../SRTG-Scheduler/bin/SRTG-Scheduler --j testData/testSet4/testSet4-"$i"-jobs.txt --rt testData/testSet4/testSet4-"$i"-releaseTimes.txt --m 99 --p 16 --d 12 >> output4/output.log
done
mv RTGS-Summary output4/RTGS-Summary

mkdir output5
for i in `seq 0 99`;
do
    echo TestSet 5 -- set-"$i"
    ./../SRTG-Scheduler/bin/SRTG-Scheduler --j testData/testSet5/testSet5-"$i"-jobs.txt --rt testData/testSet5/testSet5-"$i"-releaseTimes.txt --m 99 --p 32 --d 24 >> output5/output.log
done
mv RTGS-Summary output5/RTGS-Summary

mkdir output6
for i in `seq 0 99`;
do
    echo TestSet 6 -- set-"$i"
    ./../SRTG-Scheduler/bin/SRTG-Scheduler --j testData/testSet6/testSet6-"$i"-jobs.txt --rt testData/testSet6/testSet6-"$i"-releaseTimes.txt --m 99 --p 64 --d 48 >> output6/output.log
done
mv RTGS-Summary output6/RTGS-Summary

mkdir output7
for i in `seq 0 99`;
do
    echo TestSet 7 -- set-"$i"
    ./../SRTG-Scheduler/bin/SRTG-Scheduler --j testData/testSet7/testSet7-"$i"-jobs.txt --rt testData/testSet7/testSet7-"$i"-releaseTimes.txt --m 99 --p 128 --d 96 >> output7/output.log
done
mv RTGS-Summary output7/RTGS-Summary

