#include "mdma.h"
#include "xtc.h"
#include "xtcpipe.h"

#include <libgraph.h>

static void**
upload(xtcPipeline *pipe, xtcPrimType primtype)
{
	void **nextptr;
	mdmaList *l = xtcState.list;

	xtcpCombineMatrix();
	xtcpUploadLights();

	mdmaFlushGsRegs(xtcState.list);

	nextptr = mdmaNext(l, nil, 14, VIFflush, VIFflush);

	// some uploads and double buffer
	mdmaAddW(l, VIFbase + 0, VIFoffset + pipe->code->offset,
		STCYCL(4,4), UNPACK(V4_32, 7, vuMatrix));
	mdmaAdd(l, xtcState.matrix0);
	mdmaAdd(l, xtcState.matrix1);
	mdmaAdd(l, xtcState.matrix2);
	mdmaAdd(l, xtcState.matrix3);
	mdmaAdd(l, xtcState.xyzwScale);
	mdmaAdd(l, xtcState.xyzwOffset);
	mdmaAdd(l, xtcState.clipConsts);

	mdmaAddW(l, VIFnop, VIFnop, STCYCL(4,4), UNPACK(V4_32, 3, vuGifTag));
	mdmaAddGIFtag(l, 0, 1, 1,primtype, SCE_GIF_PACKED, 3, 0x412);
// TODO: switch when textured or other state
	mdmaAdd(l, xtcState.colorScale);
	// surf props
	mdmaAddF(l, 1.0f, 1.0f, 1.0f, 1.0f);

	mdmaAddW(l, VIFnop, VIFnop, STCYCL(4,4), UNPACK(V4_32, 1, vuCodeSwitch));
	xtcMicrocodeSwitch *swtch;
	if(xtcState.clipping) {
		switch(primtype) {
		case XTC_TRILIST:
			swtch = &pipe->code->swtch[1];
			break;
		case XTC_TRISTRIP:
			swtch = &pipe->code->swtch[2];
			break;
		default:
			printf("warning: no clipping for primtype %d\n", primtype);
			swtch = &pipe->code->swtch[0];
		}
	} else
		swtch = &pipe->code->swtch[0];
	mdmaAddW(l, swtch->process>>3, swtch->buf1, swtch->buf2, 0);

	return nextptr;
}

extern xtcMicrocode xtcCodeDefault;

static xtcPipeline pipe = {
	upload,
	&xtcCodeDefault,
};
xtcPipeline *defaultPipeline = &pipe;
