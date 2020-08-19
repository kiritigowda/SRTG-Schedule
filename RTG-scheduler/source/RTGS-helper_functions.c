/*
* RTGS-helper_functions.c
*      Author: Kiriti Nagesh Gowda
*/

#include "RTGS.h"
#ifndef _WIN32
#include <x86intrin.h>
#define __int64 int64_t
static inline __int64 my_rdtsc() { return __rdtsc(); }
#endif

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

//! \brief clock counter function
int64_t RTGS_GetClockCounter()
{
#if _WIN32
	LARGE_INTEGER v;
	QueryPerformanceCounter(&v);
	return v.QuadPart;
#else
	//return std::chrono::high_resolution_clock::now().time_since_epoch().GLOBAL_GPU_JOBS();
	return my_rdtsc();
#endif
}

//! \brief clock frequency function
int64_t RTGS_GetClockFrequency()
{
#if _WIN32
	LARGE_INTEGER v;
	QueryPerformanceFrequency(&v);
	return v.QuadPart;
#else
	//return std::chrono::high_resolution_clock::period::den/std::chrono::high_resolution_clock::period::num;
	return 100000000;
#endif
}

//! \brief environment variable function
bool RTGS_GetEnvironmentVariable(const char *name, char *value, size_t valueSize)
{
#if _WIN32
	DWORD len = GetEnvironmentVariableA(name, value, (DWORD)valueSize);
	value[valueSize - 1] = 0;
	return (len > 0) ? true : false;
#else
	const char *v = getenv(name);
	if (v)
	{
		strncpy(value, v, valueSize);
		value[valueSize - 1] = 0;
	}
	return v ? true : false;
#endif
}

const char *HTML_header =
	"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"
	"<html>\n"
	"<head>\n"
	"<title>RTGS Visual Summary M-%d</title>\n"
	"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n"
	"<meta http-equiv=\"Content-Style-Type\" content=\"text/css\">\n"
	"<meta http-equiv=\"Content-Script-Type\" content=\"text/javascript\">\n"
	"<style type=\"text/css\">\n"
	"  <!--\n"
	"  .name0 { font-family: 'Trebuchet MS'; font-size: 15px; text-align: left;  left: 10px; height: 46px; position: fixed; }\n"
	"  .name1 { font-family: 'Comic Sans MS'; font-size: 12px; text-align: left;  left: 12px; height: 46px; position: fixed; }\n"
	"  .time0 { border: 1px solid #000000; background-color: #2080c0; height: 40px; position: absolute; }\n"
	"  .time1 { border: 0px solid #000000; background-color: yellow; height: 40px; position: absolute; }\n"
	"  -->\n"
	"</style>\n"
	"<script type=\"text/JavaScript\">\n"
	"  function load() {\n";

const char *HTML_footer =
	"  }\n"
	"</script>\n"
	"</head>\n"
	"<table>\n"
	"<tr>\n"
	"<th><b><strong>Real Time GPU Scheduling</strong></b></th>\n"
	"</tr>\n"
	"<tr>\n"
	"<th><b><strong>Job Set</strong></b></th>\n"
	"</tr>\n"
	"<tr>\n"
	"<th><b><strong>Visual Summary</strong></b></th>\n"
	"</tr>\n"
	"</table>\n"
	"<body onload='load()'>\n"
	"<div style=\"width:1000px;\">\n"
	"<div id=\"Canvas\" style=\"position: absolute; left:%dpx; width:%dpx; height:%dpx; border:solid black; overflow:auto;\"></div>\n"
	"</div>\n"
	"</body>\n"
	"</html>\n";

