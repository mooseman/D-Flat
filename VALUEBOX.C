/* ------------- valuebox.c ------------ */
#include "dflat.h"

/* ---------- local prototypes ----------- */
static int KeyboardMsg(WINDOW wnd, PARAM p1, PARAM p2);
static int UpMsg(WINDOW wnd);
static int DnMsg(WINDOW wnd);
static int PgUpMsg(WINDOW wnd);
static int PgDnMsg(WINDOW wnd);
int SetMinMax(WINDOW wnd, PARAM mmi);
int GetMinMax(WINDOW wnd, MINMAXVALUE mmi);
int ValueBoxProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2);





/* ----------- KEYBOARD Message ---------- */
static int KeyboardMsg(WINDOW wnd, PARAM p1, PARAM p2)
{
    int c = (int) p1;

/*   if ( ((int) p2 ) )
      return TRUE;
*/
   if (wnd->MinMax!=NULL && wnd->MinMax->Int==TRUE)                                
           {
               if ( (c<32) || c>255 || ((c>=48) && (c<=57)) || (c=='+') || (c=='-') )
                      {
                      ;
                      } else {
                              return FALSE;
                      }
            }else {  
                if ( (c<32) || c>255 || ((c>=48) && (c<=57)) || (c=='+') || (c=='-') || (c=='E') || (c=='e') || (c=='.') ) 
                      {
                       ;
                      }else{
                              return FALSE;
                      }
            }
 
    switch (c) {
    case UP:
       UpMsg(wnd);
       break;
    case DN:        
       DnMsg(wnd);
       break;
    case PGUP:
       PgUpMsg(wnd);
       break;
    case PGDN:
       PgDnMsg(wnd);
       break;
    default:
      break;
    } /* endswitch */

 return TRUE;
}

/* ----------- UP Message ----------------*/
static int UpMsg(WINDOW wnd)
{char cp[80],*endptr;
 double diff,value;
 long int i_value,i;

if (wnd->MinMax!=NULL)
        {
        strcpy(cp,wnd->text);
        diff=wnd->MinMax->Diff;
        if (wnd->MinMax->Int==FALSE)
                   {
                   value=strtod(wnd->text,&endptr);
                   value+=diff;
                   if (value > wnd->MinMax->Max) 
                          {
/*                          sprintf(wnd->text,"%*f",wnd->MaxTextLength,wnd->MinMax->Max);*/
                            sprintf(wnd->text,"%g",wnd->MinMax->Max);

                          wnd->CurrCol=1;
                          SendMessage(wnd,PAINT,0,0);
                          } else {
                                 if (value < wnd->MinMax->Min) 
                                        {
/*                                        sprintf(wnd->text,"%*f",wnd->MaxTextLength,wnd->MinMax->Min);*/
                                        sprintf(wnd->text,"%g",wnd->MinMax->Min);

                                        wnd->CurrCol=1;
                                        SendMessage(wnd,PAINT,0,0);
                                        } else {
/*                                               sprintf(wnd->text,"%*f",wnd->MaxTextLength,value);*/
                                               sprintf(wnd->text,"%g",value);
                                               wnd->CurrCol=1;
                                               SendMessage(wnd,PAINT,0,0);
                                        }
                          }
                   return 1;
                   } else {
                          i_value=strtol(wnd->text,&endptr,10);
                          i_value+=(long)diff;
                          if (i_value > wnd->MinMax->Max)
                                {
/*                                sprintf(wnd->text,"%*d",wnd->MaxTextLength,(long)wnd->MinMax->Max);*/
                                sprintf(wnd->text,"%d",(long)wnd->MinMax->Max);
                                wnd->CurrCol=1;
                                SendMessage(wnd,PAINT,0,0);
                                } else {
                                        if (i_value<wnd->MinMax->Min)
                                                {
/*                                                sprintf(wnd->text,"%*d",wnd->MaxTextLength,(long)wnd->MinMax->Min);*/
                                                sprintf(wnd->text,"%d",(long)wnd->MinMax->Max);
                                                wnd->CurrCol=1;
                                                SendMessage(wnd,PAINT,0,0);
                                                } else {
/*                                                        sprintf(wnd->text,"%*d",wnd->MaxTextLength,i_value);*/
                                                        sprintf(wnd->text,"%d",i_value);
                                                        wnd->CurrCol=1;
                                                        SendMessage(wnd,PAINT,0,0);
                                                }
                                }
                          return 1;
                   }
        }else {
         return FALSE;
        }
}

