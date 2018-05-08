export RGTS_DEBUG_MSG=4
./bin/RTG-scheduler
./bin/RTG-scheduler --h
./bin/RTG-scheduler --j
./bin/RTG-scheduler --rt
./bin/RTG-scheduler --m
./bin/RTG-scheduler --v
./bin/RTG-scheduler --h example.txt
./bin/RTG-scheduler --j example.txt
./bin/RTG-scheduler --rt example.txt
./bin/RTG-scheduler --m example.txt
./bin/RTG-scheduler --v example.txt
./bin/RTG-scheduler --j bin/testData/set1-jobs.txt
./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt
./bin/RTG-scheduler --j bin/testData/set1-job --rt bin/testData/set1-jobReleaseTimes.txt
./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt bin/testData/set1-jobReleaseTimes
./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt bin/testData/set1-jobReleaseTimes.txt
#Job Set 1
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt bin/testData/set1-jobReleaseTimes.txt --m
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt bin/testData/set1-jobReleaseTimes.txt --m 1
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt bin/testData/set1-jobReleaseTimes.txt --m 2
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt bin/testData/set1-jobReleaseTimes.txt --m 3
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt bin/testData/set1-jobReleaseTimes.txt --m 4
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt bin/testData/set1-jobReleaseTimes.txt --m 5
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt bin/testData/set1-jobReleaseTimes.txt --m 99
#Job Set 2
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --rt bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --rt bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --rt bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --rt bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --rt bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --rt bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --rt bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --rt bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --rt bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 3
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --rt bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --rt bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --rt bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --rt bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --rt bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --rt bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --rt bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --rt bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --rt bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 4
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --rt bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --rt bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --rt bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --rt bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --rt bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --rt bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --rt bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --rt bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --rt bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 5
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 6
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --rt bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --rt bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --rt bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --rt bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --rt bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --rt bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --rt bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --rt bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --rt bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 7
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --rt bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --rt bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --rt bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --rt bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --rt bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --rt bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --rt bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --rt bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --rt bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 8
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --rt bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --rt bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --rt bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --rt bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --rt bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --rt bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --rt bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --rt bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --rt bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 9
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --rt bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --rt bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --rt bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --rt bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --rt bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --rt bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --rt bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --rt bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --rt bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 10
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --rt bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --rt bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --rt bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --rt bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --rt bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --rt bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --rt bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --rt bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --rt bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 11
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --rt bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --rt bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --rt bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --rt bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --rt bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --rt bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --rt bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --rt bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --rt bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 12
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set12-jobs.txt --rt bin/testData/set12-jobReleaseTimes.txt
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set12-jobs.txt --rt bin/testData/set12-jobReleaseTimes.txt --m 1
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set12-jobs.txt --rt bin/testData/set12-jobReleaseTimes.txt --m 2
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set12-jobs.txt --rt bin/testData/set12-jobReleaseTimes.txt --m 3
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set12-jobs.txt --rt bin/testData/set12-jobReleaseTimes.txt --m 4
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set12-jobs.txt --rt bin/testData/set12-jobReleaseTimes.txt --m 5
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set12-jobs.txt --rt bin/testData/set12-jobReleaseTimes.txt --m 99