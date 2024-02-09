/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# (c) 2005 Dan Peori <peori@oopo.net>
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#
*/

#include <libgs.h>
#include <libpad.h>


#include "main.h"




#include "bg.c"

#include "stand_0.c"
#include "stand_1.c"
#include "stand_2.c"
#include "stand_3.c"
#include "stwbry.c"

#include "run_0.c"
#include "run_1.c"
#include "run_2.c"
#include "run_3.c"
#include "run_4.c"
#include "run_5.c"

#include "sonic_0.c"
#include "sonic_1.c"
#include "sonic_2.c"
#include "sonic_3.c"
#include "sonic_4.c"
#include "sonic_5.c"
#include "sonic_6.c"
#include "sonic_7.c"
#include "sonic_8.c"
#include "sonic_9.c"
#include "sonic_10.c"
#include "sonic_11.c"
#include "sonic_12.c"


#include "dd0.c"
#include "dd1.c"
#include "dd2.c"
#include "dd3.c"







#define	SCREEN_WIDTH			640	
#define	SCREEN_HEIGHT			448

#define SCREEN_OFFSET_X			2048 - (SCREEN_WIDTH/2)
#define SCREEN_OFFSET_Y			2048 - ((SCREEN_HEIGHT/2))

#define GIF_PACKET_MAX			4




GS_DRAWENV						draw_env[2];	// 2 display environment
GS_DISPENV						disp_env[2];	// 2 draw	 environment


GS_GIF_PACKET					packets[2][GIF_PACKET_MAX]; //we need 2 for double buffering

GS_PACKET_TABLE					packet_table[2];				//we need 2 for double buffering



static unsigned char			pad0buff[256] __attribute__((aligned(64)));
unsigned int					oldpad,pad;


int		point_x=100,point_y=220;
int		dir_x=0, dir_y=0;
int		doom_y= - 300;
int		doom_x=   300;
int		doom_frames=0;



int	 InitGraphics();
void SelectDisplayContex(int contex_id);
void ClearDrawContex(int contex_id);
int DrawStand(GS_PACKET_TABLE *table, int contex_index, int frame_index);
int DrawRun(GS_PACKET_TABLE *table, int contex_index, int frame_index);
int DrawSonic(GS_PACKET_TABLE *table, int contex_index, int frame_index);
int DrawDrDoom(GS_PACKET_TABLE *table, int contex_index, int frame_index, int x, int y);

int DrawBG(GS_PACKET_TABLE *table, int contex_index);
int LoadAnimations();
int LoadBG();
void MovePoint(int delta_x, int delta_y);
void LoadDeviceDrivers();
int ControllerInit(void);






int active_buffer=0; // the buffer that we are currently writiing stuff to
GS_IMAGE	image;
int			imgaddr;



