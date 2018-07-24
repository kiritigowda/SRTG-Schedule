mkdir output1
for i in `seq 0 99`;
do
    echo TestSet 1 -- set-"$i"
    ./../RTG-Scheduler/bin/RTG-scheduler --j testData/testSet1/testSet1-"$i"-jobs.txt --rt testData/testSet1/testSet1-"$i"-releaseTimes.txt --m 99 --p 32 --d 10 >> output1/output.log
done
mv RTGS-Summary output1/RTGS-Summary

mkdir output2
for i in `seq 0 99`;
do
    echo TestSet 2 -- set-"$i"
    ./../RTG-Scheduler/bin/RTG-scheduler --j testData/testSet2/testSet2-"$i"-jobs.txt --rt testData/testSet2/testSet2-"$i"-releaseTimes.txt --m 99 --p 32 --d 20 >> output2/output.log
done
mv RTGS-Summary output2/RTGS-Summary

mkdir output3
for i in `seq 0 99`;
do
    echo TestSet 3 -- set-"$i"
    ./../RTG-Scheduler/bin/RTG-scheduler --j testData/testSet3/testSet3-"$i"-jobs.txt --rt testData/testSet3/testSet3-"$i"-releaseTimes.txt --m 99 --p 32 --d 30 >> output3/output.log
done
mv RTGS-Summary output3/RTGS-Summary

mkdir output4
for i in `seq 0 99`;
do
    echo TestSet 4 -- set-"$i"
    ./../RTG-Scheduler/bin/RTG-scheduler --j testData/testSet4/testSet4-"$i"-jobs.txt --rt testData/testSet4/testSet4-"$i"-releaseTimes.txt --m 99 --p 32 --d 40 >> output4/output.log
done
mv RTGS-Summary output4/RTGS-Summary

mkdir output5
for i in `seq 0 99`;
do
    echo TestSet 5 -- set-"$i"
    ./../RTG-Scheduler/bin/RTG-scheduler --j testData/testSet5/testSet5-"$i"-jobs.txt --rt testData/testSet5/testSet5-"$i"-releaseTimes.txt --m 99 --p 32 --d 50 >> output5/output.log
done
mv RTGS-Summary output5/RTGS-Summary

mkdir output6
for i in `seq 0 99`;
do
    echo TestSet 6 -- set-"$i"
    ./../RTG-Scheduler/bin/RTG-scheduler --j testData/testSet6/testSet6-"$i"-jobs.txt --rt testData/testSet6/testSet6-"$i"-releaseTimes.txt --m 99 --p 32 --d 60 >> output6/output.log
done
mv RTGS-Summary output6/RTGS-Summary

mkdir output7
for i in `seq 0 99`;
do
    echo TestSet 7 -- set-"$i"
    ./../RTG-Scheduler/bin/RTG-scheduler --j testData/testSet7/testSet7-"$i"-jobs.txt --rt testData/testSet7/testSet7-"$i"-releaseTimes.txt --m 99 --p 32 --d 70 >> output7/output.log
done
mv RTGS-Summary output7/RTGS-Summary

mkdir output8
for i in `seq 0 99`;
do
    echo TestSet 8 -- set-"$i"
    ./../RTG-Scheduler/bin/RTG-scheduler --j testData/testSet8/testSet8-"$i"-jobs.txt --rt testData/testSet8/testSet8-"$i"-releaseTimes.txt --m 99 --p 32 --d 80 >> output8/output.log
done
mv RTGS-Summary output8/RTGS-Summary

mkdir output9
for i in `seq 0 99`;
do
    echo TestSet 9 -- set-"$i"
    ./../RTG-Scheduler/bin/RTG-scheduler --j testData/testSet9/testSet9-"$i"-jobs.txt --rt testData/testSet9/testSet9-"$i"-releaseTimes.txt --m 99 --p 32 --d 90 >> output9/output.log
done
mv RTGS-Summary output9/RTGS-Summary

mkdir output10
for i in `seq 0 99`;
do
    echo TestSet 10 -- set-"$i"
    ./../RTG-Scheduler/bin/RTG-scheduler --j testData/testSet10/testSet10-"$i"-jobs.txt --rt testData/testSet10/testSet10-"$i"-releaseTimes.txt --m 99 --p 32 --d 100 >> output10/output.log
done
mv RTGS-Summary output10/RTGS-Summary