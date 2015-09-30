/* drvSimDetector.h
 *
 * This is a driver for a simulated area detector.
 *
 * Author: /Tim Madden
 *
 * Created:  2010
 *
 */

#ifndef DRVPCO_H_
#define DRVPCO_H_


#ifdef __cplusplus
extern "C" {
#endif


int PCOConfig(const char *portName, const char *serverPort,
                            int maxBuffers, size_t maxMemory,
                            int priority, int stackSize);

extern int pco_blocking;
extern int pco_pr_mutex;
extern int pco_pr_mutex2;

#ifdef __cplusplus
}
#endif
#endif
