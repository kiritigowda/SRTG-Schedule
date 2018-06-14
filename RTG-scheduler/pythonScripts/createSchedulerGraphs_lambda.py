import os
import getopt
import sys
import random
import collections
import csv

opts, args = getopt.getopt(sys.argv[1:], 'i:o:f:')

inputDirectory = '';
outputDirectory = '';
fileName = '';

for opt, arg in opts:
    if opt == '-i':
        inputDirectory = arg;
    elif opt == '-o':
        outputDirectory = arg;
    elif opt == '-f':
        fileName = arg;


if inputDirectory == '' or outputDirectory == '' or fileName == '':
    print('Invalid command line arguments.\n'
        '\t\t\t\t-i [input Directory - required]\n'\
        '\t\t\t\t-o [output Directory - required]\n'\
        '\t\t\t\t-f [output file name Directory - required]\n')
    exit();

if not os.path.exists(inputDirectory):
    print "ERROR Invalid Input Directory";
    exit();

if not os.path.exists(outputDirectory):
    os.makedirs(outputDirectory);

row_count = 0;
row_count_1 = 0;
row_count_2 = 0;
row_count_3 = 0;
row_count_4 = 0;
row_count_5 = 0;
with open(inputDirectory+'RTGS-Mode-1-Summary.csv') as mode1:
    reader_1 = csv.reader(mode1)
    data_1 = [r for r in reader_1]
    row_count_1 = len(data_1)

with open(inputDirectory+'RTGS-Mode-2-Summary.csv') as mode2:
    reader_2 = csv.reader(mode2)
    data_2 = [r for r in reader_2]
    row_count_2 = len(data_2)

with open(inputDirectory+'RTGS-Mode-3-Summary.csv') as mode3:
    reader_3 = csv.reader(mode3)
    data_3 = [r for r in reader_3]
    row_count_3 = len(data_3)

with open(inputDirectory+'RTGS-Mode-4-Summary.csv') as mode4:
    reader_4 = csv.reader(mode4)
    data_4 = [r for r in reader_4]
    row_count_4 = len(data_4)

with open(inputDirectory+'RTGS-Mode-5-Summary.csv') as mode5:
    reader_5 = csv.reader(mode5)
    data_5 = [r for r in reader_5]
    row_count_5 = len(data_5)

if row_count_1 != row_count_2 or row_count_2 != row_count_3 or row_count_3 != row_count_4 or row_count_4 != row_count_5:
    print "ERROR: Number of entries in Summary File are different";
    exit();
else:
    row_count = row_count_1;

