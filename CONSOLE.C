/* ----------- console.c ---------- */

#include "dflat.h"

/* ----- table of alt keys for finding shortcut keys ----- */
static int altconvert[] = {
    ALT_A,ALT_B,ALT_C,ALT_D,ALT_E,ALT_F,ALT_G,ALT_H,
    ALT_I,ALT_J,ALT_K,ALT_L,ALT_M,ALT_N,ALT_O,ALT_P,
    ALT_Q,ALT_R,ALT_S,ALT_T,ALT_U,ALT_V,ALT_W,ALT_X,
    ALT_Y,ALT_Z,ALT_0,ALT_1,ALT_2,ALT_3,ALT_4,ALT_5,
    ALT_6,ALT_7,ALT_8,ALT_9
};

unsigned video_mode;
unsigned video_page;

static int near cursorpos[MAXSAVES];
static int near cursorshape[MAXSAVES];
static int cs;

static union REGS regs;

/* ------------- clear the screen -------------- */
void clearscreen(void)
{
    int ht = SCREENHEIGHT;
    int wd = SCREENWIDTH;
    cursor(0, 0);
    regs.h.al = ' ';
    regs.h.ah = 9;
#if defined(__WATCOMC__) && defined(__386__)
    regs.x.ebx = 7;
    regs.x.ecx = ht * wd;
    int386(VIDEO, &regs, &regs);
#else
    regs.x.bx = 7;
    regs.x.cx = ht * wd;
    int86(VIDEO, &regs, &regs);
#endif
}


void SwapCursorStack(void)
{
  if (cs > 1)	{
    swap(cursorpos[cs-2], cursorpos[cs-1]);
    swap(cursorshape[cs-2], cursorshape[cs-1]);
  }
}

#ifndef MSC
#ifndef __WATCOMC__
#define ZEROFLAG 0x40
/* ---- Test for keystroke ---- */
BOOL keyhit(void)
{
    _AH = 1;
    geninterrupt(KEYBRD);
    return (_FLAGS & ZEROFLAG) == 0;
}
#endif
#endif

/* ---- Read a keystroke ---- */
int getkey(void)
{
    int c;
    while (keyhit() == FALSE)
        ;
    if (((c = bioskey(0)) & 0xff) == 0)
        c = (c >> 8) | 0x1080;
    else
        c &= 0xff;
    return c & 0x10ff;
}

/* ---------- read the keyboard shift status --------- */
int getshift(void)
{
#if defined (__WATCOMC__) && defined (__386__)
    regs.h.ah = 2;
    int386(KEYBRD, &regs, &regs);
    return regs.h.al;
#else
    regs.h.ah = 2;
    int86(KEYBRD, &regs, &regs);
    return regs.h.al;
#endif
}

#if defined (__WATCOMC__) && defined (__386__)
static volatile int  *clk =0x046c;
#else
static volatile int far *clk = MK_FP(0x40,0x6c);
#endif
/* ------- macro to wait one clock tick -------- */
#define wait()          \
{                       \
    int now = *clk;     \
    while (now == *clk) \
        ;               \
}

/* -------- sound a buzz tone ---------- */
void beep(void)
{
    wait();
    outp(0x43, 0xb6);               /* program the frequency */
    outp(0x42, (int) (COUNT % 256));
    outp(0x42, (int) (COUNT / 256));
    outp(0x61, inp(0x61) | 3);      /* start the sound */
    wait();
    outp(0x61, inp(0x61) & ~3);     /* stop the sound  */
}

/* -------- get the video mode and page from BIOS -------- */
void videomode(void)
{
#if defined (__WATCOMC__) && defined (__386__)
    regs.h.ah = 15;
    int386(VIDEO, &regs, &regs);
    video_mode = regs.h.al;
    video_page = regs.x.ebx;
    video_page &= 0xff00;
    video_mode &= 0x7f;
#else
    regs.h.ah = 15;
    int86(VIDEO, &regs, &regs);
    video_mode = regs.h.al;
    video_page = regs.x.bx;
    video_page &= 0xff00;
    video_mode &= 0x7f;
#endif
}

