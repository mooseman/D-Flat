
#include "dflat.h"

//static int TestProc(WINDOW, MESSAGE, PARAM, PARAM);


/* -------------- the TEST dialog box --------------- */
DIALOGBOX( Test )
    DB_TITLE(        "TEST",    -1,-1,19,57)
    CONTROL(TEXT,    "~Eingabe:",    3, 3, 1, 10, ID_EINGABE)
    CONTROL(VALUEBOX, NULL,       13, 3, 1,40, ID_EINGABE)
    CONTROL(BUTTON,  "   ~OK   ",    46, 7, 1, 8, ID_OK)
    CONTROL(BUTTON,  " ~Cancel ",    46,10, 1, 8, ID_CANCEL)
    CONTROL(BUTTON,  "  ~Help  ",    46,13, 1, 8, ID_HELP)
ENDDB

struct MinMaxValue TestValue,Test1Value;



static int TestProc(WINDOW wnd, MESSAGE msg, PARAM p1, PARAM p2)
{             
        int rtn;
        double Tempvalue;
        struct MinMaxValue TempMinMax;
        char Tempstr[80], *endptr;
        WINDOW cwnd;
        DBOX *db;
       CTLWINDOW *ct;

    switch (msg)    {
                case CREATE_WINDOW:
                    rtn = DefaultWndProc(wnd, msg, p1, p2);
            db = wnd->extension;
            cwnd = ControlWindow(db, ID_EINGABE);

            SendMessage(cwnd, SETTEXTLENGTH, 12, 0);
            PutItemMinMax(wnd,ID_EINGABE,&TestValue);
            PutItemValue(wnd,ID_EINGABE,35.5);
/*            PutItemText(wnd, ID_EINGABE, teststrng);*/
            return rtn;

                case COMMAND:
                        if ((int) p1 == ID_OK && (int) p2 == 0) {
                                if (CheckValue(wnd,ID_EINGABE)) {
                                   Tempvalue=GetItemValue(wnd,ID_EINGABE);
                                   break;
                                } else {
                                   return FALSE;
                                } /* endif */

                                }
                        else {
                              if((int) p1 == ID_CANCEL && (int) p2 == 0 ){
                                break;
                                }
                        break;}
                default: 
                        break;
        }
    return DefaultWndProc(wnd, msg, p1, p2);
}

