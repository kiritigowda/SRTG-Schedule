# Copyright (c) 2017 - 2022 Kiriti Nagesh Gowda, Inc. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import os
import sys
import argparse
import csv
from datetime import date

__author__ = "Kiriti Nagesh Gowda"
__copyright__ = "Copyright 2018 - 2022, Kiriti Nagesh Gowda - SRTG-Scheduler"
__license__ = "MIT"
__version__ = "1.2.0"
__maintainer__ = "Kiriti Nagesh Gowda"
__email__ = "Kiritigowda@gmail.com"
__status__ = "Shipping"

RTGS_METHOD = [
    ('RTGS_METHOD_BASE'),
    ('RTGS_METHOD_ENHANCED'),
    ('RTGS_METHOD_ENHANCED_VQS')
]

# import arguments
parser = argparse.ArgumentParser()
parser.add_argument('--input_directory', type=str, default='',
                    help='Directory - RTGS_summary directory')
parser.add_argument('--output_directory', type=str, default='',
                    help='Directory - directory to save results')
parser.add_argument('--results_filename', type=str, default='',
                    help='Results File prefix - results .html file prefix')
parser.add_argument('--method', type=str, default='0',
                    help='SRTG Method used to create the RTGS Summary at input directory')
args = parser.parse_args()

inputDirectory = args.input_directory
outputDirectory = args.output_directory
fileName = args.results_filename
method = args.method

if inputDirectory == '' or outputDirectory == '' or fileName == '':
    print("ERROR - NO Arguments Passed, use --h option")
    exit()

if not os.path.exists(inputDirectory):
    print("ERROR Invalid Input Directory")
    exit()

if not os.path.exists(outputDirectory):
    os.makedirs(outputDirectory)

row_count = 0
row_count_1 = 0
row_count_2 = 0
row_count_3 = 0
row_count_4 = 0
row_count_5 = 0
with open(inputDirectory+'/RTGS-Method-'+method+'-Mode-1-Summary.csv') as mode1:
    reader_1 = csv.reader(mode1)
    next(reader_1)
    data_1 = [r for r in reader_1]
    row_count_1 = len(data_1)

with open(inputDirectory+'/RTGS-Method-'+method+'-Mode-2-Summary.csv') as mode2:
    reader_2 = csv.reader(mode2)
    next(reader_2)
    data_2 = [r for r in reader_2]
    row_count_2 = len(data_2)

with open(inputDirectory+'/RTGS-Method-'+method+'-Mode-3-Summary.csv') as mode3:
    reader_3 = csv.reader(mode3)
    next(reader_3)
    data_3 = [r for r in reader_3]
    row_count_3 = len(data_3)

with open(inputDirectory+'/RTGS-Method-'+method+'-Mode-4-Summary.csv') as mode4:
    reader_4 = csv.reader(mode4)
    next(reader_4)
    data_4 = [r for r in reader_4]
    row_count_4 = len(data_4)

with open(inputDirectory+'/RTGS-Method-'+method+'-Mode-5-Summary.csv') as mode5:
    reader_5 = csv.reader(mode5)
    next(reader_5)
    data_5 = [r for r in reader_5]
    row_count_5 = len(data_5)

if row_count_1 != row_count_2 or row_count_2 != row_count_3 or row_count_3 != row_count_4 or row_count_4 != row_count_5:
    print("ERROR: Number of entries in Summary File are different")
    exit()
else:
    row_count = row_count_1

# help print
print("\nSRTG-ResultAnalysis - Aperiodic Job Result Accumulator and Analyzer V-"+__version__+"\n")

# date
today = date.today()
dateCreated = today.strftime("%b-%d-%Y")

# output accum file
orig_stdout = sys.stdout
result_accum_1 = outputDirectory+'/method-'+method+'-mode-1-accum-results.csv'
result_accum_2 = outputDirectory+'/method-'+method+'-mode-2-accum-results.csv'
result_accum_3 = outputDirectory+'/method-'+method+'-mode-3-accum-results.csv'
result_accum_4 = outputDirectory+'/method-'+method+'-mode-4-accum-results.csv'
result_accum_5 = outputDirectory+'/method-'+method+'-mode-5-accum-results.csv'
if not os.path.isfile(result_accum_1):
    sys.stdout = open(result_accum_1, 'w+')
    print('AVG Lambda, AVG Jobs Released, AVG Jobs Accepted, AVG Jobs Accepted Percentage, \
        Avg GCUs Requested - Accepted Jobs, Avg Exec Time - Accepted Jobs, Avg Response by Execution Time, \
        Avg Response by Relative deadline, AVG Total GPU Usage Time - Accepted Jobs, AVG Total GPU Usage Time Requested - All Jobs, \
        Avg Scheduler OverHead - Accepted Jobs, Avg Scheduler OverHead - All Jobs, Num Job Sets, \
        Avg Method-0 Schedules, Avg Method-1 Schedules, Avg Method-2 Schedules, \
        Avg Level-0 Schedules, Avg Level-1 Schedules, Avg Level-2 Schedules,  '+dateCreated)
if not os.path.isfile(result_accum_2):
    sys.stdout = open(result_accum_2, 'w+')
    print('AVG Lambda, AVG Jobs Released, AVG Jobs Accepted, AVG Jobs Accepted Percentage, \
        Avg GCUs Requested - Accepted Jobs, Avg Exec Time - Accepted Jobs, Avg Response by Execution Time, \
        Avg Response by Relative deadline, AVG Total GPU Usage Time - Accepted Jobs, AVG Total GPU Usage Time Requested - All Jobs, \
        Avg Scheduler OverHead - Accepted Jobs, Avg Scheduler OverHead - All Jobs, Num Job Sets, \
        Avg Method-0 Schedules, Avg Method-1 Schedules, Avg Method-2 Schedules, \
        Avg Level-0 Schedules, Avg Level-1 Schedules, Avg Level-2 Schedules,  '+dateCreated)
