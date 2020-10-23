# Copyright (c) 2017 - 2020 Kiriti Nagesh Gowda, Inc. All rights reserved.
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

import collections
import random
import os
import sys
import argparse
import csv
from datetime import date

__author__ = "Kiriti Nagesh Gowda"
__copyright__ = "Copyright 2018 - 2020, Kiriti Nagesh Gowda - SRTG-Scheduler"
__license__ = "MIT"
__version__ = "1.0.1"
__maintainer__ = "Kiriti Nagesh Gowda"
__email__ = "Kiritigowda@gmail.com"
__status__ = "Shipping"

# import arguments
parser = argparse.ArgumentParser()
parser.add_argument('--input_directory', type=str, default='',
                    help='Directory - RTGS_summary directory')
parser.add_argument('--output_directory', type=str, default='',
                    help='Directory - directory to save results')
parser.add_argument('--results_filename', type=str, default='',
                    help='Results File prefix - results .html file prefix')
args = parser.parse_args()

inputDirectory = args.input_directory
outputDirectory = args.output_directory
fileName = args.results_filename

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
with open(inputDirectory+'/RTGS-Mode-1-Summary.csv') as mode1:
    reader_1 = csv.reader(mode1)
    next(reader_1)
    data_1 = [r for r in reader_1]
    row_count_1 = len(data_1)

with open(inputDirectory+'/RTGS-Mode-2-Summary.csv') as mode2:
    reader_2 = csv.reader(mode2)
    next(reader_2)
    data_2 = [r for r in reader_2]
    row_count_2 = len(data_2)

with open(inputDirectory+'/RTGS-Mode-3-Summary.csv') as mode3:
    reader_3 = csv.reader(mode3)
    next(reader_3)
    data_3 = [r for r in reader_3]
    row_count_3 = len(data_3)

with open(inputDirectory+'/RTGS-Mode-4-Summary.csv') as mode4:
    reader_4 = csv.reader(mode4)
    next(reader_4)
    data_4 = [r for r in reader_4]
    row_count_4 = len(data_4)

with open(inputDirectory+'/RTGS-Mode-5-Summary.csv') as mode5:
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
result_accum_1 = outputDirectory+'/mode-1-accum-results.csv'
result_accum_2 = outputDirectory+'/mode-2-accum-results.csv'
result_accum_3 = outputDirectory+'/mode-3-accum-results.csv'
result_accum_4 = outputDirectory+'/mode-4-accum-results.csv'
result_accum_5 = outputDirectory+'/mode-5-accum-results.csv'
if not os.path.isfile(result_accum_1):
    sys.stdout = open(result_accum_1, 'w+')
    print'AVG Lambda, AVG Jobs Released, AVG Jobs Accepted, AVG Jobs Accepted Percentage, \
        Avg GCUs Requested - Accepted Jobs, Avg Exec Time - Accepted Jobs, Avg Response by Execution Time, \
        Avg Response by Relative deadline, AVG Total GPU Usage Time - Accepted Jobs, AVG Total GPU Usage Time Requested - All Jobs, \
        Avg Scheduler OverHead - Accepted Jobs, Avg Scheduler OverHead - All Jobs, Num Job Sets, '+dateCreated
if not os.path.isfile(result_accum_2):
    sys.stdout = open(result_accum_2, 'w+')
    print'AVG Lambda, AVG Jobs Released, AVG Jobs Accepted, AVG Jobs Accepted Percentage, \
        Avg GCUs Requested - Accepted Jobs, Avg Exec Time - Accepted Jobs, Avg Response by Execution Time, \
        Avg Response by Relative deadline, AVG Total GPU Usage Time - Accepted Jobs, AVG Total GPU Usage Time Requested - All Jobs, \
        Avg Scheduler OverHead - Accepted Jobs, Avg Scheduler OverHead - All Jobs, Num Job Sets, '+dateCreated
if not os.path.isfile(result_accum_3):
    sys.stdout = open(result_accum_3, 'w+')
    print'AVG Lambda, AVG Jobs Released, AVG Jobs Accepted, AVG Jobs Accepted Percentage, \
        Avg GCUs Requested - Accepted Jobs, Avg Exec Time - Accepted Jobs, Avg Response by Execution Time, \
        Avg Response by Relative deadline, AVG Total GPU Usage Time - Accepted Jobs, AVG Total GPU Usage Time Requested - All Jobs, \
        Avg Scheduler OverHead - Accepted Jobs, Avg Scheduler OverHead - All Jobs, Num Job Sets, '+dateCreated
