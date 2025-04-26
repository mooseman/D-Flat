/* --------------------- video.c -------------------- */

#include "dflat.h"

BOOL ClipString;
static BOOL snowy;
#if defined (__WATCOMC__) && defined (__386__)
static unsigned video_address;
static short near vpeek(short  *vp);
static void near vpoke(short *vp, short c);
void movefromscreen(void *bf, int offset, int len);
void movetoscreen(void *bf, int offset, int len);
#else
static unsigned video_address;
static int near vpeek(int far *vp);
static void near vpoke(int far *vp, int c);
void movefromscreen(void *bf, int offset, int len);
void movetoscreen(void *bf, int offset, int len);
#endif

/* -- read a rectangle of video memory into a save buffer -- */
#if defined (__WATCOMC__) && defined (__386__)
void getvideo(RECT rc, void  *bf)
#else
void getvideo(RECT rc, void far *bf)
#endif
{   int temp;              
    int ht = RectBottom(rc)-RectTop(rc)+1;
    int bytes_row = (RectRight(rc)-RectLeft(rc)+1) * 2;
    unsigned vadr = vad(RectLeft(rc), RectTop(rc));
    hide_mousecursor();
    while (ht--)    {
    movefromscreen(bf, vadr, bytes_row);
    vadr += SCREENWIDTH*2;
#if defined (__WATCOMC__) && defined (__386__)
        bf = (char  *)bf + bytes_row;
#else
        bf = (char far *)bf + bytes_row;
#endif
    }
    show_mousecursor();
}

/* -- write a rectangle of video memory from a save buffer -- */
#if defined (__WATCOMC__) && defined (__386__)
void storevideo(RECT rc, void  *bf)
#else
void storevideo(RECT rc, void far *bf)
#endif
{
    int ht = RectBottom(rc)-RectTop(rc)+1;
    int bytes_row = (RectRight(rc)-RectLeft(rc)+1) * 2;
    unsigned vadr = vad(RectLeft(rc), RectTop(rc));
    hide_mousecursor();
    while (ht--)    {
                movetoscreen(bf, vadr, bytes_row);
        vadr += SCREENWIDTH*2;
#if defined (__WATCOMC__) && defined (__386__)
        bf = (char  *)bf + bytes_row;
#else
        bf = (char far *)bf + bytes_row;
#endif
    }
    show_mousecursor();
}

/* -------- read a character of video memory ------- */
unsigned int GetVideoChar(int x, int y)
{
    int c;
    hide_mousecursor();
        if (snowy)
#if defined (__WATCOMC__) && defined (__386__)
            c = vpeek((short*)(video_address|vad(x,y)));
#else
            c = vpeek(MK_FP(video_address, vad(x,y)));
#endif
        else
            c = peek(video_address, vad(x,y));
    show_mousecursor();
    return c;
}

/* -------- write a character of video memory ------- */
void PutVideoChar(int x, int y, int c)
{
    if (x < SCREENWIDTH && y < SCREENHEIGHT)    {
        hide_mousecursor();
                if (snowy)
#if defined (__WATCOMC__) && defined (__386__)
                vpoke((short*)(video_address|vad(x,y)), c);
#else
                vpoke(MK_FP(video_address, vad(x,y)), c);
#endif
                else
                poke(video_address, vad(x,y), c);
        show_mousecursor();
    }
}

BOOL CharInView(WINDOW wnd, int x, int y)
{
        WINDOW nwnd = NextWindow(wnd);
        WINDOW pwnd;
        RECT rc;
    int x1 = GetLeft(wnd)+x;
    int y1 = GetTop(wnd)+y;

        if (!TestAttribute(wnd, VISIBLE))
                return FALSE;
    if (!TestAttribute(wnd, NOCLIP))    {
        WINDOW wnd1 = GetParent(wnd);
        while (wnd1 != NULL)    {
            /* --- clip character to parent's borders -- */
                        if (!TestAttribute(wnd1, VISIBLE))
                                return FALSE;
                        if (!InsideRect(x1, y1, ClientRect(wnd1)))
                return FALSE;
            wnd1 = GetParent(wnd1);
        }
    }
        while (nwnd != NULL)    {
                if (!isHidden(nwnd) /* && !isAncestor(wnd, nwnd) */ )   {
                        rc = WindowRect(nwnd);
                if (TestAttribute(nwnd, SHADOW))    {
                        RectBottom(rc)++;
                        RectRight(rc)++;
                }
                        if (!TestAttribute(nwnd, NOCLIP))       {
                                pwnd = nwnd;
                                while (GetParent(pwnd)) {
                                        pwnd = GetParent(pwnd);
                                        rc = subRectangle(rc, ClientRect(pwnd));
                                }
                        }
                        if (InsideRect(x1,y1,rc))
                                return FALSE;
                }
                nwnd = NextWindow(nwnd);
        }
    return (x1 < SCREENWIDTH && y1 < SCREENHEIGHT);
}

/* -------- write a character to a window ------- */
void wputch(WINDOW wnd, int c, int x, int y)
{
        if (CharInView(wnd, x, y))      {
                int ch = (c & 255) | (clr(foreground, background) << 8);
                int xc = GetLeft(wnd)+x;
                int yc = GetTop(wnd)+y;
        hide_mousecursor();
                if (snowy)
#if defined (__WATCOMC__) && defined (__386__)
                vpoke((short *)(video_address| vad(xc, yc)), ch);
#else
                vpoke(MK_FP(video_address, vad(xc, yc)), ch);
#endif
                else
                poke(video_address, vad(xc, yc), ch);
        show_mousecursor();
        }
}