int main()
{

	static int frame_id=0;
	static int frame_pass=0;

	
	InitGraphics();



	packet_table[0].packet_count	= GIF_PACKET_MAX;
	packet_table[0].packet			= &packets[0][0];

	packet_table[1].packet_count	= GIF_PACKET_MAX;
	packet_table[1].packet			= &packets[1][0];



	draw_env[0].bg_color.b = 40;
	draw_env[0].bg_color.r = 40;
	draw_env[0].bg_color.g = 40;
	draw_env[0].bg_color.a = 0x80;

	draw_env[1].bg_color.b = 40;
	draw_env[1].bg_color.r = 40;
	draw_env[1].bg_color.g = 40;
	draw_env[1].bg_color.a = 0x80;
/**/	



	

	// Setup controll pad
	LoadDeviceDrivers();
	ControllerInit();


	LoadBG();
	
	LoadAnimations();



	



	

	while(1)
	{
		frame_pass++;
		
		active_buffer = GsDbGetDrawBuffer();

		GsGifPacketsClear(&packet_table[active_buffer]);		// clear the area that we are going to put the sprites/triangles/....

		oldpad = pad;
		pad = ControllerRead(0);
		





		DrawBG(&packet_table[active_buffer], active_buffer);



		//draw DR DOOM
		doom_y++;
		if(doom_y>150)
		{
			doom_y=150;
			doom_x--;
		}

		if(doom_x< -300)
		{
			doom_y = -300;
			doom_x =  300;
		}

		if(doom_frames>3)
				doom_frames=0;
		DrawDrDoom(&packet_table[active_buffer], active_buffer, doom_frames, doom_x, doom_y);

		






		if(pad & PAD_RIGHT)
		{
			DrawRun(&packet_table[active_buffer], active_buffer,frame_id);
			if(frame_pass>4)
			{
				frame_pass=0;
				frame_id++;
				doom_frames++;
			}
			if(frame_id>5)
			frame_id=0;


			point_x+=6;
		}
		else if(pad & PAD_LEFT)
		{
			DrawRun(&packet_table[active_buffer], active_buffer,frame_id);
			if(frame_pass>4)
			{
				frame_pass=0;
				frame_id--;
				doom_frames++;
			}
			if(frame_id<0)
			frame_id=5;


			point_x-=6;
			
		}
		else if(pad & PAD_CROSS)
		{
			DrawSonic(&packet_table[active_buffer], active_buffer,frame_id);
			if(frame_pass>4)
			{
				frame_pass=0;
				frame_id++;
				doom_frames++;
			}
			if(frame_id>12)
			frame_id=0;


			//use collition to destroy dr doom

			if(frame_id >4)
			{
				if((doom_x+127) > (point_x+100)  &&  (doom_x+127) < (point_x+327) )
				{

					if((doom_y+127) > (point_y+0)  &&  (doom_y+127) < (point_y+200) )
					{

						//reset doom
						doom_y = -300;
						doom_x =  300;

					}
				

				}



			}

			
			
		}
		else
		{


		
			DrawStand(&packet_table[active_buffer], active_buffer,frame_id);				//add stuff to the packet area
		
			
			if(frame_pass>4)
			{
				frame_pass=0;
				frame_id++;
				doom_frames++;
			}
			if(frame_id>3)
				frame_id=0;
		}
		






		


		MovePoint(0,0);









		GsDrawSync(0);	//wait for the previous buffer to finish drawing
		GsVSync(0);

		//display the previous drawn buffer
		SelectDisplayContex(   GsDbGetDisplayBuffer()   ); //tell CRTC which context we want to see on our tv
		
		
		ClearDrawContex(active_buffer);		//clear the drawing contex before we draw on it

		GsGifPacketsExecute(&packet_table[active_buffer], 0); // '0' we dont have to wait because we have 2 buffers.  GsDrawSync(0) will make sure it finish before we draw another
		GsDbSwapBuffer();
	}


	return 0;
}



