if not os.path.isfile(result_accum_4):
    sys.stdout = open(result_accum_4, 'w+')
    print'AVG Lambda, AVG Jobs Released, AVG Jobs Accepted, AVG Jobs Accepted Percentage, \
        Avg GCUs Requested - Accepted Jobs, Avg Exec Time - Accepted Jobs, Avg Response by Execution Time, \
        Avg Response by Relative deadline, AVG Total GPU Usage Time - Accepted Jobs, AVG Total GPU Usage Time Requested - All Jobs, \
        Avg Scheduler OverHead - Accepted Jobs, Avg Scheduler OverHead - All Jobs, Num Job Sets, '+dateCreated
if not os.path.isfile(result_accum_5):
    sys.stdout = open(result_accum_5, 'w+')
    print'AVG Lambda, AVG Jobs Released, AVG Jobs Accepted, AVG Jobs Accepted Percentage, \
        Avg GCUs Requested - Accepted Jobs, Avg Exec Time - Accepted Jobs, Avg Response by Execution Time, \
        Avg Response by Relative deadline, AVG Total GPU Usage Time - Accepted Jobs, AVG Total GPU Usage Time Requested - All Jobs, \
        Avg Scheduler OverHead - Accepted Jobs, Avg Scheduler OverHead - All Jobs, Num Job Sets, '+dateCreated

# HTML File
html_output_file = outputDirectory+'/'+fileName+'-SchedulerResults.html'
sys.stdout = open(html_output_file, 'w+')

# HTML Header
print"<html>"
print"\t<head>"
print"\t\t<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>"
print"\n"
# Google Charts Script
print"\t\t<script type=\"text/javascript\">"
print"\n"
# Jobs Accepted for GPU Schedule
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
        print '\t\t\t\t['+str(x)+','+str(data_1[x][2])+','+str(data_2[x][2])+','+str(data_3[x][2])+','+str(data_4[x][2])+','+str(data_5[x][2])+'],'
    else:
        print '\t\t\t\t['+str(x)+','+str(data_1[x][2])+','+str(data_2[x][2])+','+str(data_3[x][2])+','+str(data_4[x][2])+','+str(data_5[x][2])+']'
print"\t\t\t]);"
print"\t\t\tvar options = { title:'Average Jobs Accepted for GPU Schedule', \
    titleTextStyle: { fontSize: 28, bold: true}, \
    hAxis:{ title: 'JobSet ID', titleTextStyle: { fontSize: 24, bold: true}, marginTop: '5'}, \
    vAxis:{ title: 'Number of Jobs Scheduled', titleTextStyle:{ fontSize: 24, bold: true} }, \
    series:{ 0:{lineDashStyle: [1, 1]}, 1:{lineDashStyle: [2, 2]}, 2:{lineDashStyle: [4, 4]}, 3:{lineDashStyle: [5, 1, 3] }, 4:{ lineDashStyle: [5, 5]}}, \
    legend:{ position: 'top', alignment: 'center', textStyle:{ fontSize: 26}}, \
    width:1600, height:1000 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('jobScheduled_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n\n\n"
# Job Accepted Percentage for GPU Schedule
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
        print '\t\t\t\t['+str(x)+','+str(data_1[x][3])+','+str(data_2[x][3])+','+str(data_3[x][3])+','+str(data_4[x][3])+','+str(data_5[x][3])+'],'
    else:
        print '\t\t\t\t['+str(x)+','+str(data_1[x][3])+','+str(data_2[x][3])+','+str(data_3[x][3])+','+str(data_4[x][3])+','+str(data_5[x][3])+']'
print"\t\t\t]);"
print"\t\t\tvar options = { title:'Average Jobs Accepted Percentage for GPU Schedule', \
    titleTextStyle: { fontSize: 28, bold: true}, \
    hAxis:{ title: 'JobSet ID', titleTextStyle: { fontSize: 24, bold: true}, marginTop: '5'}, \
    vAxis:{ title: 'Avg Jobs Scheduled %', titleTextStyle:{ fontSize: 24, bold: true}, minValue: 0, maxValue: 100 }, \
    series:{ 0:{lineDashStyle: [1, 1]}, 1:{lineDashStyle: [2, 2]}, 2:{lineDashStyle: [4, 4]}, 3:{lineDashStyle: [5, 1, 3] }, 4:{ lineDashStyle: [5, 5]}}, \
    legend:{ position: 'top', alignment: 'center', textStyle:{ fontSize: 26}}, \
    width:1600, height:1000 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('JobScheduledPercentage_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n\n\n"
