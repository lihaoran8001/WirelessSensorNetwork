#include "contiki.h"
#include "stdlib.h"
#include <math.h>
#include <stdio.h> /* For printf() */

# define sig_len 512
# define trans_len 8 
# define coef_len 4
#define NAN (0.0/0.0)

// static const double S[] = {
//   0.353553390593273762200422,
//   0.254897789552079584470970,
//   0.270598050073098492199862,
//   0.300672443467522640271861,
//   0.353553390593273762200422,
//   0.449988111568207852319255,
//   0.653281482438188263928322,
//   1.281457723870753089398043,
// };

// static const double A[] = {
//   NAN,
//   0.707106781186547524400844,
//   0.541196100146196984399723,
//   0.707106781186547524400844,
//   1.306562964876376527856643,
//   0.382683432365089771728460,
// };

// hard-cocded DCT matrix 8x8
float dct_mtx[trans_len][trans_len] = {{0.3536, 0.3536, 0.3536, 0.3536, 0.3536, 0.3536, 0.3536, 0.3536}, 
  {0.4904, 0.4157, 0.2778, 0.0975, -0.0975, -0.2778, -0.4157, -0.4904},
  {0.4619, 0.1913, -0.1913, -0.4619, -0.4619, -0.1913, 0.1913, 0.4619},
  {0.4157, -0.0975, -0.4904, -0.2778, 0.2778, 0.4904, 0.0975, -0.4157},
  {0.3536, -0.3536, -0.3536, 0.3536, 0.3536, -0.3536, -0.3536, 0.3536},
  {0.2778, -0.4904, 0.0975, 0.4157, -0.4157, -0.0975, 0.4904, -0.2778},
  {0.1913, -0.4619, 0.4619, -0.1913, -0.1913, 0.4619, -0.4619, 0.1913},
  {0.0975, -0.2778, 0.4157, -0.4904, 0.4904, -0.4157, 0.2778, -0.0975}};
