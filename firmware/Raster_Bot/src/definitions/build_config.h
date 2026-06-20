/*
 * build_config.h - Compile-time build/variant selection
 *
 * Flags defined here select between hardware/build variants and are
 * shared by the other definition headers so a single toggle drives
 * values that must stay consistent across files (e.g. gear ratio and
 * motor/encoder wiring orientation).
 */

#pragma once

// Motor variant: 1 for 150:1 gearmotors, 0 for 100:1 gearmotors.
// May be overridden as a compiler build flag (-DMOTOR_150=1).
#ifndef MOTOR_150
#define MOTOR_150 0
#endif
