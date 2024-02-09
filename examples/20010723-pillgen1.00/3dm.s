# 3dmath.s

.data



.set noat
.set noreorder
.text


# ---------------------
# old working code
#     .globl vectoints
#     .ent vectoints
# # void vectoints(int *dest, vec *src, int numvecs);
# vectoints:
#     lqc2 $vf4, 0($5)
#     vftoi4.xyzw $vf3, $vf4
#     sqc2 $vf3, 0($4)
#
#     addiu $4, 0x0010
#     addiu $5, 0x0010
#     addiu $6, -1
#     bnez $6, vectoints
#     nop
#
#     jr $31
#     nop
#     .end vectoints
# ---------------------


    .globl vectoints
    .ent vectoints
# void vectoints(int *dest, vec *src, int numvecs);
vectoints:
    beqz $6, vectoints.end
    nop
    slti $2, $6, 4
    bnez $2, vectoints.lastfew
    nop

    andi $3, $6, 0x0003
    srl $6, 2

vectoints.loop:
    lqc2 $vf03, 0($5)
    lqc2 $vf04, 16($5)
    lqc2 $vf05, 32($5)
    lqc2 $vf06, 48($5)
    vftoi4.xyzw $vf03xyzw, $vf03xyzw
    vftoi4.xyzw $vf04xyzw, $vf04xyzw
    vftoi4.xyzw $vf05xyzw, $vf05xyzw
    vftoi4.xyzw $vf06xyzw, $vf06xyzw
    sqc2 $vf03, 0($4)
    sqc2 $vf04, 16($4)
    sqc2 $vf05, 32($4)
    sqc2 $vf06, 48($4)

    addiu $5, 0x0040
    addiu $4, 0x0040
    addiu $6, -1
    bnez $6, vectoints.loop
    nop

vectoints.lastfew:
    lqc2 $vf4, 0($5)
    vftoi4.xyzw $vf3, $vf4
    sqc2 $vf3, 0($4)

    addiu $4, 0x0010
    addiu $5, 0x0010
    addiu $3, -1
    bnez $3, vectoints
    nop

vectoints.end:
    jr $31
    nop
    .end vectoints


    .globl vecscalef
    .ent vecscalef
# void vecscale(vec *v, int numvecs, float f);
vecscalef:
    mfc1 $2, $f12
    nop
    qmtc2 $2, $vf9

vecscalef.loop:
    lqc2 $vf4, 0($4)
    vmulx.xyz $vf4xyz, $vf4xyz, $vf9x
    sqc2 $vf4, 0($4)

    addiu $5, -1
    bnez $5, vecscalef.loop
    addiu $4, 0x0010

    jr $31
    nop
    .end vecscalef


    .globl vecxlatf
    .ent vecxlatf
# void vecxlatf(vec *v, int numvecs, float x, float y, float z);
vecxlatf:
    addiu $29, -16
    swc1 $f12, 0($29)
    swc1 $f13, 4($29)
    swc1 $f14, 8($29)
    nop
    lqc2 $vf9, 0($29)

vecxlatf.loop:
    lqc2 $vf4, 0($4)
    vadd.xyz $vf4xyz, $vf4xyz, $vf9xyz
    sqc2 $vf4, 0($4)

    addiu $5, -1
    bnez $5, vecxlatf.loop
    addiu $4, 0x0010

    addiu $29, 16
    jr $31
    nop
    .end vecxlatf


    .globl vecxlatv
    .ent vecxlatv
#void vecxlatv(vec *v int numvecs, vec *xlv);
vecxlatv:
    lqc2 $vf9, 0($6)

vecxlatv.loop:
    lqc2 $vf4, 0($4)
    vadd.xyz $vf4xyz, $vf4xyz, $vf9xyz
    sqc2 $vf4, 0($4)

    addiu $5, -1
    bnez $5, vecxlatv.loop
    addiu $4, 0x0010

    jr $31
    nop
    .end vecxlatv



    .globl vec_normalize
    .ent vec_normalize
# float vec_normalize(vec *v);
vec_normalize:
    lqc2 $vf6, 0($4)
    nop

    vmul.xyz $vf4xyz, $vf6xyz, $vf6xyz
    vaddy.x $vf4x, $vf4x, $vf4y
    vaddz.x $vf4x, $vf4x, $vf4z
    vrsqrt Q, $vf0w, $vf4x
    vwaitq
    vmulq.xyz $vf6xyz, $vf6xyz, Q
    vaddq.x $vf4x, $vf0x, Q

    sqc2 $vf6, 0($4)

    qmfc2 $2, $vf4
    nop
    mtc1 $2, $f0

    jr $31
    nop
    .end vec_normalize


    .globl vec_dot
    .ent vec_dot
