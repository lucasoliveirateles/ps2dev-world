// generate.cpp

#include "3dmath.h"
#include "generate.h"
#include "gfxpipe.h"
#include "hw.h"


vec octahedron[24] = {
    { 0.0f, 1.0f, 0.0f, 1.0f}, { 0.0f, 0.0f, 1.0f, 1.0f}, { 1.0f, 0.0f, 0.0f, 1.0f},
    { 0.0f, 1.0f, 0.0f, 1.0f}, { 1.0f, 0.0f, 0.0f, 1.0f}, { 0.0f, 0.0f,-1.0f, 1.0f},
    { 0.0f, 1.0f, 0.0f, 1.0f}, { 0.0f, 0.0f,-1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f, 1.0f},
    { 0.0f, 1.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f, 1.0f}, { 0.0f, 0.0f, 1.0f, 1.0f},
    { 0.0f,-1.0f, 0.0f, 1.0f}, { 0.0f, 0.0f, 1.0f, 1.0f}, { 1.0f, 0.0f, 0.0f, 1.0f},
    { 0.0f,-1.0f, 0.0f, 1.0f}, { 1.0f, 0.0f, 0.0f, 1.0f}, { 0.0f, 0.0f,-1.0f, 1.0f},
    { 0.0f,-1.0f, 0.0f, 1.0f}, { 0.0f, 0.0f,-1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f, 1.0f},
    { 0.0f,-1.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f, 1.0f}, { 0.0f, 0.0f, 1.0f, 1.0f},
};


// src    = every 3 vertices are considered a triangle
// numst  = number of source triangles
// radius = the amount to scale the points after normalizing (radius of the sphere)

void geodesicLOD_3cut(vec *dest, vec *src, int numst, float radius)
{
    vec center;
    vec *cs, *cd;

    cs = src;
    cd = dest;

    for (int i=0; i<numst; i++)
    {
        center.x = (cs[0].x + cs[1].x + cs[2].x);
        center.y = (cs[0].y + cs[1].y + cs[2].y);
        center.z = (cs[0].z + cs[1].z + cs[2].z);
        center.w = 1.0f;

        vec_normalize(&center);
        center.x *= radius;
        center.y *= radius;
        center.z *= radius;

        cd[0] = cs[0];
        cd[1] = cs[1];
        cd[2] = center;
        cd[3] = cs[0];
        cd[4] = cs[2];
        cd[5] = center;
        cd[6] = cs[1];
        cd[7] = cs[2];
        cd[8] = center;
        cd += 9;
        cs += 3;
    }
}


void geodesicLOD(vec *dest, vec *src, int numst, float radius, int LOD)
{
    vec c1, c2, c3;
    vec *cs, *cd;

    cs = src;
    cd = dest;

    for (int i=0; i<numst; i++)
    {
        c1.x = (cs[0].x + cs[1].x);
        c1.y = (cs[0].y + cs[1].y);
        c1.z = (cs[0].z + cs[1].z);
        c1.w = 1;

        c2.x = (cs[0].x + cs[2].x);
        c2.y = (cs[0].y + cs[2].y);
        c2.z = (cs[0].z + cs[2].z);
        c2.w = 1;

        c3.x = (cs[1].x + cs[2].x);
        c3.y = (cs[1].y + cs[2].y);
        c3.z = (cs[1].z + cs[2].z);
        c3.w = 1;

        vec_normalize(&c1);
        vec_normalize(&c2);
        vec_normalize(&c3);

        c1.x *= radius;
        c1.y *= radius;
        c1.z *= radius;

        c2.x *= radius;
        c2.y *= radius;
        c2.z *= radius;

        c3.x *= radius;
        c3.y *= radius;
        c3.z *= radius;


        cd[0] = cs[0];
        cd[1] = c1;
        cd[2] = c2;
        cd += 3;
        cd[0] = cs[1];
        cd[1] = c1;
        cd[2] = c3;
        cd += 3;
        cd[0] = cs[2];
        cd[1] = c2;
        cd[2] = c3;
        cd += 3;
        cd[0] = c1;
        cd[1] = c2;
        cd[2] = c3;
        cd += 3;

        cs += 3;
    }
}


