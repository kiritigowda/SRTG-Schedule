# Inputs
# $1 -- Scheduler Full Path - Required
# $2 -- Jobs Info Pre-fix - Required
# $3 -- Method - Required
# $4 -- output directory - Required

for i in {0..99};
do 
	echo set-"$i";
	$1 --j $2/$2-"$i"-syntheticJobs.csv --rt $2/$2-"$i"-syntheticJobReleaseTimes.csv --m 99 --method $3 >> output.log
done
mkdir -p $4
mv RTGS-Summary/* $4/
rm -rf RTGS-Summary/
mv output.log $4/output-method-$3.log