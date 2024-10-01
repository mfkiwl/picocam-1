#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../pico_psram.h"
#include "test_code.h"

#define PSRAM_LOCATION _u(0x11000000)
#define DATA_ELEMENTS 256
#define DATA_BLOCK_SIZE (DATA_ELEMENTS * sizeof(float_t))

static void erase_data_block(float_t *data_buffer)
{
    for (size_t i = 0; i < DATA_ELEMENTS; i++)
        data_buffer[i] = 0.0f;
}

static void write_data_block(float_t *source_data, float_t *data_buffer)
{
    for (size_t i = 0; i < DATA_ELEMENTS; i++)
        data_buffer[i] = source_data[i];
}

void ge_test(int num)
{
    int32_t A[G_NUM][G_NUM] = {{5, 1, 1}, {1, 3, 1}, {1, 1, 4}};
    int32_t b[G_NUM] = {7.0, 10.0, 15.0};
    int32_t x[G_NUM];

    int32_t nowtime = time_us_32();
    for (int n = 0; n < num; n++)
        pico_ge(A, b, x);
    nowtime = time_us_32() - nowtime;
    printf("elapsed time = %d[usec]\r\n", nowtime);

    for (int i = 0; i < G_NUM; i++)
    {
        printf("x[%d] = %d\n", i, x[i]);
    }

    return;
}

void gs_test(int num)
{
    int32_t a[G_NUM][G_NUM] = {{5, 1, 1}, {1, 3, 1}, {1, 1, 4}}; // 行列Aを定義
    int32_t b[G_NUM] = {7, 10, 15};                              // ベクトルbを定義
    int32_t x[G_NUM];                                            // 解ベクトルxを定義
    int32_t nowtime = time_us_32();
    for (int n = 0; n < num; n++)
        pico_gs(a, b, x);

    nowtime = time_us_32() - nowtime;
    printf("elapsed time = %d[usec]\r\n", nowtime);

    for (int i = 0; i < G_NUM; i++)
    {
        printf("x[%d] = %d\n", i, x[i]); // 解を表示
    }
    return;
}

void dct_test(int num)
{
    // num : number of calc dct
    uint8_t img[][16] = {
        {139, 144, 149, 153, 155, 155, 155, 155, 144, 151, 153, 156, 159, 156, 156, 156},
        {150, 155, 160, 163, 158, 156, 156, 156, 159, 161, 162, 160, 160, 159, 159, 159},
        {159, 160, 161, 162, 162, 155, 155, 155, 161, 161, 161, 161, 160, 157, 157, 157},
        {162, 162, 161, 163, 162, 157, 157, 157, 162, 162, 161, 161, 163, 158, 158, 158},
        {139, 144, 149, 153, 155, 155, 155, 155, 144, 151, 153, 156, 159, 156, 156, 156},
        {150, 155, 160, 163, 158, 156, 156, 156, 159, 161, 162, 160, 160, 159, 159, 159},
        {159, 160, 161, 162, 162, 155, 155, 155, 161, 161, 161, 161, 160, 157, 157, 157},
        {162, 162, 161, 163, 162, 157, 157, 157, 162, 162, 161, 161, 163, 158, 158, 158},
        {139, 144, 149, 153, 155, 155, 155, 155, 144, 151, 153, 156, 159, 156, 156, 156},
        {150, 155, 160, 163, 158, 156, 156, 156, 159, 161, 162, 160, 160, 159, 159, 159},
        {159, 160, 161, 162, 162, 155, 155, 155, 161, 161, 161, 161, 160, 157, 157, 157},
        {162, 162, 161, 163, 162, 157, 157, 157, 162, 162, 161, 161, 163, 158, 158, 158},
        {139, 144, 149, 153, 155, 155, 155, 155, 144, 151, 153, 156, 159, 156, 156, 156},
        {150, 155, 160, 163, 158, 156, 156, 156, 159, 161, 162, 160, 160, 159, 159, 159},
        {159, 160, 161, 162, 162, 155, 155, 155, 161, 161, 161, 161, 160, 157, 157, 157},
        {162, 162, 161, 163, 162, 157, 157, 157, 162, 162, 161, 161, 163, 158, 158, 158}};

    int32_t img_w = 16;
    int32_t nowtime;
    int32_t img_h = sizeof(img) / sizeof(uint8_t) / img_w;
    uint8_t *np = (uint8_t *)img;    // 1次元配列に変換
    float_t coeff[img_h * img_w];    // 1次元配列で宣言
    int32_t img_idct[img_h * img_w]; // 1次元配列で宣言
    // DCTテーブルの作成
    make_dct_table();

    for (int n = 0; n < num; n++)
    {
        nowtime = time_us_32();
        pico_dct8(np, coeff, img_h, img_w);

        nowtime = time_us_32() - nowtime;
        printf("DCT:elapsed time = %d[us]\r\n", nowtime);

        // 結果を出力
        for (int32_t h = 0; h < img_h; h++)
        {
            for (int32_t w = 0; w < img_w; w++)
            {
                printf("%8.2f", coeff[h * img_w + w]);
            }
            printf("\n");
        }
        printf("------\n");

        nowtime = time_us_32();
        pico_idct8(coeff, img_idct, img_h, img_w);
        nowtime = time_us_32() - nowtime;
        printf("IDCT:elapsed time = %d[us]\r\n", nowtime);
        // 結果を出力
        for (int32_t h = 0; h < img_h; h++)
        {
            for (int32_t w = 0; w < img_w; w++)
            {
                printf("%6d", img_idct[h * img_w + w]);
            }
            printf("\n");
        }
        printf("------\n");
    }
}

