/********************* (C) COPYRIGHT 2018 e-Design Co.,Ltd. ********************
  DS213_APP Menu.c                                               Author : bure
*******************************************************************************/
#include "Sys.h"
#include "Menu.h"

uc8 MENU_ITEM[][11] = {"Save Param", "Save Dat  ", "Save Buf  ", "Save Bmp  ",
                       "Save Csv  ", "Load Dat  ", "Load Buf  ", "BackLight ",
                       "Buzzer    ", "Standby   ", "Calibrat  " };

uc8  CMD_SEL[][9]   = {"SaveParam", "SaveDat00", "SaveBuf00", "SaveBmp00",
                       "SaveCsv00", "LoadDat00", "LoadBuf00", "BkLight00",
                       "BuzzVol00", "SleepDsbl", "Calibrate",};

uc8  LOADSEL[][9]   = {"LoadDat", "LoadBuf"};
uc8  BKLIGHT[][9]   = {"BkLight"};
uc8  BUZZVOL[][9]   = {"BuzzVol"};

uc8 TK[] = {1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1};
uc8 TM[] = {9, 8, 8, 8, 7, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2};
uc8 TN[] = {0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7};

//=================+=========+=========+=========+=========+=========+=========+
uc8  T_BASE[][8] = { " 1S  ",  "500mS",  "200mS",  "100mS",  "50mS ",  "20mS ",
                     "10mS ",  " 5mS ",  " 2mS ",  " 1mS ",  "500uS",  "200uS",
                     "100uS",  "50uS ",  "20uS ",  "10uS ",  " 5uS ",  " 2uS ",
                     " 1uS ",  "500nS",  "200nS",  "100nS",                   };
//-----------------+---------+---------+---------+---------+---------+---------+
uc8  T_SAPS[][8] = {    "30",     "60",    "150",    "300",    "600",   "1.5k",
                        "3k",     "6k",    "15k",    "30k",    "60k",   "150k",
                      "300k",   "600k",   "1.5M",     "3M",     "6M",    "15M",
                       "25M",   "50M",     "50M",   "100M",                   };
//-----------------+---------+---------+---------+---------+---------+---------+
uc16 BASE_PSC[]  = {  2000-1,   2000-1,    800-1,    400-1,    200-1,     80-1,
                        40-1,     20-1,     10-1,      5-1,      2-1,      1-1,
                         1-1,      1-1,      1-1,      1-1,      1-1,      1-1,
                         1-1,      1-1,      1-1,      1-1,                   };
//-----------------+---------+---------+---------+---------+---------+---------+
uc16 BASE_ARR[]  = {  1667-1,    833-1,    833-1,    833-1,    833-1,    833-1,
                       833-1,    833-1,    667-1,    667-1,    833-1,    667-1,
                       333-1,    167-1,     67-1,     34-1,     17-1,      7-1,
                         4-1,      2-1,      2-1,      1-1,                   };
//-----------------+---------+---------+---------+---------+---------+---------+
uc16 BASE_KP1[]  = {    1024,     1024,     1024,     1024,     1024,     1024,
                        1024,     1024,     1024,     1024,     1024,     1024,
                        1024,     1022,     1018,     1004,     1004,      975,
                         853,      853,      341,      341,                   };
//=================+=========+=========+=========+=========+=========+=========+
uc8  FRQSTR[][8] = {" 10Hz ", " 20Hz ", " 50Hz ", " 100Hz", " 200Hz", " 500Hz",
                    " 1kHz ", " 2kHz ", " 5kHz ", " 10kHz", " 20kHz", " 50kHz",
                    "0.1MHz", "0.2MHz", "0.5MHz", " 1MHz ", " 2MHz ", " 4MHz ",
                    " 6MHz ", " 8MHz "};  // Output Frequency
//-----------------+---------+---------+---------+---------+---------+---------+
uc8  DGTL_PSC[]  = {     179,      179,      179,      179,      179,      179,
                         179,      179,       17,       17,       17,        0,
                           0,        0,        0,        0,        0,        0,
                           0,        0};
//-----------------+---------+---------+---------+---------+---------+---------+
uc16 DGTL_ARR[]  = {   39999,    19999,     7999,     3999,     1999,      799,
                         399,      199,      799,      399,      199,     1439,
                         719,      359,      143,       71,       35,       17,
                          11,        8};