const char *summaryHTMLHeader =
	"<!DOCTYPE HTML PUBLIC \" -//W3C//DTD HTML 4.0 Transitional//EN\">\n"
	"\n<html>\n"
	"<head>\n"
	"\n\t<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>\n"
	"\t<title>RTG Scheduler ToolKit</title>\n"
	//"\t<link rel=\"icon\" href=\"icons/vega_icon_150.png\"/>\n"
	"\n\t<style type=\"text/css\">\n"
	"\t\n"
	"\tbody,div,table,thead,tbody,tfoot,tr,th,td,p { font-family:\"Liberation Sans\"; font-size:x-small }\n"
	"\ta.comment-indicator:hover + comment { background:#ffd; position:absolute; display:block; border:1px solid black; padding:0.5em;  }\n"
	"\ta.comment-indicator { background:red; display:inline-block; border:1px solid black; width:0.5em; height:0.5em;  }\n"
	"\tcomment { display:none;  } tr:nth-of-type(odd) { background-color:#f2f2f2;}\n"
	"\t\n"
	"\t#myImg { border-radius: 5px; cursor: pointer; transition: 0.3s; }\n"
	"\t#myImg:hover { opacity: 0.7; }\n"
	"\t.modal{ display: none; position: fixed; z-index: 8; padding-top: 100px; left: 0; top: 0;width: 100%;\n"
	"\t		height: 100%; overflow: auto; background-color: rgb(0,0,0); background-color: rgba(0,0,0,0.9); }\n"
	"\t.modal-content { margin: auto; display: block; width: 80%; max-width: 500px; }\n"
	"\t#caption { margin: auto; display: block; width: 80%; max-width: 700px; text-align: center; color: white;font-size: 18px; padding: 10px 0; height: 150px;}\n"
	"\t.modal-content, #caption {  -webkit-animation-name: zoom;  -webkit-animation-duration: 0.6s;\n"
	"\t							animation-name: zoom; animation-duration: 0.6s; }\n"
	"\t@-webkit-keyframes zoom {  from { -webkit-transform:scale(0) }  to { -webkit-transform:scale(1) }}\n"
	"\t@keyframes zoom {    from {transform:scale(0)}     to {transform:scale(1) }}\n"
	"\t.close { position: absolute; top: 15px; right: 35px; color: #f1f1f1; font-size: 40px; font-weight: bold; transition: 0.3s; }\n"
	"\t.close:hover,.close:focus { color: #bbb; text-decoration: none; cursor: pointer; }\n"
	"\t@media only screen and (max-width: 400px){ .modal-content {     width: 100%; } }\n"
	"\t\n"
	"\tbody { font-family: \"Lato\", sans-serif;}\n"
	"\t.sidenav { height: 100%; width: 0; position: fixed; z-index: 7; top: 0; left: 0; background-color: #111;\n"
	"\t\t overflow-x: hidden;    transition: 0.5s; padding-top: 60px;}\n"
	"\t.sidenav a { padding: 8px 8px 8px 32px; text-decoration: none; font-size: 25px; color: #818181; display: block; transition: 0.3s;}\n"
	"\t.sidenav a:hover { color: #f1f1f1;}\n"
	"\t.sidenav .closebtn {  position: absolute; top: 0; right: 25px; font-size: 36px; margin-left: 50px;}\n"
	"\t#main {  transition: margin-left .5s;  padding: 16px; }\n"
	"\t@media screen and (max-height: 450px) { .sidenav {padding-top: 15px;} .sidenav a {font-size: 18px;} }\n"
	"\t\n"
	"\tbody {margin:0;}\n"
	"\t.navbar {  overflow: hidden;  background-color: #333;  position: fixed; z-index: 6;  top: 0;  width: 100%;}\n"
	"\t.navbar a {  float: left;  display: block;  color: #f2f2f2;  text-align: center;  padding: 14px 16px;  text-decoration: none;  font-size: 17px; }\n"
	"\t.navbar a:hover {  background: #ddd;  color: black;}\n"
	"\t.main {  padding: 16px;  margin-top: 30px; }\n"
	"\t\n"
	"\tselect {-webkit-appearance: none; -moz-appearance: none; text-indent: 0px; text-overflow: ''; color:maroon; }\n"
	"\t\n"
	"\t.tooltip { position: relative; display: inline-block;}\n"
	"\t.tooltip .tooltiptext { visibility: hidden; width: 150px; background-color: black; color: gold;\n"
	"\t\ttext-align: center;  border-radius: 6px;  padding: 5px; position: absolute; z-index: 3;}\n"
	"\t.tooltip:hover .tooltiptext { visibility: visible;}\n"
	"\t\n"
	"\t</style>\n"
	"\n</head>\n"
	"\n\n<body>\n"
	"\t\n"
	"\t<div id=\"myModal\" class=\"modal\"> <span class=\"close\">&times;</span>  <img class=\"modal-content\" id=\"img01\">  <div id=\"caption\"></div> </div>\n"
	"\t\n"
	// table order
	"\t<div id=\"mySidenav\" class=\"sidenav\">\n"
	"\t<a href=\"javascript:void(0)\" class=\"closebtn\" onclick=\"closeNav()\">&times;</a>\n"
	"\t<A HREF=\"#table0\"><font size=\"5\">Summary</font></A><br>\n"
	"\t<A HREF=\"#table1\"><font size=\"5\">Graphs</font></A><br>\n"
	"\t<A HREF=\"#table2\"><font size=\"5\">Schedule Results</font></A><br>\n"
	"\t<A HREF=\"#table3\"><font size=\"5\">Compare</font></A><br>\n"
	"\t<A HREF=\"#table4\"><font size=\"5\">Help</font></A><br>\n"
	"\t</div>\n"
	"\t\n"
	// scripts
	"\t<script>\n"
	"\t\tfunction openNav() {\n"
	"\t\t\tdocument.getElementById(\"mySidenav\").style.width = \"250px\";\n"
	"\t\t\tdocument.getElementById(\"main\").style.marginLeft = \"250px\";}\n"
	"\t\tfunction closeNav() {\n"
	"\t\t\tdocument.getElementById(\"mySidenav\").style.width = \"0\";\n"
	"\t\t\tdocument.getElementById(\"main\").style.marginLeft= \"0\";}\n"
	"\t\tfunction myreload() { location.reload();}\n"
	"\t\n"
	"\t\tfunction sortTable(coloum,descending) {\n"
	"\t\tvar table, rows, switching, i, x, y, shouldSwitch;\n"
	"\t\ttable = document.getElementById(id=\"resultsTable\"); switching = true;\n"
	"\t\twhile (switching) {	switching = false; rows = table.getElementsByTagName(\"TR\");\n"
	"\t\t\tfor (i = 1; i < (rows.length - 1); i++) { shouldSwitch = false;\n"
	"\t\t\t\tx = rows[i].getElementsByTagName(\"TD\")[coloum];\n"
	"\t\t\t\ty = rows[i + 1].getElementsByTagName(\"TD\")[coloum];\n"
	"\t\t\t\tif(descending){if (x.innerHTML.toLowerCase() < y.innerHTML.toLowerCase()) {\n"
	"\t\t\t\t\tshouldSwitch= true;	break;}}\n"
	"\t\t\t\telse{if (x.innerHTML.toLowerCase() > y.innerHTML.toLowerCase()) {\n"
	"\t\t\t\t\tshouldSwitch= true;	break;}}}\n"
	"\t\t\t\tif (shouldSwitch) {	rows[i].parentNode.insertBefore(rows[i + 1], rows[i]);\n"
	"\t\t\t\t\tswitching = true;}}}\n"
	"\t\n"
	"\t\n"
	"\t</script>\n"
	"\t<script src=\"https://www.kryogenix.org/code/browser/sorttable/sorttable.js\"></script>\n"
	"\t<script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>\n"
	"\t<script type=\"text/javascript\">\n"
	"\t\n"
	// overall summary
	"\tgoogle.charts.load('current', {'packages':['bar']});\n"
	"\tgoogle.charts.setOnLoadCallback(drawChart);\n"
	"\tfunction drawChart(){\n"
	"\tvar data = google.visualization.arrayToDataTable([\n"
	"\t['  '     ,  'CPU Jobs'  , 'GPU Jobs'],\n"
	"\t['Summary',   50     , 50       ]\n"
	"\t]);\n"
	"\tvar options = { title: 'Overall Result Summary', vAxis: { title: 'Jobs' }, width: 400, height: 400 };\n"
	"\tvar chart = new google.charts.Bar(document.getElementById('Model_Stats'));\n"
	"\tchart.draw(data, google.charts.Bar.convertOptions(options));}\n"
	"\t\n"
	"\t</script>\n"
	// side view
	"\t<div class=\"navbar\">\n"
	"\t<a href=\"#\">\n"
	"\t<div id=\"main\">\n"
	"\t<span style=\"font-size:30px;cursor:pointer\" onclick=\"openNav()\">&#9776; Views</span>\n"
	"\t</div></a>\n"
	"\t<a href=\"https://github.com/kiritigowda/SRTG-Schedule\" target=\"_blank\">\n"
	"\t<img \" src=\"https://assets-cdn.github.com/images/modules/logos_page/GitHub-Mark.png\" alt=\"AMD GitHub\" height=\"65\"/></a>\n"
	//"\t<img \" src=\"icons/AIToolKit_400x90.png\" alt=\"AMD Inference ToolKit\" hspace=\"190\"/> \n"
	"\t</div>\n"
	"\t\n"
	"<A NAME=\"table0\"><h1 align=\"center\"><font color=\"DodgerBlue\" size=\"6\"><br><br><br><em>Overall Summary</em></font></h1></A>\n"
	"\t<table align=\"center\">\n"
	"\t<col width=\"265\">\n"
	"\t<col width=\"50\">\n"
	"\t<tr>\n"
	"\t<td><font color=\"black\" size=\"4\">Jobs <b>Sent To GPU</b></font></td>\n"
	"\t<td align=\"center\"><font color=\"black\" size=\"4\"><b>%d</b></font></td>\n"
	"\t</tr>\n"
	"\t<tr>\n"
	"\t<td><font color=\"black\" size=\"4\">Jobs <b>Sent To CPU</b></font></td>\n"
	"\t<td align=\"center\"><font color=\"black\" size=\"4\"><b>%d</b></font></td>\n"
	"\t</tr>\n"
	"\t<tr>\n"
	"\t<td><font color=\"black\" size=\"4\"><b>Total Jobs</b> sent for Schdeule</font></td>\n"
	"\t<td align=\"center\"><font color=\"black\" size=\"4\"><b>%d</b></font></td>\n"
	"\t</tr>\n"
	"\t</table>\n"
	// Graph
	"<A NAME=\"table1\"><h1 align=\"center\"><font color=\"DodgerBlue\" size=\"6\"><br><br><br><em>Graphs</em></font></h1></A>\n"
	"\t<center><div id=\"Model_Stats\" style=\"border: 1px solid #ccc\"></div></center>\n"
	"\t\n"
	// Schedule results
	"<A NAME=\"table2\"><h1 align=\"center\"><font color=\"DodgerBlue\" size=\"6\"><br><br><br><em>Schedule Results</em></font></h1></A>\n"
	"<table id=\"resultsTable\" class=\"sortable\" align=\"center\" cellspacing=\"2\" border=\"0\" style=\"width: 70%\">\n"
	"<tr>\n"
	"<td align = \"center\"><font color = \"Maroon\"><p onclick = \"sortTable(0,0)\"><b>Job</b></p></font></td>\n"
	"<td align = \"center\"><font color = \"Maroon\"><p onclick = \"sortTable(1,0)\"><b>Processors</b></p></font></td>\n"
	"<td align = \"center\"><font color = \"Maroon\"><p onclick = \"sortTable(2,0)\"><b>Execution Time</b></p></font></td>\n"
	"<td align = \"center\"><font color = \"Maroon\"><p onclick = \"sortTable(3,0)\"><b>Deadline</b></p></font></td>\n"
	"<td align = \"center\"><font color = \"Maroon\"><p onclick = \"sortTable(4,0)\"><b>Release Time</b></p></font></td>\n"
	"<td align = \"center\"><font color = \"Maroon\"><p onclick = \"sortTable(5,0)\"><b>Scheduler Overhead(microsec)</b></p></font></td>\n"
	"<td align = \"center\"><font color = \"Maroon\"><p onclick = \"sortTable(6,0)\"><b>Scheduled At</b></p></font></td>\n"
	"<td align = \"center\"><font color = \"Maroon\"><p onclick = \"sortTable(7,0)\"><b>Rescheduled to</b></p></font></td>\n"
	"<td align = \"center\"><font color = \"Maroon\"><p onclick = \"sortTable(8,0)\"><b>Completion Time</b></p></font></td>\n"
	"<td align = \"center\"><font color = \"Maroon\"><b>Scheduled Hardware</b></font></td>\n"
	"</tr>\n";