if not os.path.isfile(result_accum_3):
    sys.stdout = open(result_accum_3, 'w+')
    print('AVG Lambda, AVG Jobs Released, AVG Jobs Accepted, AVG Jobs Accepted Percentage, \
        Avg GCUs Requested - Accepted Jobs, Avg Exec Time - Accepted Jobs, Avg Response by Execution Time, \
        Avg Response by Relative deadline, AVG Total GPU Usage Time - Accepted Jobs, AVG Total GPU Usage Time Requested - All Jobs, \
        Avg Scheduler OverHead - Accepted Jobs, Avg Scheduler OverHead - All Jobs, Num Job Sets, \
        Avg Method-0 Schedules, Avg Method-1 Schedules, Avg Method-2 Schedules, \
        Avg Level-0 Schedules, Avg Level-1 Schedules, Avg Level-2 Schedules,  '+dateCreated)
if not os.path.isfile(result_accum_4):
    sys.stdout = open(result_accum_4, 'w+')
    print('AVG Lambda, AVG Jobs Released, AVG Jobs Accepted, AVG Jobs Accepted Percentage, \
        Avg GCUs Requested - Accepted Jobs, Avg Exec Time - Accepted Jobs, Avg Response by Execution Time, \
        Avg Response by Relative deadline, AVG Total GPU Usage Time - Accepted Jobs, AVG Total GPU Usage Time Requested - All Jobs, \
        Avg Scheduler OverHead - Accepted Jobs, Avg Scheduler OverHead - All Jobs, Num Job Sets, \
        Avg Method-0 Schedules, Avg Method-1 Schedules, Avg Method-2 Schedules, \
        Avg Level-0 Schedules, Avg Level-1 Schedules, Avg Level-2 Schedules,  '+dateCreated)
if not os.path.isfile(result_accum_5):
    sys.stdout = open(result_accum_5, 'w+')
    print('AVG Lambda, AVG Jobs Released, AVG Jobs Accepted, AVG Jobs Accepted Percentage, \
        Avg GCUs Requested - Accepted Jobs, Avg Exec Time - Accepted Jobs, Avg Response by Execution Time, \
        Avg Response by Relative deadline, AVG Total GPU Usage Time - Accepted Jobs, AVG Total GPU Usage Time Requested - All Jobs, \
        Avg Scheduler OverHead - Accepted Jobs, Avg Scheduler OverHead - All Jobs, Num Job Sets, \
        Avg Method-0 Schedules, Avg Method-1 Schedules, Avg Method-2 Schedules, \
        Avg Level-0 Schedules, Avg Level-1 Schedules, Avg Level-2 Schedules,  '+dateCreated)

# HTML File
html_output_file = outputDirectory+'/'+fileName+'-Method-'+method+'-SchedulerResults.html'
sys.stdout = open(html_output_file, 'w+')

# HTML Header
print("<html>")
print("\t<head>")
print("\t\t<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>")
print("\n")
# Google Charts Script
print("\t\t<script type=\"text/javascript\">")
print("\n")
# Jobs Accepted for GPU Schedule
print(
    "\t\t\tgoogle.charts.load('current', {packages: ['corechart', 'line']});")
print("\t\t\tgoogle.charts.setOnLoadCallback(jobScheduledGraph);")
print("\t\t\tfunction jobScheduledGraph() {")
print("\t\t\tvar data = new google.visualization.DataTable();")
print("\t\t\tdata.addColumn('number', 'X');")
print("\t\t\tdata.addColumn('number', 'Mode 1');")
print("\t\t\tdata.addColumn('number', 'Mode 2');")
print("\t\t\tdata.addColumn('number', 'Mode 3');")
print("\t\t\tdata.addColumn('number', 'Mode 4');")
print("\t\t\tdata.addColumn('number', 'Mode 5');")
print("\t\t\tdata.addRows([")
for x in range(row_count):
    if(x < row_count-1):
        print('\t\t\t\t['+str(x)+','+str(data_1[x][2])+','+str(data_2[x][2]) +
              ','+str(data_3[x][2])+','+str(data_4[x][2])+','+str(data_5[x][2])+'],')
    else:
        print('\t\t\t\t['+str(x)+','+str(data_1[x][2])+','+str(data_2[x][2]) +
              ','+str(data_3[x][2])+','+str(data_4[x][2])+','+str(data_5[x][2])+']')
print("\t\t\t]);")
print("\t\t\tvar options = { title:'Average Jobs Accepted for GPU Schedule', \
    titleTextStyle: { fontSize: 28, bold: true}, \
    hAxis:{ title: 'JobSet ID', titleTextStyle: { fontSize: 24, bold: true}, marginTop: '5'}, \
    vAxis:{ title: 'Number of Jobs Scheduled', titleTextStyle:{ fontSize: 24, bold: true} }, \
    series:{ 0:{lineDashStyle: [1, 1]}, 1:{lineDashStyle: [2, 2]}, 2:{lineDashStyle: [4, 4]}, 3:{lineDashStyle: [5, 1, 3] }, 4:{ lineDashStyle: [5, 5]}}, \
    legend:{ position: 'top', alignment: 'center', textStyle:{ fontSize: 26}}, \
    width:1600, height:1000 };")
print("\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('jobScheduled_chart'));")
print("\t\t\tchart.draw(data, options);}")
print("\n\n\n")
# Job Accepted Percentage for GPU Schedule
print(
    "\t\t\tgoogle.charts.load('current', {packages: ['corechart', 'line']});")
