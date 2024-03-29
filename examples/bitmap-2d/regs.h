//------------------------------------------------------------------------
// File:	regs.h
// Author:	Tony Saveski, t_saveski@yahoo.com
// Notes:	Playstation 2 Register Definitions
//------------------------------------------------------------------------
#ifndef REGS_H
#define REGS_H

// MIPS CPU Registsers
#define zero	$0		// Always 0
#define at		$1		// Assembler temporary
#define v0		$2		// Function return
#define v1		$3		//
#define a0		$4		// Function arguments
#define a1		$5
#define a2		$6
#define a3		$7
#define t0		$8		// Temporaries. No need
#define t1		$9		// to preserve in your
#define t2		$10		// functions.
#define t3		$11
#define t4		$12
#define t5		$13
#define t6		$14
#define t7		$15
#define s0		$16		// Saved Temporaries.
#define s1		$17		// Make sure to restore
#define s2		$18		// to original value
#define s3		$19		// if your function
#define s4		$20		// changes their value.
#define s5		$21
#define s6		$22
#define s7		$23
#define t8		$24		// More Temporaries.
#define t9		$25
#define k0		$26		// Reserved for Kernel
#define k1		$27
#define gp		$28		// Global Pointer
#define sp		$29		// Stack Pointer
#define fp		$30		// Frame Pointer
#define ra		$31		// Function Return Address

// Playstation2 GS Privileged Registers
#define pmode		0x12000000	// Setup CRT Controller
#define smode2		0x12000020	// CRTC Video Settings: PAL/NTCS, Interlace, etc.
#define dispfb1		0x12000070	// Setup the CRTC's Read Circuit 1 data source settings
#define display1	0x12000080	// RC1 display output settings
#define dispfb2		0x12000090	// Setup the CRTC's Read Circuit 2 data source settings
#define display2	0x120000a0	// RC2 display output settings
#define extbuf		0x120000b0	// ...
#define extdata		0x120000c0	// ...
#define extwrite	0x120000d0	// ...
#define bgcolor		0x120000e0	// Set CRTC background color
#define csr			0x12001000	// System status and reset
#define imr			0x12001010	// Interrupt Mask Register
#define busdir		0x12001040	// ...
#define siglblid	0x12001080	// ...

// Playstation2 GS General Purpose Registers
#define prim		0x00	// Select and configure current drawing primitive
#define rgbaq		0x01	// Setup current vertex color
#define st			0x02	// ...
#define uv			0x03	// ...
#define xyzf2		0x04	// Set vertex coordinate
#define xyz2		0x05	// Set vertex coordinate and 'kick' drawing
#define tex0_1		0x06	// ...
#define tex0_2		0x07	// ...
#define clamp_1		0x08	// ...
#define clamp_2		0x09	// ...
#define fog			0x0a	// ...
#define xyzf3		0x0c	// ...
#define xyz3		0x0d	// ...
#define tex1_1		0x14	// ...
#define tex1_2		0x15	// ...
#define tex2_1		0x16	// ...
#define tex2_2		0x17	// ...
#define xyoffset_1	0x18	// Mapping from Primitive to Window coordinate system (Context 1)
#define xyoffset_2	0x19	// Mapping from Primitive to Window coordinate system (Context 2)
#define prmodecont	0x1a	// ...
#define prmode		0x1b	// ...
#define texclut		0x1c	// ...
#define scanmsk		0x22	// ...
#define miptbp1_1	0x34	// ...
#define miptbp1_2	0x35	// ...
#define miptbp2_1	0x36	// ...
#define miptbp2_2	0x37	// ...
#define texa		0x3b	// ...
#define fogcol		0x3d	// ...
#define texflush	0x3f	// ...
#define scissor_1	0x40	// Setup clipping rectangle (Context 1)
#define scissor_2	0x41	// Setup clipping rectangle (Context 2)
#define alpha_1		0x42	// ...
#define alpha_2		0x43	// ...
#define dimx		0x44	// ...
#define dthe		0x45	// ...
#define colclamp	0x46	// ...
#define test_1		0x47	// ...
#define test_2		0x48	// ...
#define pabe		0x49	// ...
#define fba_1		0x4a	// ...
#define fba_2		0x4b	// ...
#define frame_1		0x4c	// Frame buffer settings (Context 1)
#define frame_2		0x4d	// Frame buffer settings (Context 2)
#define zbuf_1		0x4e	// ...
#define zbuf_2		0x4f	// ...
#define bitbltbuf	0x50	// Setup Image Transfer Between EE and GS
#define trxpos		0x51	// Setup Image Transfer Coordinates
#define trxreg		0x52	// Setup Image Transfer Size
#define trxdir		0x53	// Set Image Transfer Directon + Start Transfer
#define hwreg		0x54	// ...
#define signal		0x60	// ...
#define finish		0x61	// ...
#define label		0x62	// ...

// Playstation2 DMA Channel Registers
#define gif_chcr	0x1000a000	// GIF Channel Control Register
#define gif_madr	0x1000a010	// Transfer Address Register
#define gif_qwc		0x1000a020	// Transfer Size Register (in qwords)
#define gif_tadr	0x1000a030	// ...

#endif // REGS_H
