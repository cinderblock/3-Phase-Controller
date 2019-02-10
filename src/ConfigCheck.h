#pragma once

#define CONFIG_COUNT 0
#ifdef BED_CONTROLLER
#define CONFIG_COUNT CONFIG_COUNT + 1
#endif
#ifdef QUANTUM_DRIVE
#define CONFIG_COUNT CONFIG_COUNT + 1
#endif
#ifdef HOVER_DRIVE
#define CONFIG_COUNT CONFIG_COUNT + 1
#endif
#if CONFIG_COUNT > 1
#error "Too many configurations selected"
#endif
#if CONFIG_COUNT < 1
#error "No configuration selected"
#endif