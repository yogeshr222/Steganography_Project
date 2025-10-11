#ifndef DECODE_H
#define DECODE_H

#include "types.h" //contains user defined types
#include<stdio.h>

/*
 * Structure to store infomation required for 
 * Decode secret file from stego_image 
 * Information about output and intermediate data is 
 * also stored
 */

 #define MAX_SECRET_BUF_SIZE 1
 #define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
 #define MAX_FILE_SUFFIX 4

 typedef struct _DecodeInfo
 {
    /* Source Image info */
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char extn_stego_image_file[MAX_FILE_SUFFIX];
    char stego_data[MAX_IMAGE_BUF_SIZE];

 } DecodeInfo;

//Decoding fuction prototype

//Read and validate decode args from argv
d_Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

//Perform the decoding
d_Status do_decoding(DecodeInfo *decInfo);

//get file pointer for input and output files
d_Status open_files_decode (DecodeInfo *decInfo);

//decode Magic string
d_Status decode_magic_string(char *magic_string, int magic_len, DecodeInfo *decInfo);

//decode secret file extn size
d_Status decode_secret_file_extn_size(int *size, DecodeInfo *decInfo);

//decode secrete file extn
d_Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo);

//decode secret file data size
d_Status decode_secret_file_size(long file_size, DecodeInfo *decInfo);

//decode secret file data
d_Status decode_secret_file_data(DecodeInfo *decInfo);

//decode data from image
d_Status decode_data_from_image(char *data, int size , DecodeInfo *decInfo);

//decode bit from lsb
char decode_bit_from_lsb(char *secret_buffer);

//decode size from image
d_Status decode_size_from_image(int *size, DecodeInfo *decInfo);




 #endif