print("\t\t\tgoogle.charts.setOnLoadCallback(jobScheduledPercentageGraph);")
print("\t\t\tfunction jobScheduledPercentageGraph() {")
print("\t\t\tvar data = new google.visualization.DataTable();")
print("\t\t\tdata.addColumn('number', 'X');")
print("\t\t\tdata.addColumn('number', 'Mode 1');")
print("\t\t\tdata.addColumn('number', 'Mode 2');")
print("\t\t\tdata.addColumn('number', 'Mode 3');")
print("\t\t\tdata.addColumn('number', 'Mode 4');")
print("\t\t\tdata.addColumn('number', 'Mode 5');")
print("\t\t\tdata.addRows([")
for x in range(row_count):
    if(x < row_count-1):
        print('\t\t\t\t['+str(x)+','+str(data_1[x][3])+','+str(data_2[x][3]) +
              ','+str(data_3[x][3])+','+str(data_4[x][3])+','+str(data_5[x][3])+'],')
    else:
        print('\t\t\t\t['+str(x)+','+str(data_1[x][3])+','+str(data_2[x][3]) +
              ','+str(data_3[x][3])+','+str(data_4[x][3])+','+str(data_5[x][3])+']')
print("\t\t\t]);")
print("\t\t\tvar options = { title:'Average Jobs Accepted Percentage for GPU Schedule', \
    titleTextStyle: { fontSize: 28, bold: true}, \
    hAxis:{ title: 'JobSet ID', titleTextStyle: { fontSize: 24, bold: true}, marginTop: '5'}, \
    vAxis:{ title: 'Avg Jobs Scheduled %', titleTextStyle:{ fontSize: 24, bold: true}, minValue: 0, maxValue: 100 }, \
    series:{ 0:{lineDashStyle: [1, 1]}, 1:{lineDashStyle: [2, 2]}, 2:{lineDashStyle: [4, 4]}, 3:{lineDashStyle: [5, 1, 3] }, 4:{ lineDashStyle: [5, 5]}}, \
    legend:{ position: 'top', alignment: 'center', textStyle:{ fontSize: 26}}, \
    width:1600, height:1000 };")
print("\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('JobScheduledPercentage_chart'));")
print("\t\t\tchart.draw(data, options);}")
print("\n\n\n")
# Average Response by Execution Time
print(
    "\t\t\tgoogle.charts.load('current', {packages: ['corechart', 'line']});")
print("\t\t\tgoogle.charts.setOnLoadCallback(avgResponseTimeGraph);")
print("\t\t\tfunction avgResponseTimeGraph() {")
print("\t\t\tvar data = new google.visualization.DataTable();")
print("\t\t\tdata.addColumn('number', 'X');")
print("\t\t\tdata.addColumn('number', 'Mode 1');")
print("\t\t\tdata.addColumn('number', 'Mode 2');")
print("\t\t\tdata.addColumn('number', 'Mode 3');")
print("\t\t\tdata.addColumn('number', 'Mode 4');")
print("\t\t\tdata.addColumn('number', 'Mode 5');")
print("\t\t\tdata.addRows([")
for x in range(row_count):
    if(x < row_count-1):
        print('\t\t\t\t['+str(x)+','+str(data_1[x][6])+','+str(data_2[x][6]) +
              ','+str(data_3[x][6])+','+str(data_4[x][6])+','+str(data_5[x][6])+'],')
    else:
        print('\t\t\t\t['+str(x)+','+str(data_1[x][6])+','+str(data_2[x][6]) +
              ','+str(data_3[x][6])+','+str(data_4[x][6])+','+str(data_5[x][6])+']')
print("\t\t\t]);")
print("\t\t\tvar options = {  title:'Average Response by Execution Time', hAxis: { title: 'JobSet ID'}, vAxis: {title: 'Response by Execution Time'}, series: { 0.01: {curveType: 'function'} }, width:1600, height:1000 };")
print("\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('responseByExecTime_chart'));")
print("\t\t\tchart.draw(data, options);}")
print("\n\n\n")
# Average Response by Relative Deadline
print(
    "\t\t\tgoogle.charts.load('current', {packages: ['corechart', 'line']});")
print("\t\t\tgoogle.charts.setOnLoadCallback(avgResponseFactorGraph);")
print("\t\t\tfunction avgResponseFactorGraph() {")
print("\t\t\tvar data = new google.visualization.DataTable();")
print("\t\t\tdata.addColumn('number', 'X');")
print("\t\t\tdata.addColumn('number', 'Mode 1');")
print("\t\t\tdata.addColumn('number', 'Mode 2');")
print("\t\t\tdata.addColumn('number', 'Mode 3');")
print("\t\t\tdata.addColumn('number', 'Mode 4');")
print("\t\t\tdata.addColumn('number', 'Mode 5');")
print("\t\t\tdata.addRows([")
for x in range(row_count):
    if(x < row_count-1):
        print('\t\t\t\t['+str(x)+','+str(data_1[x][7])+','+str(data_2[x][7]) +
              ','+str(data_3[x][7])+','+str(data_4[x][7])+','+str(data_5[x][7])+'],')
    else:
        print('\t\t\t\t['+str(x)+','+str(data_1[x][7])+','+str(data_2[x][7]) +
              ','+str(data_3[x][7])+','+str(data_4[x][7])+','+str(data_5[x][7])+']')
print("\t\t\t]);")
print("\t\t\tvar options = {  title:'Average Response by Relative Deadline', hAxis: { title: 'JobSet ID'}, vAxis: {title: 'Response by Relative Deadline'}, series: { 0.01: {curveType: 'function'} }, width:1600, height:1000 };")
print("\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('responseByRelativeDeadline_chart'));")
print("\t\t\tchart.draw(data, options);}")
print("\n\n\n")
# GPU Usage Time for Jobs Accepted
print(
    "\t\t\tgoogle.charts.load('current', {packages: ['corechart', 'line']});")
