#ifndef zu_included
#define zu_included

/**
 * Prints to `stderr` before exiting the program with a failure status.
 */
void zu_panic(char *fmt, ...);

#ifndef zu_force_prefix
#define panic zu_panic
#endif

#endif