// original ecg signal (used for hard-coded method)
float ecg_sig[sig_len][1] = {{0.3500},{0.3500},{0.3500},{0.3500},{0.3500},{0.3500},{0.3500},{0.3500},{0.3500},{0.0200},{-0.2100},{-0.3300},{-0.3700},{-0.3350},{-0.2850},{-0.2300},{-0.2150},{-0.2000},{-0.1900},{-0.2000},{-0.2000},{-0.2100},{-0.2300},{-0.2500},{-0.2550},{-0.2450},{-0.2400},{-0.2150},{-0.2350},{-0.2300},{-0.2300},{-0.2050},{-0.1800},{-0.1650},{-0.1950},{-0.2100},{-0.2150},{-0.1950},{-0.1700},{-0.1550},{-0.1750},{-0.1950},{-0.2100},{-0.2000},{-0.2000},{-0.1950},{-0.1850},{-0.1850},{-0.1800},{-0.1750},{-0.1750},{-0.1650},{-0.1400},{-0.1200},{-0.1150},{-0.1300},{-0.1550},{-0.1900},{-0.1900},{-0.1850},{-0.1800},{-0.1600},{-0.1400},{-0.1300},{-0.1100},{-0.1200},{-0.1300},{-0.1300},{-0.1250},{-0.0950},{-0.0950},{-0.0900},{-0.0800},{-0.0750},{-0.0700},{-0.0750},{-0.0650},{-0.0550},{-0.0600},{-0.0550},{-0.0400},{-0.0300},{-0.0200},{-0.0150},{-0.0150},{-0.0100},{0.0150},{0.0350},{0.0250},{0.0150},{0.0150},{0.0250},{0.0250},{0.0400},{0.0400},{0.0300},{0.0150},{0.0200},{0.0350},{0.0450},{0.0500},{0.0250},{0.0200},{0.0200},{0.0100},{0.0100},{0},{-0.0050},{-0.0250},{-0.0200},{-0.0300},{-0.0300},{-0.0350},{-0.0450},{-0.0550},{-0.0600},{-0.0650},{-0.0750},{-0.0800},{-0.0750},{-0.0750},{-0.0750},{-0.0700},{-0.0750},{-0.0800},{-0.0900},{-0.1000},{-0.0950},{-0.0950},{-0.0850},{-0.0950},{-0.1000},{-0.1000},{-0.1150},{-0.1150},{-0.1300},{-0.1400},{-0.1500},{-0.1600},{-0.1450},{-0.1550},{-0.1600},{-0.1700},{-0.1850},{-0.1750},{-0.1700},{-0.1650},{-0.1700},{-0.1700},{-0.1650},{-0.1800},{-0.1800},{-0.2000},{-0.2000},{-0.2150},{-0.2050},{-0.2100},{-0.2050},{-0.2100},{-0.2200},{-0.2250},{-0.2250},{-0.2400},{-0.2250},{-0.2100},{-0.2050},{-0.1950},{-0.1900},{-0.2000},{-0.1900},{-0.1900},{-0.1850},{-0.1700},{-0.1700},{-0.1750},{-0.1650},{-0.1650},{-0.1850},{-0.1850},{-0.2050},{-0.2050},{-0.2100},{-0.1950},{-0.1950},{-0.1900},{-0.1950},{-0.1950},{-0.1950},{-0.1850},{-0.1750},{-0.1750},{-0.2000},{-0.2250},{-0.2500},{-0.2400},{-0.2400},{-0.2400},{-0.2500},{-0.2600},{-0.2700},{-0.2800},{-0.3050},{-0.3350},{-0.3500},{-0.3400},{-0.2850},{-0.2050},{-0.1350},{-0.0950},{-0.0450},{0.0100},{0.0750},{0.1400},{0.1750},{0.1350},{0.0450},{-0.1550},{-0.4250},{-0.7100},{-0.9150},{-1.0900},{-1.2550},{-1.3950},{-1.4650},{-1.5050},{-1.4900},{-1.4500},{-1.3900},{-1.3400},{-1.2700},{-1.1550},{-1.0250},{-0.9500},{-0.9300},{-0.9300},{-0.9100},{-0.8850},{-0.8650},{-0.8400},{-0.8000},{-0.7550},{-0.7150},{-0.6600},{-0.6300},{-0.5800},{-0.5400},{-0.5100},{-0.4650},{-0.4350},{-0.3950},{-0.3650},{-0.3150},{-0.2650},{-0.2000},{-0.1550},{-0.0900},{-0.0400},{-0.0150},{-0.0100},{0.0050},{0.0300},{0.0550},{0.0800},{0.0850},{0.0900},{0.1100},{0.1100},{0.1300},{0.1500},{0.1550},{0.1500},{0.1800},{0.1850},{0.2050},{0.2200},{0.2350},{0.2350},{0.2550},{0.2700},{0.2900},{0.2900},{0.3000},{0.3000},{0.3050},{0.3250},{0.3150},{0.3100},{0.3150},{0.3150},{0.3450},{0.3600},{0.3700},{0.3750},{0.3750},{0.3650},{0.3750},{0.3850},{0.4100},{0.4050},{0.4000},{0.4000},{0.4050},{0.4000},{0.4100},{0.3950},{0.4000},{0.3850},{0.3900},{0.3950},{0.3850},{0.3800},{0.3550},{0.3550},{0.3550},{0.3500},{0.3250},{0.3000},{0.2800},{0.2500},{0.2500},{0.2500},{0.2500},{0.2350},{0.2100},{0.1750},{0.1650},{0.1600},{0.1450},{0.1400},{0.1450},{0.1300},{0.1000},{0.0800},{0.0400},{0.0150},{-0.0100},{-0.0250},{-0.0250},{-0.0300},{-0.0400},{-0.0450},{-0.0700},{-0.1050},{-0.0800},{-0.0750},{-0.0850},{-0.0900},{-0.1100},{-0.1300},{-0.1200},{-0.1150},{-0.1250},{-0.1500},{-0.1700},{-0.1600},{-0.1600},{-0.1500},{-0.1550},{-0.1600},{-0.1700},{-0.1850},{-0.1800},{-0.1800},{-0.1900},{-0.1800},{-0.1700},{-0.1600},{-0.1400},{-0.1400},{-0.1450},{-0.1700},{-0.1800},{-0.1750},{-0.1600},{-0.1550},{-0.1550},{-0.1650},{-0.1500},{-0.1550},{-0.1500},{-0.1450},{-0.1450},{-0.1650},{-0.1950},{-0.2050},{-0.1900},{-0.1800},{-0.1650},{-0.1700},{-0.1600},{-0.1700},{-0.1650},{-0.1700},{-0.1700},{-0.1700},{-0.1700},{-0.1850},{-0.1900},{-0.1800},{-0.1800},{-0.1800},{-0.1750},{-0.2000},{-0.2100},{-0.2000},{-0.2000},{-0.2100},{-0.2150},{-0.2100},{-0.2000},{-0.1750},{-0.1600},{-0.1450},{-0.1450},{-0.1350},{-0.1400},{-0.1350},{-0.1300},{-0.1200},{-0.1150},{-0.1100},{-0.1050},{-0.1000},{-0.0950},{-0.1100},{-0.1400},{-0.1750},{-0.1800},{-0.1800},{-0.1800},{-0.1650},{-0.1750},{-0.1750},{-0.1750},{-0.1650},{-0.1650},{-0.1650},{-0.1750},{-0.1650},{-0.1600},{-0.1550},{-0.1500},{-0.1550},{-0.1600},{-0.1600},{-0.1550},{-0.1450},{-0.1500},{-0.1500},{-0.1850},{-0.2000},{-0.2000},{-0.1950},{-0.1900},{-0.1900},{-0.1950},{-0.2000},{-0.1950},{-0.1850},{-0.1750},{-0.1750},{-0.1550},{-0.1700},{-0.1700},{-0.1650},{-0.1500},{-0.1600},{-0.1650},{-0.1750},{-0.1700},{-0.1700},{-0.1850},{-0.2150},{-0.2250},{-0.1900},{-0.1250},{-0.0400},{0.0500},{0.1250},{0.2150},{0.3200},{0.4150},{0.5000},{0.6050},{0.7250},{0.8450},{0.9400},{1.0600},{1.1100},{1.1000},{1.0050},{0.8600},{0.6150},{0.2400},{-0.1100},{-0.3350},{-0.4600},{-0.5150},{-0.5400},{-0.5100},{-0.4400},{-0.3850},{-0.3600},{-0.3500},{-0.3350},{-0.3200},{-0.2950},{-0.3000},{-0.3000},{-0.3250},{-0.3350},{-0.3350}};


