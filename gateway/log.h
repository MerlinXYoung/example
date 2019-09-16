#pragma once

#define log_trace(format, ...) \
printf("%s:%s:%u"format, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)