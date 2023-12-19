/******************** (C) COPYRIGHT 2018 e-Design Co.,Ltd. *********************
  DS213_APP Process.c                                            Author : bure
*******************************************************************************/
#include "Process.h"
#include "DS213Bios.h"
#include "Sys.h"

#define ARM_MATH_CM3  // Define microcontroller core version for arm_math.h
#include "arm_math.h"


void Y_Measure(void);
void RmsMeasure(u8 ChN);
void TimMeasure(u8 ChN);
void MeasureWaveForm(void);
u16  WaveFormExtract(u16 Xposn);

u8   Trigg = 0, ScrnF = 0, Full = 0, Psmpl = 0, Empty = 0, Frame = 0;
u8   Recod[4*X_SIZE+1]; // Waveform recording buffer (read from file or write while taking screenshot)
s16  MeasurY[4][9];     // [CH_A~D][MAX/MIN/VPP/RMS/AVG/D_V/VTH/B_V/ACT]
u16  MeasurX[4][7];     // [CH_A~D][PTWH/PTWL/PCYC/CYCL/FPS]

u32  gDebug = 0;
u8   gDebugBuf[8] = {0};
u16  posLast = 0;

u32 SAMPLE_FREQ[] = {
    30,         // 1S
    60,         // 500mS
    150,        // 200mS
    300,        // 100mS
    600,        // 50mS
    1500,       // 20mS
    3000,       // 10mS
    6000,       // 5mS
    15000,      // 2mS
    30000,      // 1mS
    60000,      // 500uS
    150000,     // 200uS
    300000,     // 100uS
    600000,     // 50uS
    1500000,    // 20uS
    3000000,    // 10uS
    6000000,    // 5uS
    15000000,   // 2uS
    30000000,   // 1uS
    60000000,   // 500nS
    150000000,  // 200nS
    300000000,  // 100nS
};

/*******************************************************************************
  The maximum and minimum values of each waveform in the statistical display window, the unit is the number of Y coordinate points
*******************************************************************************/
void Y_Measure(void)
{
  u8* p = (u8*)Track;
  u32 SumA = 0, SumB = 0;

  MeasurY[CH_A][MAX] = 0, MeasurY[CH_A][MIN] = 255;
  MeasurY[CH_B][MAX] = 0, MeasurY[CH_B][MIN] = 255;

  MeasurY[CH_C][MAX] = Yn[CH_C]+C_AMPL, MeasurY[CH_C][MIN] = Yn[CH_C];
  MeasurY[CH_D][MAX] = Yn[CH_D]+D_AMPL, MeasurY[CH_D][MIN] = Yn[CH_D];

  // Update maximum and minimum values for channels A and B
  for(u32 i = X_BASE; i < X_SIZE; i++){ // Loop through the waveform data for statistical measurement
    if(MeasurY[CH_A][MAX] < *p) MeasurY[CH_A][MAX] = *p;
    if(MeasurY[CH_A][MIN] > *p) MeasurY[CH_A][MIN] = *p;
    SumA += *p++;
    if(MeasurY[CH_B][MAX] < *p) MeasurY[CH_B][MAX] = *p;
    if(MeasurY[CH_B][MIN] > *p) MeasurY[CH_B][MIN] = *p;
    SumB += *p++;
    p += 2;
  }
  MeasurY[CH_A][ACT]  = (MeasurY[CH_A][MAX] < Y_SIZE) ? 1 : 0;
  MeasurY[CH_A][ACT] &= (MeasurY[CH_A][MIN] > Y_BASE) ? 1 : 0;

  if(MeasurY[CH_A][ACT]){
    MeasurY[CH_A][MAX] = MeasurY[CH_A][MAX]-Yn[CH_A];
    MeasurY[CH_A][MIN] = MeasurY[CH_A][MIN]-Yn[CH_A];
  }

  // Calculate peak-to-peak (VPP) and average (AVG) values for channels A and B
  MeasurY[CH_A][VPP] = MeasurY[CH_A][MAX]-MeasurY[CH_A][MIN];
  MeasurY[CH_A][AVG] = SumA/X_SIZE-Yn[CH_A];

  MeasurY[CH_B][ACT]  = (MeasurY[CH_B][MAX] < Y_SIZE) ? 1 : 0;
  MeasurY[CH_B][ACT] &= (MeasurY[CH_B][MIN] > Y_BASE) ? 1 : 0;

  // Similar calculations for channel B
  if(MeasurY[CH_B][ACT]){
    MeasurY[CH_B][MAX] = MeasurY[CH_B][MAX]-Yn[CH_B];
    MeasurY[CH_B][MIN] = MeasurY[CH_B][MIN]-Yn[CH_B];
  }
  MeasurY[CH_B][VPP] = MeasurY[CH_B][MAX]-MeasurY[CH_B][MIN];
  MeasurY[CH_B][AVG] = SumB/X_SIZE-Yn[CH_B];

  // Calculate vertical threshold values for channels A and B
  MeasurY[CH_A][VTH] = Vt[CH_A]-Yn[CH_A];
  MeasurY[CH_B][VTH] = Vt[CH_B]-Yn[CH_B];

  // Calculate delta and average values for additional measurements
  MeasurY[0][D_V]    = Menu[V_1].Val-Menu[V_2].Val;
  MeasurY[0][B_V]    = Vbat/8;
}