// ecg_signal (used for fast algorithm)
// float ecg_sig_2[sig_len] = {0.35, 0.35, 0.35, 0.35, 0.35, 0.35, 0.35, 0.35, 0.35, 0.02, -0.21, -0.33, -0.37, -0.335, -0.285, -0.23, -0.215, -0.2, -0.19, -0.2, -0.2, -0.21, -0.23, -0.25, -0.255, -0.245, -0.24, -0.215, -0.235, -0.23, -0.23, -0.205, -0.18, -0.165, -0.195, -0.21, -0.215, -0.195, -0.17, -0.155, -0.175, -0.195, -0.21, -0.2, -0.2, -0.195, -0.185, -0.185, -0.18, -0.175, -0.175, -0.165, -0.14, -0.12, -0.115, -0.13, -0.155, -0.19, -0.19, -0.185, -0.18, -0.16, -0.14, -0.13, -0.11, -0.12, -0.13, -0.13, -0.125, -0.095, -0.095, -0.09, -0.08, -0.075, -0.07, -0.075, -0.065, -0.055, -0.06, -0.055, -0.04, -0.03, -0.02, -0.015, -0.015, -0.01, 0.015, 0.035, 0.025, 0.015, 0.015, 0.025, 0.025, 0.04, 0.04, 0.03, 0.015, 0.02, 0.035, 0.045, 0.05, 0.025, 0.02, 0.02, 0.01, 0.01, 0, -0.005, -0.025, -0.02, -0.03, -0.03, -0.035, -0.045, -0.055, -0.06, -0.065, -0.075, -0.08, -0.075, -0.075, -0.075, -0.07, -0.075, -0.08, -0.09, -0.1, -0.095, -0.095, -0.085, -0.095, -0.1, -0.1, -0.115, -0.115, -0.13, -0.14, -0.15, -0.16, -0.145, -0.155, -0.16, -0.17, -0.185, -0.175, -0.17, -0.165, -0.17, -0.17, -0.165, -0.18, -0.18, -0.2, -0.2, -0.215, -0.205, -0.21, -0.205, -0.21, -0.22, -0.225, -0.225, -0.24, -0.225, -0.21, -0.205, -0.195, -0.19, -0.2, -0.19, -0.19, -0.185, -0.17, -0.17, -0.175, -0.165, -0.165, -0.185, -0.185, -0.205, -0.205, -0.21, -0.195, -0.195, -0.19, -0.195, -0.195, -0.195, -0.185, -0.175, -0.175, -0.2, -0.225, -0.25, -0.24, -0.24, -0.24, -0.25, -0.26, -0.27, -0.28, -0.305, -0.335, -0.35, -0.34, -0.285, -0.205, -0.135, -0.095, -0.045, 0.01, 0.075, 0.14, 0.175, 0.135, 0.045, -0.155, -0.425, -0.71, -0.915, -1.09, -1.255, -1.395, -1.465, -1.505, -1.49, -1.45, -1.39, -1.34, -1.27, -1.155, -1.025, -0.95, -0.93, -0.93, -0.91, -0.885, -0.865, -0.84, -0.8, -0.755, -0.715, -0.66, -0.63, -0.58, -0.54, -0.51, -0.465, -0.435, -0.395, -0.365, -0.315, -0.265, -0.2, -0.155, -0.09, -0.04, -0.015, -0.01, 0.005, 0.03, 0.055, 0.08, 0.085, 0.09, 0.11, 0.11, 0.13, 0.15, 0.155, 0.15, 0.18, 0.185, 0.205, 0.22, 0.235, 0.235, 0.255, 0.27, 0.29, 0.29, 0.3, 0.3, 0.305, 0.325, 0.315, 0.31, 0.315, 0.315, 0.345, 0.36, 0.37, 0.375, 0.375, 0.365, 0.375, 0.385, 0.41, 0.405, 0.4, 0.4, 0.405, 0.4, 0.41, 0.395, 0.4, 0.385, 0.39, 0.395, 0.385, 0.38, 0.355, 0.355, 0.355, 0.35, 0.325, 0.3, 0.28, 0.25, 0.25, 0.25, 0.25, 0.235, 0.21, 0.175, 0.165, 0.16, 0.145, 0.14, 0.145, 0.13, 0.1, 0.08, 0.04, 0.015, -0.01, -0.025, -0.025, -0.03, -0.04, -0.045, -0.07, -0.105, -0.08, -0.075, -0.085, -0.09, -0.11, -0.13, -0.12, -0.115, -0.125, -0.15, -0.17, -0.16, -0.16, -0.15, -0.155, -0.16, -0.17, -0.185, -0.18, -0.18, -0.19, -0.18, -0.17, -0.16, -0.14, -0.14, -0.145, -0.17, -0.18, -0.175, -0.16, -0.155, -0.155, -0.165, -0.15, -0.155, -0.15, -0.145, -0.145, -0.165, -0.195, -0.205, -0.19, -0.18, -0.165, -0.17, -0.16, -0.17, -0.165, -0.17, -0.17, -0.17, -0.17, -0.185, -0.19, -0.18, -0.18, -0.18, -0.175, -0.2, -0.21, -0.2, -0.2, -0.21, -0.215, -0.21, -0.2, -0.175, -0.16, -0.145, -0.145, -0.135, -0.14, -0.135, -0.13, -0.12, -0.115, -0.11, -0.105, -0.1, -0.095, -0.11, -0.14, -0.175, -0.18, -0.18, -0.18, -0.165, -0.175, -0.175, -0.175, -0.165, -0.165, -0.165, -0.175, -0.165, -0.16, -0.155, -0.15, -0.155, -0.16, -0.16, -0.155, -0.145, -0.15, -0.15, -0.185, -0.2, -0.2, -0.195, -0.19, -0.19, -0.195, -0.2, -0.195, -0.185, -0.175, -0.175, -0.155, -0.17, -0.17, -0.165, -0.15, -0.16, -0.165, -0.175, -0.17, -0.17, -0.185, -0.215, -0.225, -0.19, -0.125, -0.04, 0.05, 0.125, 0.215, 0.32, 0.415, 0.5, 0.605, 0.725, 0.845, 0.94, 1.06, 1.11, 1.1, 1.005, 0.86, 0.615, 0.24, -0.11, -0.335, -0.46, -0.515, -0.54, -0.51, -0.44, -0.385, -0.36, -0.35, -0.335, -0.32, -0.295, -0.3, -0.3, -0.325, -0.335, -0.335};

