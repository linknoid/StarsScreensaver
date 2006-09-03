
#include "logging.h"
#include <string.h>
#include <stdio.h>
#include <time.h>


int LoggingThreshold = 0;
int TraceCount;

char BUFFER[4096];

FILE *logfile = NULL;

void datetime()
{
#ifndef DISABLE_LOGGING
	static char mytime[50];
	_strtime(mytime);
	fprintf(logfile, mytime);
	for (int i = 0; i < TraceCount; i++)
		fprintf(logfile, "  ");
#endif
}

void StartLogFile(char *filename, bool append)
{
#ifndef DISABLE_LOGGING
	if (LoggingThreshold < 0)
		return;
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
		datetime();
		va_list args;
		va_start(args, fmt);
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
		datetime();
		va_list args;
		va_start(args, fmt);
		vsprintf(BUFFER, fmt, args);
		fprintf(logfile, "  %s\n", BUFFER);
		fflush(logfile);
	}
#endif
}


MethodTrace::MethodTrace(int priority, char *fmt, ...)
{
#ifndef DISABLE_LOGGING
	fThreshold = priority;
	methodname = NULL;
	if (priority >= LoggingThreshold)
	{
		TraceCount++;
		va_list args;
		va_start(args, fmt);
		vsprintf(BUFFER, fmt, args);
		methodname = new char [strlen(BUFFER) + 1];
		strcpy(methodname, BUFFER);
		if (logfile)
		{
			datetime();
			fprintf(logfile, "  > %s\n", methodname);
			fflush(logfile);
		}
	}
#endif
}

MethodTrace::MethodTrace(char *fmt, ...)
{
#ifndef DISABLE_LOGGING
	fThreshold = 0;
	methodname = NULL;
	if (LoggingThreshold == 0)
	{
		TraceCount++;
		va_list args;
		va_start(args, fmt);
		vsprintf(BUFFER, fmt, args);
		methodname = new char [strlen(BUFFER) + 1];
		strcpy(methodname, BUFFER);
		if (logfile)
		{
			datetime();
			fprintf(logfile, "  > %s\n", methodname);
			fflush(logfile);
		}
	}
#endif
}

MethodTrace::~MethodTrace()
{
#ifndef DISABLE_LOGGING
	if ((fThreshold >= LoggingThreshold) && logfile)
	{
		datetime();
		fprintf(logfile, "  < %s\n", methodname);
		fflush(logfile);
		TraceCount--;
	}
	if (methodname)
		delete [] methodname;
#endif
}