int InitGraphics()
{
	static int	env0_address;
	static int	env1_address;

	GsInit();

	GsSetCRTCMode(1, 2, 0);
	GsSetCRTCSettings(CRTC_SETTINGS_DEFAULT1, 255); //display contex 1



	//alloc 2 buffers in vram
	env0_address =  GsVramAllocFrameBuffer(SCREEN_WIDTH, SCREEN_HEIGHT,   GS_PIXMODE_32);
	env1_address =  GsVramAllocFrameBuffer(SCREEN_WIDTH, SCREEN_HEIGHT,   GS_PIXMODE_32);



	/*********SETUP CONTEX 1 ENVIRONMENT*************/

	GsSetDefaultDrawEnv		  (&draw_env[0], SCREEN_OFFSET_X, SCREEN_OFFSET_Y, SCREEN_WIDTH, SCREEN_HEIGHT);
	GsSetDefaultDrawEnvAddress(&draw_env[0], env0_address, SCREEN_WIDTH/64, GS_PIXMODE_32);

	GsSetDefaultDisplayEnv		 (&disp_env[0], 656, 36+(36/2), SCREEN_WIDTH, SCREEN_HEIGHT);
	GsSetDefaultDisplayEnvAddress(&disp_env[0], env1_address, SCREEN_WIDTH/64, GS_PIXMODE_32);


	/*********SETUP CONTEX 2 ENVIRONMENT*************/

	GsSetDefaultDrawEnv		  (&draw_env[1], SCREEN_OFFSET_X, SCREEN_OFFSET_Y, SCREEN_WIDTH, SCREEN_HEIGHT);
	GsSetDefaultDrawEnvAddress(&draw_env[1], env1_address, SCREEN_WIDTH/64, GS_PIXMODE_32);

	GsSetDefaultDisplayEnv		 (&disp_env[1], 656, 36+(36/2), SCREEN_WIDTH, SCREEN_HEIGHT);
	GsSetDefaultDisplayEnvAddress(&disp_env[1], env0_address, SCREEN_WIDTH/64, GS_PIXMODE_32);






	//execute draw/display environment(s)  (contex 1)
	GsPutDrawEnv1	(&draw_env[0]);
	GsPutDisplayEnv1(&disp_env[0]);

	//execute draw/display environment(s)  (contex 2)
	GsPutDrawEnv2	(&draw_env[1]);
	GsPutDisplayEnv2(&disp_env[1]);

	


	//set some common stuff
	GsOverridePrimAttributes(GS_DISABLE, 0, 0, 0, 0, 0, 0, 0, 0);

	// contex 1
	GsEnableAlphaTransparency1(GS_ENABLE, GS_ALPHA_GEQUAL, 0x01, 0x00);
	// contex 2
	GsEnableAlphaTransparency2(GS_ENABLE, GS_ALPHA_GEQUAL, 0x01, 0x00);


	
	GsEnableAlphaBlending1(GS_ENABLE, 0);
	GsEnableAlphaBlending2(GS_ENABLE, 0);


	return 0;
}












void SelectDisplayContex(int contex_id)
{
	// the CRTC is used to select which contex we see on our TV/VGA/HDTV 


	if(contex_id==0)
		GsSetCRTCSettings(CRTC_SETTINGS_DEFAULT1, 255);

	if(contex_id==1)
		GsSetCRTCSettings(CRTC_SETTINGS_DEFAULT2, 255);


}

void ClearDrawContex(int contex_id)
{
	// the CRTC is used to select which contex we see on our TV/VGA/HDTV 

	if(contex_id==0)
		GsClearDrawEnv1(&draw_env[0]);	// clear the draw environment before we draw stuff on it	
	else
		GsClearDrawEnv2(&draw_env[1]);	// clear the draw environment before we draw stuff on it
	
}
















void MovePoint(int delta_x, int delta_y)
{

	if(dir_x==0)point_x -= delta_x;
	if(dir_x==1)point_x += delta_x;

	if(dir_y==0)point_y -= delta_y;
	if(dir_y==1)point_y += delta_y;



	if(point_x >SCREEN_WIDTH)
	{
		point_x= SCREEN_WIDTH;
		dir_x=0;
	}

	if(point_x <0)
	{
		point_x= 0;
		dir_x=1;
	}



	if(point_y >SCREEN_HEIGHT)
	{
		point_y= SCREEN_HEIGHT;
		dir_y=0;
	}

	if(point_y <0)
	{
		point_y= 0;
		dir_y=1;
	}


}




int		bg_addr;

int		stand_0_addr;
int		stand_1_addr;
int		stand_2_addr;
int		stand_3_addr;


int		run_0_addr;
int		run_1_addr;
int		run_2_addr;
int		run_3_addr;
int		run_4_addr;
int		run_5_addr;

int		sonic_0_addr;
int		sonic_1_addr;
int		sonic_2_addr;
int		sonic_3_addr;
int		sonic_4_addr;
int		sonic_5_addr;
int		sonic_6_addr;
int		sonic_7_addr;
int		sonic_8_addr;
int		sonic_9_addr;
int		sonic_10_addr;
int		sonic_11_addr;
int		sonic_12_addr;