print("\t\t\tgoogle.charts.setOnLoadCallback(GPUUsagePercentageGraph);")
print("\t\t\tfunction GPUUsagePercentageGraph() {")
print("\t\t\tvar data = new google.visualization.DataTable();")
print("\t\t\tdata.addColumn('number', 'X');")
print("\t\t\tdata.addColumn('number', 'Mode 1');")
print("\t\t\tdata.addColumn('number', 'Mode 2');")
print("\t\t\tdata.addColumn('number', 'Mode 3');")
print("\t\t\tdata.addColumn('number', 'Mode 4');")
print("\t\t\tdata.addColumn('number', 'Mode 5');")
print("\t\t\tdata.addRows([")
for x in range(row_count):
    if(x < row_count-1):
        print('\t\t\t\t['+str(x)+','+str(data_1[x][8])+','+str(data_2[x][8]) +
              ','+str(data_3[x][8])+','+str(data_4[x][8])+','+str(data_5[x][8])+'],')
    else:
        print('\t\t\t\t['+str(x)+','+str(data_1[x][8])+','+str(data_2[x][8]) +
              ','+str(data_3[x][8])+','+str(data_4[x][8])+','+str(data_5[x][8])+']')
print("\t\t\t]);")
print("\t\t\tvar options = {  title:'GPU Usage Jobs Accepted', hAxis: { title: 'JobSet ID'}, vAxis: {title: 'GPU Usage Jobs Accepted'}, series: { 0.01: {curveType: 'function'} }, width:1600, height:1000 };")
print("\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('GPUUsage_accepted_chart'));")
print("\t\t\tchart.draw(data, options);}")
print("\n\n\n")
# GPU Usage Requested by all jobs
print(
    "\t\t\tgoogle.charts.load('current', {packages: ['corechart', 'line']});")
print("\t\t\tgoogle.charts.setOnLoadCallback(GPUUsageGraph);")
print("\t\t\tfunction GPUUsageGraph() {")
print("\t\t\tvar data = new google.visualization.DataTable();")
print("\t\t\tdata.addColumn('number', 'X');")
print("\t\t\tdata.addColumn('number', 'Mode 1');")
print("\t\t\tdata.addColumn('number', 'Mode 2');")
print("\t\t\tdata.addColumn('number', 'Mode 3');")
print("\t\t\tdata.addColumn('number', 'Mode 4');")
print("\t\t\tdata.addColumn('number', 'Mode 5');")
print("\t\t\tdata.addRows([")
for x in range(row_count):
    if(x < row_count-1):
        print('\t\t\t\t['+str(x)+','+str(data_1[x][9])+','+str(data_2[x][9]) +
              ','+str(data_3[x][9])+','+str(data_4[x][9])+','+str(data_5[x][9])+'],')
    else:
        print('\t\t\t\t['+str(x)+','+str(data_1[x][9])+','+str(data_2[x][9]) +
              ','+str(data_3[x][9])+','+str(data_4[x][9])+','+str(data_5[x][9])+']')
print("\t\t\t]);")
print("\t\t\tvar options = {  title:'Total GPU Usage Requested', hAxis: { title: 'JobSet ID'}, vAxis: {title: 'Total GPU Usage Requested'}, series: { 0.01: {curveType: 'function'} }, width:1600, height:1000 };")
print("\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('GPUUsage_requested_chart'));")
print("\t\t\tchart.draw(data, options);}")
print("\n\n\n")
print("\t\t</script>")
print("\t</head>")

# Result Body
print("\t<body>")
# Summary of results
print("\t\t<br><br><h1><center>SRTG-ResultAnalysis: A-Periodic Job Schedule Summary</center></h2>")
print("\t\t<h3><center>Created on: <font color = \"RoyalBlue\"><b>" +
      dateCreated+"</b></font></center></h3>")
print("\t\t<br><h2><center>Method Used: <font color = \"maroon\"><b>" +
      RTGS_METHOD[int(method)]+"</b></font></center></h2><br>")
print("\t\t<table align=\"center\" style=\"width: 98%\">")
print("\t\t\t<tr>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>MODE</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>AVG Jobs Released</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>AVG Jobs Accepted</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>AVG Jobs Accepted Percentage</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>Avg GCUs Requested - Accepted Jobs</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>Avg Exec Time - Accepted Jobs</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>Avg Response by Execution Time</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>Avg Response by Relative deadline</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>AVG Total GPU Usage Time - Accepted Jobs</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>AVG Total GPU Usage Time Requested - All Jobs</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>Avg Scheduler OverHead - Accepted Jobs</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>Avg Scheduler OverHead - All Jobs</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>Avg Method 0 Schedules</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>Avg Method 1 Schedules</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>Avg Method 2 Schedules</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>Avg High GCU Schedules</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>Avg Medium GCU Schedules</b></font></center></td>")
print("\t\t\t\t<td><center><font color = \"RoyalBlue\"><b>Avg Low GCU Schedules</b></font></center></td>")
print("\t\t\t</tr>")

# Mode 1
avgJobsAccepted = 0
avgJobs = 0
avgProc = 0
avgExec = 0
totalGPUUsage = 0
avgResponseTime = 0
avgResponseFactor = 0
GPUUsagePercentage = 0
avgJobPercentage = 0
GPUScheduleOverhead = 0
AvgSchedulerOverhead = 0
avgReleaseLambda = 0
avgMethod0 = 0
avgMethod1 = 0
avgMethod2 = 0
avgLevel0 = 0
avgLevel1 = 0
avgLevel2 = 0