orig_stdout = sys.stdout
sys.stdout = open(outputDirectory+'/'+fileName+'-SchedulerResults.html','w')
print"<html>"
print"\t<head>"
print"\t\t<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>"
print"\n"
print"\t\t<script type=\"text/javascript\">"
print"\n"
print"\t\t\tgoogle.charts.load('current', {packages: ['corechart', 'line']});"
print"\t\t\tgoogle.charts.setOnLoadCallback(jobScheduledGraph);"
print"\t\t\tfunction jobScheduledGraph() {"
print"\t\t\tvar data = new google.visualization.DataTable();"
print"\t\t\tdata.addColumn('number', 'X');"
print"\t\t\tdata.addColumn('number', 'Mode 1');"
print"\t\t\tdata.addColumn('number', 'Mode 2');"
print"\t\t\tdata.addColumn('number', 'Mode 3');"
print"\t\t\tdata.addColumn('number', 'Mode 4');"
print"\t\t\tdata.addColumn('number', 'Mode 5');"
print"\t\t\tdata.addRows(["
for x in range(row_count):
    if(x < row_count-1):
        print '\t\t\t\t['+str(x)+','+str(data_1[x][0])+','+str(data_2[x][0])+','+str(data_3[x][0])+','+str(data_4[x][0])+','+str(data_5[x][0])+'],'
    else:
        print '\t\t\t\t['+str(x)+','+str(data_1[x][0])+','+str(data_2[x][0])+','+str(data_3[x][0])+','+str(data_4[x][0])+','+str(data_5[x][0])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'Average Jobs Scheduled', hAxis: { title: 'JobSet ID'}, vAxis: {title: 'Number of Jobs'}, series: { 0.01: {curveType: 'function'} }, width:1600, height:1000 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('jobScheduled_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n\n\n"
print"\t\t\tgoogle.charts.load('current', {packages: ['corechart', 'line']});"
print"\t\t\tgoogle.charts.setOnLoadCallback(GPUUsageGraph);"
print"\t\t\tfunction GPUUsageGraph() {"
print"\t\t\tvar data = new google.visualization.DataTable();"
print"\t\t\tdata.addColumn('number', 'X');"
print"\t\t\tdata.addColumn('number', 'Mode 1');"
print"\t\t\tdata.addColumn('number', 'Mode 2');"
print"\t\t\tdata.addColumn('number', 'Mode 3');"
print"\t\t\tdata.addColumn('number', 'Mode 4');"
print"\t\t\tdata.addColumn('number', 'Mode 5');"
print"\t\t\tdata.addRows(["
for x in range(row_count):
    if(x < row_count-1):
        print '\t\t\t\t['+str(x)+','+str(data_1[x][4])+','+str(data_2[x][4])+','+str(data_3[x][4])+','+str(data_4[x][4])+','+str(data_5[x][4])+'],'
    else:
        print '\t\t\t\t['+str(x)+','+str(data_1[x][4])+','+str(data_2[x][4])+','+str(data_3[x][4])+','+str(data_4[x][4])+','+str(data_5[x][4])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'Total GPU Usage', hAxis: { title: 'JobSet ID'}, vAxis: {title: 'Total GPU Usage'}, series: { 0.01: {curveType: 'function'} }, width:1600, height:1000 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('GPUUsage_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n\n\n"
print"\t\t\tgoogle.charts.load('current', {packages: ['corechart', 'line']});"
print"\t\t\tgoogle.charts.setOnLoadCallback(avgResponseTimeGraph);"
print"\t\t\tfunction avgResponseTimeGraph() {"
print"\t\t\tvar data = new google.visualization.DataTable();"
print"\t\t\tdata.addColumn('number', 'X');"
print"\t\t\tdata.addColumn('number', 'Mode 1');"
print"\t\t\tdata.addColumn('number', 'Mode 2');"
print"\t\t\tdata.addColumn('number', 'Mode 3');"
print"\t\t\tdata.addColumn('number', 'Mode 4');"
print"\t\t\tdata.addColumn('number', 'Mode 5');"
print"\t\t\tdata.addRows(["
for x in range(row_count):
    if(x < row_count-1):
        print '\t\t\t\t['+str(x)+','+str(data_1[x][5])+','+str(data_2[x][5])+','+str(data_3[x][5])+','+str(data_4[x][5])+','+str(data_5[x][5])+'],'
    else:
        print '\t\t\t\t['+str(x)+','+str(data_1[x][5])+','+str(data_2[x][5])+','+str(data_3[x][5])+','+str(data_4[x][5])+','+str(data_5[x][5])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'Average Response Time', hAxis: { title: 'JobSet ID'}, vAxis: {title: 'Response Time'}, series: { 0.01: {curveType: 'function'} }, width:1600, height:1000 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('responseTime_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n\n\n"
print"\t\t\tgoogle.charts.load('current', {packages: ['corechart', 'line']});"
print"\t\t\tgoogle.charts.setOnLoadCallback(avgResponseFactorGraph);"
print"\t\t\tfunction avgResponseFactorGraph() {"
print"\t\t\tvar data = new google.visualization.DataTable();"
print"\t\t\tdata.addColumn('number', 'X');"
print"\t\t\tdata.addColumn('number', 'Mode 1');"
print"\t\t\tdata.addColumn('number', 'Mode 2');"
print"\t\t\tdata.addColumn('number', 'Mode 3');"
print"\t\t\tdata.addColumn('number', 'Mode 4');"
print"\t\t\tdata.addColumn('number', 'Mode 5');"
print"\t\t\tdata.addRows(["
for x in range(row_count):
    if(x < row_count-1):
        print '\t\t\t\t['+str(x)+','+str(data_1[x][6])+','+str(data_2[x][6])+','+str(data_3[x][6])+','+str(data_4[x][6])+','+str(data_5[x][6])+'],'
    else:
        print '\t\t\t\t['+str(x)+','+str(data_1[x][6])+','+str(data_2[x][6])+','+str(data_3[x][6])+','+str(data_4[x][6])+','+str(data_5[x][6])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'Average Response Factor', hAxis: { title: 'JobSet ID'}, vAxis: {title: 'Response Factor'}, series: { 0.01: {curveType: 'function'} }, width:1600, height:1000 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('responseFactor_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n\n\n"
print"\t\t\tgoogle.charts.load('current', {packages: ['corechart', 'line']});"
print"\t\t\tgoogle.charts.setOnLoadCallback(GPUUsagePercentageGraph);"
print"\t\t\tfunction GPUUsagePercentageGraph() {"
print"\t\t\tvar data = new google.visualization.DataTable();"
print"\t\t\tdata.addColumn('number', 'X');"
print"\t\t\tdata.addColumn('number', 'Mode 1');"
print"\t\t\tdata.addColumn('number', 'Mode 2');"
print"\t\t\tdata.addColumn('number', 'Mode 3');"
print"\t\t\tdata.addColumn('number', 'Mode 4');"
print"\t\t\tdata.addColumn('number', 'Mode 5');"
print"\t\t\tdata.addRows(["
for x in range(row_count):
    if(x < row_count-1):
        print '\t\t\t\t['+str(x)+','+str(data_1[x][7])+','+str(data_2[x][7])+','+str(data_3[x][7])+','+str(data_4[x][7])+','+str(data_5[x][7])+'],'
    else:
        print '\t\t\t\t['+str(x)+','+str(data_1[x][7])+','+str(data_2[x][7])+','+str(data_3[x][7])+','+str(data_4[x][7])+','+str(data_5[x][7])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'GPU Usage Percentage', hAxis: { title: 'JobSet ID'}, vAxis: {title: 'GPU Usage Percentage'}, series: { 0.01: {curveType: 'function'} }, width:1600, height:1000 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('GPUUsagePercentage_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n\n\n"
print"\t\t\tgoogle.charts.load('current', {packages: ['corechart', 'line']});"
print"\t\t\tgoogle.charts.setOnLoadCallback(jobScheduledPercentageGraph);"
print"\t\t\tfunction jobScheduledPercentageGraph() {"
print"\t\t\tvar data = new google.visualization.DataTable();"
print"\t\t\tdata.addColumn('number', 'X');"
print"\t\t\tdata.addColumn('number', 'Mode 1');"
print"\t\t\tdata.addColumn('number', 'Mode 2');"
print"\t\t\tdata.addColumn('number', 'Mode 3');"
print"\t\t\tdata.addColumn('number', 'Mode 4');"
print"\t\t\tdata.addColumn('number', 'Mode 5');"
print"\t\t\tdata.addRows(["
for x in range(row_count):
    if(x < row_count-1):
        print '\t\t\t\t['+str(x)+','+str(data_1[x][8])+','+str(data_2[x][8])+','+str(data_3[x][8])+','+str(data_4[x][7])+','+str(data_5[x][8])+'],'
    else:
        print '\t\t\t\t['+str(x)+','+str(data_1[x][8])+','+str(data_2[x][8])+','+str(data_3[x][8])+','+str(data_4[x][7])+','+str(data_5[x][8])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'Job Scheduled Percentage', hAxis: { title: 'JobSet ID'}, vAxis: {title: 'Job Scheduled Percentage'}, series: { 0.01: {curveType: 'function'} }, width:1600, height:1000 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('JobScheduledPercentage_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n"
print"\t\t</script>"
print"\t</head>"
print"\t<body>"
print'\t\t<br><br><h1><center> RTG Schedule Summary</center></h2><br>'
print"\t\t<table align=\"center\" style=\"width: 95%\">"
print"\t\t\t<tr>"
print"\t\t\t\t<td><center></center></td>"
print"\t\t\t\t<td><center>Average Jobs</center></td>"
print"\t\t\t\t<td><center>Jobs Scheduled</center></td>"
print"\t\t\t\t<td><center>Avg Processors/Job</center></td>"
print"\t\t\t\t<td><center>Avg Execution Time/Job</center></td>"
print"\t\t\t\t<td><center>Total GPU Usage</center></td>"
print"\t\t\t\t<td><center>Avg Response Time/Job</center></td>"
print"\t\t\t\t<td><center>Avg Response Factor/Job</center></td>"
print"\t\t\t\t<td><center>GPU usage Percentage</center></td>"
print"\t\t\t\t<td><center>Job Scheduled Percentage</center></td>"
print"\t\t\t\t<td><center>Avg GPU Schedule Overhead/Job</center></td>"
print"\t\t\t\t<td><center>Avg Scheduler Overhead/Job</center></td>"
print"\t\t\t</tr>"
# Mode 1
avgJobsScheduled = 0;
avgJobs = 0;
avgProc = 0;
avgExec = 0;
totalGPUUsage = 0;
avgResponseTime = 0;
avgResponseFactor = 0;
GPUUsagePercentage = 0;
avgJobPercentage = 0;
GPUScheduleOverhead = 0;
AvgSchedulerOverhead = 0;
avgReleaseLambda = 0;

for x in range(row_count):
    avgJobsScheduled = avgJobsScheduled + float(data_1[x][0]);
    avgJobs = avgJobs + int(data_1[x][1]);
    avgProc = avgProc + float(data_1[x][2]);
    avgExec = avgExec + float(data_1[x][3]);
    totalGPUUsage = totalGPUUsage + float(data_1[x][4]);
    avgResponseTime = avgResponseTime + float(data_1[x][5]);
    avgResponseFactor = avgResponseFactor + float(data_1[x][6]);
    GPUUsagePercentage = GPUUsagePercentage + float(data_1[x][7]);
    avgJobPercentage = avgJobPercentage + float(data_1[x][8]);
    avgReleaseLambda = avgReleaseLambda + float(data_5[x][9]);
    GPUScheduleOverhead = GPUScheduleOverhead + float(data_1[x][10]);
    AvgSchedulerOverhead = AvgSchedulerOverhead + float(data_1[x][11]);

avgJobsScheduled = float(avgJobsScheduled)/row_count;
avgJobs = float(avgJobs)/row_count;
avgProc = float(avgProc)/row_count;
avgExec = float(avgExec)/row_count;
totalGPUUsage = float(totalGPUUsage)/row_count;
avgResponseTime = float(avgResponseTime)/row_count;
avgResponseFactor = float(avgResponseFactor)/row_count;
GPUUsagePercentage = float(GPUUsagePercentage)/row_count;
avgJobPercentage = float(avgJobPercentage)/row_count;
avgReleaseLambda = float(avgReleaseLambda)/row_count;
GPUScheduleOverhead = float(GPUScheduleOverhead)/row_count;
AvgSchedulerOverhead = float(AvgSchedulerOverhead)/row_count;

sys.stdout = open(outputDirectory+'/Mode1-SchedulerResults.csv','a')
print str(avgReleaseLambda)+','+str(avgJobs)+','+str(avgJobsScheduled)+str(avgProc)+','+str(avgExec)+','+str(avgProc)+','+str(totalGPUUsage)+','\
+str(avgResponseTime)+','+str(avgResponseFactor)+','+str(GPUUsagePercentage)+','+str(avgJobPercentage)+','\
+str(GPUScheduleOverhead)+','+str(AvgSchedulerOverhead)+','+str(avgJobPercentage)
sys.stdout = open(outputDirectory+'/'+fileName+'-SchedulerResults.html','a')

print"\t\t\t<tr>"
print"\t\t\t\t<td><center>Mode 1</center></td>"
print'\t\t\t\t<td><center>'+str(avgJobs)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobsScheduled)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgProc)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgExec)+'</center></td>'
print'\t\t\t\t<td><center>'+str(totalGPUUsage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseTime)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseFactor)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUUsagePercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobPercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUScheduleOverhead)+'</center></td>'
print'\t\t\t\t<td><center>'+str(AvgSchedulerOverhead)+'</center></td>'
print"\t\t\t</tr>"
# Mode 2
avgJobsScheduled = 0;
avgJobs = 0;
avgProc = 0;
avgExec = 0;
totalGPUUsage = 0;
avgResponseTime = 0;
avgResponseFactor = 0;
GPUUsagePercentage = 0;
avgJobPercentage = 0;
GPUScheduleOverhead = 0;
AvgSchedulerOverhead = 0;

for x in range(row_count):
    avgJobsScheduled = avgJobsScheduled + float(data_2[x][0]);
    avgJobs = avgJobs + int(data_2[x][1]);
    avgProc = avgProc + float(data_2[x][2]);
    avgExec = avgExec + float(data_2[x][3]);
    totalGPUUsage = totalGPUUsage + float(data_2[x][4]);
    avgResponseTime = avgResponseTime + float(data_2[x][5]);
    avgResponseFactor = avgResponseFactor + float(data_2[x][6]);
    GPUUsagePercentage = GPUUsagePercentage + float(data_2[x][7]);
    avgJobPercentage = avgJobPercentage + float(data_2[x][8]);
    GPUScheduleOverhead = GPUScheduleOverhead + float(data_2[x][10]);
    AvgSchedulerOverhead = AvgSchedulerOverhead + float(data_2[x][11]);

avgJobsScheduled = float(avgJobsScheduled)/row_count;
avgJobs = float(avgJobs)/row_count;
avgProc = float(avgProc)/row_count;
avgExec = float(avgExec)/row_count;
totalGPUUsage = float(totalGPUUsage)/row_count;
avgResponseTime = float(avgResponseTime)/row_count;
avgResponseFactor = float(avgResponseFactor)/row_count;
GPUUsagePercentage = float(GPUUsagePercentage)/row_count;
avgJobPercentage = float(avgJobPercentage)/row_count;
GPUScheduleOverhead = float(GPUScheduleOverhead)/row_count;
AvgSchedulerOverhead = float(AvgSchedulerOverhead)/row_count;

sys.stdout = open(outputDirectory+'/Mode2-SchedulerResults.csv','a')
print str(avgReleaseLambda)+','+str(avgJobs)+','+str(avgJobsScheduled)+str(avgProc)+','+str(avgExec)+','+str(avgProc)+','+str(totalGPUUsage)+','\
+str(avgResponseTime)+','+str(avgResponseFactor)+','+str(GPUUsagePercentage)+','+str(avgJobPercentage)+','\
+str(GPUScheduleOverhead)+','+str(AvgSchedulerOverhead)+','+str(avgJobPercentage)
sys.stdout = open(outputDirectory+'/'+fileName+'-SchedulerResults.html','a')

print"\t\t\t<tr>"
print"\t\t\t\t<td><center>Mode 2</center></td>"
print'\t\t\t\t<td><center>'+str(avgJobs)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobsScheduled)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgProc)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgExec)+'</center></td>'
print'\t\t\t\t<td><center>'+str(totalGPUUsage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseTime)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseFactor)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUUsagePercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobPercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUScheduleOverhead)+'</center></td>'
print'\t\t\t\t<td><center>'+str(AvgSchedulerOverhead)+'</center></td>'
print"\t\t\t</tr>"
# Mode 3
avgJobsScheduled = 0;
avgJobs = 0;
avgProc = 0;
avgExec = 0;
totalGPUUsage = 0;
avgResponseTime = 0;
avgResponseFactor = 0;
GPUUsagePercentage = 0;
avgJobPercentage = 0;
GPUScheduleOverhead = 0;
AvgSchedulerOverhead = 0;

