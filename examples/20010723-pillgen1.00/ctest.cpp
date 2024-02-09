#include "floatlib.h"

#include "hw.h"
#include "gfxpipe.h"
#include "generate.h"




unsigned int vzzrzzn[21*2*2] = {
    20<<4,345<<4 , 45<<4,375<<4 ,
    45<<4,375<<4 , 70<<4,345<<4 ,

    90<<4,345<<4 , 140<<4,345<<4 ,
    140<<4,345<<4 , 90<<4,375<<4 ,
    90<<4,375<<4 , 140<<4,375<<4 ,

    160<<4,345<<4 , 210<<4,345<<4 ,
    210<<4,345<<4 , 160<<4,375<<4 ,
    160<<4,375<<4 , 210<<4,375<<4 ,

    230<<4,345<<4 , 230<<4,375<<4 ,
    230<<4,345<<4 , 280<<4,345<<4 ,
    280<<4,345<<4 , 230<<4,360<<4 ,
    230<<4,360<<4 , 280<<4,375<<4 ,

    300<<4,345<<4 , 350<<4,345<<4 ,
    350<<4,345<<4 , 300<<4,375<<4 ,
    300<<4,375<<4 , 350<<4,375<<4 ,

    370<<4,345<<4 , 420<<4,345<<4 ,
    420<<4,345<<4 , 370<<4,375<<4 ,
    370<<4,375<<4 , 420<<4,375<<4 ,

    440<<4,345<<4 , 440<<4,375<<4 ,
    440<<4,345<<4 , 490<<4,375<<4 ,
    490<<4,345<<4 , 490<<4,375<<4

};



#define PART_EXISTS 1

struct particle {
    vec pos, vel, acc;
    vec rta, rai;
    int flags; int dummy[3];
};

#define NUMPARTS 94
particle plist[NUMPARTS];

// "frame rate" = 60 / FRD
#define FRD 2