int		dd0_addr;
int		dd1_addr;
int		dd2_addr;
int		dd3_addr;





int LoadAnimations()
{

	stand_0_addr =  GsVramAllocTextureBuffer(128, 112,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=88;		
	image.height		=109;		
	image.vram_addr		=stand_0_addr;	
	image.vram_width	=128/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&stand_0, &image);



	stand_1_addr =  GsVramAllocTextureBuffer(128, 112,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=88;		
	image.height		=109;		
	image.vram_addr		=stand_1_addr;	
	image.vram_width	=128/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&stand_1, &image);


	stand_2_addr =  GsVramAllocTextureBuffer(128, 112,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=88;		
	image.height		=109;		
	image.vram_addr		=stand_2_addr;	
	image.vram_width	=128/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&stand_2, &image);


	stand_3_addr =  GsVramAllocTextureBuffer(128, 112,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=88;		
	image.height		=109;		
	image.vram_addr		=stand_3_addr;	
	image.vram_width	=128/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&stand_3, &image);



	// RUN

	run_0_addr =  GsVramAllocTextureBuffer(128, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=128;		
	image.height		=128;		
	image.vram_addr		=run_0_addr;	
	image.vram_width	=128/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&run_0, &image);





	run_1_addr =  GsVramAllocTextureBuffer(128, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=128;		
	image.height		=128;		
	image.vram_addr		=run_1_addr;	
	image.vram_width	=128/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&run_1, &image);





	run_2_addr =  GsVramAllocTextureBuffer(128, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=128;		
	image.height		=128;		
	image.vram_addr		=run_2_addr;	
	image.vram_width	=128/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&run_2, &image);





	run_3_addr =  GsVramAllocTextureBuffer(128, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=128;		
	image.height		=128;		
	image.vram_addr		=run_3_addr;	
	image.vram_width	=128/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&run_3, &image);





	run_4_addr =  GsVramAllocTextureBuffer(128, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=128;		
	image.height		=128;		
	image.vram_addr		=run_4_addr;	
	image.vram_width	=128/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&run_4, &image);





	run_5_addr =  GsVramAllocTextureBuffer(128, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=128;		
	image.height		=128;		
	image.vram_addr		=run_5_addr;	
	image.vram_width	=128/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&run_5, &image);







	// SONIC BOOM


	sonic_0_addr =  GsVramAllocTextureBuffer(256, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=256;		
	image.height		=128;		
	image.vram_addr		=sonic_0_addr;	
	image.vram_width	=256/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&sonic_0, &image);



	sonic_1_addr =  GsVramAllocTextureBuffer(256, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=256;		
	image.height		=128;		
	image.vram_addr		=sonic_1_addr;	
	image.vram_width	=256/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&sonic_1, &image);




	sonic_2_addr =  GsVramAllocTextureBuffer(256, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=256;		
	image.height		=128;		
	image.vram_addr		=sonic_2_addr;	
	image.vram_width	=256/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&sonic_2, &image);




	sonic_3_addr =  GsVramAllocTextureBuffer(256, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=256;		
	image.height		=128;		
	image.vram_addr		=sonic_3_addr;	
	image.vram_width	=256/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&sonic_3, &image);



	sonic_4_addr =  GsVramAllocTextureBuffer(256, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=256;		
	image.height		=128;		
	image.vram_addr		=sonic_4_addr;	
	image.vram_width	=256/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&sonic_4, &image);




	sonic_5_addr =  GsVramAllocTextureBuffer(256, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=256;		
	image.height		=128;		
	image.vram_addr		=sonic_5_addr;	
	image.vram_width	=256/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&sonic_5, &image);



	sonic_6_addr =  GsVramAllocTextureBuffer(256, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=256;		
	image.height		=128;		
	image.vram_addr		=sonic_6_addr;	
	image.vram_width	=256/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&sonic_6, &image);



	sonic_7_addr =  GsVramAllocTextureBuffer(256, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=256;		
	image.height		=128;		
	image.vram_addr		=sonic_7_addr;	
	image.vram_width	=256/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&sonic_7, &image);




	sonic_8_addr =  GsVramAllocTextureBuffer(256, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=256;		
	image.height		=128;		
	image.vram_addr		=sonic_8_addr;	
	image.vram_width	=256/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&sonic_8, &image);




	sonic_9_addr =  GsVramAllocTextureBuffer(256, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=256;		
	image.height		=128;		
	image.vram_addr		=sonic_9_addr;	
	image.vram_width	=256/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&sonic_9, &image);



	sonic_10_addr =  GsVramAllocTextureBuffer(256, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=256;		
	image.height		=128;		
	image.vram_addr		=sonic_10_addr;	
	image.vram_width	=256/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&sonic_10, &image);




	sonic_11_addr =  GsVramAllocTextureBuffer(256, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=256;		
	image.height		=128;		
	image.vram_addr		=sonic_11_addr;	
	image.vram_width	=256/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&sonic_11, &image);


	sonic_12_addr =  GsVramAllocTextureBuffer(256, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=256;		
	image.height		=128;		
	image.vram_addr		=sonic_12_addr;	
	image.vram_width	=256/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&sonic_12, &image);



	// dr doom

	dd0_addr =  GsVramAllocTextureBuffer(128, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=128;		
	image.height		=128;		
	image.vram_addr		=dd0_addr;	
	image.vram_width	=128/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&dd0, &image);



	dd1_addr =  GsVramAllocTextureBuffer(128, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=128;		
	image.height		=128;		
	image.vram_addr		=dd1_addr;	
	image.vram_width	=128/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&dd1, &image);




	dd2_addr =  GsVramAllocTextureBuffer(128, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=128;		
	image.height		=128;		
	image.vram_addr		=dd2_addr;	
	image.vram_width	=128/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&dd2, &image);




	dd3_addr =  GsVramAllocTextureBuffer(128, 128,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=128;		
	image.height		=128;		
	image.vram_addr		=dd3_addr;	
	image.vram_width	=128/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&dd3, &image);


	return 0;
}








