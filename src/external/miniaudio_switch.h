/*
Required file to make miniaudio.h compile to Switch
*/
#ifndef miniaudio_switch_h
#define miniaudio_switch_h

int pthread_attr_setschedpolicy(void *attr, int policy) { return 0; }
int sched_get_priority_max(int policy) { return 0; }
int sched_get_priority_min(int policy) { return 0; }

#endif