# Average Response by Execution Time
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
        print '\t\t\t\t['+str(x)+','+str(data_1[x][6])+','+str(data_2[x][6])+','+str(data_3[x][6])+','+str(data_4[x][6])+','+str(data_5[x][6])+'],'
    else:
        print '\t\t\t\t['+str(x)+','+str(data_1[x][6])+','+str(data_2[x][6])+','+str(data_3[x][6])+','+str(data_4[x][6])+','+str(data_5[x][6])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'Average Response by Execution Time', hAxis: { title: 'JobSet ID'}, vAxis: {title: 'Response by Execution Time'}, series: { 0.01: {curveType: 'function'} }, width:1600, height:1000 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('responseByExecTime_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n\n\n"
# Average Response by Relative Deadline
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
        print '\t\t\t\t['+str(x)+','+str(data_1[x][7])+','+str(data_2[x][7])+','+str(data_3[x][7])+','+str(data_4[x][7])+','+str(data_5[x][7])+'],'
    else:
        print '\t\t\t\t['+str(x)+','+str(data_1[x][7])+','+str(data_2[x][7])+','+str(data_3[x][7])+','+str(data_4[x][7])+','+str(data_5[x][7])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'Average Response by Relative Deadline', hAxis: { title: 'JobSet ID'}, vAxis: {title: 'Response by Relative Deadline'}, series: { 0.01: {curveType: 'function'} }, width:1600, height:1000 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('responseByRelativeDeadline_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n\n\n"
# GPU Usage Time for Jobs Accepted
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
        print '\t\t\t\t['+str(x)+','+str(data_1[x][8])+','+str(data_2[x][8])+','+str(data_3[x][8])+','+str(data_4[x][8])+','+str(data_5[x][8])+'],'
    else:
        print '\t\t\t\t['+str(x)+','+str(data_1[x][8])+','+str(data_2[x][8])+','+str(data_3[x][8])+','+str(data_4[x][8])+','+str(data_5[x][8])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'GPU Usage Jobs Accepted', hAxis: { title: 'JobSet ID'}, vAxis: {title: 'GPU Usage Jobs Accepted'}, series: { 0.01: {curveType: 'function'} }, width:1600, height:1000 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('GPUUsage_accepted_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n\n\n"
# GPU Usage Requested by all jobs
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
        print '\t\t\t\t['+str(x)+','+str(data_1[x][9])+','+str(data_2[x][9])+','+str(data_3[x][9])+','+str(data_4[x][9])+','+str(data_5[x][9])+'],'
    else:
        print '\t\t\t\t['+str(x)+','+str(data_1[x][9])+','+str(data_2[x][9])+','+str(data_3[x][9])+','+str(data_4[x][9])+','+str(data_5[x][9])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'Total GPU Usage Requested', hAxis: { title: 'JobSet ID'}, vAxis: {title: 'Total GPU Usage Requested'}, series: { 0.01: {curveType: 'function'} }, width:1600, height:1000 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('GPUUsage_requested_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n\n\n"
print"\t\t</script>"
print"\t</head>"
# Result Body
print"\t<body>"
# Summary of results
print'\t\t<br><br><h1><center>SRTG-ResultAnalysis: A-Periodic Job Schedule Summary</center></h2><br>'
print"\t\t<table align=\"center\" style=\"width: 95%\">"
print"\t\t\t<tr>"
print"\t\t\t\t<td><center></center></td>"
print"\t\t\t\t<td><center>AVG Jobs Released</center></td>"
print"\t\t\t\t<td><center>AVG Jobs Accepted</center></td>"
print"\t\t\t\t<td><center>AVG Jobs Accepted Percentage</center></td>"
print"\t\t\t\t<td><center>Avg GCUs Requested - Accepted Jobs</center></td>"
print"\t\t\t\t<td><center>Avg Exec Time - Accepted Jobs</center></td>"
print"\t\t\t\t<td><center>Avg Response by Execution Time</center></td>"
print"\t\t\t\t<td><center>Avg Response by Relative deadline</center></td>"
print"\t\t\t\t<td><center>AVG Total GPU Usage Time - Accepted Jobs</center></td>"
print"\t\t\t\t<td><center>AVG Total GPU Usage Time Requested - All Jobs</center></td>"
print"\t\t\t\t<td><center>Avg Scheduler OverHead - Accepted Jobs</center></td>"
print"\t\t\t\t<td><center>Avg Scheduler OverHead - All Jobs</center></td>"
print"\t\t\t</tr>"
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

# accum results
sys.stdout = open(result_accum_1, 'a')
print(str(avgReleaseLambda)+', '+str(avgJobs)+', '+str(avgJobsAccepted)+', '+str(avgJobPercentage)+', '
      + str(avgProc)+', '+str(avgExec)+', ' +
      str(avgResponseTime)+', '+str(avgResponseFactor)+', '
      + str(GPUUsagePercentage)+', '+str(totalGPUUsage)+', '
      + str(GPUScheduleOverhead)+','+str(AvgSchedulerOverhead)+','+str(row_count))
sys.stdout = open(html_output_file, 'a')

print"\t\t\t<tr>"
print"\t\t\t\t<td><center>Mode 1</center></td>"
print'\t\t\t\t<td><center>'+str(avgJobs)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobsAccepted)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobPercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgProc)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgExec)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseTime)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseFactor)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUUsagePercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(totalGPUUsage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUScheduleOverhead)+'</center></td>'
print'\t\t\t\t<td><center>'+str(AvgSchedulerOverhead)+'</center></td>'
print"\t\t\t</tr>"
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