for x in range(row_count):
    avgJobs = avgJobs + int(data_1[x][0])
    avgReleaseLambda = avgReleaseLambda + float(data_1[x][1])
    avgJobsAccepted = avgJobsAccepted + float(data_1[x][2])
    avgJobPercentage = avgJobPercentage + float(data_1[x][3])
    avgProc = avgProc + float(data_1[x][4])
    avgExec = avgExec + float(data_1[x][5])
    avgResponseTime = avgResponseTime + float(data_1[x][6])
    avgResponseFactor = avgResponseFactor + float(data_1[x][7])
    GPUUsagePercentage = GPUUsagePercentage + float(data_1[x][8])
    totalGPUUsage = totalGPUUsage + float(data_1[x][9])
    GPUScheduleOverhead = GPUScheduleOverhead + float(data_1[x][10])
    AvgSchedulerOverhead = AvgSchedulerOverhead + float(data_1[x][11])
    avgMethod0 = avgMethod0 + float(data_1[x][12])
    avgMethod1 = avgMethod1 + float(data_1[x][13])
    avgMethod2 = avgMethod2 + float(data_1[x][14])
    avgLevel0 = avgLevel0 + float(data_1[x][15])
    avgLevel1 = avgLevel1 + float(data_1[x][16])
    avgLevel2 = avgLevel2 + float(data_1[x][17])

avgJobsAccepted = float(avgJobsAccepted)/row_count
avgJobs = float(avgJobs)/row_count
avgProc = float(avgProc)/row_count
avgExec = float(avgExec)/row_count
totalGPUUsage = float(totalGPUUsage)/row_count
avgResponseTime = float(avgResponseTime)/row_count
avgResponseFactor = float(avgResponseFactor)/row_count
GPUUsagePercentage = float(GPUUsagePercentage)/row_count
avgJobPercentage = float(avgJobPercentage)/row_count
GPUScheduleOverhead = float(GPUScheduleOverhead)/row_count
AvgSchedulerOverhead = float(AvgSchedulerOverhead)/row_count
avgReleaseLambda = float(avgReleaseLambda)/row_count
avgMethod0 = float(avgMethod0)/row_count
avgMethod1 = float(avgMethod1)/row_count
avgMethod2 = float(avgMethod2)/row_count
avgLevel0 = float(avgLevel0)/row_count
avgLevel1 = float(avgLevel1)/row_count
avgLevel2 = float(avgLevel2)/row_count

# accum results
sys.stdout = open(result_accum_1, 'a')
print(str(round(avgReleaseLambda, 4))+', '+str(round(avgJobs, 2))+', '+str(round(avgJobsAccepted, 2))+', '+str(round(avgJobPercentage, 2))+', '
      + str(round(avgProc, 2))+', '+str(round(avgExec, 2))+', '
      + str(round(avgResponseTime, 4))+', '+str(round(avgResponseFactor, 4))+', '
      + str(round(GPUUsagePercentage, 4))+', '+str(round(totalGPUUsage, 4))+', '
      + str(round(GPUScheduleOverhead, 6))+', ' 
      + str(round(AvgSchedulerOverhead, 6))+', '+str(row_count)+', '
      + str(round(avgMethod0, 4))+', '+str(round(avgMethod1, 4))+', '+str(round(avgMethod2, 4))+', '
      + str(round(avgLevel0, 4))+', '+str(round(avgLevel1, 4))+', '+str(round(avgLevel2, 4)))
sys.stdout = open(html_output_file, 'a')

print("\t\t\t<tr>")
print("\t\t\t\t<td><center><font color = \"maroon\"><b>1</b></font></center></td>")
print("\t\t\t\t<td><center>"+str(round(avgJobs, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgJobsAccepted, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgJobPercentage, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgProc, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgExec, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgResponseTime, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgResponseFactor, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(GPUUsagePercentage, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(totalGPUUsage, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(GPUScheduleOverhead, 6))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(AvgSchedulerOverhead, 6))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgMethod0, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgMethod1, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgMethod2, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgLevel0, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgLevel1, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgLevel2, 4))+"</center></td>")
print("\t\t\t</tr>")

# Mode 2
avgJobsAccepted = 0
avgJobs = 0
avgProc = 0
avgExec = 0
totalGPUUsage = 0
avgResponseTime = 0
avgResponseFactor = 0
GPUUsagePercentage = 0
avgJobPercentage = 0
GPUScheduleOverhead = 0
AvgSchedulerOverhead = 0
avgReleaseLambda = 0
avgMethod0 = 0
avgMethod1 = 0
avgMethod2 = 0
avgLevel0 = 0
avgLevel1 = 0
avgLevel2 = 0

for x in range(row_count):
    avgJobs = avgJobs + int(data_2[x][0])
    avgReleaseLambda = avgReleaseLambda + float(data_2[x][1])
    avgJobsAccepted = avgJobsAccepted + float(data_2[x][2])
    avgJobPercentage = avgJobPercentage + float(data_2[x][3])
    avgProc = avgProc + float(data_2[x][4])
    avgExec = avgExec + float(data_2[x][5])
    avgResponseTime = avgResponseTime + float(data_2[x][6])
    avgResponseFactor = avgResponseFactor + float(data_2[x][7])
    GPUUsagePercentage = GPUUsagePercentage + float(data_2[x][8])
    totalGPUUsage = totalGPUUsage + float(data_2[x][9])
    GPUScheduleOverhead = GPUScheduleOverhead + float(data_2[x][10])
    AvgSchedulerOverhead = AvgSchedulerOverhead + float(data_2[x][11])
    avgMethod0 = avgMethod0 + float(data_2[x][12])
    avgMethod1 = avgMethod1 + float(data_2[x][13])
    avgMethod2 = avgMethod2 + float(data_2[x][14])
    avgLevel0 = avgLevel0 + float(data_2[x][15])
    avgLevel1 = avgLevel1 + float(data_2[x][16])
    avgLevel2 = avgLevel2 + float(data_2[x][17])

