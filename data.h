#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum // Result type
{
    e_failure,
    e_success
} Result;

typedef enum // Mode type
{
    Help,
    View,
    Edit,
    Close,
    Invalid
} Mode;

#endif