/********************************
 *	this file is used to stat more user info
 *	created by Jerry Zhou
 *	2005/10/16 20:27
 *********************************/

#ifndef _USERSTAT_H
#define _USERSTAT_H

#if defined (c_plusplus) || defined (__cplusplus)
extern "C"
{
#endif

#define RULES_MAX				12
#define INDEX_MIN				0
#define INDEX_MAX				1

    typedef struct iprule
    {
        unsigned long match;
        unsigned long mask;
        unsigned long range[2];

        //show area of the campus. 'X' = xuhui, 'M' = minhang. 'U' = unkown.
        char area;

        //show whether there are iprules which has the same match but different range.
        int isRanged;
    } IPRULE;

    typedef struct iptables
    {
        IPRULE iprules[RULES_MAX];
    } IPTABLES;

    struct userstat
    {
        int malesum;
        int femalesum;
        int incamsum;
        int outcamsum;
        int xuhuisum;
        int minhangsum;
    };

    extern struct userstat userstatist();

    extern int insjtunetwork(char *ip);

#if defined (c_plusplus) || defined (__cplusplus)
}
#endif

#endif
