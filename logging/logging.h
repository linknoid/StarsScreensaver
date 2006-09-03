#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <stdarg.h>

extern int LoggingThreshold;

void StartLogFile(char *filename, bool append = true);
void EndLogFile();

void SendLogMessage(int priority, char *fmt, ...);
void SendLogMessage(char *fmt, ...);

class TraceMethod
{
#ifndef DISABLE_LOGGING
private:
	char *methodname;
	int fThreshold;
#endif
public:
	TraceMethod(int threshold, char *fmt, ...);
	TraceMethod(char *fmt, ...);
	~TraceMethod();
};


#endif /* _LOGGING_H_ */