const char *summaryHTMLFooter =
	"</table>\n"
	"<A NAME=\"table3\"><h1 align=\"center\"><font color=\"DodgerBlue\" size=\"6\"><br><br><br><em>Compare</em></font></h1></A>\n"
	"\t\n"
	"\t\n"
	"<A NAME=\"table4\"><h1 align=\"center\"><font color=\"DodgerBlue\" size=\"6\"><br><br><br><em>Help</em></font></h1></A>\n"
	"\t\n"
	"\n</body>\n"
	"\n</html>\n";

int RTGS_PrintScheduleSummary(int mode, int maxKernels, jobAttributes *kernelInfoList)
{
#if _WIN32
	CreateDirectory("RTGS-Summary", NULL);
#else
	struct stat st = {0};
	if (stat("RTGS-Summary", &st) == -1)
	{
		mkdir("RTGS-Summary", 0700);
	}
#endif

	char profiler[1024] = "RTGS-Summary/RTGS";

	char pCSVfile[1024];
	sprintf(pCSVfile, "%s-Mode-%d-Job-Summary.csv", profiler, mode);
	char pHTMLfile[1024];
	sprintf(pHTMLfile, "%s-Mode-%d-Job-Summary.html", profiler, mode);
	char pSummaryfile[1024];
	sprintf(pSummaryfile, "%s-Mode-%d-Summary.html", profiler, mode);
	char pModeSummaryFile[1024];
	sprintf(pModeSummaryFile, "%s-Mode-%d-Summary.csv", profiler, mode);

	if (GLOBAL_RTGS_DEBUG_MSG > 1)
	{
		FILE *fp = fopen(pCSVfile, "w");
		if (!fp)
		{
			printf("ERROR: unable to create '%s'\n", pCSVfile);
			return RTGS_ERROR_NO_RESOURCES;
		}
		fprintf(fp, "Job,Processors,Execution Time,Deadline,Release Time,Scheduler Overhead (microSec),Scheduled At,Rescheduled to,Completion Time,Scheduled Hardware\n");
		for (int i = 0; i < maxKernels; i++)
		{
			fprintf(fp, "%d,%d,%d,%d,%d,%.2f,%d,%d,%d,%d\n", i,
					kernelInfoList[i].processor_req,
					kernelInfoList[i].execution_time,
					kernelInfoList[i].deadline,
					kernelInfoList[i].release_time,
					kernelInfoList[i].schedule_overhead * 100,
					kernelInfoList[i].scheduled_execution,
					kernelInfoList[i].rescheduled_execution,
					kernelInfoList[i].completion_time,
					kernelInfoList[i].schedule_hardware);
		}
		fclose(fp);

		FILE *fh = fopen(pHTMLfile, "w");
		if (!fh)
		{
			printf("ERROR: unable to create '%s'\n", pHTMLfile);
			return RTGS_ERROR_NO_RESOURCES;
		}
		fprintf(fh, HTML_header, mode);
		int width = 1000, height = 400;
		int xstart = 300, max_time = 0;
		int offset = 50;
		for (int i = 0; i < maxKernels; i++)
		{
			max_time = MAX(max_time, kernelInfoList[i].completion_time);
		}
		max_time += 10;
		// Timing header
		for (int k = 0; k <= max_time; k += 5)
		{
			int barx = xstart + (k * 10);
			fprintf(fh, "    d = document.createElement('div'); d.title = '%d T'; d.className='time0'; d.style.backgroundColor='#800000'; d.style.top='%dpx'; d.style.left='%dpx'; d.style.width='%dpx'; document.getElementsByTagName('body')[0].appendChild(d);\n",
					k, 15, barx - 1, 1);
			fprintf(fh, "    d = document.createElement('div'); e = document.createTextNode('%3d T'); d.appendChild(e); d.className='time1'; d.style.backgroundColor='#FFFFFF'; d.style.top='%dpx'; d.style.left='%dpx'; document.getElementsByTagName('body')[0].appendChild(d);\n",
					k, 15, barx + 2);
		}

		// Jobs scheduling pattern
		for (int k = 0; k < maxKernels; k++)
		{

			float schedulerOverhead = kernelInfoList[k].schedule_overhead * 100;

			// plot release time
			float start = (float)kernelInfoList[k].release_time, duration = 0.2f;
			int barx = xstart + (int)(start * 10);
			int barw = (int)(duration * 10);
			fprintf(fh, "    d = document.createElement('div'); d.title = 'Release'; d.className='time1'; d.style.top='%dpx'; d.style.left='%dpx'; d.style.width='%dpx'; document.getElementsByTagName('body')[0].appendChild(d);\n",
					(offset + 50 * (k + 1)), barx, barw);
			fprintf(fh, "    d = document.createElement('div'); d.title = 'Release @ %5.3f T'; d.className='time0'; d.style.backgroundColor='yellow'; d.style.top='%dpx'; d.style.left='%dpx'; d.style.width='%dpx'; document.getElementsByTagName('body')[0].appendChild(d);\n",
					start, (offset + 50 * (k + 1)), barx, barw);

			// plot scheduler overhead
			start = (float)(0.2 + kernelInfoList[k].release_time);
			duration = 0.2f;
			barx = xstart + (int)(start * 10);
			barw = (int)(duration * 10);
			fprintf(fh, "    d = document.createElement('div'); d.title = 'Scheduler-Overhead'; d.className='time1'; d.style.top='%dpx'; d.style.left='%dpx'; d.style.width='%dpx'; document.getElementsByTagName('body')[0].appendChild(d);\n",
					(offset + 50 * (k + 1)), barx, barw);
			fprintf(fh, "    d = document.createElement('div'); d.title = 'Scheduler Overhead of %5.3f microsec'; d.className='time0'; d.style.backgroundColor='blue'; d.style.top='%dpx'; d.style.left='%dpx'; d.style.width='%dpx'; document.getElementsByTagName('body')[0].appendChild(d);\n",
					schedulerOverhead, (offset + 50 * (k + 1)), barx, barw);

			// plot scheduled jobs
			if (kernelInfoList[k].completion_time != -1)
			{
				if (kernelInfoList[k].scheduled_execution == kernelInfoList[k].release_time)
				{
					start = (float)(0.4 + kernelInfoList[k].scheduled_execution);
					duration = (float)kernelInfoList[k].execution_time;
					barx = xstart + (int)(start * 10);
					barw = (int)(duration * 10);
				}
				else
				{
					start = (float)kernelInfoList[k].scheduled_execution;
					duration = (float)kernelInfoList[k].execution_time;
					barx = xstart + (int)(start * 10);
					barw = (int)(duration * 10);
				}
				float ScheduledTime = (float)kernelInfoList[k].scheduled_execution;
				float completionTime = (float)kernelInfoList[k].completion_time;
				fprintf(fh, "    d = document.createElement('div'); d.title = 'Scheduled'; d.className='time1'; d.style.top='%dpx'; d.style.left='%dpx'; d.style.width='%dpx'; document.getElementsByTagName('body')[0].appendChild(d);\n",
						(offset + 50 * (k + 1)), barx, barw);
				fprintf(fh, "    d = document.createElement('div'); d.title = 'Scheduled @ %5.3f T -- Completion @ %5.3f T'; d.className='time0'; d.style.backgroundColor='green'; d.style.top='%dpx'; d.style.left='%dpx'; d.style.width='%dpx'; document.getElementsByTagName('body')[0].appendChild(d);\n",
						ScheduledTime, completionTime, (offset + 50 * (k + 1)), barx, barw);
			}

			// plot returned jobs
			if (kernelInfoList[k].completion_time == -1)
			{
				start = (float)(kernelInfoList[k].release_time + 0.4);
				duration = 0.5f;
				barx = xstart + (int)(start * 10);
				barw = (int)(duration * 10);
				float releaseTime = (float)kernelInfoList[k].release_time;
				fprintf(fh, "    d = document.createElement('div'); d.title = 'CPU-JOB'; d.className='time1'; d.style.top='%dpx'; d.style.left='%dpx'; d.style.width='%dpx'; document.getElementsByTagName('body')[0].appendChild(d);\n",
						(offset + 50 * (k + 1)), barx, barw);
				fprintf(fh, "    d = document.createElement('div'); d.title = 'Sent Back to CPU @ Release-%5.3f T + overhead-%5.3f microsec'; d.className='time0'; d.style.backgroundColor='red'; d.style.top='%dpx'; d.style.left='%dpx'; d.style.width='%dpx'; document.getElementsByTagName('body')[0].appendChild(d);\n",
						releaseTime, schedulerOverhead, (offset + 50 * (k + 1)), barx, barw);
			}

			if (width < (barx + barw))
			{
				width = barx + barw;
			}
		}

		// Job ids
		for (int e = 0; e < maxKernels; e++)
		{
			fprintf(fh, "    d = document.createElement('div'); e = document.createTextNode('Job-%d'); d.appendChild(e); d.className='name0'; d.style.top='%dpx'; document.getElementsByTagName('body')[0].appendChild(d);\n",
					e, (offset + 50 * (e + 1)) + 3);
			fprintf(fh, "    d = document.createElement('div'); e = document.createTextNode('%d(Pn) %d(Texe) %d(Td) %d(Tls)'); d.appendChild(e); d.className='name1'; d.style.top='%dpx'; document.getElementsByTagName('body')[0].appendChild(d);\n",
					kernelInfoList[e].processor_req, kernelInfoList[e].execution_time, kernelInfoList[e].deadline, kernelInfoList[e].latest_schedulable_time, (offset + 50 * (e + 1)) + 3 + 18);
		}
		height = offset + maxKernels * 50;
		fprintf(fh, HTML_footer, xstart - 50, width - xstart + 200, height);
		fclose(fh);

		FILE *fs = fopen(pSummaryfile, "w");
		if (!fs)
		{
			printf("ERROR: unable to create '%s'\n", pSummaryfile);
			return RTGS_ERROR_NO_RESOURCES;
		}
		fprintf(fs, "%s", summaryHTMLHeader);
		// table summary
		for (int i = 0; i < maxKernels; i++)
		{
			fprintf(fs, "<tr>\n");
			fprintf(fs, "<td align = \"center\"><font color = \"black\"><b>%d</b></font></td>\n", i);
			fprintf(fs, "<td align = \"center\"><font color = \"black\"><b>%d</b></font></td>\n", kernelInfoList[i].processor_req);
			fprintf(fs, "<td align = \"center\"><font color = \"black\"><b>%d</b></font></td>\n", kernelInfoList[i].execution_time);
			fprintf(fs, "<td align = \"center\"><font color = \"black\"><b>%d</b></font></td>\n", kernelInfoList[i].deadline);
			fprintf(fs, "<td align = \"center\"><font color = \"black\"><b>%d</b></font></td>\n", kernelInfoList[i].release_time);
			fprintf(fs, "<td align = \"center\"><font color = \"black\"><b>%.4f</b></font></td>\n", kernelInfoList[i].schedule_overhead * 100);
			fprintf(fs, "<td align = \"center\"><font color = \"black\"><b>%d</b></font></td>\n", kernelInfoList[i].scheduled_execution);
			fprintf(fs, "<td align = \"center\"><font color = \"black\"><b>%d</b></font></td>\n", kernelInfoList[i].rescheduled_execution);
			fprintf(fs, "<td align = \"center\"><font color = \"black\"><b>%d</b></font></td>\n", kernelInfoList[i].completion_time);
			if (kernelInfoList[i].schedule_hardware == 1)
				fprintf(fs, "<td align = \"center\"><font color = \"green\"><b>GPU</b></font></td>\n");
			else if (kernelInfoList[i].schedule_hardware == 2)
				fprintf(fs, "<td align = \"center\"><font color = \"red\"><b>CPU</b></font></td>\n");
			else
				fprintf(fs, "<td align = \"center\"><font color = \"blue\"><b>MISSING</b></font></td>\n");
			fprintf(fs, "</tr>\n");
		}
		fprintf(fs, "%s", summaryHTMLFooter);
		fclose(fs);
	}

	float avgProcessorUsage = 0, avgExecTime = 0, gpuJobs = 0, totalJobs = 0;
	float GPU_usageTime = 0, responseTimeAvg = 0, responseFactorAvg = 0;
	float totalGPUUsage = 0, maxReleaseTime = 0;
	float GPUJobsSchedulerOverHead = 0, avgSchedulerOverHeadAll = 0;

	for (int i = 0; i < maxKernels; i++)
	{
		if (maxReleaseTime < (float)kernelInfoList[i].release_time)
		{
			maxReleaseTime = (float)kernelInfoList[i].release_time;
		}
		float processors = (float)kernelInfoList[i].processor_req;
		float maxProcessors = GLOBAL_MAX_PROCESSORS;
		if (kernelInfoList[i].schedule_hardware == 1)
		{
			avgProcessorUsage += kernelInfoList[i].processor_req;
			avgExecTime += kernelInfoList[i].execution_time;
			float response = (float)kernelInfoList[i].completion_time - kernelInfoList[i].release_time;
			float totalTime = (float)kernelInfoList[i].deadline - kernelInfoList[i].release_time;
			responseTimeAvg += response;
			responseFactorAvg += response / totalTime;
			GPU_usageTime += ((processors / maxProcessors) * kernelInfoList[i].execution_time);
			GPUJobsSchedulerOverHead += kernelInfoList[i].schedule_overhead;
		}
		totalGPUUsage += ((processors / maxProcessors) * kernelInfoList[i].execution_time);
		avgSchedulerOverHeadAll += kernelInfoList[i].schedule_overhead;
	}
	avgProcessorUsage = avgProcessorUsage / GLOBAL_GPU_JOBS;
	avgExecTime = avgExecTime / GLOBAL_GPU_JOBS;
	responseTimeAvg = responseTimeAvg / GLOBAL_GPU_JOBS;
	responseFactorAvg = responseFactorAvg / GLOBAL_GPU_JOBS;
	float GPUTimePercentage = (GPU_usageTime / totalGPUUsage) * 100;
	gpuJobs = (float)GLOBAL_GPU_JOBS;
	totalJobs = (float)maxKernels;
	float jobScheduledPercentage = (gpuJobs / maxKernels) * 100;
	float releaseLambda = totalJobs / maxReleaseTime;
	avgSchedulerOverHeadAll = avgSchedulerOverHeadAll / totalJobs;
	GPUJobsSchedulerOverHead = GPUJobsSchedulerOverHead / gpuJobs;

	FILE *fms = fopen(pModeSummaryFile, "a");
	if (!fms)
	{
		printf("ERROR: unable to create '%s'\n", pModeSummaryFile);
		return RTGS_ERROR_NO_RESOURCES;
	}
	fprintf(fms, "%d,%d,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.6f,%.6f\n", GLOBAL_GPU_JOBS, maxKernels, avgProcessorUsage,
			avgExecTime, GPU_usageTime, responseTimeAvg, responseFactorAvg, GPUTimePercentage, jobScheduledPercentage,
			releaseLambda, GPUJobsSchedulerOverHead, avgSchedulerOverHeadAll);
	fclose(fms);
	return RTGS_SUCCESS;
}

