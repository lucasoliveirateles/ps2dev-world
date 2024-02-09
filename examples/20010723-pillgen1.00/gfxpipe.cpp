// gfxpipe.cpp

#include "gfxpipe.h"
#include "hw.h"

gfxpipe thegp;


void initializeGfxPipe(unsigned long *dmatadr)
{
    dmatadr[0] = 0x0000000070000000;
    dmatadr[1] = 0;
    //dmatadr[2] = 0x1000000000008000;
    //dmatadr[3] = 0x000000000000000e;
}


unsigned int gp_bytesLeft(gfxpipe *p)
{
    unsigned int eob, cgt;

    eob = (unsigned int)p->curpipe;
    eob += (p->memsize >> 1);
    cgt = (unsigned int)p->curgiftag;

    return eob - cgt;
}


void gp_hardflush(gfxpipe *p)
{
    Dma02Wait();
    DCacheFlush();
    SendDma02(p->curpipe);

    if (p->curpipe == p->dmatadrA)
    {
        initializeGfxPipe(p->dmatadrB);
        p->curpipe = p->curdmatadr = p->dmatadrB;
        p->curgiftag = p->curdmatadr + 2;
    }
    else
    {
        initializeGfxPipe(p->dmatadrA);
        p->curpipe = p->curdmatadr = p->dmatadrA;
        p->curgiftag = p->curdmatadr + 2;
    }
}

void gp_checkflush(gfxpipe *p)
{
    if ( gp_bytesLeft(p) < (GP_MINSPACE << 3))
    {
        gp_hardflush(p);
    }
}

// size must be less than 1MB
int createGfxPipe(gfxpipe *pipeline, void *buffer, int size)
{
    if ((int)buffer & 0xf)
        return 0;

    if (size < 0x1000)
        return 0;

    pipeline->dmatadrA = (unsigned long *)buffer;
    pipeline->dmatadrB = pipeline->dmatadrA + (size >> 4);
    pipeline->memsize = size;

    initializeGfxPipe(pipeline->dmatadrA);

    pipeline->curpipe = pipeline->curdmatadr = pipeline->dmatadrA;
    pipeline->curgiftag = pipeline->dmatadrA + 2;

    return 1;
}



/*
the rules:
    - the list is sendable via gp_hardflush after ANY gp_* function call
    - each function must advance the giftag pointer to empty space
    - a function is guaranteed GP_MINSPACE dwords to work with;
      if it needs more, it must check against the remaining size
      of the pipeline ((gp->curdmatadr + (gp->memsize / 2)) - gp->curgiftag)
    - a function must call gp_checkflush after it is done adding to the queue
    - a function must update the size field of the dmatag
    - the current dmatag is of the 'count' style after a function returns
*/


void gp_line(gfxpipe *p, unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned z, unsigned color)
{
    unsigned long dt = *(p->curdmatadr);
    *(p->curdmatadr) = (dt & 0xffffffffffff0000) | ((dt & 0xffff) + 3);

    p->curgiftag[0] = 0x4400000000008001;
    p->curgiftag[1] = 0xffffffffffff5d10;
    p->curgiftag[2] = 0x0000000000000001;
    p->curgiftag[3] = 0x3f80000000000000 | color;
    p->curgiftag[4] = ((unsigned long)z << 32) | (y1 << 16) | x1;
    p->curgiftag[5] = ((unsigned long)z << 32) | (y2 << 16) | x2;
    p->curgiftag = &p->curgiftag[6];        // advance the packet pointer

    gp_checkflush(p);
}

/*
// bigger memory version of the above
void gp_line(gfxpipe *p, unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned color)
{
    unsigned long dt = *p->curdmatadr;
    *p->curdmatadr = (dt & 0xffffffffffff0000) | ((dt & 0xffff) + 5);

    p->curgiftag[0] = 0x1000000000008004;
    p->curgiftag[1] = 0xfffffffffffffffe;

    p->curgiftag[2] = 1;
    p->curgiftag[3] = 0;
    p->curgiftag[4] = 0x3f80000000000000 | color;
    p->curgiftag[5] = 1;
    p->curgiftag[6] = (y1 << 16) | x1;
    p->curgiftag[7] = 0xd;
    p->curgiftag[8] = (y2 << 16) | x2;
    p->curgiftag[9] = 5;
    p->curgiftag = &p->curgiftag[10];

    gp_checkflush(p);
} */

void gp_ltriangle(gfxpipe *p, unsigned x1, unsigned y1, unsigned z1, unsigned c1,
                              unsigned x2, unsigned y2, unsigned z2, unsigned c2,
                              unsigned x3, unsigned y3, unsigned z3, unsigned c3)
{
    unsigned long dt = *(p->curdmatadr);
    *(p->curdmatadr) = (dt & 0xffffffffffff0000) | ((dt & 0xffff) + 6);

    p->curgiftag[0] = 0x9400000000008001;
    p->curgiftag[1] = 0xfffffff515151d10;
    p->curgiftag[2] = 0x000000000000000a;
    p->curgiftag[3] = 0x3f80000000000000 | c1;
    p->curgiftag[4] = ((unsigned long)z1 << 32) | (y1 << 16) | x1;
    p->curgiftag[5] = 0x3f80000000000000 | c2;
    p->curgiftag[6] = ((unsigned long)z2 << 32) | (y2 << 16) | x2;
    p->curgiftag[7] = 0x3f80000000000000 | c3;
    p->curgiftag[8] = ((unsigned long)z3 << 32) | (y3 << 16) | x3;
    p->curgiftag[9] = p->curgiftag[3];
    p->curgiftag[10] = p->curgiftag[4];
    p->curgiftag = &p->curgiftag[12];        // advance the packet pointer

    gp_checkflush(p);
}