//-----------------+---------+---------+---------+---------+---------+---------+
uc8  ANLG_PSC[]  = {    18-1,     18-1,     18-1,     18-1,     18-1,     18-1,
                        18-1,     18-1,     18-1,     18-1,     18-1,};
//-----------------+---------+---------+---------+---------+---------+---------+
uc16 ANLG_ARR[]  = {  4000-1,   2000-1,    800-1,    400-1,    200-1,     80-1,
                        40-1,     20-1,      8-1,      4-1,     4-1,};
//=================+=========+=========+=========+=========+=========+=========+

//=======+======+======+======+======+======+======+======+======+======+======+
uc8 Y_STR[][6] =
//-------+------+------+------+------+------+------+------+------+------+------+
         {"10mV","20mV","50mV","0.1V","0.2V","0.5V"," 1V "," 2V "," 5V ","!10V!"};
//-------+------+------+------+------+------+------+------+------+------+------+
uc32 Y_SCALE[] =
//-------+------+------+------+------+------+------+------+------+------+------+
         {   400,   800,  2000,  4000,  8000, 20000, 40000, 80000,200000,400000};
//=======+======+======+======+======+======+======+======+======+======+======+

uc8  CURR_ST[][8] = {"STOP", "!RUN!"};                        // Current Running State
uc8  BTRY_ST[][8] = {"~``'", "~``}", "~`;}", "~;;}", "{;;}"}; // Battery Status
uc8  COUPLER[][8] = {" -- ", "!!DC!!","!!AC!!"," DC@", " AC@"};
uc8  V_RANGE[][8] = {"10mV","20mV","50mV","0.1V","0.2V",
                     "0.5V"," 1V "," 2V "," 5V ","10V "};

uc8  V_RANGE2[][8] = {"0.1V","0.2V","0.5V"," 1V "," 2V ",
                     " 5V ","10V ","20V ","50V ","100V"};

uc8  TRACK_3[][8] = {"! -- !", "CH(C)", "(A+B)", "(A-B)",
                     "(C&D)",  "(C|D)", "INV A", "INV B"};    // Track 3 source
uc8  TRACK_4[][8] = {"! -- !", "CH(D)", "REC_A", "REC_B",
                     "REC_C", "REC_D"};                       // Track 4 source
uc8  SYNCTYP[][8] = {"!AUTO!", "!NORM!", "SINGL", "!ROLL!"};  // Sync Mode
uc8  OUT_TYP[][8] = {"Sin\"\"\"", "Tri###", "Saw$$$", "Sqr,,,"}; // Signal Output Type
uc8  TRIGTYP[][8] = {"! \\ !", "! ^ !", "!<Vt!", "!>Vt!",
                     "!<TL!", "!>TL!",  "!<TH!", "!>TH!"};    // Trigger Type Str
uc8  TXMETER[][8] = {"TwH :", "TwL :", "Cycl:", "Frqn:",
                     "Duty:", " [T :", " Fps:"};
uc8  VXMETER[][8] = {"Vmax:", "Vmin:", "Vp-p:", "Vrms:",
                     "Vavg:", " [V :", "Vtrg:", "Vbat:"};
uc8  XNP_MAX[4]   = {3, 21, 55, 123};                         // XNP Limits
uc8  CALIBRA[5]   = "CAL";                                    // Offset Calibration
uc8  Xn_POSN[5]   = "Xpos";                                   // X VIEW WINDOW POS
uc8  Yn_POSN[5]   = "!Yp!";                                   // Track n position
uc8  VTP_STR[5]   = "Vtrg";                                   // Trigg Voltage Str
uc8  V1P_STR[5]   = "!V1!";                                   // V1 Vernie Str
uc8  V2P_STR[5]   = "!V2!";                                   // V2 Vernie Str
uc8  T0P_STR[5]   = "!T0!";                                   // Presampling Depth
uc8  T1P_STR[5]   = "!T1!";                                   // T1 Vernie Str
uc8  T2P_STR[5]   = "!T2!";                                   // T2 Vernie Str
uc8  V_UNIT[][5]  = {"uV", "mV", "V ", "--"};
uc8  T_UNIT[][5]  = {"!nS!", "!uS!", "!mS!", " S ", "---"};
uc8  F_UNIT[][5]  = {"!Hz!", "kHz",  "MHz", "---"};
uc8  D_UNIT[][5]  = {" % ", " % "," % ","---"};
uc8  FSTR[][8]    = {"SaveWav", "LoadWav", "SaveBuf", "SaveCsv"};
uc8  VOLUME[]     =  "Volume";
uc8  BKLGHT[]     =  "LcdBkl";
uc8  STANDBY[]    =  "PwrDnT ";
uc8  TIP1[][11]   = {" Press K3 ", "          ","          "};
uc8  TIP2[][11]   = {"to Confirm", "Completed ","  Error   "};