//! \brief Check GPU Offload readiness
int RTGS_checkGPUReadiness()
{
	printf("GPU Offloading Usage:\n\n");
	printf("\tPlease check instructions on how to recompile the SRTG-Scheduler to use AMD or NVIDIA GPU in the documentation\n\n");
	printf("GPU Offloading Requirements:\n");
	printf("\tAMD GPU:\n");
	printf("\t\t ROCm Supported AMD GPU - https://rocmdocs.amd.com/en/latest/ \n");
	printf("\t\t Latest AMD GPU Driver\n");
	printf("\t\t OpenCL\n");
	printf("\t\t AMD HSA Runtime API\n");
	printf("\tNVIDIA GPU:\n");
	printf("\t\t CUDA 6 and Above Supported NVIDIA GPU - https://developer.nvidia.com/cuda-toolkit \n");
	printf("\t\t Latest NVIDIA GPU Driver\n");
	printf("\t\t CUDA Toolkit\n");
	printf("\n\n");
	return RTGS_ERROR_RECOMPILE;
}

// insert a pre-scheduled job in a given position
genericBackupNode *position_insert_preScheduledJobs(
	genericBackupNode *head,
	genericBackupNode *positionInsertVariable,
	int position)
{
	genericBackupNode *temp = head;
	genericBackupNode *temp1 = positionInsertVariable;
	int count = 0;

	if (temp == NULL)
	{
		head = temp1;
		return head;
	}

	if (position == 1)
	{
		temp1->next = head;
		head = temp1;
		return head;
	}

	while (temp != NULL)
	{
		++count;
		if (count == position - 1)
		{
			temp1->next = temp->next;
			temp->next = temp1;
			return head;
		}
		temp = temp->next;
	}

	if (count + 1 == position)
	{
		head = insert_preScheduledJobs(head, positionInsertVariable);
		return head;
	}

	printf("ERROR position_insert_preScheduledJobs - Position insert Failed\n");
	return head;
}