// assumes enough space is at dest (16 * numv * 2)
// format : color, point
void genCircleXZ(vec *dest, int numv, vec *color)
{
    for (int i=0; i<numv; i++)
    {
        dest[0] = *color;
        dest[1].x = cosf( 2 * PI * (1.0 * i) / numv);
        dest[1].y = 0;
        dest[1].z = sinf( 2 * PI * (1.0 * i) / numv);
        dest[1].w = 1;
        dest += 2;
    }
}



// extrudes ring along y-axis (in direction of h)
// assumes there is enough space in destination (numrows * (6 * (numrv + 1))) vectors
// format: color, point, normal
int genCylTriStrip(vec *dst, vec *ringptr, int numrv, float h, float rad, int numcrows)
{
    vec *dest = dst;
    h /= 2;

    for (int y=0; y < numcrows; y++)
    {
        vec *ring = ringptr;
        for (int x=0; x < numrv; x++)
        {
            dest[2] = ring[1]; // normal
            dest[0] = ring[0];
            dest[1] = ring[1];
            vecscalef(&dest[1], 1, rad);
            dest[1].y += 2*h*(y+0) / numcrows - h;
            dest[5] = ring[1]; // normal
            dest[3] = ring[0];
            dest[4] = ring[1];
            vecscalef(&dest[4], 1, rad);
            dest[4].y += 2*h*(y+1) / numcrows - h;
            dest += 6; ring += 2;
        }
        dest[2] = ringptr[1]; // normal
        dest[0] = ringptr[0];
        dest[1] = ringptr[1];
        vecscalef(&dest[1], 1, rad);
        dest[1].y += 2*h*(y+0) / numcrows - h;
        dest[5] = ringptr[1]; // normal
        dest[3] = ringptr[0];
        dest[4] = ringptr[1];
        vecscalef(&dest[4], 1, rad);
        dest[4].y += 2*h*(y+1) / numcrows - h;
        dest += 6;
    }

    return dest - dst;
}


// extrudes ring along y-axis (in direction of positive and negative h)
// generates _one_ big triangle strip for a sphere !
// numrows must be a multiple of 2, numrows >= 4
// numvecs returned = (6 * numrv + 3) + (numrows - 1) * (3 + 6*numrv + 6) + (3 + 6*numrv + 3)
// format : color, point, normal
int genSphTriStrip(vec *dst, vec *ringptr, int numrv, float h, int numrows)
{
    int x, y, i, j;
    float cht1, cht2, brad1, brad2;
    vec *ring = ringptr;
    vec *dest = dst;

    j = (numrows - 1);

    // generate negative cap first
    vec tv;
    tv.x = 0.0; tv.y = -h; tv.z = 0.0; tv.w = 1.0;
    cht2 = h * cosf( (PI * 1.0 / numrows) + PI);
    brad2 = h * sinf( PI * 1.0 / numrows );
    for (x = 0; x < numrv; x++)
    {
        dest[0] = ring[0];
        dest[1] = ring[1];
        vecscalef(&dest[1], 1, brad2);
        dest[1].y += cht2;
        dest[2] = dest[1];
        vec_normalize(&dest[2]);
        dest[3] = ring[0];
        dest[4] = tv;
        dest[5] = tv;
        dest[5].y = -1;
        dest += 6; ring += 2;
    }
    dest[0] = ringptr[0];
    dest[1] = ringptr[1];
    vecscalef(&dest[1], 1, brad2);
    dest[1].y += cht2;
    dest[2] = dest[1];
    vec_normalize(&dest[2]);
    dest += 3;


    // generate body
    for (y = 1; y < numrows; y++)
    {
        cht1 = cht2;
        cht2 = h * cosf( ( PI * y / numrows ) + PI );
        brad1 = brad2;
        brad2 = h * sinf(  PI * y / numrows );
        ring = ringptr;

        dest[0] = ring[0];
        dest[1] = ring[1];
        vecscalef(&dest[1], 1, brad2);
        dest[1].y += cht2;
        dest[2] = dest[1];
        vec_normalize(&dest[2]);
        dest += 3;
        for (x = 0; x < numrv; x ++)
        {
            dest[0] = ring[0];
            dest[1] = ring[1];
            vecscalef(&dest[1], 1, brad1);
            dest[1].y += cht1;
            dest[2] = dest[1];
            vec_normalize(&dest[2]);
            dest[3] = ring[0];
            dest[4] = ring[1];
            vecscalef(&dest[4], 1, brad2);
            dest[4].y += cht2;
            dest[5] = dest[4];
            vec_normalize(&dest[5]);
            dest += 6; ring += 2;
        }
        dest[0] = ringptr[0];
        dest[1] = ringptr[1];
        vecscalef(&dest[1], 1, brad1);
        dest[1].y += cht1;
        dest[2] = dest[1];
        vec_normalize(&dest[2]);
        dest[3] = ringptr[0];
        dest[4] = ringptr[1];
        vecscalef(&dest[4], 1, brad2);
        dest[4].y += cht2;
        dest[5] = dest[4];
        vec_normalize(&dest[5]);
        dest += 6;
    }

    //brad2 = h * sinf( PI * y / numrows );
    //cht2 = h*cosf( (PI * y / numrows) + PI );
    // generate positive cap
    tv.y = h;
    ring = ringptr;
    dest[0] = ring[0];
    dest[1] = tv;
    dest[2] = tv;
    dest[2].y = 1;
    dest += 3;
    for (x = 0; x < numrv; x ++)
    {
        dest[0] = ring[0];
        dest[1] = ring[1];
        vecscalef(&dest[1], 1, brad2);
        dest[1].y += cht2;
        dest[2] = dest[1];
        vec_normalize(&dest[2]);
        dest[3] = ring[0];
        dest[4] = tv;
        dest[5] = tv;
        dest[5].y = 1;
        dest += 6; ring += 2;
    }
    dest[0] = ringptr[0];
    dest[1] = ringptr[1];
    vecscalef(&dest[1], 1, brad2);
    dest[1].y += cht2;
    dest[2] = dest[1];
    vec_normalize(&dest[2]);
    dest += 3;


    return dest - dst;
}