u8   PopUpdt = 0;

// . The following are the working parameters that can be saved and read out

//=================+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
// Offset error correction    | 10mv| 20mv| 50mv| 0.1v| 0.2v| 0.5v|  1v |  2v |  5v | 10v |
//=================+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
u16 Diff[][10] =  {{ 436,  407,  390,  386,  383,  411,  395,  387,  384,  386},
                   { 421,  406,  396,  392,  391,  419,  404,  396,  393,  394}};
//=================+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
// Gain error correction    | 10mv| 20mv| 50mv| 0.1v| 0.2v| 0.5v|  1v |  2v |  5v | 10v |
//=================+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
u16 Gain0[][10] = {{1143, 1126, 1092, 1088, 1041, 1126, 1109, 1104, 1098, 1056},
                   {1143, 1126, 1109, 1088, 1041, 1126, 1109, 1109, 1136, 1040}};
u16 Gain1[][10] = {{1054, 1044, 1044, 1057, 1021, 1071, 1067, 1063, 1059, 1038},
                   {1061, 1066, 1050, 1063, 1028, 1076, 1067, 1078, 1059, 1024}};
u16 Gain2[][10] = {{1118, 1105, 1101, 1101, 1056, 1139, 1125, 1127, 1119, 1062},
                   {1125, 1116, 1110, 1114, 1064, 1154, 1143, 1144, 1142, 1091}};

//=================+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
u16* pGain; // Hw.pAdcTypStr == AD9288->Gain0, HW9288->Gain1, MXT2088->Gain2

u16 Slope[2]   =  { 2157, 2157,}; // Slope correction factor

u16 Vt[4] = {160, 80, 51, 21};
u16 Yn[4] = {150, 75, 50, 25};
u16 Item  = VM1;                  // Limit 0 ~ VM1
u16 Line  = 0;                    // Limit 0 ~ 10  The current command line in the pop-up window

uimenu Pop[]   = {//........ Val, Src, Flag Working parameters that can be saved and read out
//==============+====+====+====+====+=====+======+===================+
//|     pStr    | x0 | y0 | >= | <= | Val |  Src |       Flag        |
//==============+====+====+====+====+=====+======+===================+
  {&FSTR[0][0],    5, 127,   0,  999,   0, CNUM1,      FNUM+LOOP+UPDT},// [SWAV]
  {&FSTR[1][0],    5, 112,   0,  999,   0, CNUM1,      FNUM+LOOP+UPDT},// [LWAV]
  {&FSTR[2][0],    5,  97,   0,  999,   0, CNUM1,      FNUM+LOOP+UPDT},// [SBUF]
  {&FSTR[3][0],    5,  82,   0,  999,   0, CNUM1,      FNUM+LOOP+UPDT},// [SCSV]
  {&VOLUME[0],     5,  67,   0,  100,  50, CNUM3,      PCNT+    +UPDT},// [SVOL]
  {&BKLGHT[0],     5,  53,  10,  100,  50, CNUM3,      PCNT+    +UPDT},// [SBKL]
  {&STANDBY[0],    5,  37,   0,   60,   2, CNUM3,      TCNT+    +UPDT},// [SPDT]
  {&TIP1[0][0],    5,  19,   0,    0,   0, CNUM2,      PCNT+    +UPDT},
  {&TIP2[0][0],    5,   4,   0,    0,   0, CNUM2,      PCNT+    +UPDT},
//==============+====+====+====+====+=====+======+===================+
};

