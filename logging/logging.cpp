
#include "logging.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

#ifdef WIN32
# include <windows.h>
	LARGE_INTEGER PerformanceFrequency;
	struct _SYSTEMTIME basetime;
	LARGE_INTEGER basecount;
#else
# include <sys/time.h>
#endif

int LoggingThreshold = 0;

#ifndef DISABLE_LOGGING
int TraceCount;

char BUFFER[4096];

FILE *logfile = NULL;
char logname[1024];
#endif // DISABLE_LOGGING

void datetime()
{
#ifndef DISABLE_LOGGING
#ifdef WIN32
	static long long msec = 0;
	static struct _SYSTEMTIME calctime;
//	static struct _SYSTEMTIME wintime;
//	GetLocalTime(&wintime);
//	msec = wintime.wMilliseconds;
	static LARGE_INTEGER PerformanceCount;
	QueryPerformanceCounter(&PerformanceCount);
	msec = (long long)(PerformanceCount.QuadPart * 1000.0 / PerformanceFrequency.QuadPart);
	calctime = basetime;
	calctime.wMilliseconds += msec;
	calctime.wSecond += calctime.wMilliseconds / 1000;
	calctime.wMinute += calctime.wSecond / 60;
	calctime.wHour += calctime.wMinute / 60;
	calctime.wMilliseconds %= 1000;
	calctime.wSecond %= 60;
	calctime.wMinute %= 60;
	calctime.wHour %= 24;
	fprintf(logfile, "%02i:%02i:%02i.%03i", calctime.wHour, calctime.wMinute, calctime.wSecond, calctime.wMilliseconds);
#else
	static int msec = 0;
	static char mytime[50];
	static struct timeval smalltime;
	gettimeofday(&smalltime, NULL);
	msec = smalltime.tv_usec / 1000;
	_strtime(mytime);
	fprintf(logfile, "%s.%03i", mytime, msec);
#endif
	for (int i = 0; i < TraceCount; i++)
		fprintf(logfile, "  ");
#endif
}

void StartLogFile(char *filename, bool append)
{
#ifndef DISABLE_LOGGING
#ifdef WIN32
	LARGE_INTEGER PerformanceCount;
	GetLocalTime(&basetime);
	int msec = basetime.wMilliseconds;
	while (basetime.wMilliseconds == msec)
		GetLocalTime(&basetime);
	msec = basetime.wMilliseconds;
	QueryPerformanceCounter(&basecount);
	QueryPerformanceFrequency(&PerformanceFrequency);
//	Add_msec = 1000 + msec - ((long long)(PerformanceCount.QuadPart * 1000.0 / PerformanceFrequency.QuadPart) % 1000);
#endif
	if (LoggingThreshold < 0)
		return;
	if (!strncmp(filename, logname, strlen(filename)))
		return;
	strncpy(logname, filename, 1023);
	logname[1023] = 0;
	if (logfile)
		fclose(logfile);
	TraceCount = 0;
	logfile = fopen(filename, append ? "a" : "w");
	if (logfile)
	{
		fprintf(logfile, "\n");
		datetime();
		fprintf(logfile, "  Logging started\n");
		fflush(logfile);
	}
#endif
}

void EndLogFile()
{
#ifndef DISABLE_LOGGING
	if (logfile)
	{
		datetime();
		fprintf(logfile, "  Logging ended\n\n");
		fflush(logfile);
		fclose(logfile);
	}
#endif
}


void SendLogMessage(int priority, char *fmt, ...)
{
#ifndef DISABLE_LOGGING
	if ((priority >= LoggingThreshold) && logfile)
	{
		va_list args;
		va_start(args, fmt);
		datetime();
		vsprintf(BUFFER, fmt, args);
		fprintf(logfile, "  %s\n", BUFFER);
		fflush(logfile);
	}
#endif
}

void SendLogMessage(char *fmt, ...)
{
#ifndef DISABLE_LOGGING
	if (logfile)
	{
		va_list args;
		va_start(args, fmt);
		datetime();
		vsprintf(BUFFER, fmt, args);
		fprintf(logfile, "  %s\n", BUFFER);
		fflush(logfile);
	}
#endif
}


TraceMethod::TraceMethod(int priority, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
#ifndef DISABLE_LOGGING
	fThreshold = priority;
	methodname = NULL;
	if (priority >= LoggingThreshold)
	{
		vsprintf(BUFFER, fmt, args);
		methodname = new char [strlen(BUFFER) + 1];
		strcpy(methodname, BUFFER);
		if (logfile)
		{
			datetime();
			fprintf(logfile, "  > %s\n", methodname);
			fflush(logfile);
		}
		TraceCount++;
	}
#endif
}

TraceMethod::TraceMethod(char *fmt, ...)
{
#ifndef DISABLE_LOGGING
	va_list args;
	va_start(args, fmt);
	fThreshold = -1;
	methodname = NULL;
	vsprintf(BUFFER, fmt, args);
	methodname = new char [strlen(BUFFER) + 1];
	strcpy(methodname, BUFFER);
	if (logfile)
	{
		datetime();
		fprintf(logfile, "  > %s\n", methodname);
		fflush(logfile);
	}
	TraceCount++;
#endif
}

TraceMethod::~TraceMethod()
{
#ifndef DISABLE_LOGGING
	if (methodname)
		TraceCount--;
	if ((fThreshold >= LoggingThreshold) || (fThreshold == -1))
	{
		if (logfile)
		{
			datetime();
			fprintf(logfile, "  < %s\n", methodname);
			fflush(logfile);
		}
	}
	if (methodname)
		delete [] methodname;
#endif
}