int ps2main()
{
    int whichdrawbuf = 0;
    int i, j;

    /* initialize hardware type stuff */

    DmaReset();
    initGraph(2);               // NTSC
    SetVideoMode();

    install_VRstart_handler();

    SetDrawFrameBuffer(1);
    SetDrawFrameBuffer(0);
    SetCrtFrameBuffer(1);

    createGfxPipe(&thegp, (void *)0x100000, 0x40000);

    resetVU0();
    vu_srand(0x2222);


    /* initialize large data structures */

    memset(plist, 0, sizeof(plist));


    // // if you just want two pills rotating in center of screen :
    // plist[0].pos.x = 256; plist[0].pos.y = 192; plist[0].pos.z = 500;
    // plist[0].rta.y = PI / 3;
    // plist[0].rai.x = PI / 60;
    // // plist[0].flags |= PART_EXISTS;
    // plist[1].pos.x = 128; plist[1].pos.y = 192; plist[1].pos.z = 500;
    // plist[1].rta.y = PI / 3;
    // plist[1].rai.x = PI / 40;
    // plist[1].flags |= PART_EXISTS;
    // // don't forget to comment out the generator part as well, if this is all you want

    int RINGPTS = 16;

    vec *ring = (vec *)0x300000;
    vec col, col2;
    col.x = 0.8*15; col.y = 0.1*15; col.z = 0.2*15; col.w = 0;
    col2.x = 0.1*15; col2.y = 0.8*15; col2.z = 0.8*15; col2.w = 0;
    genCircleXZ(ring, RINGPTS, &col);

    vecTriList *smlpillvtl = (vecTriList *)(0x600000 + 16*2*RINGPTS);

    smlpillvtl->numv = genPillTriStrip(smlpillvtl, ring, RINGPTS, 50.0*0.25, 140.0*0.25, 8, 1, &col, &col2);
    smlpillvtl->giftag[0]  =   0x3006400000008000 + (smlpillvtl->numv / 3);  // div 3 because that is # of (color, point, normal) triples; nop for normal qword
    smlpillvtl->giftag[1]  =   0xffffffffffffff51;



    //
    // wait 1 second for screen image to stabilize after screen mode setting
    while (TestVRstart() < 60 * 1);
    ClearVRcount();


    whichdrawbuf = 0;
    float theta = 0;

    // main loop
    while (1)
    {
        /* stuff we have to get done */

        vecTriList *vtl = (vecTriList *)0x700000;

        for (i=0; i<NUMPARTS; i++)
        {
            if (plist[i].flags & PART_EXISTS)
            {
                plist[i].vel.x += plist[i].acc.x; plist[i].vel.y += plist[i].acc.y; plist[i].vel.z += plist[i].acc.z;
                plist[i].pos.x += plist[i].vel.x; plist[i].pos.y += plist[i].vel.y; plist[i].pos.z += plist[i].vel.z;
                plist[i].rta.x += plist[i].rai.x; plist[i].rta.y += plist[i].rai.y; plist[i].rta.z += plist[i].rai.z;

                if (plist[i].pos.y > (384 - 30.0))
                {
                    plist[i].flags &= ~PART_EXISTS;
                    continue;
                }

                qmemcpy(vtl, smlpillvtl, smlpillvtl->numv + 2);

                vec ptm[4], nrm[4];

                vec_genMtxRotateX(nrm, plist[i].rta.x);
                vec_mtxRotateY(nrm, plist[i].rta.y);
                vec_mtxRotateZ(nrm, plist[i].rta.z);

                qmemcpy(ptm, nrm, 4);
                vec_mtxXlatV(ptm, &plist[i].pos);

                vec_arrayApplyMtx(&vtl->vlist[2], nrm, vtl->numv / 3, 1, 2);
                vec_arrayApplyMtx(&vtl->vlist[1], ptm, vtl->numv / 3, 1, 2);

                // z-lighting
                for (j=0; j<vtl->numv; j+=3)
                    vecscalef(&vtl->vlist[j], 1, fabsf(vtl->vlist[j+2].z));

                vectoints((int *)vtl->vlist, vtl->vlist, vtl->numv);

                gp_drawVTL_inplace_cpn(&thegp, vtl);
                gp_hardflush(&thegp);

                if (vtl == (vecTriList *)0x700000)
                    vtl = (vecTriList *)0x800000;
                else
                    vtl = (vecTriList *)0x700000;
            }
        }



        gp_frect(&thegp, 0<<4, 340<<4, 512<<4, 380<<4, 0x7fff, 0x3f8f5f);
        for (i=0; i<21*2*2; i+=4)
            gp_line(&thegp, vzzrzzn[i], vzzrzzn[i+1], vzzrzzn[i+2], vzzrzzn[i+3], 0x8000, 0xffffff);

        gp_frect(&thegp, 0<<4, 0<<4, 512<<4, 40<<4, 0x7fff, 0xa24021);

        int xc = (int) ( 256.0 + 240.0 * sinf(theta) );
        gp_frect(&thegp, (xc-8)<<4, 2<<4, (xc+8)<<4, 18<<4, 0x8000, 0xffffff);
        gp_frect(&thegp, 0<<4, 1<<4, 8<<4, 19<<4, 0x8000, 0x0000ff);
        gp_frect(&thegp, 504<<4, 1<<4, 512<<4, 19<<4, 0x8000, 0x0000ff);
        theta += PI * FRD / 120;

        gp_hardflush(&thegp);


        /* optional stuff that we want to do as much of as possible (!) -- like generate particles for example */

        i = 0;
        while (TestVRstart() < FRD)
        {
            if (i >= NUMPARTS)
                continue;

            if (plist[i].flags & PART_EXISTS)
            {
                i++;
                continue;
            }

            plist[i].pos.x = 50.0 + 400.0 * nextr();
            plist[i].pos.y = 30.0; plist[i].pos.z = 500.0 + 1500.0 * nextr(); plist[i].pos.w = 1;
            plist[i].vel.y = FRD * ( 0.5 + 1.0 * nextr() );
            plist[i].vel.x = 0; plist[i].vel.z = 0; plist[i].vel.w = 1;
            plist[i].acc.x = 0; plist[i].acc.y = 0; plist[i].acc.z = 0; plist[i].acc.w = 1;
            plist[i].rta.x = PI / 4 * nextr();
            plist[i].rta.y = PI / 6 * nextr();
            plist[i].rta.z = 0; plist[i].rta.w = 1;
            plist[i].rai.x = FRD * ( 1.8 * PI / 180 + 1.8 * PI / 180 * nextr() );
            plist[i].rai.y = FRD * ( 0.0 * PI / 180 + 0.9 * PI / 180 * nextr() );
            plist[i].rai.z = FRD * ( 0.0 * PI / 180 + 1.4 * PI / 180 * nextr() );
            plist[i].rai.w = 1;
            plist[i].flags |= PART_EXISTS;

            i++;
        }

        /* swap buffers */


        ClearVRcount();
        SetCrtFrameBuffer(whichdrawbuf);
        whichdrawbuf ^= 1;
        SetDrawFrameBuffer(whichdrawbuf);
    }
}


// Make the compiler happy.
extern "C" int _start()
{
    initMainThread();
    return ps2main();
}