for x in range(row_count):
    avgJobsScheduled = avgJobsScheduled + float(data_3[x][0]);
    avgJobs = avgJobs + int(data_3[x][1]);
    avgProc = avgProc + float(data_3[x][2]);
    avgExec = avgExec + float(data_3[x][3]);
    totalGPUUsage = totalGPUUsage + float(data_3[x][4]);
    avgResponseTime = avgResponseTime + float(data_3[x][5]);
    avgResponseFactor = avgResponseFactor + float(data_3[x][6]);
    GPUUsagePercentage = GPUUsagePercentage + float(data_3[x][7]);
    avgJobPercentage = avgJobPercentage + float(data_3[x][8]);
    GPUScheduleOverhead = GPUScheduleOverhead + float(data_3[x][10]);
    AvgSchedulerOverhead = AvgSchedulerOverhead + float(data_3[x][11]);

avgJobsScheduled = float(avgJobsScheduled)/row_count;
avgJobs = float(avgJobs)/row_count;
avgProc = float(avgProc)/row_count;
avgExec = float(avgExec)/row_count;
totalGPUUsage = float(totalGPUUsage)/row_count;
avgResponseTime = float(avgResponseTime)/row_count;
avgResponseFactor = float(avgResponseFactor)/row_count;
GPUUsagePercentage = float(GPUUsagePercentage)/row_count;
avgJobPercentage = float(avgJobPercentage)/row_count;
GPUScheduleOverhead = float(GPUScheduleOverhead)/row_count;
AvgSchedulerOverhead = float(AvgSchedulerOverhead)/row_count;