/*******************************************************************************
  Measure the effective value of the waveform of the specified ChN channel in the 
  current display window, the unit is the number of Y coordinate points
*******************************************************************************/
void RmsMeasure(u8 ChN)
{
  s32 Mid = (MeasurY[ChN][MAX]+MeasurY[ChN][MIN])/2;
  u8* ptr = &Track[ChN];
  u32 Curr = 0, Last = 0, BgnF = 0, Ssum = 0, Savg = 0, Xcnt = 0;
  for(u32 x = X_BASE; x < X_SIZE; x++){
    s32 Yin = *ptr-Yn[ChN];
    Curr = (Yin > Mid) ? 1 : 0;
    if(x > X_BASE){                          // Ignore the first data
      if(BgnF != 0) Ssum += Yin*Yin, Xcnt++; // Cumulative squared value after the start of the period
      if(Last != Curr){                      // There is a jump
        u32 Flag = Last*2+Curr;
        if(BgnF == Flag)   Savg = Ssum/Xcnt; // Calculate the average value at the end of the nth period
        else if(BgnF == 0) BgnF = Flag;      // Set the start flag at the beginning of the 0th cycle
      }
    }
    Last = Curr, ptr += 4;
  }
  MeasurY[ChN][RMS] = Sqrt32(Savg);
}
/*******************************************************************************
  Measure the time parameter of the waveform of the specified channel in the 
  current display window, the unit is the number of X coordinate points
*******************************************************************************/
void TimMeasure(u8 ChN)
{
  u32 Mid = (MeasurY[ChN][MAX]+MeasurY[ChN][MIN])/2+Yn[ChN];
  u32 Curr = 0, Last = 0, BgnF = 0, Xcnt = 0, SumH = 0, SumL = 0;
  u32 TwsH = 0, TwsL = 0, TwsN = 0;

  u8* ptr = &Track[ChN];

  for(u32 x = X_BASE; x < X_SIZE; x++){
    if(*ptr > Mid+5) Curr = 1;
    if(*ptr < Mid-5) Curr = 0;
    if(x > X_BASE){                                   // Ignore the first data
      Xcnt++;                                         // Pulse width count+1
      if(Last != Curr){                               // There is a jump
        u32 Flag = Last*2+Curr;
        if(BgnF == 0) BgnF = Flag;                    // Cycle 0 start flag
        else {
          if(Curr == 0) SumH += Xcnt;                 // Cumulative positive pulse width
          else          SumL += Xcnt;                 // Cumulative negative pulse width
          if(BgnF == Flag){                           // End of nth integer period
            TwsN++;                                   // Number of cycles+1
            TwsH = SumH, TwsL = SumL;                 // Save high and low pulse width count
          }
        }
        Xcnt = 0;                                     // Clear pulse width count
      }
    }
    Last = Curr, ptr += 4;
  }
  u32 Mask = (TwsN != 0 && TwsN < 30)? 100 : 0;       // Premultiplied to ensure accuracy 100
  MeasurX[ChN][PTWH] = Mask*TwsH/TwsN;
  MeasurX[ChN][PTWL] = Mask*TwsL/TwsN;
  MeasurX[ChN][CYCL] = Mask*(TwsH+TwsL)/TwsN;
  MeasurX[ChN][FQRN] = Mask*(TwsH+TwsL)/TwsN;
  MeasurX[ChN][DUTY] = Mask*TwsH/(TwsH+TwsL);
  MeasurX[0][D_T]    = 100*abs(Menu[T_2].Val-Menu[T_1].Val);
  MeasurX[0][FPS]    = Fps;
}
void MeasureWaveForm(void)
{
  Y_Measure();                                        // Vertical value measurement
  if(Menu[VM1].Val == RMS) RmsMeasure(Menu[VM1].Src); // Valid value in vertical direction
  if(Menu[VM2].Val == RMS) RmsMeasure(Menu[VM2].Src);
  if(Menu[TM1].Val != D_T) TimMeasure(Menu[TM1].Src); // Measurement of horizontal value
  if(Menu[TM2].Val != D_T) TimMeasure(Menu[TM2].Src);
}