void fft_test(void)
{

    int sz = pico_setup_psram(47);
    if (sz > 0)
    {
        printf("PSRAM OK: size = %d\n", sz);
    }
    else
    {
        printf("No PSRAM ?\n");
    }

    // int32_t ar2[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1, 1, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0};
    int32_t ar2[256] = {
        139, 144, 149, 153, 155, 155, 155, 155, 144, 151, 153, 156, 159, 156, 156, 156,
        150, 155, 160, 163, 158, 156, 156, 156, 159, 161, 162, 160, 160, 159, 200, 159,
        159, 160, 161, 162, 162, 155, 155, 155, 161, 161, 161, 161, 160, 157, 157, 157,
        162, 162, 161, 163, 162, 157, 157, 157, 162, 162, 161, 161, 163, 158, 158, 158,
        139, 144, 149, 153, 155, 155, 155, 155, 144, 151, 153, 156, 159, 156, 156, 156,
        150, 155, 160, 163, 158, 156, 156, 156, 159, 161, 162, 160, 160, 159, 159, 159,
        159, 160, 161, 162, 162, 155, 155, 155, 161, 161, 161, 161, 160, 157, 157, 157,
        162, 162, 161, 163, 162, 157, 157, 157, 162, 162, 161, 161, 163, 158, 158, 158,
        139, 144, 149, 153, 155, 155, 155, 155, 144, 151, 153, 156, 159, 156, 156, 156,
        150, 155, 160, 163, 158, 156, 156, 156, 159, 161, 162, 160, 160, 159, 159, 159,
        159, 160, 161, 162, 162, 155, 155, 155, 161, 161, 161, 161, 160, 157, 157, 157,
        162, 162, 161, 163, 162, 157, 157, 157, 162, 162, 161, 161, 163, 158, 158, 158,
        139, 144, 149, 153, 155, 155, 155, 155, 144, 151, 153, 156, 159, 156, 156, 156,
        150, 155, 160, 163, 158, 156, 156, 156, 159, 161, 162, 160, 160, 159, 159, 159,
        159, 160, 161, 162, 162, 155, 155, 155, 161, 161, 161, 161, 160, 157, 157, 157,
        162, 162, 161, 163, 162, 157, 157, 157, 162, 162, 161, 161, 163, 158, 158, 158};

    int32_t ai2[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int32_t n, nmax;
    int32_t wr2[16];
    int32_t wi2[16];

    n = nmax = 16;
    float_t ar3[256] = {
        139, 144, 149, 153, 155, 155, 155, 155, 144, 151, 153, 156, 159, 156, 156, 156,
        150, 155, 160, 163, 158, 156, 156, 156, 159, 161, 162, 160, 160, 159, 200, 159,
        159, 160, 161, 162, 162, 155, 155, 155, 161, 161, 161, 161, 160, 157, 157, 157,
        162, 162, 161, 163, 162, 157, 157, 157, 162, 162, 161, 161, 163, 158, 158, 158,
        139, 144, 149, 153, 155, 155, 155, 155, 144, 151, 153, 156, 159, 156, 156, 156,
        150, 155, 160, 163, 158, 156, 156, 156, 159, 161, 162, 160, 160, 159, 159, 159,
        159, 160, 161, 162, 162, 155, 155, 155, 161, 161, 161, 161, 160, 157, 157, 157,
        162, 162, 161, 163, 162, 157, 157, 157, 162, 162, 161, 161, 163, 158, 158, 158,
        139, 144, 149, 153, 155, 155, 155, 155, 144, 151, 153, 156, 159, 156, 156, 156,
        150, 155, 160, 163, 158, 156, 156, 156, 159, 161, 162, 160, 160, 159, 159, 159,
        159, 160, 161, 162, 162, 155, 155, 155, 161, 161, 161, 161, 160, 157, 157, 157,
        162, 162, 161, 163, 162, 157, 157, 157, 162, 162, 161, 161, 163, 158, 158, 158,
        139, 144, 149, 153, 155, 155, 155, 155, 144, 151, 153, 156, 159, 156, 156, 156,
        150, 155, 160, 163, 158, 156, 156, 156, 159, 161, 162, 160, 160, 159, 159, 159,
        159, 160, 161, 162, 162, 155, 155, 155, 161, 161, 161, 161, 160, 157, 157, 157,
        162, 162, 161, 163, 162, 157, 157, 157, 162, 162, 161, 161, 163, 158, 158, 158};
    float_t ai3[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    float_t *data_buffer = (float_t *)(PSRAM_LOCATION);
    erase_data_block(data_buffer);
    write_data_block(ar3, data_buffer);
    float_t *arf;
    arf = data_buffer;

    data_buffer += 256;
    erase_data_block(data_buffer);
    write_data_block(ai3, data_buffer);
    float_t *aif;
    aif = data_buffer;

    printf("float FFT2\r\n");
    int32_t nowtime = time_us_32();
    for (int ll = 0; ll < 256; ll++)
    {
        pico_fft2(n, nmax, arf, aif);
        pico_ifft2(n, nmax, arf, aif);
    }
    nowtime = time_us_32() - nowtime;
    printf("elapsed time = %d[usec]\r\n", nowtime);
    for (int j = 0; j < n * nmax; j += n)
    {
        for (int i = 0; i < n; i++)
        {
            printf("%.1f,  ", arf[(i + j)]);
        }
        printf("\n");
    }

    n = nmax = 16;
    printf("Integer FFT2\r\n");
    nowtime = time_us_32();
    for (int i = 0; i < 256; i++)
    {
        pico_int_fft2(n, nmax, ar2, ai2, wr2, wi2);
        pico_int_ifft2(n, nmax, ar2, ai2, wr2, wi2);
    }
    nowtime = time_us_32() - nowtime;
    printf("elapsed time = %d[usec]\r\n", nowtime);
    // // real
    for (int j = 0; j < n * nmax; j += n)
    {
        for (int i = 0; i < n; i++)
        {
            printf("%d,  ", ar2[(i + j)]);
        }
        printf("\n");
    }
    // for(int i=0; i < (n * nmax); i+=4){
    //     printf("ar2[%d]=%d\t ar2[%d]=%d\t ar2[%d]=%d\t ar2[%d]=%d\r\n",i,ar2[i],i+1,ar2[i+1],i+2,ar2[i+2],i+3,ar2[i+3]);
    // }

    // // imag
    // for(int i=0; i < (n * nmax); i+=4){
    //     printf("ai2[%d]=%d\t ai2[%d]=%d\t ai2[%d]=%d\t ai2[%d]=%d\r\n",i,ai2[i],i+1,ai2[i+1],i+2,ai2[i+2],i+3,ai2[i+3]);
    // }
}
