/* ------------- mouse.c ------------- */

#include "dflat.h"


static union REGS inregs,regs;
static struct SREGS sregs;

#if defined (__WATCOMC__) && defined (__386__)
static void mouse(short m1,short m2,short m3,short m4)
{ 
   inregs.w.dx = m4;
   inregs.w.cx = m3;
   inregs.w.bx = m2;
   inregs.w.ax = m1;
   int386(MOUSE, &inregs, &regs);


}
#else
static void near mouse(int m1,int m2,int m3,int m4)
{
	regs.x.dx = m4;
	regs.x.cx = m3;
	regs.x.bx = m2;
	regs.x.ax = m1;
	int86x(MOUSE, &regs, &regs, &sregs);
}
#endif

#if defined (__WATCOMC__) && defined (__386__)
/* ---------- reset the mouse ---------- */
void resetmouse(void)
{   int386(0x33, &inregs, &regs);
	 mouse(0,0,0,0);
}
#else
/* ---------- reset the mouse ---------- */
void resetmouse(void)
{   int86(0x33, &inregs, &regs);
	 mouse(0,0,0,0);
}

#endif


/* ----- test to see if the mouse driver is installed ----- */
BOOL mouse_installed(void)
{
    static int installed = -1;
 
    if (installed == -1)
    {
        if (SCREENWIDTH > 80)
            installed = 0;
        else
        {
#if defined (__WATCOMC__) && defined (__386__)
            unsigned char *ms;
            unsigned short rm_mouse_seg, rm_mouse_off;
 
            rm_mouse_seg = *(unsigned short *)(MOUSE*4+2);
            rm_mouse_off = *(unsigned short *)(MOUSE*4);
 
            ms = (unsigned char *)((rm_mouse_seg << 4) + rm_mouse_off);
#else
            unsigned char far *ms;
 
            ms = MK_FP(peek(0, 0x33*4+2), peek(0, 0x33*4));
#endif 
            installed = (ms != NULL && *ms != 0xcf);
        }
    }
    return installed;
}



/* ------ return true if mouse buttons are pressed ------- */
int mousebuttons(void)
{
   if (mouse_installed())	{
     mouse(3,0,0,0);
#if defined(__WATCOMC__) && defined(__386__)
	    return regs.w.bx & 3;
#else
	    return regs.x.bx & 3;
#endif
	}
	return 0;
}

/* ---------- return mouse coordinates ---------- */
void get_mouseposition(int *x, int *y)
{
	*x = *y = -1;
    if (mouse_installed())    {
        mouse(3,0,0,0);
#if defined(__WATCOMC__) && defined(__386__)
        *x = regs.w.cx/8;
        *y = regs.w.dx/8;
#else
        *x = regs.x.cx/8;
        *y = regs.x.dx/8;
#endif
		if (SCREENWIDTH == 40)
			*x /= 2;
    }
}

/* -------- position the mouse cursor -------- */
void set_mouseposition(int x, int y)
{
    if (mouse_installed())	{
		segread(&sregs);
		if (SCREENWIDTH == 40)
			x *= 2;
        mouse(4,0,(short)x*8,(short)y*8);
	}
}

/* --------- display the mouse cursor -------- */
void show_mousecursor(void)
{
    if (mouse_installed())	{
       mouse(1,0,0,0);
	}
}

/* --------- hide the mouse cursor ------- */
void hide_mousecursor(void)
{
    if (mouse_installed())	{
        mouse(2,0,0,0);
        }
}

/* --- return true if a mouse button has been released --- */
int button_releases(void)
{
    if (mouse_installed())	{
         mouse(6,0,0,0);
#if defined(__WATCOMC__) && defined(__386__)
	    return regs.w.bx;
#else
	    return regs.x.bx;
#endif
	}
	return 0;
}

/* ----- set mouse travel limits ------- */
void set_mousetravel(int minx, int maxx, int miny, int maxy)
{
    if (mouse_installed())	{
		if (SCREENWIDTH == 40)	{
			minx *= 2;
			maxx *= 2;
		}
   mouse(7, 0, (short)minx*8, (short)maxx*8);
   mouse(8, 0, (short)miny*8, (short)maxy*8);
	}
}


/*
BOOL mouse_installed(void)
{
    if (SCREENWIDTH > 80)
        return 0;
    else
    {   unsigned char *ms;
        unsigned short rm_mouse_seg, rm_mouse_off;
        rm_mouse_seg = *(unsigned short *)(MOUSE*4+2);
        rm_mouse_off = *(unsigned short *)(MOUSE*4);
        ms = (unsigned char *)((rm_mouse_seg << 4) + rm_mouse_off);
        return (ms != NULL && *ms != 0xcf)
    }
}
*/  

