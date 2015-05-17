#ifndef TRACE_H_
#define TRACE_H_

#define TRACE(x) trace_##x
int trace_printf(const char* format, ...);

#endif /* TRACE_H_ */