avgJobsAccepted = float(avgJobsAccepted)/row_count
avgJobs = float(avgJobs)/row_count
avgProc = float(avgProc)/row_count
avgExec = float(avgExec)/row_count
totalGPUUsage = float(totalGPUUsage)/row_count
avgResponseTime = float(avgResponseTime)/row_count
avgResponseFactor = float(avgResponseFactor)/row_count
GPUUsagePercentage = float(GPUUsagePercentage)/row_count
avgJobPercentage = float(avgJobPercentage)/row_count
GPUScheduleOverhead = float(GPUScheduleOverhead)/row_count
AvgSchedulerOverhead = float(AvgSchedulerOverhead)/row_count
avgReleaseLambda = float(avgReleaseLambda)/row_count
avgMethod0 = float(avgMethod0)/row_count
avgMethod1 = float(avgMethod1)/row_count
avgMethod2 = float(avgMethod2)/row_count
avgLevel0 = float(avgLevel0)/row_count
avgLevel1 = float(avgLevel1)/row_count
avgLevel2 = float(avgLevel2)/row_count

# accum results
sys.stdout = open(result_accum_2, 'a')
print(str(round(avgReleaseLambda, 4))+', '+str(round(avgJobs, 2))+', '+str(round(avgJobsAccepted, 2))+', '+str(round(avgJobPercentage, 2))+', '
      + str(round(avgProc, 2))+', '+str(round(avgExec, 2))+', '
      + str(round(avgResponseTime, 4))+', '+str(round(avgResponseFactor, 4))+', '
      + str(round(GPUUsagePercentage, 4))+', '+str(round(totalGPUUsage, 4))+', '
      + str(round(GPUScheduleOverhead, 6))+', ' 
      + str(round(AvgSchedulerOverhead, 6))+', '+str(row_count)+', '
      + str(round(avgMethod0, 4))+', '+str(round(avgMethod1, 4))+', '+str(round(avgMethod2, 4))+', '
      + str(round(avgLevel0, 4))+', '+str(round(avgLevel1, 4))+', '+str(round(avgLevel2, 4)))
sys.stdout = open(html_output_file, 'a')

print("\t\t\t<tr>")
print("\t\t\t\t<td><center><font color = \"maroon\"><b>2</b></font></center></td>")
print("\t\t\t\t<td><center>"+str(round(avgJobs, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgJobsAccepted, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgJobPercentage, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgProc, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgExec, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgResponseTime, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgResponseFactor, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(GPUUsagePercentage, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(totalGPUUsage, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(GPUScheduleOverhead, 6))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(AvgSchedulerOverhead, 6))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgMethod0, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgMethod1, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgMethod2, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgLevel0, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgLevel1, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgLevel2, 4))+"</center></td>")
print("\t\t\t</tr>")

# Mode 3
avgJobsAccepted = 0
avgJobs = 0
avgProc = 0
avgExec = 0
totalGPUUsage = 0
avgResponseTime = 0
avgResponseFactor = 0
GPUUsagePercentage = 0
avgJobPercentage = 0
GPUScheduleOverhead = 0
AvgSchedulerOverhead = 0
avgReleaseLambda = 0
avgMethod0 = 0
avgMethod1 = 0
avgMethod2 = 0
avgLevel0 = 0
avgLevel1 = 0
avgLevel2 = 0

for x in range(row_count):
    avgJobs = avgJobs + int(data_3[x][0])
    avgReleaseLambda = avgReleaseLambda + float(data_3[x][1])
    avgJobsAccepted = avgJobsAccepted + float(data_3[x][2])
    avgJobPercentage = avgJobPercentage + float(data_3[x][3])
    avgProc = avgProc + float(data_3[x][4])
    avgExec = avgExec + float(data_3[x][5])
    avgResponseTime = avgResponseTime + float(data_3[x][6])
    avgResponseFactor = avgResponseFactor + float(data_3[x][7])
    GPUUsagePercentage = GPUUsagePercentage + float(data_3[x][8])
    totalGPUUsage = totalGPUUsage + float(data_3[x][9])
    GPUScheduleOverhead = GPUScheduleOverhead + float(data_3[x][10])
    AvgSchedulerOverhead = AvgSchedulerOverhead + float(data_3[x][11])
    avgMethod0 = avgMethod0 + float(data_3[x][12])
    avgMethod1 = avgMethod1 + float(data_3[x][13])
    avgMethod2 = avgMethod2 + float(data_3[x][14])
    avgLevel0 = avgLevel0 + float(data_3[x][15])
    avgLevel1 = avgLevel1 + float(data_3[x][16])
    avgLevel2 = avgLevel2 + float(data_3[x][17])

avgJobsAccepted = float(avgJobsAccepted)/row_count
avgJobs = float(avgJobs)/row_count
avgProc = float(avgProc)/row_count
avgExec = float(avgExec)/row_count
totalGPUUsage = float(totalGPUUsage)/row_count
avgResponseTime = float(avgResponseTime)/row_count
avgResponseFactor = float(avgResponseFactor)/row_count
GPUUsagePercentage = float(GPUUsagePercentage)/row_count
avgJobPercentage = float(avgJobPercentage)/row_count
GPUScheduleOverhead = float(GPUScheduleOverhead)/row_count
AvgSchedulerOverhead = float(AvgSchedulerOverhead)/row_count
avgReleaseLambda = float(avgReleaseLambda)/row_count
avgMethod0 = float(avgMethod0)/row_count
avgMethod1 = float(avgMethod1)/row_count
avgMethod2 = float(avgMethod2)/row_count
avgLevel0 = float(avgLevel0)/row_count
avgLevel1 = float(avgLevel1)/row_count
avgLevel2 = float(avgLevel2)/row_count

