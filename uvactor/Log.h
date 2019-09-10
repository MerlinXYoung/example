#pragma once

#define _UVACTOR_LOG
#ifdef _UVACTOR_LOG

#define uvactor_log(fmt, ...) printf(fmt"\n", __VA_ARGS__)

#else
#define uvactor_log(...) 
#endif