// use algorithm to generate dct matrix
// void FastDct8_transform(float vector[static 8]) {
//   const float v0 = vector[0] + vector[7];
//   const float v1 = vector[1] + vector[6];
//   const float v2 = vector[2] + vector[5];
//   const float v3 = vector[3] + vector[4];
//   const float v4 = vector[3] - vector[4];
//   const float v5 = vector[2] - vector[5];
//   const float v6 = vector[1] - vector[6];
//   const float v7 = vector[0] - vector[7];
  
//   const float v8 = v0 + v3;
//   const float v9 = v1 + v2;
//   const float v10 = v1 - v2;
//   const float v11 = v0 - v3;
//   const float v12 = -v4 - v5;
//   const float v13 = (v5 + v6) * A[3];
//   const float v14 = v6 + v7;
  
//   const float v15 = v8 + v9;
//   const float v16 = v8 - v9;
//   const float v17 = (v10 + v11) * A[1];
//   const float v18 = (v12 + v14) * A[5];
  
//   const float v19 = -v12 * A[2] - v18;
//   const float v20 = v14 * A[4] - v18;
  
//   const float v21 = v17 + v11;
//   const float v22 = v11 - v17;
//   const float v23 = v13 + v7;
//   const float v24 = v7 - v13;
  
//   const float v25 = v19 + v24;
//   const float v26 = v23 + v20;
//   const float v27 = v23 - v20;
//   const float v28 = v24 - v19;
  
