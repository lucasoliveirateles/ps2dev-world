// 3dmath.cpp

#include "floatlib.h"
#include "3dmath.h"

const float cPI = 3.1415926535897998662747492629788;

/*void vec_normalize(vec *v)
{
    float d = 1.0 / sqrtf(v->x*v->x + v->y*v->y + v->z*v->z);
    v->x *= d;
    v->y *= d;
    v->z *= d;
}*/




// this will become VU0 code eventually

void vec_rotate(vec *v, float xa, float ya, float za)
{
    vec tv;

    if (xa / (2*PI) > 0.001)
    {
        tv.y = v->y * cosf(xa) - v->z * sinf(xa);
        tv.z = v->y * sinf(xa) + v->z * cosf(xa);
        v->y = tv.y;
        v->z = tv.z;
    }

    if (ya / (2*PI) > 0.001)
    {
        tv.x = v->x * cosf(ya) - v->z * sinf(ya);
        tv.z = v->x * sinf(ya) + v->z * cosf(ya);
        v->x = tv.x;
        v->z = tv.z;
    }

    if (za / (2*PI) > 0.001)
    {
        tv.x = v->x * cosf(za) - v->y * sinf(za);
        tv.y = v->x * sinf(za) + v->y * cosf(za);
        v->x = tv.x;
        v->y = tv.y;
    }
}


void vec_arbrotate(vec *dest, vec *p, float costheta, float sintheta, vec *axis)
{
    dest[0].x = dest[0].y = dest[0].z = 0;
    dest[0].w = 1;

    dest[0].x += (costheta + (1 - costheta) * axis[0].x * axis[0].x) * p[0].x;
    dest[0].x += ((1 - costheta) * axis[0].x * axis[0].y - axis[0].z * sintheta) * p[0].y;
    dest[0].x += ((1 - costheta) * axis[0].x * axis[0].z + axis[0].y * sintheta) * p[0].z;

    dest[0].y += ((1 - costheta) * axis[0].x * axis[0].y + axis[0].z * sintheta) * p[0].x;
    dest[0].y += (costheta + (1 - costheta) * axis[0].y * axis[0].y) * p[0].y;
    dest[0].y += ((1 - costheta) * axis[0].y * axis[0].z - axis[0].x * sintheta) * p[0].z;

    dest[0].z += ((1 - costheta) * axis[0].x * axis[0].z - axis[0].y * sintheta) * p[0].x;
    dest[0].z += ((1 - costheta) * axis[0].y * axis[0].z + axis[0].x * sintheta) * p[0].y;
    dest[0].z += (costheta + (1 - costheta) * axis[0].z * axis[0].z) * p[0].z;
}








// matrix functions



extern "C" void vec_mtxRotateX(vec *M, float ang) // rotate a matrix in-place along x-axis
{
    vec rotm[4];
    vec_genMtxRotateX(rotm, ang);
    vec_mulMtxMtx(M, rotm, M);
}

extern "C" void vec_mtxRotateY(vec *M, float ang)
{
    vec rotm[4];
    vec_genMtxRotateY(rotm, ang);
    vec_mulMtxMtx(M, rotm, M);
}

extern "C" void vec_mtxRotateZ(vec *M, float ang)
{
    vec rotm[4];
    vec_genMtxRotateZ(rotm, ang);
    vec_mulMtxMtx(M, rotm, M);
}







// extra junk

void memset(void *dest, unsigned char c, int n)
{
    unsigned char *cp = (unsigned char *)dest;
    for (int i=0; i<n; i++)
        cp[i] = c;
}





























