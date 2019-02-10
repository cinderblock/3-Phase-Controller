#pragma once

#define CONFIG_COUNT 0
#ifdef BED_CONTROLLER
#define TEMP CONFIG_COUNT
#undef CONFIG_COUNT
#define CONFIG_COUNT TEMP + 1
#undef TEMP
#endif
#ifdef QUANTUM_DRIVE
#define TEMP CONFIG_COUNT
#undef CONFIG_COUNT
#define CONFIG_COUNT TEMP + 1
#undef TEMP
#endif
#ifdef HOVER_DRIVE
#define TEMP CONFIG_COUNT
#undef CONFIG_COUNT
#define CONFIG_COUNT TEMP + 1
#undef TEMP
#endif
#if CONFIG_COUNT > 1
#error "Too many configurations selected"
#endif
#if CONFIG_COUNT < 1
#error "No configuration selected"
#endif