sys.stdout = open(outputDirectory+'/Mode3-SchedulerResults.csv','a')
print str(avgReleaseLambda)+','+str(avgJobs)+','+str(avgJobsScheduled)+str(avgProc)+','+str(avgExec)+','+str(avgProc)+','+str(totalGPUUsage)+','\
+str(avgResponseTime)+','+str(avgResponseFactor)+','+str(GPUUsagePercentage)+','+str(avgJobPercentage)+','\
+str(GPUScheduleOverhead)+','+str(AvgSchedulerOverhead)+','+str(avgJobPercentage)
sys.stdout = open(outputDirectory+'/'+fileName+'-SchedulerResults.html','a')

print"\t\t\t<tr>"
print"\t\t\t\t<td><center>Mode 3</center></td>"
print'\t\t\t\t<td><center>'+str(avgJobs)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobsScheduled)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgProc)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgExec)+'</center></td>'
print'\t\t\t\t<td><center>'+str(totalGPUUsage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseTime)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseFactor)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUUsagePercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobPercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUScheduleOverhead)+'</center></td>'
print'\t\t\t\t<td><center>'+str(AvgSchedulerOverhead)+'</center></td>'
print"\t\t\t</tr>"
# Mode 4
avgJobsScheduled = 0;
avgJobs = 0;
avgProc = 0;
avgExec = 0;
totalGPUUsage = 0;
avgResponseTime = 0;
avgResponseFactor = 0;
GPUUsagePercentage = 0;
avgJobPercentage = 0;
GPUScheduleOverhead = 0;
AvgSchedulerOverhead = 0;