uimenu Menu[] = {//......... Val, Src, Flag Working parameters that can be saved and read out
//==============+====+====+====+====+=====+======+===================+
//|     pStr    | x0 | y0 | >= | <= | Val |  Src |       Flag        |
//==============+====+====+====+====+=====+======+===================+
  {(u8*)COUPLER,  37, 228,   0,   4,    2, TRCK1, SLCT+LOOP+INVR+UPDT}, // [CPA]: Channel A Coupler
  {(u8*)V_RANGE,  37, 215,   0,   9,    6, TRCK1, SLCT+          UPDT}, // [RNA]: Channel A Range
  {(u8*)COUPLER,  73, 228,   0,   4,    2, TRCK2, SLCT+LOOP+INVR+UPDT}, // [CPB]: Channel B Coupler
  {(u8*)V_RANGE,  73, 215,   0,   9,    6, TRCK2, SLCT+          UPDT}, // [RNB]: Channel B Range
//--------------+----+----+----+----+-----+------+-------------------+
  {(u8*)TRACK_3, 109, 228,   0,   7,    1, TRCK3, SLCT+LOOP+INVR+UPDT}, // [T3S]: Track 3 Source
  {(u8*)TRACK_4, 153, 228,   0,   5,    1, TRCK4, SLCT+LOOP+INVR+UPDT}, // [T4S]: Track 4 source
  {(u8*)Xn_POSN, 197, 228,   0, 123,    0, XATTR,           INVR+UPDT}, // [XNP]: View Window ptr 0~136 Grid
  {(u8*)VTP_STR, 233, 228,   5, 195, VOID, TRCK2,      LOOP+INVR+UPDT}, // [V_T]: CH A~B Trigger threshold
  {(u8*)TRIGTYP, 269, 228,   0,   7,    0, TRCK2, SLCT+LOOP+INVR+UPDT}, // [TRG]: CH A~D Trigger Type
//--------------+----+----+----+----+-----+------+-------------------+
  {(u8*)SYNCTYP, 305, 228,   0,   3,    0, XATTR, SLCT+LOOP+INVR+UPDT}, // [SYN]: Sync Mode
  {(u8*)T_BASE,  305, 215,   0,  21,   15, XATTR, SLCT+          UPDT}, // [TIM]: Time Base Range
  {(u8*)OUT_TYP, 349, 228,   0,   3,    0, FOUTC, SLCT+LOOP+INVR+UPDT}, // [OUT]: Output Wave Type
  {(u8*)FRQSTR,  349, 215,   0,  19,    9, FOUTC, SLCT+          UPDT}, // [FRQ]: Output Frequency
//--------------+----+----+----+----+-----+------+-------------------+
  {(u8*)Yn_POSN, 376, 156,   5, 195, VOID, TRCK2,           INVR+UPDT}, // [YNP]: CH A~D Y Position
  {(u8*)V1P_STR, 376, 140,   2, 198,  175, TXT1C,                UPDT}, // [V_1]: V1 Cursor
  {(u8*)V2P_STR, 376, 124,   2, 198,  150, TXT1C,                UPDT}, // [V_2]: V2 Cursor
  {(u8*)T0P_STR, 376, 108,   1, 132,    6, TRCK2,           INVR+UPDT}, // [T_0]: Presampling ptr 0~136 Grid
  {(u8*)T1P_STR, 376,  92,   2, 358,   90, TXT1C,                UPDT}, // [T_1]: T1 Cursor
  {(u8*)T2P_STR, 376,  76,   2, 358,  120, TXT1C,                UPDT}, // [T_2]: T2 Cursor
  {(u8*)CALIBRA, 376,  60, 350, 480,  400, TRCK1,           INVR+UPDT}, // [CAL]: CH A~B Offset Calibration
//--------------+----+----+----+----+-----+------+-------------------+
  {(u8*)TXMETER, 300,   0,   0,   6,    0, TRCK2, SLCT+LOOP+INVR+UPDT}, // [TM2]: Measure F & T
  {(u8*)TXMETER, 200,   0,   0,   6,    3, TRCK3, SLCT+LOOP+INVR+UPDT}, // [TM1]: Measure F & T
  {(u8*)VXMETER, 100,   0,   0,   7,    0, TRCK2, SLCT+LOOP+INVR+UPDT}, // [VM2]: Measure V
  {(u8*)VXMETER,   0,   0,   0,   7,    0, TRCK1, SLCT+LOOP+INVR+UPDT}, // [VM1]: Measure V
//--------------+----+----+----+----+-----+------+-------------------+
  {(u8*)CURR_ST,   1, 228,   0,   1,    1, N_2_W, SLCT+LOOP+INVR+UPDT}, // [RUN]: Running state
  {(u8*)BTRY_ST,   1, 215,   0,   4,    4, N_2_W, SLCT+          UPDT}, // [BTY]: Battery status
//==============+====+====+====+====+=====+======+===================+
};

