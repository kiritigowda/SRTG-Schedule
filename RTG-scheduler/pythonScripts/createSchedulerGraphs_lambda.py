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
with open(inputDirectory+'Mode1-SchedulerResults.csv') as mode1:
    reader_1 = csv.reader(mode1)
    data_1 = [r for r in reader_1]
    row_count_1 = len(data_1)

with open(inputDirectory+'Mode2-SchedulerResults.csv') as mode2:
    reader_2 = csv.reader(mode2)
    data_2 = [r for r in reader_2]
    row_count_2 = len(data_2)

with open(inputDirectory+'Mode3-SchedulerResults.csv') as mode3:
    reader_3 = csv.reader(mode3)
    data_3 = [r for r in reader_3]
    row_count_3 = len(data_3)

with open(inputDirectory+'Mode4-SchedulerResults.csv') as mode4:
    reader_4 = csv.reader(mode4)
    data_4 = [r for r in reader_4]
    row_count_4 = len(data_4)

with open(inputDirectory+'Mode5-SchedulerResults.csv') as mode5:
    reader_5 = csv.reader(mode5)
    data_5 = [r for r in reader_5]
    row_count_5 = len(data_5)

if row_count_1 != row_count_2 or row_count_2 != row_count_3 or row_count_3 != row_count_4 or row_count_4 != row_count_5:
    print "ERROR: Number of entries in Summary File are different";
    exit();
else:
    row_count = row_count_1;

orig_stdout = sys.stdout
sys.stdout = open(outputDirectory+'/'+fileName+'-SchedulerResultsSummary.html','w')
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
print"\t\t\tdata.addRows([[0,0,0,0,0,0],"
lambdaVar = 0;
for x in range(row_count):
    lambdaVar = lambdaVar + 0.1;
    if(x < row_count-1):
        print '\t\t\t\t['+str(lambdaVar)+','+str(data_1[x][3])+','+str(data_2[x][3])+','+str(data_3[x][3])+','+str(data_4[x][3])+','+str(data_5[x][3])+'],'
    else:
        print '\t\t\t\t['+str(lambdaVar)+','+str(data_1[x][3])+','+str(data_2[x][3])+','+str(data_3[x][3])+','+str(data_4[x][3])+','+str(data_5[x][3])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'Average Jobs Scheduled Percentage', hAxis: { title: 'Release Time Lambda Value'}, vAxis: {title: 'Avg Number of Jobs Scheduled Percentage'}, series: { 0.01: {curveType: 'function'} }, width:900, height:600 };"
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
print"\t\t\tdata.addRows([[0,0,0,0,0,0],"
lambdaVar = 0;
for x in range(row_count):
    lambdaVar = lambdaVar + 0.1
    if(x < row_count-1):
        print '\t\t\t\t['+str(lambdaVar)+','+str(data_1[x][6])+','+str(data_2[x][6])+','+str(data_3[x][6])+','+str(data_4[x][6])+','+str(data_5[x][6])+'],'
    else:
        print '\t\t\t\t['+str(lambdaVar)+','+str(data_1[x][6])+','+str(data_2[x][6])+','+str(data_3[x][6])+','+str(data_4[x][6])+','+str(data_5[x][6])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'Total GPU Usage', hAxis: { title: 'Release Time Lambda Value'}, vAxis: {title: 'Total GPU Usage'}, series: { 0.01: {curveType: 'function'} }, width:900, height:600 };"
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
print"\t\t\tdata.addRows([[0,0,0,0,0,0],"
lambdaVar = 0;
for x in range(row_count):
    lambdaVar = lambdaVar + 0.1;
    if(x < row_count-1):
        print '\t\t\t\t['+str(lambdaVar)+','+str(data_1[x][8])+','+str(data_2[x][8])+','+str(data_3[x][8])+','+str(data_4[x][8])+','+str(data_5[x][8])+'],'
    else:
        print '\t\t\t\t['+str(lambdaVar)+','+str(data_1[x][8])+','+str(data_2[x][8])+','+str(data_3[x][8])+','+str(data_4[x][8])+','+str(data_5[x][8])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'Average Response Time', hAxis: { title: 'Release Time Lambda Value'}, vAxis: {title: 'Response Time'}, series: { 0.01: {curveType: 'function'} }, width:900, height:600 };"
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
print"\t\t\tdata.addRows([[0,0,0,0,0,0],"
lambdaVar = 0;
for x in range(row_count):
    lambdaVar = lambdaVar + 0.1;
    if(x < row_count-1):
        print '\t\t\t\t['+str(lambdaVar)+','+str(data_1[x][9])+','+str(data_2[x][9])+','+str(data_3[x][9])+','+str(data_4[x][9])+','+str(data_5[x][9])+'],'
    else:
        print '\t\t\t\t['+str(lambdaVar)+','+str(data_1[x][9])+','+str(data_2[x][9])+','+str(data_3[x][9])+','+str(data_4[x][9])+','+str(data_5[x][9])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'Average Response Factor', hAxis: { title: 'Release Time Lambda Value'}, vAxis: {title: 'Response Factor'}, series: { 0.01: {curveType: 'function'} }, width:900, height:600 };"
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
print"\t\t\tdata.addRows([[0,0,0,0,0,0],"
lambdaVar = 0;
for x in range(row_count):
    lambdaVar = lambdaVar + 0.1;
    if(x < row_count-1):
        print '\t\t\t\t['+str(lambdaVar)+','+str(data_1[x][7])+','+str(data_2[x][7])+','+str(data_3[x][7])+','+str(data_4[x][7])+','+str(data_5[x][7])+'],'
    else:
        print '\t\t\t\t['+str(lambdaVar)+','+str(data_1[x][7])+','+str(data_2[x][7])+','+str(data_3[x][7])+','+str(data_4[x][7])+','+str(data_5[x][7])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'GPU Usage Percentage', hAxis: { title: 'Release Time Lambda Value'}, vAxis: {title: 'GPU Usage Percentage'}, series: { 0.01: {curveType: 'function'} }, width:900, height:600 };"
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
print"\t\t\tdata.addRows([[0,0,0,0,0,0],"
lambdaVar = 0;
for x in range(row_count):
    lambdaVar = lambdaVar + 0.1;
    if(x < row_count-1):
        print '\t\t\t\t['+str(lambdaVar)+','+str(data_1[x][10])+','+str(data_2[x][10])+','+str(data_3[x][10])+','+str(data_4[x][10])+','+str(data_5[x][10])+'],'
    else:
        print '\t\t\t\t['+str(lambdaVar)+','+str(data_1[x][10])+','+str(data_2[x][10])+','+str(data_3[x][10])+','+str(data_4[x][10])+','+str(data_5[x][10])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'GPU Schedule Overhead Per Job (ms)', hAxis: { title: 'Release Time Lambda Value'}, vAxis: {title: 'GPU Schedule Overhead'}, series: { 0.01: {curveType: 'function'} }, width:900, height:600 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('GPUScheduleOverhead_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n"
print"\t\t</script>"
print"\t</head>"
print"\t<body>"
print'\t\t<br><br><h2><center>Release Time Lambda Variations</center></h2><br>'
print"\t\t<center><div id=\"jobScheduled_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"GPUScheduleOverhead_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"responseTime_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"responseFactor_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"GPUUsagePercentage_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"GPUUsage_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t</body>"
print"</html>"
