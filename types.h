#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in encode fn. return type */
typedef enum
{
    e_success,
    e_failure
} Status;

typedef enum
{
    e_encode,
    e_decode,
    e_unsupported
} OperationType;

/* d_Status will be used in decode fn. return type */
typedef enum
{
    d_success,
    d_failure
} d_Status;

#endif