void qmemcpyd(void *dest, void *src, int n)
{
    unsigned *d = (unsigned *)dest, *s = (unsigned *)src;

    for (int i=(n<<2)-1; i>=0; i--)
    {
        d[i] = s[i];
    }
}



// yes, one triangle strip
// makes a pill with lower half of color1, upper half of color2
// sets every element of ring's color to color1

// numvecs returned = ( (6 * numrv + 3) + (numrows - 1) * (3 + 6*numrv + 6) + (3 + 6*numrv + 3) ) + numcrows * 6*(numrv+1)*2
// format: color, vec, normal
int genPillTriStrip(vecTriList *vtl, vec *ring, int numrv, float rad, float cylh, int numrows, int numcrows, vec *color1, vec *color2)
{
    int i=0;

    for (i=0; i<numrv; i+=2)
        ring[i] = *color1;

    vtl->numv = genSphTriStrip(vtl->vlist, ring, numrv, rad, numrows);

    int hsofs = (6*numrv+3) + ((numrows) * (9 + 6*numrv)) / 2;
    vec *vptr1 = vtl->vlist + hsofs;
    vec *vptr2 = vtl->vlist + hsofs + numcrows*(6 * (numrv + 1))*2;
    vec *tv;

    qmemcpyd(vptr2, vptr1, hsofs);

    tv = vtl->vlist;
    for (i=0; i<hsofs; i+=3)
    {
        tv[1].y += -cylh / 2;
        tv += 3;
    }
    tv = vptr2;
    for (i=0; i<hsofs; i+=3)
    {
        tv[1].y += cylh / 2;
        tv += 3;
    }

    //vtl->numv += numcrows * 2 * (4 * (numrv + 1));

    tv = vptr1;
    vtl->numv += 2 * genCylTriStrip(tv, ring, numrv, cylh / 2, rad, numcrows);


    vptr2 = vptr1 + numcrows * (6 * (numrv + 1));
    qmemcpyd(vptr2, vptr1, numcrows * 6 * (numrv + 1));


    tv = vptr1;
    for (i=0; i < numcrows*6*(numrv + 1); i+=3)
    {
        tv[i+1].y += -cylh / 4;
        tv[numcrows*6*(numrv + 1) + i+1].y += cylh / 4;
    }

    for (i=0; i < (vtl->vlist + vtl->numv - vptr2); i+=3)
        vptr2[i] = *color2;


    return vtl->numv;
}
















