export RGTS_DEBUG_MSG=4
# Error & Help Cases
./bin/RTG-scheduler
./bin/RTG-scheduler --h
./bin/RTG-scheduler --j
./bin/RTG-scheduler --r
./bin/RTG-scheduler --m
./bin/RTG-scheduler --p
./bin/RTG-scheduler --d
./bin/RTG-scheduler --v
./bin/RTG-scheduler --s
./bin/RTG-scheduler --g
./bin/RTG-scheduler --h example.txt
./bin/RTG-scheduler --j example.txt
./bin/RTG-scheduler --r example.txt
./bin/RTG-scheduler --m example.txt
./bin/RTG-scheduler --p 14
./bin/RTG-scheduler --d 75
./bin/RTG-scheduler --s 0
./bin/RTG-scheduler --s 1
./bin/RTG-scheduler --s 0 --g AMD
./bin/RTG-scheduler --s 1 --g NVIDIA
./bin/RTG-scheduler --g AMD
./bin/RTG-scheduler --g RADEON
./bin/RTG-scheduler --v example.txt
./bin/RTG-scheduler --j bin/testData/set1-jobs.txt
./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r
./bin/RTG-scheduler --j bin/testData/set1-job --r bin/testData/set1-jobReleaseTimes.txt
./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r bin/testData/set1-jobReleaseTimes
./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r bin/testData/set1-jobReleaseTimes.txt
./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r bin/testData/set1-jobReleaseTimes.txt
./bin/RTG-scheduler --r bin/testData/set1-jobs.txt --j bin/testData/set1-jobReleaseTimes.txt
./bin/RTG-scheduler --r bin/testData/set1-jobs.txt --j bin/testData/set1-jobs.txt
./bin/RTG-scheduler --r bin/testData/set1-jobReleaseTimes.txt --j bin/testData/set1-jobReleaseTimes.txt
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set12-jobs.txt --r bin/testData/set12-jobReleaseTimes.txt --m 99
#Simulation VS Hardaware
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/aperiodic-set-0-syntheticJobs.csv --r bin/testData/aperiodic-set-0-syntheticJobReleaseTimes.csv --s 0
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r bin/testData/set1-jobReleaseTimes.txt --s 1
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r bin/testData/set1-jobReleaseTimes.txt --g AMD
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r bin/testData/set1-jobReleaseTimes.txt --g RADEON
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r bin/testData/set1-jobReleaseTimes.txt --g AMD --s 1
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r bin/testData/set1-jobReleaseTimes.txt --g AMD --s 0
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r bin/testData/set1-jobReleaseTimes.txt --g NVIDIA --s 0
#Aperiodic Set
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/aperiodic-set-0-syntheticJobs.csv --r bin/testData/aperiodic-set-0-syntheticJobReleaseTimes.csv
./bin/RTG-scheduler --j bin/testData/aperiodic-set-0-syntheticJobs.csv --r bin/testData/aperiodic-set-0-syntheticJobReleaseTimes.csv --m
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/aperiodic-set-0-syntheticJobs.csv --r bin/testData/aperiodic-set-0-syntheticJobReleaseTimes.csv --m 1
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/aperiodic-set-0-syntheticJobs.csv --r bin/testData/aperiodic-set-0-syntheticJobReleaseTimes.csv --m 2
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/aperiodic-set-0-syntheticJobs.csv --r bin/testData/aperiodic-set-0-syntheticJobReleaseTimes.csv --m 3
RTGS_DEBUG_MSG=3 ./bin/RTG-scheduler --j bin/testData/aperiodic-set-0-syntheticJobs.csv --r bin/testData/aperiodic-set-0-syntheticJobReleaseTimes.csv --m 4
./bin/RTG-scheduler --j bin/testData/aperiodic-set-0-syntheticJobs.csv --r bin/testData/aperiodic-set-0-syntheticJobReleaseTimes.csv --m 5
./bin/RTG-scheduler --j bin/testData/aperiodic-set-0-syntheticJobs.csv --r bin/testData/aperiodic-set-0-syntheticJobReleaseTimes.csv --m 99
./bin/RTG-scheduler --j bin/testData/aperiodic-set-0-syntheticJobs.csv --r bin/testData/aperiodic-set-0-syntheticJobReleaseTimes.csv --m 99 --d
./bin/RTG-scheduler --j bin/testData/aperiodic-set-0-syntheticJobs.csv --r bin/testData/aperiodic-set-0-syntheticJobReleaseTimes.csv --m 99 --d 80
./bin/RTG-scheduler --j bin/testData/aperiodic-set-0-syntheticJobs.csv --r bin/testData/aperiodic-set-0-syntheticJobReleaseTimes.csv --m 99 --p 
./bin/RTG-scheduler --j bin/testData/aperiodic-set-0-syntheticJobs.csv --r bin/testData/aperiodic-set-0-syntheticJobReleaseTimes.csv --m 99 --p 24
#Job Set 1
RTGS_DEBUG_MSG=2 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r bin/testData/set1-jobReleaseTimes.txt --m
RTGS_DEBUG_MSG=2 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r bin/testData/set1-jobReleaseTimes.txt --m 1
RTGS_DEBUG_MSG=2 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r bin/testData/set1-jobReleaseTimes.txt --m 2
RTGS_DEBUG_MSG=2 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r bin/testData/set1-jobReleaseTimes.txt --m 3
RTGS_DEBUG_MSG=2 ./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r bin/testData/set1-jobReleaseTimes.txt --m 4
./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r bin/testData/set1-jobReleaseTimes.txt --m 5
./bin/RTG-scheduler --j bin/testData/set1-jobs.txt --r bin/testData/set1-jobReleaseTimes.txt --m 99
#Job Set 2
./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --r bin/testData/set2-jobReleaseTimes.txt --m 99
./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --r bin/testData/set3-jobReleaseTimes.txt --m 99
./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --r bin/testData/set4-jobReleaseTimes.txt --m 99
./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --r bin/testData/set5-jobReleaseTimes.txt --m 99
./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --r bin/testData/set6-jobReleaseTimes.txt --m 99
./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --r bin/testData/set7-jobReleaseTimes.txt --m 99
./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --r bin/testData/set8-jobReleaseTimes.txt --m 99
./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --r bin/testData/set9-jobReleaseTimes.txt --m 99
./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --r bin/testData/set10-jobReleaseTimes.txt --m 99
./bin/RTG-scheduler --j bin/testData/set2-jobs.txt --r bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 3
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --r bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --r bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --r bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --r bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --r bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --r bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --r bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --r bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --r bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set3-jobs.txt --r bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 4
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --r bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --r bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --r bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --r bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --r bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --r bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --r bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --r bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --r bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set4-jobs.txt --r bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 5
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --r bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --r bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --r bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --r bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --r bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --r bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --r bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --r bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --r bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set5-jobs.txt --r bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 6
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --r bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --r bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --r bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --r bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --r bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --r bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --r bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --r bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --r bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set6-jobs.txt --r bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 7
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --r bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --r bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --r bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --r bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --r bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --r bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --r bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --r bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --r bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set7-jobs.txt --r bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 8
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --r bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --r bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --r bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --r bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --r bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --r bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --r bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --r bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --r bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set8-jobs.txt --r bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 9
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --r bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --r bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --r bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --r bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --r bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --r bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --r bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --r bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --r bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set9-jobs.txt --r bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 10
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --r bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --r bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --r bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --r bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --r bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --r bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --r bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --r bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --r bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set10-jobs.txt --r bin/testData/set11-jobReleaseTimes.txt --m 99
#Job Set 11
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set2-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set3-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set4-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set5-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set6-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set7-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set8-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set9-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set10-jobReleaseTimes.txt --m 99
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set11-jobReleaseTimes.txt --m 99
# Option Test
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set2-jobReleaseTimes.txt --m 99 --p 16 --d 40
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set3-jobReleaseTimes.txt --m 99 --p 14 --d 50
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set4-jobReleaseTimes.txt --m 99 --p 32 --d 75
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set5-jobReleaseTimes.txt --m 99 --p 14 --d 75
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set6-jobReleaseTimes.txt --m 99 --p 64 --d 30
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set7-jobReleaseTimes.txt --m 99 --p 64 --d 60
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set8-jobReleaseTimes.txt --m 99 --p 64 --d 90
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set9-jobReleaseTimes.txt --m 99 --p 16 --d 45
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set10-jobReleaseTimes.txt --m 99 --p 16 --d 90
RTGS_DEBUG_MSG=0 ./bin/RTG-scheduler --j bin/testData/set11-jobs.txt --r bin/testData/set11-jobReleaseTimes.txt --m 99 --p 18 --d 75