void DispDebugStr(void)
{
  u16To5DecStr(gDebugBuf, gDebug);
  RowPosi(Menu[TM2].X0, Menu[TM2].Y0);
  AddStr(TXT2C, CHAR, gDebugBuf);
}
void DispNum(u16 val, u16 offX, u16 offY)
{
  u16To5DecStr(gDebugBuf, val);
  RowPosi(Menu[TM2].X0+offX, Menu[TM2].Y0+offY);
  AddStr(TXT2C, CHAR, gDebugBuf);
}

void DispStr2(u8 str[], u16 offX, u16 offY)
{
  RowPosi(Menu[TM2].X0+offX, Menu[TM2].Y0+offY);
  AddStr(TXT2C, CHAR, str);
}

// Wave scroll processing
void WaveFormRoll(u16 pos)
{
  u32 *src = (u32*)TrackRoll;
  u32 *dst = (u32*)Track;
  s32 i = 0, newNum = 0;

  if(pos >= posLast) {
    newNum = pos - posLast;
    for(i=0; i<X_SIZE; i++) {
        dst[i] = (i < X_SIZE-newNum) ? dst[i+newNum] : src[posLast + i - (X_SIZE - newNum)];
    }
  }
  posLast = (pos >= X_SIZE) ? X_BASE : pos;
}

