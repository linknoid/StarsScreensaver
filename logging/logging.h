#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <stdarg.h>

extern int LoggingThreshold;

void StartLogFile(char *filename, bool append = true);
void EndLogFile();

void SendLogMessage(int priority, char *fmt, ...);
void SendLogMessage(char *fmt, ...);

class MethodTrace
{
private:
	char *methodname;
	int fThreshold;
public:
	MethodTrace(int threshold, char *fmt, ...);
	MethodTrace(char *fmt, ...);
	~MethodTrace();
};


#endif /* _LOGGING_H_ */