void updata_RNAB_p(void)
{
  if(DC_10 == Menu[CPA].Val || AC_10 == Menu[CPA].Val){
    if(Menu[RNA].STR != (u8 *)V_RANGE2){
      Menu[RNA].STR = (u8 *)V_RANGE2;
      Menu[RNA].Flg |= UPDT;
    }
  }else{
    if(Menu[RNA].STR != (u8 *)V_RANGE){
      Menu[RNA].STR = (u8 *)V_RANGE;
      Menu[RNA].Flg |= UPDT;
    }
  }
  if(DC_10 == Menu[CPB].Val || AC_10 == Menu[CPB].Val){
    if(Menu[RNB].STR != (u8 *)V_RANGE2){
      Menu[RNB].STR = (u8 *)V_RANGE2;
      Menu[RNB].Flg |= UPDT;
    }
  }else{
    if(Menu[RNB].STR != (u8 *)V_RANGE){
      Menu[RNB].STR = (u8 *)V_RANGE;
      Menu[RNB].Flg |= UPDT;
    }
  }

}

/*******************************************************************************
  Update the main interface menu display
*******************************************************************************/
void UpdtMainMenu(void)
{
  I32STR_RES Num;
  static u8 Last;
  for(u32 i = 0; i < ITEM_END; i++){
    u32 Flag = Menu[i].Flg;
    if(i == Item && Last != Flashing){
      Flag = (Flag^Flashing) | FLSH;                   // Curr Item Flashing
      Menu[VM1].Flg |= UPDT;                           // Update Vn Display
      Menu[TM1].Flg |= UPDT;                           // Update FTn Display
      Menu[VM2].Flg |= UPDT;                           // Update Vn Display
      Menu[TM2].Flg |= UPDT;                           // Update FTn Display
      Last = Flashing;
    }
    if(Flag & (UPDT | FLSH)){
      if(Flag & UPDT){
        CtrlUpdate(i);                                 // Update hardware settings
        if(i == V_T || i == T_0 || i == XNP) Litimg(); // Update buffer thumbnail
      }
      UpdateTag();                                     // Update signal label
      Menu[i].Flg &= ~(UPDT | FLSH);                   // Clear update flag
      RowPosi(Menu[i].X0, Menu[i].Y0);                 // Set the displayed measurement value. Set the size and position of the display area here
      u32 Indx = Menu[i].Val;
      u32 MxEn = (i >= TM2 && i <= VM1) ? 1 : 0;
      u32 Mssk = (Indx == D_V || Indx == B_V || Indx == D_T || Indx == FPS) ? MxEn : 0;
      u32 Ch_N = (Mssk) ? 0 : Menu[i].Src;
      u32 Numb = (Mssk) ? TXT1C : Ch_N;                // Change the display color number
      u32 Slct = (Flag & SLCT) ? 0x08 : 0x00;
      u8* pStr = (u8*)Menu[i].STR+Indx*Slct;           // String pointer
      u32 Mode = (Flag & INVR) ? INVR : CHAR;
      if(Numb == N_2_W) Numb = Indx ? NORMO : WARNG;   // Automatically switch warning colors
      PrintStr(Numb, Mode, pStr);                      // Display the standard string

//===========================Refresh the two information boxes in the lower left corner below============================
      if(i == VM1 || i == VM2){
        u32 ChAB = Ch_N & 1;
        if(!MeasurY[ChAB][ACT] && Indx <= AVG)
          AddStr(Numb, Mode, " ----- ");
        else {
          u32 Yrange = Y_SCALE[Menu[ChAB ? RNB : RNA].Val];
          u32 Yscale = (Indx == B_V) ? 1000 : Yrange;
          s32 Ymeter = Yscale*MeasurY[ChAB][Indx];

          // Processing attenuation 10 times
          // ChAb determines whether it is currently displayed on channel A or channel B
          // 3 and 4 correspond to selected attenuation 10 times
          if(0 == ChAB && B_V != Indx && (DC_10 == Menu[CPA].Val || AC_10 == Menu[CPA].Val)){
            Ymeter *= 10;
          }
          if(1 == ChAB && B_V != Indx && (DC_10 == Menu[CPB].Val || AC_10 == Menu[CPB].Val)){
            Ymeter *= 10;
          }

          Int32String_sign(&Num, Ymeter);               // Signed string of three significant digits
          AddStr(Numb, Mode, Num.str);                  // Display measurement value
          AddStr(Numb, Mode, (u8*)&V_UNIT[Num.decPos]); // Display voltage measurement dimension
        }
//=========Display the contents of the two boxes in the lower right corner=============
      } else if(i == TM1 || i == TM2){
        u16* pS = &MeasurX[Ch_N][Indx];
        if(*pS == 0)
          AddStr(Numb, Mode, " ----- ");
        else{
          u32  Cb = Menu[TIM].Val;                          // Curr T_Base
          u32  Ts = (*pS)*TK[Cb];
          u32  Tf = (*pS)*TK[Cb];
          u32  Fn = 0, Tn = 0, Dm = 0;
          uc8* pM = NULL;
          switch(Indx){
            case PTWH:
            case PTWL:
            case CYCL:
            case D_T:
              Tn = (Power(10, TM[Cb]-5)*Ts+15)/30*1000;    // Cb >= 1mS
              Dm = (Cb < 10) ? Tn : (Power(10, TM[Cb]-2)*Ts+15)/30;
              Int32String(&Num, (u32)Dm, 3);
              pM = T_UNIT[0];
              pM += Num.decPos*5;
              break;
            case FQRN:
              Fn = (Power(10, TN[Cb]+2)*30+Tf/2)/Tf;       // Cb >= 1mS
              Dm = (Cb < 10) ? Fn : (Power(10, TN[Cb])*30+Tf/2)/Tf*100;
              Int32String(&Num, (u32)Dm, 3);
              pM = F_UNIT[0];
              pM += Num.decPos*5;
              break;
            case DUTY:
              Dm = *pS;
              Int32String(&Num, (u32)Dm, 3);
              pM = D_UNIT[0];
              pM += Num.decPos*5;
              break;
            case FPS:
              Dm = *pS * 1000UL;
              Int32String(&Num, (u32)Dm, 3);
              pM = "fps";
              break;
            default:
              break;
          }
          AddStr(Numb, Mode, Num.str);                      // Display measurement value
          AddStr(Numb, Mode, (u8*)pM);                      // Display measurement dimension
        }
      }
    }
  }
}
/*******************************************************************************

*******************************************************************************/
void PopValInc(u16 Line, u16 Unit)
{
  s32 Temp = Pop[Line].Val+Unit;
  if(Temp <= Pop[Line].Max)     Pop[Line].Val = Temp;
  else if(LOOP & Pop[Line].Flg) Pop[Line].Val = Pop[Line].Min;
}
/*******************************************************************************

*******************************************************************************/
void PopValDec(u16 Line, u16 Unit)
{
  s32 Temp = Pop[Line].Val-Unit;
  if(Temp >= Pop[Line].Min)     Pop[Line].Val = Temp;
  else if(LOOP & Pop[Line].Flg) Pop[Line].Val = Pop[Line].Max;
}
/*******************************************************************************

*******************************************************************************/
u16 IndxInc(u16 Val, u16 Unit, u16 Indx)
{
  s32 Temp = Val+Unit;
  return (Temp <= Menu[Indx].Max) ? Temp :
         (LOOP &  Menu[Indx].Flg) ? Menu[Indx].Min : Val;
}
/*******************************************************************************

*******************************************************************************/
u16 IndxDec(u16 Val, u16 Unit, u16 Indx)
{
  s32 Temp = Val-Unit;
  return (Temp >= Menu[Indx].Min) ? Temp :
         (LOOP &  Menu[Indx].Flg) ? Menu[Indx].Max : Val;
}

/******************************** END OF FILE *********************************/