# float vec_dot(vec *u, vec *v);
vec_dot:
    lqc2 $vf4, 0($4)
    lqc2 $vf5, 0($5)

    vmul.xyz $vf4xyz, $vf4xyz, $vf5xyz
    vaddy.x $vf4x, $vf4x, $vf4y
    vaddz.x $vf4x, $vf4x, $vf4z

    qmfc2 $2, $vf4
    nop
    mtc1 $2, $f0

    jr $31
    nop
    .end vec_dot


    .globl vec_cross
    .ent vec_cross
# void vec_cross(vec *d, vec *u, vec *v);
vec_cross:
    lqc2 $vf4, 0($5)
    lqc2 $vf5, 0($6)

    vopmula.xyz ACCxyz, $vf4xyz, $vf5xyz
    vopmsub.xyz $vf6xyz, $vf4xyz, $vf5xyz

    sqc2 $vf6, 0($4)

    jr $31
    nop
    .end vec_cross


    .globl vec1tocolor
    .ent vec1tocolor
# int vec1tocolor(vec *v);
vec1tocolor:
    dli $2, 0x43800000437f0000           # 256.0f, 255.0f
    lqc2 $vf4, 0($4)
    qmtc2 $2, $vf5
    nop

    vmulx.xyz $vf4xyz, $vf4xyz, $vf5x
    vftoi0.xyz $vf4xyz, $vf4xyz

    qmfc2 $2, $vf4

    addiu $29, -16

    sq $2, 0($29)

    dsrl $3, $2, 24
    andi $3, 0xff00
    or $2, $3

    lw $3, 8($29)
    andi $3, 0x00ff
    sll $3, 16

    or $2, $3

    addiu $29, 16

    jr $31
    nop
    .end vec1tocolor



# ------------------------------------------
# ------------------------------------------



    .globl vu_srand
    .ent vu_srand
# void vu_srand(int r);
vu_srand:
    qmtc2 $4, $vf4
    nop
    vrinit R, $vf4x

    jr $31
    nop
    .end vu_srand


# returns a number between 0.0 and 1.0
    .globl vu_lastrand
    .ent vu_lastrand
# float vu_lastrand();
vu_lastrand:
    vrget.x $vf4x, R
    vsubw.x $vf4x, $vf4x, $vf0w
    qmfc2 $2, $vf4
    nop
    mtc1 $2, $f0

    jr $31
    nop
    .end vu_lastrand


# returns the next random number, between 0.0 and 1.0
    .globl vu_nextrand
    .ent vu_nextrand
# float vu_nextrand();
vu_nextrand:
    vrnext.x $vf4x, R
    vsubw.x $vf4x, $vf4x, $vf0w
    qmfc2 $2, $vf4
    nop
    mtc1 $2, $f0

    jr $31
    nop
    .end vu_nextrand




# ------------------------------------------
# ------------------------------------------




# matrix functions :


    .globl vec_applyMtx
    .ent vec_applyMtx
# void vec_applyMtx(vec *dest, vec *M); // apply to single vector
vec_applyMtx:
    lqc2 $vf03, 0($4)
    lqc2 $vf04, 0($5)
    lqc2 $vf05, 16($5)
    lqc2 $vf06, 32($5)
    lqc2 $vf07, 48($5)

    vmulax.xyzw ACCxyzw, $vf04xyzw, $vf03x
    vmadday.xyzw ACCxyzw, $vf05xyzw, $vf03y
    vmaddaz.xyzw ACCxyzw, $vf06xyzw, $vf03z
    vmaddw.xyzw $vf02xyzw, $vf07xyzw, $vf03w

    sqc2 $vf02, 0($4)

    jr $31
    nop
    .end vec_applyMtx


    .globl vec_mulMtxMtx
    .ent vec_mulMtxMtx