int LoadBG()
{

	bg_addr =  GsVramAllocTextureBuffer(320, 320,   GS_PIXMODE_16);
	
	image.x				=0;			
	image.y				=0;			
	image.width			=320;		
	image.height		=240;		
	image.vram_addr		=bg_addr;	
	image.vram_width	=320/64;	
	image.pix_mode		=GS_PIXMODE_16;

	GsLoadImage(&bg_image, &image);





	return 0;
}









int DrawStand(GS_PACKET_TABLE *table, int contex_index, int frame_index)
{

	int		tex_addr;

	QWORD	*p;


	switch(frame_index)
	{
	case 0:
		tex_addr = stand_0_addr;
	break;
	case 1:
		tex_addr = stand_1_addr;
	break;
	case 2:
		tex_addr = stand_2_addr;
	break;
	case 3:
		tex_addr = stand_3_addr;
	break;
	}

	
	
	// top left
		
	p = GsGifPacketsAlloc(table, 2); 

	gs_setGIF_TAG(((GS_GIF_TAG	*)&p[0]), 1,1,0,0,0,0,1,0x0e);
	if(contex_index==0)
		gs_setR_TEX0_1(((GS_R_TEX0	*)&p[1]), tex_addr, 128/64, GS_TEX_16, twh4(88), twh4(109), 1, 1, 0, 0, 0, 0, 0);
	else
		gs_setR_TEX0_2(((GS_R_TEX0	*)&p[1]), tex_addr, 128/64, GS_TEX_16, twh4(88), twh4(109), 1, 1, 0, 0, 0, 0, 0);
							


	p = GsGifPacketsAlloc(table, 7); 

	gs_setGIF_TAG(((GS_GIF_TAG	*)&p[0]), 6,1,0,0,0,0,1,0x0e);
	gs_setR_PRIM(((GS_R_PRIM	*)&p[1]), GS_PRIM_SPRITE,0, 1, 0, 0, 0, 1, contex_index, 0);
	gs_setR_RGBAQ(((GS_R_RGBAQ	*)&p[2]), 0x80, 0x80, 0x80, 0x80, 0.0f);
	gs_setR_UV(((GS_R_UV		*)&p[3]), (0)<<4, (0)<<4);
	gs_setR_XYZ2(((GS_R_XYZ		*)&p[4]), (SCREEN_OFFSET_X+point_x)<<4,		(SCREEN_OFFSET_Y+point_y)<<4, 0x00000000);
	gs_setR_UV(((GS_R_UV		*)&p[5]), (87)<<4, (108)<<4);
	gs_setR_XYZ2(((GS_R_XYZ		*)&p[6]), (SCREEN_OFFSET_X+point_x+(88*2))<<4,		(SCREEN_OFFSET_Y+point_y+(109*2))<<4, 0x00000000);


	

	return 0;

}