// insert pre-scheduled jobs function
genericBackupNode *insert_preScheduledJobs(
	genericBackupNode *head,
	genericBackupNode *insertVariable)
{
	genericBackupNode *temp;
	genericBackupNode *temp1 = insertVariable;

	if (head == NULL)
	{
		head = temp1;
	}
	else
	{
		temp = head;
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = temp1;
	}
	return head;
}

// ascending insert pre scheduled jobs function
genericBackupNode *ascending_insert_preScheduledJobs(
	genericBackupNode *head,
	int job_release_time,
	int processor_release_time,
	int processors_allocated,
	int jobNumber)
{
	int count = 1;
	genericBackupNode *temp = head;
	genericBackupNode *temp1 = (genericBackupNode *)malloc(sizeof(scheduledResourceNode));
	//Values into the variable
	temp1->data = job_release_time;
	temp1->jobNumber = jobNumber;
	temp1->processor_release_time = processor_release_time;
	temp1->processors_allocated = processors_allocated;
	temp1->processors_requested = processors_allocated;
	temp1->next = NULL;

	if (head == NULL)
	{
		head = temp1;
		return head;
	}
	else if (job_release_time < temp->data)
	{
		temp1->next = head;
		head = temp1;
		return head;
	}
	else
	{
		while (temp != NULL)
		{
			if (job_release_time < temp->data)
			{
				head = position_insert_preScheduledJobs(head, temp1, count);
				return head;
			}
			else if (job_release_time == temp->data)
			{
				count++;
				head = position_insert_preScheduledJobs(head, temp1, count);
				return head;
			}
			temp = temp->next;
			count++;
		}
	}

	head = insert_preScheduledJobs(head, temp1);
	return head;
}