//   vector[0] = S[0] * v15;
//   vector[1] = S[1] * v26;
//   vector[2] = S[2] * v21;
//   vector[3] = S[3] * v28;
//   vector[4] = S[4] * v16;
//   vector[5] = S[5] * v25;
//   vector[6] = S[6] * v22;
//   vector[7] = S[7] * v27;
// }
// multiply two matrix
void mul_mtx(float first[][trans_len], float second[][1], float mult[][1], int r1, int c1, int r2, int c2) {
  // initialize result matrix to zero
  int i,j,k;
  for (i = 0; i < r1; ++i) {
      for (j = 0; j < c2; ++j) {
          mult[i][j] = 0;
      }
  }
    // multiply first matrix with second matrix
  for (i = 0; i < r1; ++i) {
      for (j = 0; j < c2; ++j) {
          for (k = 0; k < c1; ++k) {
              mult[i][j] += first[i][k] * second[k][j];   
          }
      }
  }
}

// print float in contiki
void PrintFloat(float value)
{
    int tmp,tmp2;
    char flag = ' ';
    tmp = (int)value;
    tmp2 = (int)((value-tmp)*10000);
    if(tmp < 0 || tmp2 < 0){
      flag = '-';
    }
    printf("value=%c%d.%04d\n",flag,abs(tmp),abs(tmp2));
}

/*---------------------------------------------------------------------------*/
PROCESS(data_compression_process, "data_compression_process");
AUTOSTART_PROCESSES(&data_compression_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(data_compression_process, ev, data)
{
  
  PROCESS_BEGIN();
  unsigned long start = clock_time();
  int i,j;
  // convert the data using matrix and print them

  for(i = 0; i < sig_len/trans_len; i++){
    // H[8x8] * temp[8x1]
    // use fast dct algorithm
    // FastDct8_transform(ecg_sig_2+(i*trans_len));
    // for(j = 0; j < 8; j++){
    //   PrintFloat((ecg_sig_2+(i*trans_len))[j]);
    // }
    // use hard-coded matrix
    float result[trans_len][1];
    mul_mtx(dct_mtx, ecg_sig+i*trans_len, result, trans_len, trans_len, trans_len, 1);
    for(j = 0; j < trans_len; j++){
      PrintFloat(result[j][0]);
    }
  }

  unsigned long end = clock_time();
  printf("time elapsed: %lu milliseconds\n", end - start);
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/