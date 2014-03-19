//
// Copyright (c) Hiteg Ltd.  All rights reserved.
//
//	file: DisplayMenu.c
/*++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.
Copyright (c) 2012. Hiteg Ltd  All rights reserved.

Module Name:  
			Display
Abstract:

    Menu to configure Display attached to the TenByTen6410

rev:
	2013.11.29	: added Display Current settings
	2012.03.30	: initial version, by Sven Riemann sven.riemann@hiteg.com

Notes: 
	
	provides function DisplayMenu(DWORD *displayType)
	depends on:
		getChar()
		initializeDisplay()

--*/
#include "DisplayMenu.h"
#include "s3c6410_ldi.h"
#include "utils.h"
#include "loader.h"

// we call this one each time the user chooses a display
// forward declartion, tbf at main.c
extern void identifyDisplay();
extern unsigned char getChar();

static int bpp;

DWORD enterBGColor()
{
	char szCount[7];
	EdbgOutputDebugString("\r\n#############################################\r\n");
	EdbgOutputDebugString("\r\n####   Enter WEB FORMAT RGB, like FFFFFF ####\r\n");
	EdbgOutputDebugString("\r\n#############################################\r\n");
	EdbgOutputDebugString("(eg: FFFFFF for white) >#");
	InputNumericalHex(szCount,6);
	return (mystrtoul(szCount, 16));
}

void __drawLineH16(int x,int y, int w)
{
	int n;
	unsigned short *fb=(unsigned short *)IMAGE_FRAMEBUFFER_PA_START;
	for(n=0;n<w;n++)
	{
		fb[y*OEMGetWidth()+x+n]=0x0;
	}
}
void __drawLineV16(int x,int y, int h)
{
	int n;
	unsigned short *fb=(unsigned short *)IMAGE_FRAMEBUFFER_PA_START;
	for(n=0;n<h;n++)
	{
		fb[(n+y)*OEMGetWidth()+x]=0x0;
	}
}

void __drawLineH24(int x,int y, int w)
{
	int n;
	unsigned  *fb=(unsigned  *)IMAGE_FRAMEBUFFER_PA_START;
	for(n=0;n<w;n++)
	{
		fb[y*OEMGetWidth()+x+n]=0x0;
	}
}
void __drawLineV24(int x,int y, int h)
{
	int n;
	unsigned  *fb=(unsigned  *)IMAGE_FRAMEBUFFER_PA_START;
	for(n=0;n<h;n++)
	{
		fb[(n+y)*OEMGetWidth()+x]=0x0;
	}
}

void __drawLineH(int x,int y, int w)
{
	if(bpp==16) {
		__drawLineH16(x,y,w);
	}else{
		__drawLineH24(x,y,w);}
}
void __drawLineV(int x,int y, int h)
{
	if(bpp==16) {
		__drawLineV16(x,y,h);}
	else{
		__drawLineV24(x,y,h);}
}
extern void displayLogo();

void __TestPattern()
{
	DWORD scr_width, scr_height;
	unsigned  n,m=0;
	unsigned step=128;
	LDI_clearScreen(IMAGE_FRAMEBUFFER_PA_START, 0x00FFFFFF); //white screen
	// black lines
	scr_width=OEMGetWidth();
	scr_height=OEMGetHeight();
	
	for(n=256;n<scr_width;n+=step)
	{
		__drawLineV(n,0,scr_height);
		m++;
		if(m==2)
		{
			m=0;
			if(step>=3)
				step>>=1;
		}
		
	}

	step=64;m=0;

	for(n=64;n<scr_height;n+=step)
	{
		__drawLineH(0,n,scr_width);
		m++;
		if(m==2)
		{
			m=0;
			if(step>=3)
				step>>=1;
		}
		
	}
}

static DWORD current_counter=0;

char *LCD_current_names[]=
{
	"2mA",
	"4mA",
	"7mA",
	"9mA"
};

#define LCDCURRENTNAME LCD_current_names[(OEMgetDisplayCurrent())&0x3]

