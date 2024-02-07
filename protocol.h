#pragma once
#include <stdio.h>
#include <stdlib.h>

#define PORT "5000"
#define HOST "localhost"
#define BACKLOG 20




void throw(const char * msg)
{
    fprintf(stderr, "%s \n", msg);
    exit(EXIT_FAILURE);
}