// gfxpipe.h

#ifndef _GFXPIPE_H_
#define _GFXPIPE_H_

#include "3dmath.h"

#define GP_MINSPACE     18      // minimum space left in pipeline before flush (must be at least 36 dwords)

typedef struct gfxpipe gfxpipe;
typedef struct vecTriList vecTriList;

#pragma pack(1)
struct gfxpipe {
    unsigned long *dmatadrA;    // 'pipe 1' ... base of allotted pipeline memory
    unsigned long *dmatadrB;    // 'pipe 2' ... dmatadrA + (memsize / 2)
    unsigned int memsize;       // # of bytes allotted to the pipelines (total)

    unsigned long *curpipe;     // pointer to current 'pipe' .. may only be equal to
                                // either dmatadrA or dmatadrB
    unsigned long *curdmatadr;  // pointer to the the dma block currently being added to
    unsigned long *curgiftag;   // pointer to current "block" we can add prims to

    // need to add state information of zbuffer, alpha test, etc. in here
    int flags;          // not implemented yet
};

#pragma pack(1)
struct vecTriList {
    int numv; int d0[3];

    unsigned long giftag[2];
    vec vlist[0];   // if you say 'vecTriList *vtl = malloc(1032);' then you can access
                    //    from 'vlist[0]' through 'vlist[1000]'.
};

void gp_hardflush(gfxpipe *p);
void gp_checkflush(gfxpipe *p);
// size must be less than 1MB
int createGfxPipe(gfxpipe *pipeline, void *buffer, int size);
void gp_line(gfxpipe *p, unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned z, unsigned color);
void gp_ltriangle(gfxpipe *p, unsigned x1, unsigned y1, unsigned z1, unsigned c1,
                              unsigned x2, unsigned y2, unsigned z2, unsigned c2,
                              unsigned x3, unsigned y3, unsigned z3, unsigned c3);
void gp_triangle(gfxpipe *p, unsigned x1, unsigned y1, unsigned z1, unsigned c1,
                              unsigned x2, unsigned y2, unsigned z2, unsigned c2,
                              unsigned x3, unsigned y3, unsigned z3, unsigned c3);
void gp_ftriangle(gfxpipe *p, unsigned x1, unsigned y1, unsigned z1,
                              unsigned x2, unsigned y2, unsigned z2,
                              unsigned x3, unsigned y3, unsigned z3, unsigned color);
void gp_frect(gfxpipe *p, unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned z, unsigned color);

// doesn't work !
void gp_drawVTL_cp(gfxpipe *p, vecTriList *vtl);
// --------------

void gp_drawVTL_inplace_cpn(gfxpipe *p, vecTriList *vtl);



void gp_enablezbuf(gfxpipe *p);
void gp_disablezbuf(gfxpipe *p);


extern gfxpipe thegp;

#endif
