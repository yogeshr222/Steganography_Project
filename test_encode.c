#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"

int main(int argc , char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decodeInfo;
    //uint img_size;

    // Fill with sample filenames
    //encInfo.src_image_fname = "beautiful.bmp";
    // encInfo.secret_fname = "secret.txt";
    //encInfo.stego_image_fname = "stego_img.bmp";

    //check number of arguments
    if(argc < 3) {
        printf("Invalid Input!\n");
        printf("For Encoding : \n./a.out -e beautiful.bmp secret.txt [stego.bmp]\n");
        printf("For Decoding : \n./a.out -d stego.bmp\n");
        return -1;
    }
    //function call for operaton type 
    int res = check_operation_type(argv);
    if(res == e_encode){
        printf("Encoding Selected\n");

        //function call for read and validate the arguments
        if (read_and_validate_encode_args(argv, &encInfo) == e_success){
            printf("INFO : Read and Validation of source,secret and output file is Successful\n");
            
            //function call for do_encoding
            if (do_encoding(&encInfo) == e_success) {

                //function call for check capacity
                if(check_capacity(&encInfo) == e_success) {
                    printf("INFO : Secret data can fit in the given image\n");
                    printf("## ENCODING STARTS FROM HERE ##\n");

                    //function call
                    if (copy_bmp_header (encInfo.fptr_src_image, encInfo.fptr_stego_image) == e_success){
                        printf("INFO : Copied bmp header successfully\n");

                        //function call for encode magic string
                        if( encode_magic_string(MAGIC_STRING, &encInfo) == e_success){
                            printf("INFO : Encoded magic string successfully\n");

                            //function call for encode secret file extention size
                            strcpy (encInfo.extn_secret_file , strstr(encInfo.secret_fname , "."));
                            printf("%s\n",encInfo.extn_secret_file);

                            int len = strlen(encInfo.extn_secret_file);
                            if (encode_secret_file_extn_size (len, &encInfo) == e_success) {
                                printf("INFO : Encoded secret file extention size successsfully\n");

                                //function call for encoding secret file extention
                                if (encode_secret_file_extn (encInfo.extn_secret_file, &encInfo) == e_success) {
                                    printf("INFO : Encoded secret file extention successfully\n");

                                    //function call for encoding secrete file data size
                                    if (encode_secret_file_size (encInfo.size_secret_file, &encInfo) == e_success) {
                                        printf("INFO : Encoded secret file data size successfully\n");

                                        //function call for encode the secret file data to output.bmp file
                                        if (encode_secret_file_data (&encInfo) == e_success) {
                                            printf("INFO : Encoded secret file data successfully\n");
                                            
                                            //function call for copy remaining bytes from source image file (beautiful.bmp)
                                            if (copy_remaining_img_data (&encInfo) == e_success) {
                                                printf("INFO : Encoded all remaining bytes from souce file to stego.bmp file successfully\n");
                                            
                                            } else {
                                                printf("ERROR : Failed to copy remaining bytes from souce file to stego.bmp file\n");
                                                return e_failure;
                                            }
                                            
                                        } else {
                                            printf("ERROR : Failed to encode secret file data\n");
                                            return e_failure;
                                        }

                                    } else {
                                        printf("ERROR : Failed to encode secret file data size\n");
                                        return e_failure;
                                    }

                                } else {
                                    printf("ERROR : Failed to encode secret file extention\n");
                                    return e_failure;
                                }
                            
                            } else {
                                printf("ERROR : Failed to encode secret file extention size\n");
                                return e_failure;
                            }
                            
                        } else {
                            printf("ERROR : Failed to Encode magic string\n");
                            return e_failure;
                        }
                        
                    } else {
                        printf("ERROR : Failed to Copy bmp header\n");
                        return e_failure;
                    }
                    

                } else {
                    printf("ERROR : Failure, can't fit secret data in given image file\n");
                    return e_failure;
                }
                printf("SUCCESS : Encoding is success\n");
            } else {
                printf("ERROR : Failed to encode\n");
                return e_failure;
            }
        } else {
            printf("ERROR : Read and Validation of source,secret and output file is Failure\n");
            return e_failure;
        }

    } else if(res == e_decode) {
        printf("Decoding Selected\n");

        //function call for read and validating args from argv
        if (read_and_validate_decode_args (argv, &decodeInfo) == d_success) {
            printf("INFO : Read and validation of stego.bmp and secret_data file successfully\n");

            //function call for do_decoding
            if (do_decoding (&decodeInfo) == d_success ) {
                printf("## DECODING STARTS FROM HERE ##\n");

                //function call for decode magic string
                char magic_buf[3];                 /* MAGIC_STRING "#*" length 2 + 1 for '\0' */
                if (decode_magic_string(magic_buf, strlen(MAGIC_STRING), &decodeInfo) == d_success) {

                    //verify magic string
                    printf("Magic string is : %s\n",magic_buf);
                    if (strcmp (magic_buf, MAGIC_STRING)== 0)
                        printf("INFO : Decoded magic string successfully\n");
                    else {
                        printf("Original magic is %s, not %s\n",MAGIC_STRING, magic_buf);
                        return d_failure;
                    }
                    
                    //function call for decode secret_data file extention size
                    int extn_size;
                    if (decode_secret_file_extn_size(&extn_size, &decodeInfo) == d_success) {
                        if (extn_size == 4) {
                            printf("Extention size = %d\n",extn_size);
                            printf("INFO : Decoded secret_file_extention size successfully\n");
                            
                            //function call to decode secret_file extention (char)
                            if (decode_secret_file_extn(decodeInfo.extn_secret_file, &decodeInfo) == d_success) {
                                printf("INFO : Decoded secret_file_extention successfully\n");

                            } else {
                                printf("ERROR : Failed to decode secrete_file extention\n");
                                return d_failure;
                            }
                            
                        } else {
                            printf("ERROR : Failed to decode secrete file_extention size\n");
                            printf("Extention size = %d. This should be 4\n",extn_size);
                            return d_failure;
                        }

                    } else {
                        printf("ERROR : Failed to decode secrete file_extention size\n");
                        return d_failure;
                    }
                    
                } else {
                    printf("ERROR : Failed to decode magic string\n");
                    return d_failure;
                }

                printf("SUCCESS : Decoding is success\n");
            } else {
                printf("ERROR : Failed to decode\n");
                return d_failure;
            }

        } else {
            printf("ERROR : Failed to Read and validation of stego.bmp and secret_data file\n");
            return d_failure;
        }

    } else {
        printf("Invalid Option\nEnter -e for Encoding\nEnter -d for Decoding\n");
        return 0;
    }

    /*
    // Test get_image_size_for_bmp
    img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
    printf("INFO: Image size = %u\n", img_size);
    */
    return 0;
}

OperationType check_operation_type(char *argv[]){

    if(strcmp (argv[1], "-e") == 0){
        return e_encode;
    }
    else if(strcmp(argv[1] , "-d") == 0) {
        return e_decode;
    }
    else{
        return e_unsupported;
    }
}