// Backup processor jobBackupList
genericBackupNode *insert_preScheduledJob_list(
	genericBackupNode *head,
	int job_release_time,
	int processor_release_time,
	int processors_allocated,
	int jobNumber)
{
	head = ascending_insert_preScheduledJobs(head, job_release_time, processor_release_time, processors_allocated, jobNumber);
	return head;
}

// delete a pre-scheduled job by position
genericBackupNode *position_delete_preScheduledJob(genericBackupNode *head, int position)
{
	genericBackupNode *temp;
	genericBackupNode *temp1;
	int count = 1;
	temp = head;

	if (temp == NULL)
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 1)
		{
			printf("The List is empty\n");
		}
		return head;
	}

	if (position == 1)
	{
		head = temp->next;
		free(temp);
		return head;
	}

	while (temp->next != NULL)
	{
		if (count == (position - 1))
		{
			temp1 = temp->next;
			temp->next = temp1->next;
			free(temp1);
			break;
		}
		temp = temp->next;
		++count;
	}
	return head;
}

// Backup processor jobBackupList
genericBackupNode *insert_node(genericBackupNode *head, int x)
{
	genericBackupNode *temp = (genericBackupNode *)malloc(sizeof(genericBackupNode));

	temp->data = x;
	temp->processor_release_time = 0;
	temp->processors_allocated = 0;
	temp->next = NULL;

	if (head == NULL)
	{
		head = temp;
	}
	else
	{
		genericBackupNode *temp1;
		temp1 = head;
		while (temp1 != NULL)
		{
			if (temp1->next == NULL)
			{
				temp1->next = temp;
				return head;
			}
			temp1 = temp1->next;
		}
	}
	return head;
}

