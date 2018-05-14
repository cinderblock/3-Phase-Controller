/* 
 * File:   Config.h
 * Author: Cameron
 *
 * Created on December 10, 2014, 4:12 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

// #define QUANTUM_DRIVE
#define BED_CONTROLLER


#ifdef BED_CONTROLLER
#ifdef QUANTUM_DRIVE
#error "Can't have both BED_CONTROLLER and QUANTUM_DRIVE defined at once!"
#endif
#endif

#endif	/* CONFIG_H */