/* ------- write a string to a window ---------- */
void wputs(WINDOW wnd, void *s, int x, int y)
{
        int x1 = GetLeft(wnd)+x;
        int x2 = x1;
        int y1 = GetTop(wnd)+y;
    if (x1 < SCREENWIDTH && y1 < SCREENHEIGHT && isVisible(wnd))        {
                short ln[200];
                short *cp1 = ln;
            unsigned char *str = s;
            int fg = foreground;
        int bg = background;
            int len;
                int off = 0;
        while (*str)    {
            if (*str == CHANGECOLOR)    {
                str++;
                foreground = (*str++) & 0x7f;
                background = (*str++) & 0x7f;
                continue;
            }
            if (*str == RESETCOLOR)    {
                foreground = fg & 0x7f;
                background = bg & 0x7f;
                str++;
                continue;
            }
                        if (*str == ('\t' | 0x80) || *str == ('\f' | 0x80))
                        *cp1 = ' ' | (clr(foreground, background) << 8);
                        else 
                        *cp1 = (*str & 255) | (clr(foreground, background) << 8);
                        if (ClipString)
                                if (!CharInView(wnd, x, y))
                                        *cp1 = peek(video_address, vad(x2,y1));
                        cp1++;
                        str++;
                        x++;
                        x2++;
        }
        foreground = fg;
        background = bg;
                len = (int)(cp1-ln);
                if (x1+len > SCREENWIDTH)
                len = SCREENWIDTH-x1;

                if (!ClipString && !TestAttribute(wnd, NOCLIP)) {
                        /* -- clip the line to within ancestor windows -- */
                        RECT rc = WindowRect(wnd);
                        WINDOW nwnd = GetParent(wnd);
                        while (len > 0 && nwnd != NULL) {
                                if (!isVisible(nwnd))   {
                                        len = 0;
                                        break;
                                }
                                rc = subRectangle(rc, ClientRect(nwnd));
                                nwnd = GetParent(nwnd);
                        }
                        while (len > 0 && !InsideRect(x1+off,y1,rc))    {
                                off++;
                                --len;
                        }
                        if (len > 0)    {
                                x2 = x1+len-1;
                                while (len && !InsideRect(x2,y1,rc))    {
                                        --x2;
                                        --len;
                                }
                        }
                }
                if (len > 0)    {
                hide_mousecursor();
                        movetoscreen(ln+off, vad(x1+off,y1), len*2);
                show_mousecursor();
                }
    }
}

/* --------- get the current video mode -------- */
void get_videomode(void)
{
    videomode();
    /* ---- Monochrome Display Adaptor or text mode ---- */
        snowy = FALSE;
    if (ismono())
#if defined (__WATCOMC__) && defined (__386__)
        video_address = 0x0b0000;
#else
        video_address = 0xb000;
#endif 
    else        {
        /* ------ Text mode -------- */
#if defined (__WATCOMC__) && defined (__386__)
        video_address = 0x0b8000 + video_page;
#else
        video_address = 0xb800 + video_page;
#endif 
                if (!isEGA() && !isVGA())
                        /* -------- CGA --------- */
                        snowy = cfg.snowy;
        }
}

/* --------- scroll the window. d: 1 = up, 0 = dn ---------- */
void scroll_window(WINDOW wnd, RECT rc, int d)
{
        if (RectTop(rc) != RectBottom(rc))      {
                union REGS regs;
                regs.h.cl = RectLeft(rc);
                regs.h.ch = RectTop(rc);
                regs.h.dl = RectRight(rc);
                regs.h.dh = RectBottom(rc);
                regs.h.bh = clr(WndForeground(wnd),WndBackground(wnd));
                regs.h.ah = 7 - d;
                regs.h.al = 1;
        hide_mousecursor();
#if defined (__WATCOMC__) && defined (__386__)
        int386(VIDEO, &regs, &regs);
#else
        int86(VIDEO, &regs, &regs);
#endif
        show_mousecursor();
        }
}


static void near waitforretrace(void)
{
#ifndef __WATCOMC__
asm             mov             dx,3dah
loop1:
asm             mov             cx,6
loop2:
asm             in              al,dx
asm             test    al,8
asm             jnz             loop2
asm             test    al,1
asm             jz              loop2
asm             cli
loop3:
asm             in              al,dx
asm             test    al,1
asm             loopnz  loop3
asm             sti
asm             jz              loop1
#endif
}

#if defined (__WATCOMC__) && defined (__386__)
void movetoscreen(void *bf, short offset, short len)
{
  if (snowy)
	waitforretrace();
// movedata(FP_SEG(bf), FP_OFF(bf), video_address, offset, len);
  memcpy((short *)(video_address|offset), bf, len);
#else
void movetoscreen(void *bf, int offset, int len)
{
  if (snowy)
	waitforretrace();
  movedata(FP_SEG(bf), FP_OFF(bf), video_address, offset, len);
#endif
}

#if defined (__WATCOMC__) && defined (__386__)
void movefromscreen(void *bf, short offset, short len)
{

  if (snowy)
	waitforretrace();
  memcpy(bf, (short *)(video_address|offset), len);
//  movedata(video_address, offset,     FP_SEG(bf), FP_OFF(bf), len);

#else
void movefromscreen(void *bf, int offset, int len)
{
  if (snowy)
   waitforretrace();
  movedata(video_address, offset,       FP_SEG(bf), FP_OFF(bf), len);
#endif
}



#if defined (__WATCOMC__) && defined (__386__)
static short near vpeek(short *vp)
#else
static int near vpeek(int far *vp)
#endif
{                        
        int c;
        waitforretrace();
        c = *vp;
        return c;
}

#if defined (__WATCOMC__) && defined (__386__)
static void near vpoke(short  *vp, short c)
#else
static void near vpoke(int far *vp, int c)
#endif
{
        waitforretrace();
        *vp = c;
}

