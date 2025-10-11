#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Decode function definitions */

d_Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo) {

    //for stego.bmp
    if (strstr  (argv[2] , ".bmp") == NULL) {
        printf("INFO : Stego image file should be of .bmp extention\n");
        return d_failure;
    }

    if (strcmp (strstr (argv[2], ".bmp"), ".bmp") == 0) {
        decInfo->stego_image_fname = argv[2];

    } else {
        return d_failure;
    }

    //for secret_data.txt
    if (argv[3] != NULL) {
        if (strstr(argv[3], ".txt") == NULL) {
            printf("ERROR : Secret_data file should of .txt extension!\n");
            return d_failure;
        }
        if (strcmp (strstr (argv[3] , ".txt"), ".txt") == 0){
            decInfo->secret_fname = argv[3];

        } else {
            return e_failure;
        }
    } else {
        decInfo->secret_fname = "default_data.txt";
        printf("INFO : Secret_data file is not mentioned. Creating default_data.txt file as default\n");
    }

    return d_success;
}

d_Status do_decoding(DecodeInfo *decInfo) {

     //function calling for open all required files
    if (open_files_decode(decInfo) == d_success){
        printf("INFO : %s function completed\n", "open_files for decode" );

    } else {
        printf("ERROR : %s function failed\n", "open_files for decode" );
        return d_failure;
    }
    return d_success;
}

d_Status open_files_decode(DecodeInfo *decInfo) {

    printf("INFO : Opening required files\n");
    //Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    //Do error handling
    if (decInfo->fptr_stego_image == NULL) {

        perror("fopen");
        fprintf(stderr, "ERROR : Unable to open file %s\n", decInfo->stego_image_fname);

        return d_failure;
    } else {
        //printf("INFO : Opened %s file successfully\n",decInfo->stego_image_fname);
    }

    //Secret_data.txt
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    //Do error handling
    if (decInfo->fptr_secret == NULL) {

        perror("fopen");
        fprintf(stderr, "ERROR : Unable to open file %s\n", decInfo->secret_fname);

        return d_failure;
    } else {
        //printf("INFO : Opened %s file successfully\n",decInfo->secret_fname);
    }


    return d_success;
}

d_Status decode_magic_string (char *magic_string, int magic_len, DecodeInfo *decInfo) {

    //function calling for decode magicstring data from image
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    if (decode_data_from_image (magic_string, magic_len, decInfo) == d_success)

    printf("DEBUG: Current file position = %ld\n", ftell(decInfo->fptr_stego_image));

    return d_success;
}

d_Status decode_data_from_image(char *data, int size , DecodeInfo *decInfo) {

    char image_buffer[8];

    for(int i = 0; i < size; i++) {
        //Read size*8 bytes from RGB data (bcz to decode 1 char = need 8 byte need to be read)
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);

        //call decode bit from lsb function
        data[i] = decode_bit_from_lsb (image_buffer);

        printf("Decoded char[%d] = %c (ASCII %d)\n", i, data[i], data[i]);


    }
    data[size] = '\0';
    return d_success;
}

char decode_bit_from_lsb(char *secret_buffer) {

    char ch = 0;
    for(int i = 0; i < 8; i++) {

        ch = ch | ((secret_buffer[i] & 0x01) << i) ;
    }
    return ch;
}

d_Status decode_secret_file_extn_size(int *size, DecodeInfo *decInfo) {

    fseek(decInfo->fptr_stego_image, 70, SEEK_SET);
    //function call for decode_size from image
    decode_size_from_image(size,decInfo);

    printf("DEBUG: Current file position = %ld\n", ftell(decInfo->fptr_stego_image));

    return d_success;

}

d_Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo) {

    decode_data_from_image(file_extn, strlen(file_extn), decInfo);
    printf("Secret_file_extention is : %s\n",file_extn);
    
    printf("DEBUG: Current file position = %ld\n", ftell(decInfo->fptr_stego_image));

    return d_success;
}
    
d_Status decode_size_from_image(int *size, DecodeInfo *decInfo) {

    char image_buffer[32];  // To store 32 bytes from image
    int i;
    unsigned int size_from_img = 0;

    // Read 32 bytes from stego image
    if (fread(image_buffer, 1, 32, decInfo->fptr_stego_image) != 32)
    {
        fprintf(stderr, "ERROR: Unable to read 32 bytes from size_from_img.\n");
        return d_failure;
    }

    // Extract 1 LSB bit from each of the 32 bytes
    for (i = 0; i < 32; i++)
    {
        // Extract LSB and shift it into correct position
        size_from_img |= ((image_buffer[i] & 1) << i);
    }

    // Store the result in *size
    *size = size_from_img;

    return d_success;

}