for x in range(row_count):
    avgJobsScheduled = avgJobsScheduled + float(data_4[x][0]);
    avgJobs = avgJobs + int(data_4[x][1]);
    avgProc = avgProc + float(data_4[x][2]);
    avgExec = avgExec + float(data_4[x][3]);
    totalGPUUsage = totalGPUUsage + float(data_4[x][4]);
    avgResponseTime = avgResponseTime + float(data_4[x][5]);
    avgResponseFactor = avgResponseFactor + float(data_4[x][6]);
    GPUUsagePercentage = GPUUsagePercentage + float(data_4[x][7]);
    avgJobPercentage = avgJobPercentage + float(data_4[x][8]);
    GPUScheduleOverhead = GPUScheduleOverhead + float(data_4[x][10]);
    AvgSchedulerOverhead = AvgSchedulerOverhead + float(data_4[x][11]);

avgJobsScheduled = float(avgJobsScheduled)/row_count;
avgJobs = float(avgJobs)/row_count;
avgProc = float(avgProc)/row_count;
avgExec = float(avgExec)/row_count;
totalGPUUsage = float(totalGPUUsage)/row_count;
avgResponseTime = float(avgResponseTime)/row_count;
avgResponseFactor = float(avgResponseFactor)/row_count;
GPUUsagePercentage = float(GPUUsagePercentage)/row_count;
avgJobPercentage = float(avgJobPercentage)/row_count;
GPUScheduleOverhead = float(GPUScheduleOverhead)/row_count;
AvgSchedulerOverhead = float(AvgSchedulerOverhead)/row_count;