int DrawRun(GS_PACKET_TABLE *table, int contex_index, int frame_index)
{

	int		tex_addr;

	QWORD	*p;


	switch(frame_index)
	{
	case 0:
		tex_addr = run_0_addr;
	break;
	case 1:
		tex_addr = run_1_addr;
	break;
	case 2:
		tex_addr = run_2_addr;
	break;
	case 3:
		tex_addr = run_3_addr;
	break;
	case 4:
		tex_addr = run_4_addr;
	break;
	case 5:
		tex_addr = run_5_addr;
	break;
	}

	
	
	// top left
		
	p = GsGifPacketsAlloc(table, 2); 

	gs_setGIF_TAG(((GS_GIF_TAG	*)&p[0]), 1,1,0,0,0,0,1,0x0e);
	if(contex_index==0)
		gs_setR_TEX0_1(((GS_R_TEX0	*)&p[1]), tex_addr, 128/64, GS_TEX_16, twh4(128), twh4(128), 1, 1, 0, 0, 0, 0, 0);
	else
		gs_setR_TEX0_2(((GS_R_TEX0	*)&p[1]), tex_addr, 128/64, GS_TEX_16, twh4(128), twh4(128), 1, 1, 0, 0, 0, 0, 0);
							


	p = GsGifPacketsAlloc(table, 7); 

	gs_setGIF_TAG(((GS_GIF_TAG	*)&p[0]), 6,1,0,0,0,0,1,0x0e);
	gs_setR_PRIM(((GS_R_PRIM	*)&p[1]), GS_PRIM_SPRITE,0, 1, 0, 0, 0, 1, contex_index, 0);
	gs_setR_RGBAQ(((GS_R_RGBAQ	*)&p[2]), 0x80, 0x80, 0x80, 0x80, 0.0f);
	gs_setR_UV(((GS_R_UV		*)&p[3]), (0)<<4, (0)<<4);
	gs_setR_XYZ2(((GS_R_XYZ		*)&p[4]), (SCREEN_OFFSET_X+point_x)<<4,		(SCREEN_OFFSET_Y+point_y)<<4, 0x00000000);
	gs_setR_UV(((GS_R_UV		*)&p[5]), (127)<<4, (127)<<4);
	gs_setR_XYZ2(((GS_R_XYZ		*)&p[6]), (SCREEN_OFFSET_X+point_x+(128*2))<<4,		(SCREEN_OFFSET_Y+point_y+(128*2))<<4, 0x00000000);


	

	return 0;

}