void gp_triangle(gfxpipe *p,  unsigned x1, unsigned y1, unsigned z1, unsigned c1,
                              unsigned x2, unsigned y2, unsigned z2, unsigned c2,
                              unsigned x3, unsigned y3, unsigned z3, unsigned c3)
{
    unsigned long dt = *(p->curdmatadr);
    *(p->curdmatadr) = (dt & 0xffffffffffff0000) | ((dt & 0xffff) + 5);

    p->curgiftag[0] = 0x7400000000008001;
    p->curgiftag[1] = 0xfffffffff5151d10;
    p->curgiftag[2] = 0x000000000000000b;
    p->curgiftag[3] = 0x3f80000000000000 | c1;
    p->curgiftag[4] = ((unsigned long)z1 << 32) | (y1 << 16) | x1;
    p->curgiftag[5] = 0x3f80000000000000 | c2;
    p->curgiftag[6] = ((unsigned long)z2 << 32) | (y2 << 16) | x2;
    p->curgiftag[7] = 0x3f80000000000000 | c3;
    p->curgiftag[8] = ((unsigned long)z3 << 32) | (y3 << 16) | x3;
    p->curgiftag = &p->curgiftag[10];        // advance the packet pointer

    gp_checkflush(p);
}

void gp_ftriangle(gfxpipe *p, unsigned x1, unsigned y1, unsigned z1,
                              unsigned x2, unsigned y2, unsigned z2,
                              unsigned x3, unsigned y3, unsigned z3, unsigned color)
{
    unsigned long dt = *(p->curdmatadr);
    *(p->curdmatadr) = (dt & 0xffffffffffff0000) | ((dt & 0xffff) + 4);

    p->curgiftag[0] = 0x5400000000008001;
    p->curgiftag[1] = 0xfffffffffff5dd10;
    p->curgiftag[2] = 0x0000000000000003;
    p->curgiftag[3] = 0x3f80000000000000 | color;
    p->curgiftag[4] = ((unsigned long)z1 << 32) | (y1 << 16) | x1;
    p->curgiftag[5] = ((unsigned long)z2 << 32) | (y2 << 16) | x2;
    p->curgiftag[6] = ((unsigned long)z3 << 32) | (y3 << 16) | x3;
    p->curgiftag = &p->curgiftag[8];        // advance the packet pointer

    gp_checkflush(p);
}

void gp_frect(gfxpipe *p, unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned z, unsigned color)
{
    unsigned long dt = *(p->curdmatadr);
    *(p->curdmatadr) = (dt & 0xffffffffffff0000) | ((dt & 0xffff) + 4);

    p->curgiftag[0] = 0x6400000000008001;
    p->curgiftag[1] = 0xffffffffff55dd10;
    p->curgiftag[2] = 0x0000000000000004;
    p->curgiftag[3] = 0x3f80000000000000 | color;
    p->curgiftag[4] = ((unsigned long)z << 32) | (y1 << 16) | x1;
    p->curgiftag[5] = ((unsigned long)z << 32) | (y1 << 16) | x2;
    p->curgiftag[6] = ((unsigned long)z << 32) | (y2 << 16) | x1;
    p->curgiftag[7] = ((unsigned long)z << 32) | (y2 << 16) | x2;
    p->curgiftag = &p->curgiftag[8];        // advance the packet pointer

    gp_checkflush(p);
}


void gp_enablezbuf(gfxpipe *p)
{
    unsigned long dt = *p->curdmatadr;
    *p->curdmatadr = (dt & 0xffffffffffff0000) | ((dt & 0xffff) + 2);

    p->curgiftag[0] = 0x1000000000008001;
    p->curgiftag[1] = 0xfffffffffffffffe;

    p->curgiftag[2] = 0x00070000;
    p->curgiftag[3] = 0x47;
    p->curgiftag = &p->curgiftag[4];        // advance the packet pointer

    gp_checkflush(p);
}

void gp_disablezbuf(gfxpipe *p)
{
    unsigned long dt = *p->curdmatadr;
    *p->curdmatadr = (dt & 0xffffffffffff0000) | ((dt & 0xffff) + 2);

    p->curgiftag[0] = 0x1000000000008001;
    p->curgiftag[1] = 0xfffffffffffffffe;

    p->curgiftag[2] = 0x00030000;
    p->curgiftag[3] = 0x47;
    p->curgiftag = &p->curgiftag[4];        // advance the packet pointer

    gp_checkflush(p);
}


// assumes already converted data
// vtl->numv must be strictly less than 65535 (that's ~ 1MB !)
// format: color, point, normal
void gp_drawVTL_inplace_cpn(gfxpipe *p, vecTriList *vtl)
{
    unsigned long dt = *p->curdmatadr;
    *p->curdmatadr = (dt & 0xffffffff8fffffff) | (1 << 28);
    p->curdmatadr = p->curgiftag;
    p->curdmatadr[0] = (0x0000000030000000 + vtl->numv + 1) | (((unsigned long)(&vtl->giftag[0]) & 0x7fffffff) << 32);
    p->curdmatadr[1] = 0;
    p->curdmatadr += 2;
    p->curdmatadr[0] = 0x0000000070000000;
    p->curdmatadr[1] = 0x0000000000000000;

    p->curgiftag = p->curdmatadr + 2;

    gp_checkflush(p);
}

