/* ------ position the cursor ------ */
void cursor(int x, int y)
{
#if defined (__WATCOMC__) && defined (__386__)
    videomode();
    regs.x.edx = ((y << 8) & 0xff00) + x;
    regs.h.ah = SETCURSOR;
    regs.x.ebx = video_page;
    int386(VIDEO, &regs, &regs);
#else
    videomode();
    regs.x.dx = ((y << 8) & 0xff00) + x;
    regs.h.ah = SETCURSOR;
    regs.x.bx = video_page;
    int86(VIDEO, &regs, &regs);
#endif
}

/* ------ get cursor shape and position ------ */
static void near getcursor(void)
{
#if defined (__WATCOMC__) && defined (__386__)
    videomode();
    regs.h.ah = READCURSOR;
    regs.x.ebx = video_page;
    int386(VIDEO, &regs, &regs);
#else
    videomode();
    regs.h.ah = READCURSOR;
    regs.x.bx = video_page;
    int86(VIDEO, &regs, &regs);
#endif
}

/* ------- get the current cursor position ------- */
void curr_cursor(int *x, int *y)
{
    getcursor();
    *x = regs.h.dl;
    *y = regs.h.dh;
}

/* ------ save the current cursor configuration ------ */
void savecursor(void)
{
#if defined (__WATCOMC__) && defined (__386__)
    if (cs < MAXSAVES)    {
        getcursor();
        cursorshape[cs] = regs.x.ecx;
        cursorpos[cs] = regs.x.edx;
        cs++;
    }
#else
    if (cs < MAXSAVES)    {
        getcursor();
        cursorshape[cs] = regs.x.cx;
        cursorpos[cs] = regs.x.dx;
        cs++;
    }
#endif
}

/* ---- restore the saved cursor configuration ---- */
void restorecursor(void)
{
#if defined (__WATCOMC__) && defined (__386__)
    if (cs)    {
        --cs;
        videomode();
        regs.x.edx = cursorpos[cs];
        regs.h.ah = SETCURSOR;
        regs.x.ebx = video_page;
        int386(VIDEO, &regs, &regs);
        set_cursor_type(cursorshape[cs]);
    }
#else
    if (cs)    {
        --cs;
        videomode();
        regs.x.dx = cursorpos[cs];
        regs.h.ah = SETCURSOR;
        regs.x.bx = video_page;
        int86(VIDEO, &regs, &regs);
        set_cursor_type(cursorshape[cs]);
    }
#endif
}

/* ------ make a normal cursor ------ */
void normalcursor(void)
{
    set_cursor_type(0x0607);
}

/* ------ hide the cursor ------ */
void hidecursor(void)
{
#if defined (__WATCOMC__) && defined (__386__)
    getcursor();
    regs.h.ch |= HIDECURSOR;
    regs.h.ah = SETCURSORTYPE;
    int386(VIDEO, &regs, &regs);
#else
    getcursor();
    regs.h.ch |= HIDECURSOR;
    regs.h.ah = SETCURSORTYPE;
    int86(VIDEO, &regs, &regs);
#endif
}

/* ------ unhide the cursor ------ */
void unhidecursor(void)
{
#if defined (__WATCOMC__) && defined (__386__)
    getcursor();
    regs.h.ch &= ~HIDECURSOR;
    regs.h.ah = SETCURSORTYPE;
    int386(VIDEO, &regs, &regs);
#else
    getcursor();
    regs.h.ch &= ~HIDECURSOR;
    regs.h.ah = SETCURSORTYPE;
    int86(VIDEO, &regs, &regs);
#endif
}

/* ---- use BIOS to set the cursor type ---- */
void set_cursor_type(unsigned t)
{
#if defined (__WATCOMC__) && defined (__386__)
    videomode();
    regs.h.ah = SETCURSORTYPE;
    regs.x.ebx = video_page;
    regs.x.ecx = t;
    int386(VIDEO, &regs, &regs);
#else
    videomode();
    regs.h.ah = SETCURSORTYPE;
    regs.x.bx = video_page;
    regs.x.cx = t;
    int86(VIDEO, &regs, &regs);
#endif
}

/* ---- test for EGA -------- */
BOOL isEGA(void)
{
#if defined (__WATCOMC__) && defined (__386__)
    if (isVGA())
        return FALSE;
    regs.h.ah = 0x12;
    regs.h.bl = 0x10;
    int386(VIDEO, &regs, &regs);
    return regs.h.bl != 0x10;
#else
    if (isVGA())
        return FALSE;
    regs.h.ah = 0x12;
    regs.h.bl = 0x10;
    int86(VIDEO, &regs, &regs);
    return regs.h.bl != 0x10;
#endif
}

