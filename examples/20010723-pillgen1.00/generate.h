// generate.h

#ifndef _GENERATE_H_
#define _GENERATE_H_

#include "3dmath.h"
#include "gfxpipe.h"




extern vec octahedron[24];

void geodesicLOD_3cut(vec *dest, vec *src, int numst, float radius);
void geodesicLOD(vec *dest, vec *src, int numst, float radius, int LOD);

void genCircleXZ(vec *dest, int numv, vec *color);
int genCylTriStrip(vec *dst, vec *ringptr, int numrv, float h, float rad, int numcrows);
int genSphTriStrip(vec *dst, vec *ringptr, int numrv, float h, int numrows);
int genPillTriStrip(vecTriList *vtl, vec *ring, int numrv, float rad, float cylh, int numrows, int numcrows, vec *color1, vec *color2);

void qmemcpyd(void *dest, void *src, int n);




#endif