void DisplayMenu(DWORD *displayType)
{
	DWORD display;
	int n;
	bpp=16;
	current_counter=OEMgetDisplayCurrent();
displayMenu:
	EdbgOutputDebugString("\r\n#############################################\r\n");
	EdbgOutputDebugString("S) Set Display manually\r\n");
	EdbgOutputDebugString("\r\n#############################################\r\n");
	for(n=0; n<MAX_DISPLAYS; n++)
	{
		EdbgOutputDebugString("%d) %s%s\r\n",n+1,(n==*displayType)?"*":"",LDI_getDisplayName(n));
	}
	
	EdbgOutputDebugString("\r\n#############################################\r\n");
	EdbgOutputDebugString("N) No display connected\r\n");
	EdbgOutputDebugString("\r\n#############################################\r\n");
	EdbgOutputDebugString("L) LCD Current: %s\r\n",LCDCURRENTNAME);
	EdbgOutputDebugString("\r\n#############################################\r\n");
	EdbgOutputDebugString("1) %sDepth 16BIT RGB565\r\n",(OEMgetLCDBpp()==16)?"*":"");
	EdbgOutputDebugString("2) %sDepth 24BIT RGB888\r\n",(OEMgetLCDBpp()==24)?"*":"");
	EdbgOutputDebugString("\r\n#############################################\r\n");
	EdbgOutputDebugString("C) Change background Color [%x]\r\n",OEMgetBGColor());
	EdbgOutputDebugString("\r\n#############################################\r\n");
	EdbgOutputDebugString("R) Test-Fill with red\r\n");
	EdbgOutputDebugString("G) Test-Fill with green\r\n");
	EdbgOutputDebugString("B) Test-Fill with blue\r\n");
	EdbgOutputDebugString("W) Test-Fill with white\r\n");
	EdbgOutputDebugString("E) Test-Fill with black\r\n");
	EdbgOutputDebugString("\r\n#############################################\r\n");
	EdbgOutputDebugString("T) Test Pattern\r\n");
	EdbgOutputDebugString("\r\n#############################################\r\n");
	EdbgOutputDebugString("Q) Exit\r\n");
	EdbgOutputDebugString("\r\n#############################################\r\n");
	EdbgOutputDebugString("Choose, Q or ESC to exit\r\n");
	while (TRUE)     
    {        
        unsigned char key=getChar();
        
		if(key=='S')
        {
			EdbgOutputDebugString("Type number [1..%d], press RETURN when done > ", MAX_DISPLAYS);
			
			display=getNumber(0);
			display--;
			if(display<MAX_DISPLAYS)
				OEMsetDisplayType(display);
			initializeDisplay();
			LDI_clearScreen(IMAGE_FRAMEBUFFER_PA_START, OEMgetBGColor());
            goto displayMenu;
        }
		if(key=='L')
		{
			current_counter++;
			if(current_counter==4) current_counter=0;
			OEMsetDisplayCurrent(current_counter);
			initializeDisplay();
			goto displayMenu;
		}
        if(key=='N')
        {
            OEMsetDisplayType(NO_DISPLAY); // defined in bsp_cfg.h
			initializeDisplay();
            goto displayMenu;
        }
        if(key=='1')
        {
            OEMsetLCDBpp(16); // defined in bsp_cfg.h
			initializeDisplay();
			bpp=16;
            goto displayMenu;
        }
        if(key=='C')
        {
			OEMsetBGColor(enterBGColor());
			LDI_clearScreen(IMAGE_FRAMEBUFFER_PA_START, OEMgetBGColor());
            goto displayMenu;
        }
        if(key=='R')
        {
			LDI_clearScreen(IMAGE_FRAMEBUFFER_PA_START, 0x00FF0000);
            goto displayMenu;
        }
        if(key=='T')
        {
			__TestPattern();

            goto displayMenu;
        }
        if(key=='G')
        {
			LDI_clearScreen(IMAGE_FRAMEBUFFER_PA_START, 0x0000FF00);
            goto displayMenu;
        }
        if(key=='D')
        {
			OEMclrLogoHW();
            goto displayMenu;
        }
        if(key=='B')
        {
			LDI_clearScreen(IMAGE_FRAMEBUFFER_PA_START, 0x000000FF);
            goto displayMenu;
        }
        if(key=='W')
        {
			LDI_clearScreen(IMAGE_FRAMEBUFFER_PA_START, 0x00FFFFFF);
            goto displayMenu;
        }
        if(key=='E')
        {
			LDI_clearScreen(IMAGE_FRAMEBUFFER_PA_START, 0x00000000);
            goto displayMenu;
        }
		if(key=='Y')
		{
		 editDisplayMenu(*displayType);
		 goto displayMenu;
		}
        if(key=='2')
        {
            OEMsetLCDBpp(24); // defined in bsp_cfg.h
			initializeDisplay();
			bpp=24;
            goto displayMenu;
        }
		if(key=='Q' || key==27){
			initializeDisplay();
            break;
		}
		if(key=='I'){
			identifyDisplay();
			goto displayMenu;
		}
    };
	
}