/* ---- test for VGA -------- */
BOOL isVGA(void)
{int temp;
#if defined (__WATCOMC__) && defined (__386__)
    regs.x.eax = 0x1a00;
    int386(VIDEO, &regs, &regs);
    temp=( regs.h.al == 0x1a && regs.h.bl > 6);
    return temp;
#else
    regs.x.ax = 0x1a00;
    int86(VIDEO, &regs, &regs);
    return regs.h.al == 0x1a && regs.h.bl > 6;
#endif 
}

static void Scan350(void)
{
#if defined (__WATCOMC__) && defined (__386__)
    regs.x.eax = 0x1201;
    regs.h.bl = 0x30;
    int386(VIDEO, &regs, &regs);
    regs.h.ah = 0x0f;
    int386(VIDEO, &regs, &regs);
    regs.h.ah = 0x00;
    int386(VIDEO, &regs, &regs);
#else
    regs.x.ax = 0x1201;
    regs.h.bl = 0x30;
    int86(VIDEO, &regs, &regs);
    regs.h.ah = 0x0f;
    int86(VIDEO, &regs, &regs);
    regs.h.ah = 0x00;
    int86(VIDEO, &regs, &regs);
#endif
}

static void Scan400(void)
{
#if defined (__WATCOMC__) && defined (__386__)
    regs.x.eax = 0x1202;
    regs.h.bl = 0x30;
    int386(VIDEO, &regs, &regs);
    regs.h.ah = 0x0f;
    int386(VIDEO, &regs, &regs);
    regs.h.ah = 0x00;
    int386(VIDEO, &regs, &regs);
#else
    regs.x.ax = 0x1202;
    regs.h.bl = 0x30;
    int86(VIDEO, &regs, &regs);
    regs.h.ah = 0x0f;
    int86(VIDEO, &regs, &regs);
    regs.h.ah = 0x00;
    int86(VIDEO, &regs, &regs);
#endif
}

/* ---------- set 25 line mode ------- */
void Set25(void)
{
#if defined (__WATCOMC__) && defined (__386__)
    if (isVGA())	{
        Scan400();
    regs.x.eax = 0x1114;
    }
    else
    regs.x.eax = 0x1111;
    regs.h.bl = 0;
    int386(VIDEO, &regs, &regs);
#else
    if (isVGA())	{
        Scan400();
    regs.x.ax = 0x1114;
    }
    else
    regs.x.ax = 0x1111;
    regs.h.bl = 0;
    int86(VIDEO, &regs, &regs);
#endif
}

/* ---------- set 43 line mode ------- */
void Set43(void)
{
#if defined (__WATCOMC__) && defined (__386__)
    if (isVGA())
        Scan350();
    regs.x.eax = 0x1112;
    regs.h.bl = 0;
    int386(VIDEO, &regs, &regs);
#else
    if (isVGA())
        Scan350();
    regs.x.ax = 0x1112;
    regs.h.bl = 0;
    int86(VIDEO, &regs, &regs);
#endif
}

/* ---------- set 50 line mode ------- */
void Set50(void)
{
#if defined (__WATCOMC__) && defined (__386__)
    if (isVGA())
        Scan400();
    regs.x.eax = 0x1112;
    regs.h.bl = 0;
    int386(VIDEO, &regs, &regs);
#else
    if (isVGA())
        Scan400();
    regs.x.ax = 0x1112;
    regs.h.bl = 0;
    int86(VIDEO, &regs, &regs);
#endif
}

/* ------ convert an Alt+ key to its letter equivalent ----- */
int AltConvert(int c)
{
  int i, a = 0;
  for (i = 0; i < 36; i++)
    if (c == altconvert[i])
      break;
  if (i < 26)
    a = 'a' + i;
  else if (i < 36)
    a = '0' + i - 26;
  return a;
}

#if MSC | __WATCOMC__
int getdisk(void)
{
  unsigned int cd;
  _dos_getdrive(&cd);
  cd -= 1;
  return cd;
}
#endif