# accum results
sys.stdout = open(result_accum_3, 'a')
print(str(round(avgReleaseLambda, 4))+', '+str(round(avgJobs, 2))+', '+str(round(avgJobsAccepted, 2))+', '+str(round(avgJobPercentage, 2))+', '
      + str(round(avgProc, 2))+', '+str(round(avgExec, 2))+', '
      + str(round(avgResponseTime, 4))+', '+str(round(avgResponseFactor, 4))+', '
      + str(round(GPUUsagePercentage, 4))+', '+str(round(totalGPUUsage, 4))+', '
      + str(round(GPUScheduleOverhead, 6))+', ' 
      + str(round(AvgSchedulerOverhead, 6))+', '+str(row_count)+', '
      + str(round(avgMethod0, 4))+', '+str(round(avgMethod1, 4))+', '+str(round(avgMethod2, 4))+', '
      + str(round(avgLevel0, 4))+', '+str(round(avgLevel1, 4))+', '+str(round(avgLevel2, 4)))
sys.stdout = open(html_output_file, 'a')

print("\t\t\t<tr>")
print("\t\t\t\t<td><center><font color = \"maroon\"><b>3</b></font></center></td>")
print("\t\t\t\t<td><center>"+str(round(avgJobs, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgJobsAccepted, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgJobPercentage, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgProc, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgExec, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgResponseTime, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgResponseFactor, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(GPUUsagePercentage, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(totalGPUUsage, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(GPUScheduleOverhead, 6))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(AvgSchedulerOverhead, 6))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgMethod0, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgMethod1, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgMethod2, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgLevel0, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgLevel1, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgLevel2, 4))+"</center></td>")
print("\t\t\t</tr>")

# Mode 4
avgJobsAccepted = 0
avgJobs = 0
avgProc = 0
avgExec = 0
totalGPUUsage = 0
avgResponseTime = 0
avgResponseFactor = 0
GPUUsagePercentage = 0
avgJobPercentage = 0
GPUScheduleOverhead = 0
AvgSchedulerOverhead = 0
avgReleaseLambda = 0
avgMethod0 = 0
avgMethod1 = 0
avgMethod2 = 0
avgLevel0 = 0
avgLevel1 = 0
avgLevel2 = 0

for x in range(row_count):
    avgJobs = avgJobs + int(data_4[x][0])
    avgReleaseLambda = avgReleaseLambda + float(data_4[x][1])
    avgJobsAccepted = avgJobsAccepted + float(data_4[x][2])
    avgJobPercentage = avgJobPercentage + float(data_4[x][3])
    avgProc = avgProc + float(data_4[x][4])
    avgExec = avgExec + float(data_4[x][5])
    avgResponseTime = avgResponseTime + float(data_4[x][6])
    avgResponseFactor = avgResponseFactor + float(data_4[x][7])
    GPUUsagePercentage = GPUUsagePercentage + float(data_4[x][8])
    totalGPUUsage = totalGPUUsage + float(data_4[x][9])
    GPUScheduleOverhead = GPUScheduleOverhead + float(data_4[x][10])
    AvgSchedulerOverhead = AvgSchedulerOverhead + float(data_4[x][11])
    avgMethod0 = avgMethod0 + float(data_4[x][12])
    avgMethod1 = avgMethod1 + float(data_4[x][13])
    avgMethod2 = avgMethod2 + float(data_4[x][14])
    avgLevel0 = avgLevel0 + float(data_4[x][15])
    avgLevel1 = avgLevel1 + float(data_4[x][16])
    avgLevel2 = avgLevel2 + float(data_4[x][17])

avgJobsAccepted = float(avgJobsAccepted)/row_count
avgJobs = float(avgJobs)/row_count
avgProc = float(avgProc)/row_count
avgExec = float(avgExec)/row_count
totalGPUUsage = float(totalGPUUsage)/row_count
avgResponseTime = float(avgResponseTime)/row_count
avgResponseFactor = float(avgResponseFactor)/row_count
GPUUsagePercentage = float(GPUUsagePercentage)/row_count
avgJobPercentage = float(avgJobPercentage)/row_count
GPUScheduleOverhead = float(GPUScheduleOverhead)/row_count
AvgSchedulerOverhead = float(AvgSchedulerOverhead)/row_count
avgReleaseLambda = float(avgReleaseLambda)/row_count
avgMethod0 = float(avgMethod0)/row_count
avgMethod1 = float(avgMethod1)/row_count
avgMethod2 = float(avgMethod2)/row_count
avgLevel0 = float(avgLevel0)/row_count
avgLevel1 = float(avgLevel1)/row_count
avgLevel2 = float(avgLevel2)/row_count

# accum results
sys.stdout = open(result_accum_4, 'a')
print(str(round(avgReleaseLambda, 4))+', '+str(round(avgJobs, 2))+', '+str(round(avgJobsAccepted, 2))+', '+str(round(avgJobPercentage, 2))+', '
      + str(round(avgProc, 2))+', '+str(round(avgExec, 2))+', '
      + str(round(avgResponseTime, 4))+', '+str(round(avgResponseFactor, 4))+', '
      + str(round(GPUUsagePercentage, 4))+', '+str(round(totalGPUUsage, 4))+', '
      + str(round(GPUScheduleOverhead, 6))+', ' 
      + str(round(AvgSchedulerOverhead, 6))+', '+str(row_count)+', '
      + str(round(avgMethod0, 4))+', '+str(round(avgMethod1, 4))+', '+str(round(avgMethod2, 4))+', '
      + str(round(avgLevel0, 4))+', '+str(round(avgLevel1, 4))+', '+str(round(avgLevel2, 4)))
sys.stdout = open(html_output_file, 'a')

print("\t\t\t<tr>")
print("\t\t\t\t<td><center><font color = \"maroon\"><b>4</b></font></center></td>")
print("\t\t\t\t<td><center>"+str(round(avgJobs, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgJobsAccepted, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgJobPercentage, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgProc, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgExec, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgResponseTime, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgResponseFactor, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(GPUUsagePercentage, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(totalGPUUsage, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(GPUScheduleOverhead, 6))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(AvgSchedulerOverhead, 6))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgMethod0, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgMethod1, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgMethod2, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgLevel0, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgLevel1, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgLevel2, 4))+"</center></td>")
print("\t\t\t</tr>")