for x in range(row_count):
    avgJobsAccepted = avgJobsAccepted + float(data_2[x][2])
    avgJobPercentage = avgJobPercentage + float(data_2[x][3])
    avgJobs = avgJobs + int(data_2[x][0])
    avgProc = avgProc + float(data_2[x][4])
    avgExec = avgExec + float(data_2[x][5])
    avgResponseTime = avgResponseTime + float(data_2[x][6])
    avgResponseFactor = avgResponseFactor + float(data_2[x][7])
    GPUUsagePercentage = GPUUsagePercentage + float(data_2[x][8])
    totalGPUUsage = totalGPUUsage + float(data_2[x][9])
    GPUScheduleOverhead = GPUScheduleOverhead + float(data_2[x][10])
    AvgSchedulerOverhead = AvgSchedulerOverhead + float(data_2[x][11])

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

# accum results
sys.stdout = open(result_accum_2, 'a')
print(str(avgReleaseLambda)+', '+str(avgJobs)+', '+str(avgJobsAccepted)+', '+str(avgJobPercentage)+', '
      + str(avgProc)+', '+str(avgExec)+', ' +
      str(avgResponseTime)+', '+str(avgResponseFactor)+', '
      + str(GPUUsagePercentage)+', '+str(totalGPUUsage)+', '
      + str(GPUScheduleOverhead)+','+str(AvgSchedulerOverhead)+','+str(row_count))
sys.stdout = open(html_output_file, 'a')

print"\t\t\t<tr>"
print"\t\t\t\t<td><center>Mode 2</center></td>"
print'\t\t\t\t<td><center>'+str(avgJobs)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobsAccepted)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobPercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgProc)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgExec)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseTime)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseFactor)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUUsagePercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(totalGPUUsage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUScheduleOverhead)+'</center></td>'
print'\t\t\t\t<td><center>'+str(AvgSchedulerOverhead)+'</center></td>'
print"\t\t\t</tr>"
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

for x in range(row_count):
    avgJobsAccepted = avgJobsAccepted + float(data_3[x][2])
    avgJobs = avgJobs + int(data_3[x][0])
    avgProc = avgProc + float(data_3[x][4])
    avgExec = avgExec + float(data_3[x][5])
    totalGPUUsage = totalGPUUsage + float(data_3[x][9])
    avgResponseTime = avgResponseTime + float(data_3[x][6])
    avgResponseFactor = avgResponseFactor + float(data_3[x][7])
    GPUUsagePercentage = GPUUsagePercentage + float(data_3[x][8])
    avgJobPercentage = avgJobPercentage + float(data_3[x][3])
    GPUScheduleOverhead = GPUScheduleOverhead + float(data_3[x][10])
    AvgSchedulerOverhead = AvgSchedulerOverhead + float(data_3[x][11])

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

# accum results
sys.stdout = open(result_accum_3, 'a')
print(str(avgReleaseLambda)+', '+str(avgJobs)+', '+str(avgJobsAccepted)+', '+str(avgJobPercentage)+', '
      + str(avgProc)+', '+str(avgExec)+', ' +
      str(avgResponseTime)+', '+str(avgResponseFactor)+', '
      + str(GPUUsagePercentage)+', '+str(totalGPUUsage)+', '
      + str(GPUScheduleOverhead)+','+str(AvgSchedulerOverhead)+','+str(row_count))