int DrawSonic(GS_PACKET_TABLE *table, int contex_index, int frame_index)
{

	int		tex_addr;

	QWORD	*p;


	switch(frame_index)
	{
	case 0:
		tex_addr = sonic_0_addr;
	break;
	case 1:
		tex_addr = sonic_1_addr;
	break;
	case 2:
		tex_addr = sonic_2_addr;
	break;
	case 3:
		tex_addr = sonic_3_addr;
	break;
	case 4:
		tex_addr = sonic_4_addr;
	break;
	case 5:
		tex_addr = sonic_5_addr;
	break;
	case 6:
		tex_addr = sonic_6_addr;
	break;
	case 7:
		tex_addr = sonic_7_addr;
	break;
	case 8:
		tex_addr = sonic_8_addr;
	break;
	case 9:
		tex_addr = sonic_9_addr;
	break;
	case 10:
		tex_addr = sonic_10_addr;
	break;
	case 11:
		tex_addr = sonic_11_addr;
	break;
	case 12:
		tex_addr = sonic_12_addr;
	break;
	}

	
	
	// top left
		
	p = GsGifPacketsAlloc(table, 2); 

	gs_setGIF_TAG(((GS_GIF_TAG	*)&p[0]), 1,1,0,0,0,0,1,0x0e);
	if(contex_index==0)
		gs_setR_TEX0_1(((GS_R_TEX0	*)&p[1]), tex_addr, 256/64, GS_TEX_16, twh4(256), twh4(128), 1, 1, 0, 0, 0, 0, 0);
	else
		gs_setR_TEX0_2(((GS_R_TEX0	*)&p[1]), tex_addr, 256/64, GS_TEX_16, twh4(256), twh4(128), 1, 1, 0, 0, 0, 0, 0);
							


	p = GsGifPacketsAlloc(table, 7); 

	gs_setGIF_TAG(((GS_GIF_TAG	*)&p[0]), 6,1,0,0,0,0,1,0x0e);
	gs_setR_PRIM(((GS_R_PRIM	*)&p[1]), GS_PRIM_SPRITE,0, 1, 0, 0, 0, 1, contex_index, 0);
	gs_setR_RGBAQ(((GS_R_RGBAQ	*)&p[2]), 0x80, 0x80, 0x80, 0x80, 0.0f);
	gs_setR_UV(((GS_R_UV		*)&p[3]), (0)<<4, (0)<<4);
	gs_setR_XYZ2(((GS_R_XYZ		*)&p[4]), (SCREEN_OFFSET_X+point_x-80)<<4,		(SCREEN_OFFSET_Y+point_y+6)<<4, 0x00000000);
	gs_setR_UV(((GS_R_UV		*)&p[5]), (255)<<4, (127)<<4);
	gs_setR_XYZ2(((GS_R_XYZ		*)&p[6]), (SCREEN_OFFSET_X+point_x+(220*2))<<4,		(SCREEN_OFFSET_Y+point_y+(135*2))<<4, 0x00000000);


	

	return 0;

}









int DrawDrDoom(GS_PACKET_TABLE *table, int contex_index, int frame_index, int x, int y)
{

	int		tex_addr;

	QWORD	*p;


	switch(frame_index)
	{
	case 0:
		tex_addr = dd0_addr;
	break;
	case 1:
		tex_addr = dd1_addr;
	break;
	case 2:
		tex_addr = dd2_addr;
	break;
	case 3:
		tex_addr = dd3_addr;
	break;
	
	}

	
	
	// top left
		
	p = GsGifPacketsAlloc(table, 2); 

	gs_setGIF_TAG(((GS_GIF_TAG	*)&p[0]), 1,1,0,0,0,0,1,0x0e);
	if(contex_index==0)
		gs_setR_TEX0_1(((GS_R_TEX0	*)&p[1]), tex_addr, 128/64, GS_TEX_16, twh4(128), twh4(128), 1, 1, 0, 0, 0, 0, 0);
	else
		gs_setR_TEX0_2(((GS_R_TEX0	*)&p[1]), tex_addr, 128/64, GS_TEX_16, twh4(128), twh4(128), 1, 1, 0, 0, 0, 0, 0);
							


	p = GsGifPacketsAlloc(table, 7); 

	gs_setGIF_TAG(((GS_GIF_TAG	*)&p[0]), 6,1,0,0,0,0,1,0x0e);
	gs_setR_PRIM(((GS_R_PRIM	*)&p[1]), GS_PRIM_SPRITE,0, 1, 0, 0, 0, 1, contex_index, 0);
	gs_setR_RGBAQ(((GS_R_RGBAQ	*)&p[2]), 0x80, 0x80, 0x80, 0x80, 0.0f);
	gs_setR_UV(((GS_R_UV		*)&p[3]), (0)<<4, (0)<<4);
	gs_setR_XYZ2(((GS_R_XYZ		*)&p[4]), (SCREEN_OFFSET_X+x)<<4,		(SCREEN_OFFSET_Y+y)<<4, 0x00000000);
	gs_setR_UV(((GS_R_UV		*)&p[5]), (127)<<4, (127)<<4);
	gs_setR_XYZ2(((GS_R_XYZ		*)&p[6]), (SCREEN_OFFSET_X+x+(128*2))<<4,		(SCREEN_OFFSET_Y+y+(128*2))<<4, 0x00000000);


	

	return 0;

}