//clean jobBackupList
genericBackupNode *clean_list(genericBackupNode *head)
{
	genericBackupNode *temp1;

	while (head != NULL)
	{
		temp1 = head->next;
		free(head);
		head = temp1;
	}
	return head;
}

//Ascending insert function
scheduledResourceNode *ascending_insert(
	scheduledResourceNode *head,
	int ascendingVariable,
	int processor_release_time,
	int processorReleased,
	int jobNumber,
	int schedule_method)
{
	int count = 1, flag = 0;
	scheduledResourceNode *temp = head;
	scheduledResourceNode *temp1 = (scheduledResourceNode *)malloc(sizeof(scheduledResourceNode));
	//Values into the variable
	temp1->job_release_time = 0;
	temp1->data = ascendingVariable;
	temp1->processor_release_time = processor_release_time;
	temp1->processors_allocated = processorReleased;
	temp1->schedule_method = schedule_method;
	temp1->jobNumber = jobNumber;
	temp1->next = NULL;
	temp1->job_next = NULL;

	if (head == NULL)
	{
		head = temp1;
		flag = 1;
	}
	else if (ascendingVariable <= temp->data)
	{
		temp1->next = head;
		head = temp1;
		head = remove_recurring_node(head);
		flag = 1;
	}
	else
	{
		while (temp != NULL)
		{
			if (ascendingVariable <= temp->data)
			{
				head = position_insert(head, temp1, count);
				head = remove_recurring_node(head);
				flag = 0;
				return head;
			}
			temp = temp->next;
			count++;
		}
	}

	if (flag == 0)
	{
		head = insert(head, temp1);
		head = remove_recurring_node(head);
	}
	return head;
}

