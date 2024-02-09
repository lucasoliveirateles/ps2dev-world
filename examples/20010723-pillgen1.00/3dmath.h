// 3dmath.h

#ifndef _3DMATH_H_
#define _3DMATH_H_

#include "floatlib.h"

typedef struct vec vec;

#pragma pack
struct vec {
    float x, y, z, w;
};

void vec_rotate(vec *v, float xa, float ya, float za);
void vec_arbrotate(vec *dest, vec *p, float costheta, float sintheta, vec *axis);

void memset(void *dest, unsigned char c, int n);


#ifdef __cplusplus
extern "C" {
#endif

void vectoints(int *dest, vec *src, int numvecs);
void vecscalef(vec *v, int numvecs, float f);
void vecxlatf(vec *v, int numvecs, float x, float y, float z);
void vecxlatv(vec *v, int numvecs, vec *xlv);
float vec_normalize(vec *v);
float vec_dot(vec *u, vec *v);
void vec_cross(vec *d, vec *u, vec *v);
int vec1tocolor(vec *v);

void vu_srand(int r);
float vu_lastrand();
float vu_nextrand();

#define srand(a) vu_srand(a)
#define nextr() vu_nextrand()



// matrix functions

    // all matrices are _column_vector_ matrices; matrices are applied to column vectors such as d = M x v

// starting at address dest, for i = 0 to numblks-1 { for j = 0 to count-1 { xform(dest); dest++; } dest += skip }
void vec_arrayApplyMtx(vec *dest, vec *M, int numblks, int count, int skip);
void vec_applyMtx(vec *dest, vec *M); // apply to single vector
void vec_mulMtxMtx(vec *M, vec *A, vec *B); // M = A x B

void vec_mtxRotateX(vec *M, float ang); // rotate a matrix in-place along x-axis
void vec_mtxRotateY(vec *M, float ang);
void vec_mtxRotateZ(vec *M, float ang);
void vec_mtxXlat(vec *M, float x, float y, float z);
void vec_mtxXlatV(vec *M, vec *xlv);
void vec_mtxScale(vec *M, float x, float y, float z);

void vec_genMtxRotateX(vec *M, float ang); // angle in radians
void vec_genMtxRotateY(vec *M, float ang);
void vec_genMtxRotateZ(vec *M, float ang);
void vec_genMtxXlat(vec *M, float x, float y, float z);
//void vec_genMtxXlatV(vec *M, vec *xlv);
void vec_genMtxScale(vec *M, float x, float y, float z);
//void vec_genMtxScaleV(vec *M, vec *sv);

//void vec_mtxTranspose(vec *T, vec *M); // T = transpose(M) ; can be used to convert column vectors to row vectors and vice-versa

// ---



int fptosi(float x);
int fptoui(float x);

#ifdef __cplusplus
}
#endif

extern const float cPI;

#define PI 3.1415926535897998662747492629788

#endif