sys.stdout = open(outputDirectory+'/Mode4-SchedulerResults.csv','a')
print str(avgReleaseLambda)+','+str(avgJobs)+','+str(avgJobsScheduled)+str(avgProc)+','+str(avgExec)+','+str(avgProc)+','+str(totalGPUUsage)+','\
+str(avgResponseTime)+','+str(avgResponseFactor)+','+str(GPUUsagePercentage)+','+str(avgJobPercentage)+','\
+str(GPUScheduleOverhead)+','+str(AvgSchedulerOverhead)+','+str(avgJobPercentage)
sys.stdout = open(outputDirectory+'/'+fileName+'-SchedulerResults.html','a')

print"\t\t\t<tr>"
print"\t\t\t\t<td><center>Mode 4</center></td>"
print'\t\t\t\t<td><center>'+str(avgJobs)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobsScheduled)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgProc)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgExec)+'</center></td>'
print'\t\t\t\t<td><center>'+str(totalGPUUsage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseTime)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseFactor)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUUsagePercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobPercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUScheduleOverhead)+'</center></td>'
print'\t\t\t\t<td><center>'+str(AvgSchedulerOverhead)+'</center></td>'
print"\t\t\t</tr>"
# Mode 5
avgJobsScheduled = 0;
avgJobs = 0;
avgProc = 0;
avgExec = 0;
totalGPUUsage = 0;
avgResponseTime = 0;
avgResponseFactor = 0;
GPUUsagePercentage = 0;
avgJobPercentage = 0;
GPUScheduleOverhead = 0;
AvgSchedulerOverhead = 0;