# void vec_mulMtxMtxD(vec *M, vec *A, vec *B); // M = A x B
vec_mulMtxMtx:
    lqc2 $vf08, 0($5)
    lqc2 $vf09, 16($5)
    lqc2 $vf10, 32($5)
    lqc2 $vf11, 48($5)
    lqc2 $vf03, 0($6)

    vmulax.xyzw ACCxyzw, $vf08xyzw, $vf03x
    vmadday.xyzw ACCxyzw, $vf09xyzw, $vf03y
    vmaddaz.xyzw ACCxyzw, $vf10xyzw, $vf03z
    vmaddw.xyzw $vf03xyzw, $vf11xyzw, $vf03w

    lqc2 $vf02, 16($6)
    sqc2 $vf03, 0($4)

    vmulax.xyzw ACCxyzw, $vf08xyzw, $vf02x
    vmadday.xyzw ACCxyzw, $vf09xyzw, $vf02y
    vmaddaz.xyzw ACCxyzw, $vf10xyzw, $vf02z
    vmaddw.xyzw $vf02xyzw, $vf11xyzw, $vf02w

    lqc2 $vf03, 32($6)
    sqc2 $vf02, 16($4)

    vmulax.xyzw ACCxyzw, $vf08xyzw, $vf03x
    vmadday.xyzw ACCxyzw, $vf09xyzw, $vf03y
    vmaddaz.xyzw ACCxyzw, $vf10xyzw, $vf03z
    vmaddw.xyzw $vf03xyzw, $vf11xyzw, $vf03w

    lqc2 $vf02, 48($6)
    sqc2 $vf03, 32($4)

    vmulax.xyzw ACCxyzw, $vf08xyzw, $vf02x
    vmadday.xyzw ACCxyzw, $vf09xyzw, $vf02y
    vmaddaz.xyzw ACCxyzw, $vf10xyzw, $vf02z
    vmaddw.xyzw $vf02xyzw, $vf11xyzw, $vf02w

    sqc2 $vf02, 48($4)

    jr $31
    nop
    .end vec_mulMtxMtx


    .globl vec_genMtxRotateX
    .ent vec_genMtxRotateX
# void vec_genMtxRotateX(vec *M, float ang);
vec_genMtxRotateX:
    addiu $29, -16
    sw $31, 8($29)
    swc1 $f12, 4($29)
    sw $4, 0($29)

    sqc2 $vf00, 48($4)
    sq $0, 0($4)
    lui $2, 0x3f80
    sq $0, 16($4)
    sq $0, 32($4)
    sw $2, 0($4)

    addiu $29, -16
    jal cosf
    nop
    addiu $29, 16

    lw $4, 0($29)
    nop
    swc1 $f0, 20($4)
    lwc1 $f12, 4($29)
    swc1 $f0, 40($4)

    addiu $29, -16
    jal sinf
    nop
    addiu $29, 16

    lw $4, 0($29)
    mtc1 $0, $f12
    swc1 $f0, 24($4)
    sub.s $f0, $f12, $f0    # negate it
    swc1 $f0, 36($4)

    lw $31, 8($29)
    jr $31
    addiu $29, 16
    .end vec_genMtxRotateX


    .globl vec_genMtxRotateY
    .ent vec_genMtxRotateY
# void vec_genMtxRotateY(vec *M, float ang);
vec_genMtxRotateY:
    addiu $29, -16
    sw $31, 8($29)
    swc1 $f12, 4($29)
    sw $4, 0($29)

    sqc2 $vf00, 48($4)
    sq $0, 0($4)
    lui $2, 0x3f80
    sq $0, 16($4)
    sq $0, 32($4)
    sw $2, 20($4)

    jal cosf
    nop

    lw $4, 0($29)
    nop
    swc1 $f0, 0($4)
    lwc1 $f12, 4($29)
    swc1 $f0, 40($4)

    jal sinf
    nop

    lw $4, 0($29)
    mtc1 $0, $f12
    swc1 $f0, 32($4)
    sub.s $f0, $f12, $f0    # negate it
    swc1 $f0, 8($4)

    lw $31, 8($29)
    jr $31
    addiu $29, 16
    .end vec_genMtxRotateY


    .globl vec_genMtxRotateZ
    .ent vec_genMtxRotateZ
# void vec_genMtxRotateZ(vec *M, float ang);
vec_genMtxRotateZ:
    addiu $29, -16
    sw $31, 8($29)
    swc1 $f12, 4($29)
    sw $4, 0($29)

    sqc2 $vf00, 48($4)
    sq $0, 0($4)
    lui $2, 0x3f80
    sq $0, 16($4)
    sq $0, 32($4)
    sw $2, 40($4)

    jal cosf
    nop

    lw $4, 0($29)
    nop
    swc1 $f0, 0($4)
    lwc1 $f12, 4($29)
    swc1 $f0, 20($4)

    jal sinf
    nop

    lw $4, 0($29)
    mtc1 $0, $f12
    swc1 $f0, 4($4)
    sub.s $f0, $f12, $f0    # negate it
    swc1 $f0, 16($4)

    lw $31, 8($29)
    jr $31
    addiu $29, 16
    .end vec_genMtxRotateZ


    .globl vec_genMtxXlat
    .ent vec_genMtxXlat