/*******************************************************************************
  Display synchronization
*******************************************************************************/
void DispSync(void)
{
  u32 Status = FPGA_CtrlRW(CH_A, SMPL_ST) | FPGA_CtrlRW(CH_B, SMPL_ST);
  Trigg = (Status & TRIGG) ? 1 : 0;
  ScrnF = (Status & SCRNF) ? 1 : 0;
  Psmpl = (Status & PSMPL) ? 1 : 0;
  Full  = (Status & FULL ) ? 1 : 0;
  Empty = (Status & EMPTY) ? 1 : 0;

  u32 SyncM = Menu[SYN].Val;
  u32 Tbase = Menu[TIM].Val;
  u32 Xposn = Menu[XNP].Val;
  u32 ViewP = 30*Xposn*BASE_KP1[Tbase]/1024;         // Window position after interpolation compensation
  u32 Npts = 0;
  switch(SyncM){
    case AUTO:
      // Time base >= 200ms/div Use scroll mode
      if(Tbase <= ROLL_200MS){
        if(SMPL == Menu[RUN].Val){
          Npts = WaveFormExtract(0);                 // Extract from scratch
          WaveFormRoll(Npts);
        }
      } else {
        if(Trigg || (!Trigg && ScrnF && SMPL == Menu[RUN].Val)){
          Npts = WaveFormExtract(ViewP);             // Extract from the specified window position
        }
      }




      // TODO FFT --------------------------------------------------------------------------------------

      #define FFT_LEN 512 // Supported FFT Lengths are 32, 64, 128, 256, 512, 1024, 2048. Limited by what's displayed on screen (360 -> 512/2)
      // const u16 fft_len = 64;  
      float32_t FFTin[FFT_LEN];
      float32_t FFTout[FFT_LEN];
      float32_t FFTmag[FFT_LEN / 2];
      float32_t FFTmagscaled[FFT_LEN / 2];


      for (int i = 0; i < FFT_LEN; i++)
      {
        FFTin[i] = Track[i*4+1];
      }
      
      arm_rfft_fast_instance_f32 rFFT_S;			//Instance structure for the floating-point RFFT/RIFFT function
      arm_rfft_fast_init_f32(&rFFT_S, FFT_LEN);

      // void FFT_calculation(void) {
        float32_t FFTMaxValue;
        float32_t threshold;
        uint32_t MaxIndex;

        arm_rfft_fast_f32(&rFFT_S, FFTin, FFTout, 0);	// Direct FFT calculation
        FFTout[0] = 0; // Remove DC
        FFTout[1] = 0; // Remove DC

        //Calculate the Magnitude
        arm_cmplx_mag_f32(FFTout, FFTmag, FFT_LEN / 2);

      u16 min_len = X_SIZE;
      if (FFT_LEN / 2 < X_SIZE)
        min_len = FFT_LEN / 2;

      u8 avg = 5;
      float32_t val = 0;
      for (int i = 0; i < min_len-avg; i++)
      {
        val = 0;
        for (int j = 0; j < avg-1; j++) val += FFTmag[i + j];      // Avg filter
        val = val / (float)avg;

        val = 40.0f * val / (float)FFT_LEN * (1.05 + (Menu[F_V].Val - 10) / 10.0f);

        float32_t val2 = val;
        if (val > 255.0f) val2 = 255.0f;
        if (val < 1.0f) val2 = 1.0f;

        // Track[4*i+2] = val2;
        FFTmagscaled[i] = val;  // FFT magnitude scaled to screen 
      }

      // Interpolate to fill full screen width
      float* a = FFTmagscaled;
      const u16 len_aa = FFT_LEN/2;
      const u16 len_b = X_SIZE;
      const u16 a_start = ((float)(Menu[T_1].Val - 2) / 356.0f) * len_aa;
      const u16 a_end = ((float)(Menu[T_2].Val - 2) / 356.0f) * len_aa;
      const u16 len_a = abs(a_end - a_start);
      float32_t scale_factor = (float32_t)(len_a-1) / (float32_t)(len_b-1);

      for (u16 i = 0; i < len_b; ++i) {
          float32_t index_f = i * scale_factor + a_start;
          u16 index_low = (u16)index_f;
          u16 index_high = (index_low + 1 < len_a) ? (index_low + 1) : index_low;
          float32_t weight_high = index_f - index_low;
          float32_t weight_low = 1 - weight_high;
          float32_t val = (u16)(a[index_low] * weight_low + a[index_high] * weight_high);

          if (val > 255.0f) val = 255.0f;
          if (val < 1.0f) val = 1.0f;

          Track[4*i+3] = val;
      }
         



        const u8 div_num = 12;
        u32 fft_freq_max = SAMPLE_FREQ[Menu[TIM].Val] / 2.0f;
        u32 fft_freq_start = fft_freq_max * a_start / len_aa;
        u32 fft_freq_end = fft_freq_max * a_end / len_aa;
        u32 fft_freq_div = (fft_freq_end - fft_freq_start) / div_num;

      // TODO END FFT --------------------------------------------------------------------------------------


      DisplayWaveForm();                             // Display the extracted waveform

      
        char suffix[] = " Hz";
        if (fft_freq_end < 10e5)
        {
          suffix[0] = ' ';
          fft_freq_start /= 1.0f;
          fft_freq_end   /= 1.0f;
          fft_freq_div   /= 1.0f;
        } else if (fft_freq_end < 10e8) {
          suffix[0] = 'k';
          fft_freq_start /= 10e3;
          fft_freq_end   /= 10e3;
          fft_freq_div   /= 10e3;
        }
        else if (fft_freq_end < 10e11) {
          suffix[0] = 'M';
          fft_freq_start /= 10e6;
          fft_freq_end   /= 10e6;
          fft_freq_div   /= 10e6;
        }

        DispStr2("  F2:", -8*5, 13*2);
        DispStr2("Fdiv:", -8*5, 13*3);
        DispStr2("  F1:", -8*5, 13*4);
        DispNum(fft_freq_end,   0, 13*2);
        DispNum(fft_freq_div,   0, 13*3);
        DispNum(fft_freq_start, 0, 13*4);
        DispStr2(suffix, 7*6, 13*2);
        DispStr2(suffix, 7*6, 13*3);
        DispStr2(suffix, 7*6, 13*4);


      MeasureWaveForm();
      if(X_SIZE == Npts){
        SmplStart();                                 // Resample
      }
      break;
    case NORM:
      if(Trigg){
        Npts = WaveFormExtract(ViewP);               // Extract from the specified window position
      }
      DisplayWaveForm();                             // Display the extracted waveform
      MeasureWaveForm();
      if(X_SIZE == Npts){
        SmplStart();                                 // Resample
      }
      break;
    case SNGL:
      if(Trigg){
        Npts = WaveFormExtract(ViewP);               // Extract from the specified window position
      } else {
        TrackClr();                                  // Clear waveform trace
      }
      if(Full){
        Menu[RUN].Val  = STOP,                       // Pause after full screen
        Menu[RUN].Flg |= UPDT;                       // Refresh display pause sign
      }
      DisplayWaveForm();                             // Display the extracted waveform
      MeasureWaveForm();
      break;
    case ROLL:
      if(SMPL == Menu[RUN].Val){
        Npts = WaveFormExtract(0);                   // Extract from scratch
        WaveFormRoll(Npts);
      }
      DisplayWaveForm();                             // Display the extracted waveform
      MeasureWaveForm();
      if(X_SIZE == Npts){
        SmplStart();                                 // Resample
      }
      break;
    default:
      break;
  }
}
/*******************************************************************************
  Display waveform data limit
*******************************************************************************/
u16 DataLimit(s16 Data)
{
  return ((Data >= Y_SIZE)? Y_SIZE : (Data <= Y_BASE) ? Y_BASE : Data);
}