for x in range(row_count):
    avgJobsScheduled = avgJobsScheduled + float(data_5[x][0]);
    avgJobs = avgJobs + int(data_5[x][1]);
    avgProc = avgProc + float(data_5[x][2]);
    avgExec = avgExec + float(data_5[x][3]);
    totalGPUUsage = totalGPUUsage + float(data_5[x][4]);
    avgResponseTime = avgResponseTime + float(data_5[x][5]);
    avgResponseFactor = avgResponseFactor + float(data_5[x][6]);
    GPUUsagePercentage = GPUUsagePercentage + float(data_5[x][7]);
    avgJobPercentage = avgJobPercentage + float(data_5[x][8]);
    GPUScheduleOverhead = GPUScheduleOverhead + float(data_5[x][10]);
    AvgSchedulerOverhead = AvgSchedulerOverhead + float(data_5[x][11]);

avgJobsScheduled = float(avgJobsScheduled)/row_count;
avgJobs = float(avgJobs)/row_count;
avgProc = float(avgProc)/row_count;
avgExec = float(avgExec)/row_count;
totalGPUUsage = float(totalGPUUsage)/row_count;
avgResponseTime = float(avgResponseTime)/row_count;
avgResponseFactor = float(avgResponseFactor)/row_count;
GPUUsagePercentage = float(GPUUsagePercentage)/row_count;
avgJobPercentage = float(avgJobPercentage)/row_count;
GPUScheduleOverhead = float(GPUScheduleOverhead)/row_count;
AvgSchedulerOverhead = float(AvgSchedulerOverhead)/row_count;

sys.stdout = open(outputDirectory+'/Mode5-SchedulerResults.csv','a')
print str(avgReleaseLambda)+','+str(avgJobs)+','+str(avgJobsScheduled)+str(avgProc)+','+str(avgExec)+','+str(avgProc)+','+str(totalGPUUsage)+','\
+str(avgResponseTime)+','+str(avgResponseFactor)+','+str(GPUUsagePercentage)+','+str(avgJobPercentage)+','\
+str(GPUScheduleOverhead)+','+str(AvgSchedulerOverhead)+','+str(avgJobPercentage)
sys.stdout = open(outputDirectory+'/'+fileName+'-SchedulerResults.html','a')

print"\t\t\t<tr>"
print"\t\t\t\t<td><center>Mode 5</center></td>"
print'\t\t\t\t<td><center>'+str(avgJobs)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobsScheduled)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgProc)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgExec)+'</center></td>'
print'\t\t\t\t<td><center>'+str(totalGPUUsage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseTime)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseFactor)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUUsagePercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobPercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUScheduleOverhead)+'</center></td>'
print'\t\t\t\t<td><center>'+str(AvgSchedulerOverhead)+'</center></td>'
print"\t\t\t</tr>"
print"\t\t</table>"
print'\t\t<br><br><h2><center> Avg Release Time Lambda:'+str(avgReleaseLambda)+'</center></h2><br>'
print"\t\t<center><div id=\"JobScheduledPercentage_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"jobScheduled_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"GPUUsagePercentage_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"GPUUsage_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"responseTime_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"responseFactor_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t</body>"
print"</html>"
