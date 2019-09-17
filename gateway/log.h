#pragma once

#define _log(f, format, ...) \
fprintf(f, "[%s:%s:%u]" format "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
fflush(f)


#define log_trace(format, ...) \
_log(stdout, format, ##__VA_ARGS__); 

#define log_error(format, ...) \
_log(stderr, format, ##__VA_ARGS__);



