# fptoint.s

    .globl fptosi
    .ent fptosi
fptosi:
    cvt.w.s $f12, $f12
    mfc1 $2, $f12

    jr $31
    nop
    .end fptosi


    .globl fptoui
    .ent fptoui
fptoui:
    abs.s $f12, $f12
    cvt.w.s $f12, $f12
    mfc1 $2, $f12

    jr $31
    nop
    .end fptoui

