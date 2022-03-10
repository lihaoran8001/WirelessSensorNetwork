A DCT-II compressioin encoding method implemented on TelosB Mote, and also can be executed in Cooja.

In data_compression.c, I implement two modes of compression. 

First is original one. I use hard-coded DCT matrix (8x8) and leverage matrix multiplication to achieve encoding. To RUN in this mode, we need to UNCOMMENT variable `dct_mtx` and `ecg_sig`. Function `mul_mtx` and `PrintFloat` are also called to process data and doing display. UNCOMMENT line145-149 to execute in  this mode. 
```c
// use hard-coded matrix
    float result[trans_len][1];
    mul_mtx(dct_mtx, ecg_sig+i*trans_len, result, trans_len, trans_len, trans_len, 1);
    for(j = 0; j < trans_len; j++){
      PrintFloat(result[j][0]);
    }
```
Other functions and static variables can be commented to improve performance and avoid ROM overflow.

Second one is fast DCT transform. In this mode, we don't need `dct_mtx` and `ecg_sig`. Instead, we use `ecg_sig_2` and  `FastDct8_transform` function, which needs static variable `S` and `A`.  To run in this mode, we need to UNCOMMENT line 140-143.

```c
// use fast dct algorithm     
     FastDct8_transform(ecg_sig_2+(i*trans_len));
     for(j = 0; j < 8; j++){
       PrintFloat((ecg_sig_2+(i*trans_len))[j]);
     }
```

We can also adjust the coefficient length to achieve higher/lower compression loss.

This program will print all the compression data and also count execution time.

On average, fast DCT algorithm is twice faster than matrix method. 