//Remove recurring variables function
scheduledResourceNode *remove_recurring_node(scheduledResourceNode *head)
{
	scheduledResourceNode *temp, *kernel_check;
	scheduledResourceNode *temp1;

	temp = head;
	while (temp->next != NULL)
	{
		temp1 = temp->next;
		if (temp->data == temp1->data)
		{
			scheduledResourceNode *t1 = (scheduledResourceNode *)malloc(sizeof(scheduledResourceNode));
			scheduledResourceNode *t2 = (scheduledResourceNode *)malloc(sizeof(scheduledResourceNode));

			t1->data = temp->data;
			t1->job_release_time = temp->job_release_time;
			t1->processor_release_time = temp->processor_release_time;
			t1->processors_allocated = temp->processors_allocated;
			t1->schedule_method = temp->schedule_method;
			t1->jobNumber = temp->jobNumber;
			t1->next = NULL;
			t1->job_next = NULL;

			t2->data = temp1->data;
			t2->job_release_time = temp1->job_release_time;
			t2->processor_release_time = temp1->processor_release_time;
			t2->processors_allocated = temp1->processors_allocated;
			t2->schedule_method = temp1->schedule_method;
			t2->jobNumber = temp1->jobNumber;
			t2->next = NULL;
			t2->job_next = temp1->job_next;

			if (t2->job_next == NULL)
			{
				temp->job_next = t2;
				t2->job_next = t1;
			}
			else
			{
				free(t2);
				temp->job_next = temp1->job_next;
				kernel_check = temp1->job_next;
				while (kernel_check->job_next != NULL)
					kernel_check = kernel_check->job_next;

				if (kernel_check->job_next == NULL)
					kernel_check->job_next = t1;
			}
			temp->processors_allocated = temp->processors_allocated + temp1->processors_allocated;
			temp->jobNumber = MULTIPLE_JOBS_SCHEDULED;
			temp->job_release_time = MULTIPLE_JOBS_SCHEDULED;
			temp->next = temp1->next;

			free(temp1);

			if (temp->next == NULL)
			{
				return head;
			}
		}
		temp = temp->next;
	}
	return head;
}

//Insert a variable function
scheduledResourceNode *insert(scheduledResourceNode *head, scheduledResourceNode *insertVariable)
{
	scheduledResourceNode *temp;
	scheduledResourceNode *temp1 = insertVariable;

	if (head == NULL)
		head = temp1;
	else
	{
		temp = head;
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = temp1;
	}
	return head;
}

//Insert a variable in a given position
scheduledResourceNode *position_insert(scheduledResourceNode *head, scheduledResourceNode *positionInsertVariable, int p)
{
	scheduledResourceNode *temp;
	scheduledResourceNode *temp1;
	int count = 1;
	temp = head;
	temp1 = positionInsertVariable;

	if (p == 1)
	{
		temp1->next = head;
		head = temp1;
		return head;
	}

	if (temp == NULL)
	{
		head = temp1;
		return head;
	}

	while (temp->next != NULL)
	{
		if (count == (p - 1))
		{
			temp1->next = temp->next;
			temp->next = temp1;
			return head;
		}
		temp = temp->next;
		++count;
	}
	if (count == p + 1)
	{
		head = insert(head, positionInsertVariable);
		return head;
	}

	return head;
}

//Delete a node from the jobBackupList
scheduledResourceNode *position_delete(scheduledResourceNode *head, int p)
{
	scheduledResourceNode *temp;
	scheduledResourceNode *temp1;
	int count = 1;
	temp = head;

	if (temp == NULL)
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 1)
		{
			printf("The List is empty\n");
		}
		return head;
	}

	if (p == 1)
	{
		head = temp->next;
		free(temp);
		return head;
	}

	while (temp->next != NULL)
	{
		if (count == (p - 1))
		{
			temp1 = temp->next;
			temp->next = temp1->next;

			free(temp1);
			return head;
		}

		temp = temp->next;
		++count;
	}
	return head;
}

// Print the processors retrived list
void print_processorsAllocated_list(scheduledResourceNode *head)
{
	scheduledResourceNode *temp;
	temp = head;
	printf("Scheduled Job List\n");
	while (temp != NULL)
	{
		if (temp->jobNumber != MULTIPLE_JOBS_SCHEDULED)
		{
			printf("	Job-%d	-- Completion Time:%d,	Processors Retrived:%d\n", temp->jobNumber, temp->data, temp->processors_allocated);
		}
		else
		{
			scheduledResourceNode *temp1 = temp->job_next;
			printf("	Job-MJ	-- Completion Time:%d,	Processors Retrived:%d\n", temp->data, temp->processors_allocated);
			while (temp1 != NULL)
			{
				printf("	      	   Job-%d	-- Schdeuled for Completion\n", temp1->jobNumber);
				temp1 = temp1->job_next;
			}
		}
		temp = temp->next;
	}
	return;
}

//Print the job pre scheduled list
void print_preScheduledJob_list(scheduledResourceNode *head)
{
	scheduledResourceNode *temp;
	temp = head;
	printf("Jobs Scheduled for GPU Execution\n");
	while (temp != NULL)
	{
		if (temp->jobNumber != MULTIPLE_JOBS_SCHEDULED)
		{
			printf("	Job-%d	-- Job Release Time:%d,	Processor Allocated:%d\n", temp->jobNumber, temp->data, temp->processors_allocated);
		}
		else
		{
			scheduledResourceNode *temp1 = temp->job_next;
			printf("	Job-MJ	-- Job Release Time:%d,	Processor Allocated:%d\n", temp->data, temp->processors_allocated);
			while (temp1 != NULL)
			{
				printf("	      	   Job-%d	-- Schdeuled for Release -- Processors Allocated:%d\n", temp1->jobNumber, temp1->processors_allocated);
				temp1 = temp1->job_next;
			}
		}
		temp = temp->next;
	}
	return;
}

//Print the job pre scheduled list
void print_preQueuedJob_list(genericBackupNode *head)
{
	genericBackupNode *temp;
	temp = head;
	printf("Jobs Queued for GPU Execution\n");
	while (temp != NULL)
	{
		printf("	Job-%d	-- Job Release Time:%d,	Processor Allocated:%d\n", temp->jobNumber, temp->data, temp->processors_requested);
		temp = temp->next;
	}
	return;
}