// Determine whether the scrolling mode
u8 isSynRoll(void)
{
  return (ROLL == Menu[SYN].Val || (AUTO == Menu[SYN].Val && Menu[TIM].Val <= ROLL_200MS)) ? 1 : 0;
}

/*******************************************************************************
  Display waveform extraction and interpolation
*******************************************************************************/
u16 WaveFormExtract(u16 Xposn)
{
  FPGA_DataWr(A_C_CH, SMPL_RPTR, Xposn);              // Current window position read pointer
  FPGA_DataWr(B_D_CH, SMPL_RPTR, Xposn);

  s32 GainA  = pGain[CH_A*10+Menu[RNA].Val];          // A Channel current gear gain coefficient
  s32 GainB  = pGain[CH_B*10+Menu[RNB].Val];          // B Channel current gear gain coefficient
  u32 Posn1  = Yn[TRCK1], Posn2 = Yn[TRCK2];          // Waveform trace 1, 2 vertical zero position
  u32 Posn3  = Yn[TRCK3], Posn4 = Yn[TRCK4];          // Waveform trace 3, 4 vertical zero position

  s32 i = BASE_KP1[Menu[TIM].Val];                    // Current gear interpolation coefficient
  u32 n = Menu[T3S].Val;                              // n = 2-7 Corresponding to various operations
  u32 m = Menu[T4S].Val;                              // m = 2-5 Corresponds to record 0-3
  u8* q = (u8*)&Recod[m-2];
  u32 r = Recod[4*359+m-2];
  u32 x = 0, k, LastA, LastB, LastC, LastD;
  u8* p = (u8*)Track;

  if(isSynRoll()){
    p = TrackRoll;
  }

  while(1){                           // Read all samples into p until sample is Empty or sample reaches last screen column
    u32 SmplAC = FPGA_SmplRd(A_C_CH);
    u32 SmplBD = FPGA_SmplRd(B_D_CH);
    u16 Status = SmplAC | SmplBD;
    Trigg = (Status & TRIGG) ? 1 : 0;
    ScrnF = (Status & SCRNF) ? 1 : 0;
    Full  = (Status & FULL ) ? 1 : 0;
    Psmpl = (Status & PSMPL) ? 1 : 0;
    Empty = (Status & EMPTY) ? 1 : 0;
    if(Empty) return x;                                // Exit when the buffer is empty

    s32 AmplA = CALIBRATE((SmplAC >> 8)-Posn1, GainA); // A Channel amplitude extraction correction
    u32 CurrA = DataLimit(AmplA+Posn1);                // A Channel display data limit
    s32 AmplB = CALIBRATE((SmplBD >> 8)-Posn2, GainB); // B Channel amplitude extraction correction
    u32 CurrB = DataLimit(AmplB+Posn2);                // B Channel display data limit

    s32 AmplD = (m > 1) ? *q-r : (SmplBD & 1)*D_AMPL;  // D Channel extraction
    u32 CurrD = DataLimit(AmplD+Posn4);                // D Channel display data limit

    s32 AmplC = (SmplAC & 1)*C_AMPL;                   // C Channel amplitude extraction
    switch (n){                                        // C Channel computing
      case A_ADD_B: AmplC = AmplA + AmplB; break;
      case A_SUB_B: AmplC = AmplA - AmplB; break;
      case C_AND_D: AmplC = AmplC & AmplD; break;
      case C_OR_D:  AmplC = AmplC | AmplD; break;
      case INV_A:   AmplC = -AmplA; break;
      case INV_B:   AmplC = -AmplB; break;
    }
    u32 CurrC = DataLimit(AmplC+Posn3);                // C Channel display data limit

    if(x == X_BASE){                                   // Numerical initialization
      *p++ = CurrA, *p++ = CurrB, *p++ = CurrC, *p++ = CurrD;
      k = 0, x++;
    } else {
    #if 1
      s32 DiffA = CurrA-LastA, DiffB = CurrB-LastB;
      s32 DiffC = CurrC-LastC, DiffD = CurrD-LastD;
      while(k <= 1024){                                // Interpolation (not used in all time scales? Look at i and BASE_KP1)
        if(m > 1) q += 4;
        u32 InstA = LastA+DiffA*k/1024, InstB = LastB+DiffB*k/1024;
        *p++ = InstA, *p++ = InstB;
        u32 InstC = LastC+DiffC*k/1024, InstD = LastD+DiffD*k/1024;
        *p++ = InstC, *p++ = InstD;
        k += i;
        if(++x == X_SIZE) return x;                    // Exit when screen last column is reached
      }
      k -= 1024;
    #else
      *p++ = CurrA, *p++ = CurrB, *p++ = CurrC, *p++ = CurrD;
      if(++x == X_SIZE) return x;
    #endif
    }
    LastA = CurrA, LastB = CurrB, LastC = CurrC, LastD = CurrD;
  }
}
/*******************************************************************************
  Update_Range:
*******************************************************************************/
void CtrlUpdate(u8 Item)
{
  u32 Value = Menu[Item].Val;
  u32 Tw;

  switch(Item){
    case RUN:
      if(Menu[RUN].Val == SMPL){
        SmplStart();                             // Restart sampling
        TrackClr();                              // Clear display waveform trace
      }
      break;
    case CPA:
      *Hw.pOut_A_Coupl = Value & 1, YnHide[TRCK1] = !Value; // If "--" then Hide Track#1
      break;
    case CPB:
      *Hw.pOut_B_Coupl = Value & 1, YnHide[TRCK2] = !Value; // If "--" then Hide Track#2
      break;
    case RNA:
      SetRangeA(Value), SetOffsetA(Value, Yn[CH_A]);
      break;
    case RNB:
      SetRangeB(Value), SetOffsetB(Value, Yn[CH_B]);
      break;
    case T3S:
      YnHide[TRCK3] = !Value;                    // If "--" then Hide Track#3
      break;
    case T4S:
      YnHide[TRCK4] = !Value;                    // If "--" then Hide Track#4
      break;
    case YNP:
    case CAL:
      SetOffsetA(Menu[RNA].Val, Yn[CH_A]);
      SetOffsetB(Menu[RNB].Val, Yn[CH_B]);
      break;
    case T_0:
      SetPreSmplDpth(Value*30);                  // Pre-sampling depth
      break;
    case V_T:
      FPGA_ByteWr(A_C_CH, TRIG_VOLT, Vt[CH_A]);
      FPGA_ByteWr(B_D_CH, TRIG_VOLT, Vt[CH_B]);
      break;
    case T_1:
    case T_2:
      if(Menu[T_1].Val > Menu[T_2].Val)
        Tw = Menu[T_1].Val-Menu[T_2].Val;
      else
        Tw = Menu[T_2].Val-Menu[T_1].Val;
      FPGA_DataWr(A_C_CH, TRIG_WDTH, Tw);
      FPGA_DataWr(B_D_CH, TRIG_WDTH, Tw);
      break;
    case OUT:
    case FRQ:
      if(Menu[OUT].Val == DGTL)
        SetDgtlFrq(Menu[FRQ].Val);
      else
        SetAnlgFrq(Menu[FRQ].Val);
      break;
    case TIM:
      SetBase(Value);
      break;
    case TRG:
      SetTriggTyp(Menu[TRG].Val, Menu[V_T].Src);
      break;
    default:
      break;
  }
  SetTriggTyp(isSynRoll() ? TRIG_TYPE_ANY : Menu[TRG].Val, Menu[V_T].Src);
}

/*******************************************************************************
  Dummy Functions:
*******************************************************************************/
int *__errno(void) {
    // Provide a dummy implementation of __errno (needed for CMSIS DSP FFT Library to work)
    static int dummy_errno;
    return &dummy_errno;
}
/******************************** END OF FILE *********************************/
