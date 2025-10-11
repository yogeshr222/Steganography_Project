#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    printf("INFO : Opening required files\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    } else {
        //printf("INFO : Opened beautiful.bmp file in read mode\n");
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    } else {
        //printf("INFO : Opened Secret.txt file in read mode\n");
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    } else {
        //printf("INFO : Opened stegno_img.bmp file in write mode\n");
    }

    // No failure return e_success
    return e_success;
}


//function call for read and validate the encoding arguments
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //for beautiful.bmp file
    if (strstr(argv[2] , ".bmp") == NULL){
        printf("Source image file should be of .bmp extension!\n");
        return e_failure;
    }
    if (strcmp (strstr (argv[2] , ".bmp"), ".bmp") == 0){
        encInfo->src_image_fname = argv[2];

    } else {
        return e_failure;
    }

    //for secret.txt file
    if(strstr(argv[3] , ".txt") == NULL){
        printf("ERROR : Secret file should of .txt extension!\n");
        return e_failure;
    }
    if(strcmp (strstr(argv[3] , ".txt"), ".txt") == 0){
        encInfo->secret_fname = argv[3];

    } else {
        return e_failure;
    }

    //for output image
    if(argv[4] != NULL){
        if (strstr(argv[4] , ".bmp") == NULL){
        printf("ERROR : Stego image file should be of .bmp extension!\n");
        return e_failure;
        }
        if (strcmp (strstr (argv[4] , ".bmp"), ".bmp") == 0){
            encInfo->stego_image_fname = argv[4];

        } else {
            return e_failure;
        }
    } else {
        encInfo->stego_image_fname = "stegno_img.bmp";
        printf("INFO : Output file is not mentioned. Creating stegno_img.bmp as default\n");
    }

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo){
    //function calling for open all required files
    if (open_files(encInfo) == e_success){
        printf("SUCCESS : %s function completed\n", "open_files" );

    } else {
        printf("ERROR : %s function failed\n", "open_files" );
        return e_failure;
    }
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo) {

    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    //printf("beautiful.bmp file size = %u\n", encInfo->image_capacity);

    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    if(encInfo->image_capacity > (16 + 32 + 32 + 32 + encInfo->size_secret_file * 8)) {
        return e_success;
    }
    else {
        return e_failure;
    }
}

uint get_file_size(FILE *fptr){

    fseek(fptr, 0,SEEK_END);
    //printf("Secret file size  = %ld\n", ftell(fptr));
    return ftell(fptr);
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image) {

    char arr[54];
    rewind(fptr_src_image);
    fread(arr, 54, 1 , fptr_src_image);
    fwrite(arr,54, 1, fptr_dest_image);
    return e_success;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo) {

    //function calling for encode magic string data to image
    encode_data_to_image (magic_string, strlen(magic_string), encInfo);
    return e_success;
}

Status encode_secret_file_extn (char *file_extn, EncodeInfo *encInfo) {

    //function calling for encode secret_file_extention data to image
    encode_data_to_image (file_extn, strlen(file_extn), encInfo);
    return e_success;
}

//This function is for encoding character to image
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo) {

    for(int i = 0; i < size; i++){
        //Read 8 bytes from RGB data
        fread(encInfo->image_data, 8, 1, encInfo->fptr_src_image);

        //call encode byte to lsb function
        encode_byte_to_lsb (data[0], encInfo->image_data);

        //write encodded data files to stego.bmp
        fwrite(encInfo->image_data, 8 , 1, encInfo->fptr_stego_image);
    }

}

Status encode_byte_to_lsb(char data, char *image_buffer){

    //Important
    for(int i = 0; i < 8; i++) {

        // clear lsb(image_buffer[i] & 0xFE)) -> get lsb(((data >> i) & 0x01)) -> set lsb(OR)
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> i) & 0x01);
    }
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo) {

    //function calling for encode size;
    int res = encode_size_to_lsb (size, encInfo);
    return res;
}

//This function is for encoding integer
Status encode_size_to_lsb(int size, EncodeInfo *encInfo) {

    char str[32];
    //read 32 bytes from beautiful.bmp
    fread( str, 32, 1, encInfo->fptr_src_image);
    //encode 32 bits into image
    for( int i = 0; i < 32; i++){
        str[i] = (str[i] & 0xFE) | ((size >> i) & 0x01);
    }
    //write to stego.bmp
    fwrite(str, 32, 1, encInfo->fptr_stego_image);

    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo) {

    //function call for encode size to lsb function
    if (encode_size_to_lsb (file_size, encInfo) == e_success){
        return e_success;
    } else {
        return e_failure;
    }
}

Status encode_secret_file_data (EncodeInfo *encInfo){

    char data[encInfo->size_secret_file];
    rewind(encInfo->fptr_secret);
    fgets(data, encInfo->size_secret_file, encInfo->fptr_secret);
    //printf("Secret file data : ");
    //puts(data);
    //function call for encode all characters in secret file
    encode_data_to_image (data, encInfo->size_secret_file, encInfo);

    return e_success;
}

Status copy_remaining_img_data(EncodeInfo *encInfo) {

    int size = encInfo->image_capacity + 54 - ftell(encInfo->fptr_src_image);
    char remaing_data[size];
    fread (remaing_data, size, 1, encInfo->fptr_src_image);
    fwrite(remaing_data, size, 1, encInfo->fptr_stego_image);
    return e_success;
}