# Mode 5
avgJobsAccepted = 0
avgJobs = 0
avgProc = 0
avgExec = 0
totalGPUUsage = 0
avgResponseTime = 0
avgResponseFactor = 0
GPUUsagePercentage = 0
avgJobPercentage = 0
GPUScheduleOverhead = 0
AvgSchedulerOverhead = 0
avgReleaseLambda = 0
avgMethod0 = 0
avgMethod1 = 0
avgMethod2 = 0
avgLevel0 = 0
avgLevel1 = 0
avgLevel2 = 0

for x in range(row_count):
    avgJobs = avgJobs + int(data_5[x][0])
    avgReleaseLambda = avgReleaseLambda + float(data_5[x][1])
    avgJobsAccepted = avgJobsAccepted + float(data_5[x][2])
    avgJobPercentage = avgJobPercentage + float(data_5[x][3])
    avgProc = avgProc + float(data_5[x][4])
    avgExec = avgExec + float(data_5[x][5])
    avgResponseTime = avgResponseTime + float(data_5[x][6])
    avgResponseFactor = avgResponseFactor + float(data_5[x][7])
    GPUUsagePercentage = GPUUsagePercentage + float(data_5[x][8])
    totalGPUUsage = totalGPUUsage + float(data_5[x][9])
    GPUScheduleOverhead = GPUScheduleOverhead + float(data_5[x][10])
    AvgSchedulerOverhead = AvgSchedulerOverhead + float(data_5[x][11])
    avgMethod0 = avgMethod0 + float(data_5[x][12])
    avgMethod1 = avgMethod1 + float(data_5[x][13])
    avgMethod2 = avgMethod2 + float(data_5[x][14])
    avgLevel0 = avgLevel0 + float(data_5[x][15])
    avgLevel1 = avgLevel1 + float(data_5[x][16])
    avgLevel2 = avgLevel2 + float(data_5[x][17])

avgJobsAccepted = float(avgJobsAccepted)/row_count
avgJobs = float(avgJobs)/row_count
avgProc = float(avgProc)/row_count
avgExec = float(avgExec)/row_count
totalGPUUsage = float(totalGPUUsage)/row_count
avgResponseTime = float(avgResponseTime)/row_count
avgResponseFactor = float(avgResponseFactor)/row_count
GPUUsagePercentage = float(GPUUsagePercentage)/row_count
avgJobPercentage = float(avgJobPercentage)/row_count
GPUScheduleOverhead = float(GPUScheduleOverhead)/row_count
AvgSchedulerOverhead = float(AvgSchedulerOverhead)/row_count
avgReleaseLambda = float(avgReleaseLambda)/row_count
avgMethod0 = float(avgMethod0)/row_count
avgMethod1 = float(avgMethod1)/row_count
avgMethod2 = float(avgMethod2)/row_count
avgLevel0 = float(avgLevel0)/row_count
avgLevel1 = float(avgLevel1)/row_count
avgLevel2 = float(avgLevel2)/row_count

# accum results
sys.stdout = open(result_accum_5, 'a')
print(str(round(avgReleaseLambda, 4))+', '+str(round(avgJobs, 2))+', '+str(round(avgJobsAccepted, 2))+', '+str(round(avgJobPercentage, 2))+', '
      + str(round(avgProc, 2))+', '+str(round(avgExec, 2))+', '
      + str(round(avgResponseTime, 4))+', '+str(round(avgResponseFactor, 4))+', '
      + str(round(GPUUsagePercentage, 4))+', '+str(round(totalGPUUsage, 4))+', '
      + str(round(GPUScheduleOverhead, 6))+', ' 
      + str(round(AvgSchedulerOverhead, 6))+', '+str(row_count)+', '
      + str(round(avgMethod0, 4))+', '+str(round(avgMethod1, 4))+', '+str(round(avgMethod2, 4))+', '
      + str(round(avgLevel0, 4))+', '+str(round(avgLevel1, 4))+', '+str(round(avgLevel2, 4)))
sys.stdout = open(html_output_file, 'a')

print("\t\t\t<tr>")
print("\t\t\t\t<td><center><font color = \"maroon\"><b>5</b></font></center></td>")
print("\t\t\t\t<td><center>"+str(round(avgJobs, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgJobsAccepted, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgJobPercentage, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgProc, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgExec, 2))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgResponseTime, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgResponseFactor, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(GPUUsagePercentage, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(totalGPUUsage, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(GPUScheduleOverhead, 6))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(AvgSchedulerOverhead, 6))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgMethod0, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgMethod1, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgMethod2, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgLevel0, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgLevel1, 4))+"</center></td>")
print("\t\t\t\t<td><center>"+str(round(avgLevel2, 4))+"</center></td>")
print("\t\t\t</tr>")
print("\t\t</table>")
# Release time Lambda
print("\t\t<br><br><h2><center> Avg Release Time Lambda:" +
      str(avgReleaseLambda)+"</center></h2><br>")
# Google Charts
print("\t\t<center><div id=\"JobScheduledPercentage_chart\" style=\"border: 1px solid #ccc\"></div></center>")
print("\t\t<center><div id=\"jobScheduled_chart\" style=\"border: 1px solid #ccc\"></div></center>")
print("\t\t<center><div id=\"GPUUsage_accepted_chart\" style=\"border: 1px solid #ccc\"></div></center>")
print("\t\t<center><div id=\"GPUUsage_requested_chart\" style=\"border: 1px solid #ccc\"></div></center>")
print("\t\t<center><div id=\"responseByExecTime_chart\" style=\"border: 1px solid #ccc\"></div></center>")
print("\t\t<center><div id=\"responseByRelativeDeadline_chart\" style=\"border: 1px solid #ccc\"></div></center>")
print("\t</body>")
print("</html>")
