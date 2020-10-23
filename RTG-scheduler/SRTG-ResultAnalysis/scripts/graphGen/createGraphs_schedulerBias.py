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
with open(inputDirectory+'/mode-1-accum-results.csv') as mode1:
    reader_1 = csv.reader(mode1)
    next(reader_1)
    data_1 = [r for r in reader_1]
    row_count_1 = len(data_1)

with open(inputDirectory+'/mode-2-accum-results.csv') as mode2:
    reader_2 = csv.reader(mode2)
    next(reader_2)
    data_2 = [r for r in reader_2]
    row_count_2 = len(data_2)

with open(inputDirectory+'/mode-3-accum-results.csv') as mode3:
    reader_3 = csv.reader(mode3)
    next(reader_3)
    data_3 = [r for r in reader_3]
    row_count_3 = len(data_3)

with open(inputDirectory+'/mode-4-accum-results.csv') as mode4:
    reader_4 = csv.reader(mode4)
    next(reader_4)
    data_4 = [r for r in reader_4]
    row_count_4 = len(data_4)

with open(inputDirectory+'/mode-5-accum-results.csv') as mode5:
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
print("\nSRTG-ResultAnalysis - Aperiodic Job Release Lambda Analyzer V-"+__version__+"\n")

# date
today = date.today()
dateCreated = today.strftime("%b-%d-%Y")

# CLI Print
orig_stdout = sys.stdout

# HTML File
html_output_file = outputDirectory+'/'+fileName+'-schedulerBiasSummary.html'
sys.stdout = open(html_output_file, 'w+')

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
print"\t\t\tdata.addRows([[0,0,0,0,0,0],"
delayElement = 0
for x in range(row_count):
    delayElement = delayElement + 10
    if(x < row_count-1):
        print '\t\t\t\t['+str(delayElement)+','+str(data_1[x][2])+','+str(data_2[x][2])+','+str(data_3[x][2])+','+str(data_4[x][2])+','+str(data_5[x][2])+'],'
    else:
        print '\t\t\t\t['+str(delayElement)+','+str(data_1[x][2])+','+str(data_2[x][2])+','+str(data_3[x][2])+','+str(data_4[x][2])+','+str(data_5[x][2])+']'
print"\t\t\t]);"
print"\t\t\tvar options = { title:'Average Jobs Accepted with Scheduler Bias Variation', \
    titleTextStyle: { fontSize: 28, bold: true}, \
    hAxis:{ title: 'Scheduler Bias  Value', titleTextStyle: { fontSize: 24, bold: true}, marginTop: '5'}, \
    vAxis:{ title: 'Number of Jobs Scheduled', titleTextStyle:{ fontSize: 24, bold: true} }, \
    series:{ 0:{lineDashStyle: [1, 1]}, 1:{lineDashStyle: [2, 2]}, 2:{lineDashStyle: [4, 4]}, 3:{lineDashStyle: [5, 1, 3] }, 4:{ lineDashStyle: [5, 5]}}, \
    legend:{ position: 'top', alignment: 'center', textStyle:{ fontSize: 26}}, \
    width:1600, height:1000 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('jobScheduled_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n\n\n"