/* ----------- DN Message ----------------*/
static int DnMsg(WINDOW wnd)
{char cp[80],*endptr;
 double diff,value;
 long int i_value,i;

if (wnd->MinMax!=NULL)
        {
        strcpy(cp,wnd->text);
        diff=wnd->MinMax->Diff;
        if (wnd->MinMax->Int==FALSE)
                   {
                   value=strtod(wnd->text,&endptr);
                   value-=diff;
                   if (value > wnd->MinMax->Max) 
                          {
/*                          sprintf(wnd->text,"%*f",wnd->MaxTextLength,wnd->MinMax->Max);*/
                          sprintf(wnd->text,"%g",wnd->MinMax->Max);
                          wnd->CurrCol=1;
                          SendMessage(wnd,PAINT,0,0);
                          } else {
                                 if (value < wnd->MinMax->Min) 
                                        {
/*                                        sprintf(wnd->text,"%*f",wnd->MaxTextLength,wnd->MinMax->Min);*/
                                        sprintf(wnd->text,"%g",wnd->MinMax->Min);
                                        wnd->CurrCol=1;
                                        SendMessage(wnd,PAINT,0,0);
                                        } else {
/*                                               sprintf(wnd->text,"%*f",wnd->MaxTextLength,value);*/
                                               sprintf(wnd->text,"%g",value);
                                               wnd->CurrCol=1;
                                               SendMessage(wnd,PAINT,0,0);
                                        }
                          }
                   return 1;
                   } else {
                          i_value=strtol(wnd->text,&endptr,10);
                          i_value-=(long)diff;
                          if (i_value > wnd->MinMax->Max)
                                {
/*                                sprintf(wnd->text,"%*d",wnd->MaxTextLength,(long)wnd->MinMax->Max);*/
                                sprintf(wnd->text,"%d",(long)wnd->MinMax->Max);
                                wnd->CurrCol=1;
                                SendMessage(wnd,PAINT,0,0);
                                } else {
                                        if (i_value<wnd->MinMax->Min)
                                                {
/*                                                sprintf(wnd->text,"%*d",wnd->MaxTextLength,(long)wnd->MinMax->Min);*/
                                                sprintf(wnd->text,"%d",(long)wnd->MinMax->Min);
                                                wnd->CurrCol=1;
                                                SendMessage(wnd,PAINT,0,0);
                                                } else {
/*                                                        sprintf(wnd->text,"%*d",wnd->MaxTextLength,i_value);*/
                                                        sprintf(wnd->text,"%d",i_value);
                                                        wnd->CurrCol=1;
                                                        SendMessage(wnd,PAINT,0,0);
                                                }
                                }
                          return 1;
                   }
        }else {
         return FALSE;
        }
}



/* ----------- PGUP Message -------------*/
static int PgUpMsg(WINDOW wnd)
{ double value;
  long i_value,i;

if (wnd->MinMax!=NULL) {
        if (wnd->MinMax->Int==FALSE) 
                {
                value=wnd->MinMax->Max;
/*                sprintf(wnd->text,"%*f",wnd->MaxTextLength,value);*/
                sprintf(wnd->text,"%g",value);
                wnd->CurrCol=1;
                SendMessage(wnd,PAINT,0,0);
                return 1;
                } else {
                        i_value=(long)wnd->MinMax->Max;
/*                        sprintf(wnd->text,"%*d",wnd->MaxTextLength,i_value);*/
                        sprintf(wnd->text,"%d",i_value);
                        wnd->CurrCol=1;
                        SendMessage(wnd,PAINT,0,0);
                        return 1;
                }
} else {
        return 0;
}
}


/* ----------- PGDN Message -------------*/
static int PgDnMsg(WINDOW wnd)
{ double value;
  long i_value,i;

if (wnd->MinMax!=NULL) {
        if (wnd->MinMax->Int==FALSE)
                {
                value=wnd->MinMax->Min;
/*                sprintf(wnd->text,"%*f",wnd->MaxTextLength,value);*/
                sprintf(wnd->text,"%g",value);

                SendMessage(wnd,PAINT,0,0);
                return 1;
                }else {
                        i_value=(long) wnd->MinMax->Min;
/*                        sprintf(wnd->text,"%*d",wnd->MaxTextLength,i_value);*/
                        sprintf(wnd->text,"%d",i_value);
                        SendMessage(wnd,PAINT,0,0);
                        return 1;
                }
} else {
        return 0;
}
}


/* */
int SetMinMax(WINDOW wnd, PARAM mmi)
{

wnd->MinMax = (MINMAXVALUE) mmi;

}


/* */
int GetMinMax(WINDOW wnd, MINMAXVALUE mmi)
{
if (wnd->MinMax!=NULL) {
        mmi->Min = wnd->MinMax->Min;
        mmi->Max = wnd->MinMax->Max;
        mmi->Int = wnd->MinMax->Int;
        return 1;
} 
return 0;
}

/* ------- Window processing module for ValueBOX class ------ */
int ValueBoxProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{
    int rtn;
    MINMAXVALUE mmi;
    switch (msg)    {
         case KEYBOARD:
         if (KeyboardMsg(wnd, p1, p2)){
               break;
            } else {
               return TRUE;
            } /* endif */
         case ADDMINMAX:
            SetMinMax(wnd, p1);  
            break;
         case GETMINMAX:
            mmi=(MINMAXVALUE) p1;
            GetMinMax(wnd, mmi);
            break;                  
         case SETFOCUS:
   /*         if (p1) {
            beep();
            beep();
            delay(500);
            } else {
            beep();
                delay(200);
            beep();
                delay(500);
            }*/ /* endif */
            break;
         case CLOSE_WINDOW:
            break;
         default:
            break;
        }
    return BaseWndProc(VALUEBOX, wnd, msg, p1, p2);
}

