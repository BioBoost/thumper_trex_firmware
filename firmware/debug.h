#pragma once

#define _DEBUG_MODE

#ifdef _DEBUG_MODE
#define debug(text, ...) { Serial.print(text, ## __VA_ARGS__); }
#define debugln(text, ...) { Serial.println(text, ## __VA_ARGS__); }
#else
#define debug(format, ...) { }
#define debugln(format, ...) { }
#endif