# Jobs Accepted Percentage for GPU Schedule
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
delayElement = 0
for x in range(row_count):
    delayElement = delayElement + 10
    if(x < row_count-1):
        print '\t\t\t\t['+str(delayElement)+','+str(data_1[x][3])+','+str(data_2[x][3])+','+str(data_3[x][3])+','+str(data_4[x][3])+','+str(data_5[x][3])+'],'
    else:
        print '\t\t\t\t['+str(delayElement)+','+str(data_1[x][3])+','+str(data_2[x][3])+','+str(data_3[x][3])+','+str(data_4[x][3])+','+str(data_5[x][3])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'Average Jobs Scheduled Percentage', hAxis: { title: 'delay Schedule limit percentage'}, vAxis: {title: 'Avg Number of Jobs Scheduled Percentage'}, series: { 0.01: {curveType: 'function'} }, width:900, height:600 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('jobScheduled_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n\n\n"
# AVG Total GPU Usage Time - Accepted Jobs
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
delayElement = 0
for x in range(row_count):
    delayElement = delayElement + 10
    if(x < row_count-1):
        print '\t\t\t\t['+str(delayElement)+','+str(data_1[x][8])+','+str(data_2[x][8])+','+str(data_3[x][8])+','+str(data_4[x][8])+','+str(data_5[x][8])+'],'
    else:
        print '\t\t\t\t['+str(delayElement)+','+str(data_1[x][8])+','+str(data_2[x][8])+','+str(data_3[x][8])+','+str(data_4[x][8])+','+str(data_5[x][8])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'AVG Total GPU Usage Time - Accepted Jobs', hAxis: { title: 'delay Schedule limit percentage'}, vAxis: {title: 'AVG Total GPU Usage Time - Accepted Jobs'}, series: { 0.01: {curveType: 'function'} }, width:900, height:600 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('GPUUsage_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n\n\n"
#  Avg Response by Execution Time - Accepted Jobs
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
delayElement = 0
for x in range(row_count):
    delayElement = delayElement + 10
    if(x < row_count-1):
        print '\t\t\t\t['+str(delayElement)+','+str(data_1[x][6])+','+str(data_2[x][6])+','+str(data_3[x][6])+','+str(data_4[x][6])+','+str(data_5[x][6])+'],'
    else:
        print '\t\t\t\t['+str(delayElement)+','+str(data_1[x][6])+','+str(data_2[x][6])+','+str(data_3[x][6])+','+str(data_4[x][6])+','+str(data_5[x][6])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'Avg Response by Execution Time - Accepted Jobs', hAxis: { title: 'delay Schedule limit percentage'}, vAxis: {title: 'Avg Response by Execution Time - Accepted Jobs'}, series: { 0.01: {curveType: 'function'} }, width:900, height:600 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('responseTime_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n\n\n"
#  Avg Response by Relative deadline - Accepted Jobs
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
delayElement = 0
for x in range(row_count):
    delayElement = delayElement + 10
    if(x < row_count-1):
        print '\t\t\t\t['+str(delayElement)+','+str(data_1[x][7])+','+str(data_2[x][7])+','+str(data_3[x][7])+','+str(data_4[x][7])+','+str(data_5[x][7])+'],'
    else:
        print '\t\t\t\t['+str(delayElement)+','+str(data_1[x][7])+','+str(data_2[x][7])+','+str(data_3[x][7])+','+str(data_4[x][7])+','+str(data_5[x][7])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'Avg Response by Relative deadline - Accepted Jobs', hAxis: { title: 'delay Schedule limit percentage'}, vAxis: {title: 'Avg Response by Relative deadline - Accepted Jobs'}, series: { 0.01: {curveType: 'function'} }, width:900, height:600 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('responseFactor_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n\n\n"
# Scheduler Overhead
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
delayElement = 0
for x in range(row_count):
    delayElement = delayElement + 10
    if(x < row_count-1):
        print '\t\t\t\t['+str(delayElement)+','+str(data_1[x][10])+','+str(data_2[x][10])+','+str(data_3[x][10])+','+str(data_4[x][10])+','+str(data_5[x][10])+'],'
    else:
        print '\t\t\t\t['+str(delayElement)+','+str(data_1[x][10])+','+str(data_2[x][10])+','+str(data_3[x][10])+','+str(data_4[x][10])+','+str(data_5[x][10])+']'
print"\t\t\t]);"
print"\t\t\tvar options = {  title:'GPU Schedule Overhead Per Job (ms)', hAxis: { title: 'delay Schedule limit percentage'}, vAxis: {title: 'GPU Schedule Overhead'}, series: { 0.01: {curveType: 'function'} }, width:900, height:600 };"
print"\t\t\tvar chart = new google.visualization.LineChart(document.getElementById('GPUScheduleOverhead_chart'));"
print"\t\t\tchart.draw(data, options);}"
print"\n"
print"\t\t</script>"
print"\t</head>"
print"\t<body>"
# Lambda Calculations
avgLambda = 0
for x in range(row_count):
    avgLambda = avgLambda + data_1[x][0]
avgLambda = (avgLambda/row_count)
print'\t\t<br><br><h2><center> SRTG-ResultAnalysis: Aperiodic Job Release Scheduler Bias Analyzer </center></h2><br>'
print'\t\t<br><br><h2><center>Release Time Lambda: '+str(avgLambda)+'</center></h2><br>'
print'\t\t<br><br><h3><center>Created on: '+dateCreated+'</center></h3><br>'
print"\t\t<center><div id=\"jobScheduled_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"GPUScheduleOverhead_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"responseTime_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"responseFactor_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t\t<center><div id=\"GPUUsage_chart\" style=\"border: 1px solid #ccc\"></div></center>"
print"\t</body>"
print"</html>"
