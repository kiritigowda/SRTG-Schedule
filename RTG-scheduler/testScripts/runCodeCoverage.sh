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
./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt bin/testData/set1-jobReleaseTimes.txt --m
./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt bin/testData/set1-jobReleaseTimes.txt --m 1
./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt bin/testData/set1-jobReleaseTimes.txt --m 2
./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt bin/testData/set1-jobReleaseTimes.txt --m 3
./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt bin/testData/set1-jobReleaseTimes.txt --m 4
RGTS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt bin/testData/set1-jobReleaseTimes.txt --m 5
RGTS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --rt bin/testData/set1-jobReleaseTimes.txt --m 99
#Job Set 2
./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --rt bin/testData/set2-jobReleaseTimes.txt --m 1
./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --rt bin/testData/set2-jobReleaseTimes.txt --m 2
./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --rt bin/testData/set2-jobReleaseTimes.txt --m 3
./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --rt bin/testData/set2-jobReleaseTimes.txt --m 4
RGTS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --rt bin/testData/set2-jobReleaseTimes.txt --m 5
RGTS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --rt bin/testData/set2-jobReleaseTimes.txt --m 99
#Job Set 3
./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --rt bin/testData/set3-jobReleaseTimes.txt --m 1
./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --rt bin/testData/set3-jobReleaseTimes.txt --m 2
./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --rt bin/testData/set3-jobReleaseTimes.txt --m 3
./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --rt bin/testData/set3-jobReleaseTimes.txt --m 4
RGTS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --rt bin/testData/set3-jobReleaseTimes.txt --m 5
RGTS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --rt bin/testData/set3-jobReleaseTimes.txt --m 99
#Job Set 4
./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 1
./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 2
./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 3
./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 4
RGTS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 5
RGTS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 99
#Job Set 5
./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 1
./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 2
./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 3
./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 4
RGTS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 5
RGTS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set4-jobReleaseTimes.txt --m 99
#Job Set 5
./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set5-jobReleaseTimes.txt --m 1
./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set5-jobReleaseTimes.txt --m 2
./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set5-jobReleaseTimes.txt --m 3
./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set5-jobReleaseTimes.txt --m 4
RGTS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set5-jobReleaseTimes.txt --m 5
RGTS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --rt bin/testData/set5-jobReleaseTimes.txt --m 99