# void vec_genMtxXlat(vec *M, float x, float y, float z);
vec_genMtxXlat:
    lui $2, 0x3f80
    sqc2 $vf00, 48($4)
    sq $0, 0($4)
    sq $0, 16($4)
    sq $0, 32($4)
    sw $2, 0($4)
    sw $2, 20($4)
    sw $2, 40($4)
    swc1 $f12, 48($4)
    swc1 $f13, 52($4)
    swc1 $f14, 56($4)

    jr $31
    nop
    .end vec_genMtxXlat


    .globl vec_genMtxScale
    .ent vec_genMtxScale
# void vec_genMtxScale(vec *M, float x, float y, float z);
vec_genMtxScale:
    sq $0, 0($4)
    sq $0, 16($4)
    sq $0, 32($4)
    sqc2 $vf00, 48($4)
    swc1 $f12, 0($4)
    swc1 $f13, 20($4)
    swc1 $f14, 40($4)

    jr $31
    nop
    .end vec_genMtxScale


# starting at address dest, for i = 0 to numblks-1 { for j = 0 to count-1 { xform(dest); dest++; } dest += skip }
    .globl vec_arrayApplyMtx
    .ent vec_arrayApplyMtx
# void vec_arrayApplyMtx(vec *dest, vec *M, int numblks, int count, int skip);
vec_arrayApplyMtx:
    lqc2 $vf08, 0($5)
    lqc2 $vf09, 16($5)
    lqc2 $vf10, 32($5)
    lqc2 $vf11, 48($5)
    lqc2 $vf03, 0($4)       # preload
    move $3, $7
    sll $8, 4

vec_arrayApplyMtx.loop:
    vmulax.xyzw ACCxyzw, $vf08xyzw, $vf03x
    vmadday.xyzw ACCxyzw, $vf09xyzw, $vf03y
    vmaddaz.xyzw ACCxyzw, $vf10xyzw, $vf03z
    vmaddw.xyzw $vf02xyzw, $vf11xyzw, $vf03w
    lqc2 $vf03, 16($4)
    sqc2 $vf02, 0($4)
    addiu $4, 0x0010
    addiu $3, -1
    bnez $3, vec_arrayApplyMtx.loop
    nop

    addu $4, $8
    lqc2 $vf03, 0($4)
    move $3, $7
    addiu $6, -1
    bnez $6, vec_arrayApplyMtx.loop
    nop

    jr $31
    nop
    .end vec_arrayApplyMtx


    .globl vec_mtxXlat
    .ent vec_mtxXlat
# void vec_mtxXlat(vec *M, float x, float y, float z);
vec_mtxXlat:
    addiu $29, -16
    swc1 $f03, 0($29)
    swc1 $f04, 4($29)

    lwc1 $f03, 48($4)
    lwc1 $f04, 52($4)
    add.s $f03, $f12
    add.s $f04, $f13
    swc1 $f03, 48($4)
    swc1 $f04, 52($4)
    lwc1 $f03, 56($4)
    add.s $f03, $f14
    swc1 $f03, 56($4)

    lwc1 $f03, 0($29)
    lwc1 $f04, 4($29)

    jr $31
    addiu $29, 16
    .end vec_mtxXlat


    .globl vec_mtxXlatV
    .ent vec_mtxXlatV
# void vec_mtxXlatV(vec *M, vec *xlv);
vec_mtxXlatV:
    lqc2 $vf03, 0($5)
    lqc2 $vf04, 48($4)

    vadd.xyz $vf03xyz, $vf03xyz, $vf04xyz

    sqc2 $vf03, 48($4)

    jr $31
    nop
    .end vec_mtxXlatV


    .globl vec_mtxScale
    .ent vec_mtxScale
# void vec_mtxScale(vec *M, float x, float y, float z);
vec_mtxScale:
    addiu $29, -16
    swc1 $f03, 0($29)
    swc1 $f04, 4($29)
    swc1 $f05, 8($29)

    lwc1 $f03, 0($4)
    lwc1 $f04, 20($4)
    lwc1 $f05, 40($4)
    mul.s $f03, $f12
    mul.s $f04, $f13
    mul.s $f05, $f14
    swc1 $f03, 0($4)
    swc1 $f04, 20($4)
    swc1 $f05, 40($4)

    lwc1 $f03, 0($29)
    lwc1 $f04, 4($29)
    lwc1 $f05, 8($29)

    jr $31
    addiu $29, 16
    .end vec_mtxScale

