int DrawBG(GS_PACKET_TABLE *table, int contex_index)
{

	QWORD	*p;


	
	
	// top left
	
	p = GsGifPacketsAlloc(table, 2); 

	gs_setGIF_TAG(((GS_GIF_TAG	*)&p[0]), 1,1,0,0,0,0,1,0x0e);
	if(contex_index==0)
		gs_setR_TEX0_1(((GS_R_TEX0	*)&p[1]), bg_addr, 320/64, GS_TEX_16, twh4(320), twh4(240), 0, 1, 0, 0, 0, 0, 0);
	else
		gs_setR_TEX0_2(((GS_R_TEX0	*)&p[1]), bg_addr, 320/64, GS_TEX_16, twh4(320), twh4(240), 0, 1, 0, 0, 0, 0, 0);
							


	p = GsGifPacketsAlloc(table, 7); 

	gs_setGIF_TAG(((GS_GIF_TAG	*)&p[0]), 6,1,0,0,0,0,1,0x0e);
	gs_setR_PRIM(((GS_R_PRIM	*)&p[1]), GS_PRIM_SPRITE,0, 1, 0, 0, 0, 1, contex_index, 0);
	gs_setR_RGBAQ(((GS_R_RGBAQ	*)&p[2]), 0x80, 0x80, 0x80, 0x80, 0.0f);
	gs_setR_UV(((GS_R_UV		*)&p[3]), (0)<<4, (0)<<4);
	gs_setR_XYZ2(((GS_R_XYZ		*)&p[4]), (SCREEN_OFFSET_X+0)<<4,		(SCREEN_OFFSET_Y+0)<<4, 0x00000000);
	gs_setR_UV(((GS_R_UV		*)&p[5]), (319)<<4, (239)<<4);
	gs_setR_XYZ2(((GS_R_XYZ		*)&p[6]), (SCREEN_OFFSET_X+SCREEN_WIDTH)<<4,		(SCREEN_OFFSET_Y+SCREEN_HEIGHT)<<4, 0x00000000);


	

	return 0;

}

























int ControllerInit(void)
{
	padInit(0);

	padPortOpen(0, 0, &pad0buff);

	



	

}



unsigned int ControllerRead(int pad_id)
{
	unsigned int	ret;
	unsigned short	pad;
	struct padButtonStatus	data;
	unsigned char	*ptr;



	if(padGetState(pad_id, 0) != 6  && padGetState(pad_id, 0) != 2/*2==pad with no vibrate*/)return 0;



	padRead(pad_id, 0,  &data);

	ptr = (unsigned char *)&data.btns;

	pad = (0xffff ^ ((ptr[0] << 8) | ptr[1]));

	ret = pad;


	return 0xffff ^ data.btns;
}






void LoadDeviceDrivers()
{
	// Load most common drivers
	SifLoadModule("rom0:SIO2MAN", 0, 0);
	SifLoadModule("rom0:MCMAN", 0, 0);
	SifLoadModule("rom0:MCSERV", 0, 0);
	SifLoadModule("rom0:PADMAN", 0, 0);

}


/*EOF*/