sys.stdout = open(html_output_file, 'a')

print"\t\t\t<tr>"
print"\t\t\t\t<td><center>Mode 3</center></td>"
print'\t\t\t\t<td><center>'+str(avgJobs)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobsAccepted)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobPercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgProc)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgExec)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseTime)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseFactor)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUUsagePercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(totalGPUUsage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUScheduleOverhead)+'</center></td>'
print'\t\t\t\t<td><center>'+str(AvgSchedulerOverhead)+'</center></td>'
print"\t\t\t</tr>"
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

for x in range(row_count):
    avgJobsAccepted = avgJobsAccepted + float(data_4[x][2])
    avgJobs = avgJobs + int(data_4[x][0])
    avgProc = avgProc + float(data_4[x][4])
    avgExec = avgExec + float(data_4[x][5])
    totalGPUUsage = totalGPUUsage + float(data_4[x][9])
    avgResponseTime = avgResponseTime + float(data_4[x][6])
    avgResponseFactor = avgResponseFactor + float(data_4[x][7])
    GPUUsagePercentage = GPUUsagePercentage + float(data_4[x][8])
    avgJobPercentage = avgJobPercentage + float(data_4[x][3])
    GPUScheduleOverhead = GPUScheduleOverhead + float(data_4[x][10])
    AvgSchedulerOverhead = AvgSchedulerOverhead + float(data_4[x][11])

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

# accum results
sys.stdout = open(result_accum_4, 'a')
print(str(avgReleaseLambda)+', '+str(avgJobs)+', '+str(avgJobsAccepted)+', '+str(avgJobPercentage)+', '
      + str(avgProc)+', '+str(avgExec)+', ' +
      str(avgResponseTime)+', '+str(avgResponseFactor)+', '
      + str(GPUUsagePercentage)+', '+str(totalGPUUsage)+', '
      + str(GPUScheduleOverhead)+','+str(AvgSchedulerOverhead)+','+str(row_count))
sys.stdout = open(html_output_file, 'a')

print"\t\t\t<tr>"
print"\t\t\t\t<td><center>Mode 4</center></td>"
print'\t\t\t\t<td><center>'+str(avgJobs)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobsAccepted)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobPercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgProc)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgExec)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseTime)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseFactor)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUUsagePercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(totalGPUUsage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUScheduleOverhead)+'</center></td>'
print'\t\t\t\t<td><center>'+str(AvgSchedulerOverhead)+'</center></td>'
print"\t\t\t</tr>"
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

for x in range(row_count):
    avgJobs = avgJobs + int(data_5[x][0])
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

# accum results
sys.stdout = open(result_accum_5, 'a')
print(str(avgReleaseLambda)+', '+str(avgJobs)+', '+str(avgJobsAccepted)+', '+str(avgJobPercentage)+', '
      + str(avgProc)+', '+str(avgExec)+', ' +
      str(avgResponseTime)+', '+str(avgResponseFactor)+', '
      + str(GPUUsagePercentage)+', '+str(totalGPUUsage)+', '
      + str(GPUScheduleOverhead)+','+str(AvgSchedulerOverhead)+','+str(row_count))
sys.stdout = open(html_output_file, 'a')

print"\t\t\t<tr>"
print"\t\t\t\t<td><center>Mode 5</center></td>"
print'\t\t\t\t<td><center>'+str(avgJobs)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobsAccepted)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgJobPercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgProc)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgExec)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseTime)+'</center></td>'
print'\t\t\t\t<td><center>'+str(avgResponseFactor)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUUsagePercentage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(totalGPUUsage)+'</center></td>'
print'\t\t\t\t<td><center>'+str(GPUScheduleOverhead)+'</center></td>'
print'\t\t\t\t<td><center>'+str(AvgSchedulerOverhead)+'</center></td>'
print"\t\t\t</tr>"
print"\t\t</table>"
# Release time Lambda
print'\t\t<br><br><h2><center> Avg Release Time Lambda:'+str(avgReleaseLambda)+'</center></h2><br>'
# Google Charts
print"\t\t<center><div id=\"JobScheduledPercentage_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"jobScheduled_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"GPUUsage_accepted_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"GPUUsage_requested_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"responseByExecTime_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"responseByRelativeDeadline_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t</body>"
print"</html>"
