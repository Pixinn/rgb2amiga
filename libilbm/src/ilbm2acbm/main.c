/*
 * Copyright (c) 2012 Sander van der Burg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#if HAVE_GETOPT_H == 1
#include <getopt.h>
#elif _MSC_VER
#include <string.h>
#else
#include <unistd.h>
#endif

#include <stdio.h>
#include "ilbm2acbm.h"

#define TRUE 1
#define FALSE 0

static void printUsage(const char *command)
{
    fprintf(stderr, "Usage:\n");
#if _MSC_VER
    fprintf(stderr, "%s [/i file.IFF] [/o file.IFF] file.ILBM\n\n", command);
    fprintf(stderr, "/?    Shows the usage of this command to the user\n");
#else
    fprintf(stderr, "%s [-i file.IFF] [-o file.IFF] file.ILBM\n\n", command);
    fprintf(stderr, "-h, --help    Shows the usage of this command to the user\n");
#endif
}

int main(int argc, char *argv[])
{
    char *inputFilename = NULL;
    char *outputFilename = NULL;
#if _MSC_VER
    unsigned int optind = 1;
    unsigned int i;
    int inputFilenameFollows = FALSE;
    int outputFilenameFollows = FALSE;
    
    for(i = 1; i < argc; i++)
    {
        if (inputFilenameFollows)
        {
            inputFilename = argv[i];
            inputFilenameFollows = FALSE;
            optind++;
        }
        else if (outputFilenameFollows)
        {
            outputFilename = argv[i];
            outputFilenameFollows = FALSE;
            optind++;
        }
        else if (strcmp(argv[i], "/i") == 0)
        {
            inputFilenameFollows = TRUE;
            optind++;
        }
        else if (strcmp(argv[i], "/o") == 0)
        {
            outputFilenameFollows = FALSE;
            optind++;
        }
        else if (strcmp(argv[i], "/?") == 0)
        {
            printUsage(argv[0]);
            return 0;
        }
    }
#else
    int c;
#if HAVE_GETOPT_H == 1
    int option_index = 0;
    struct option long_options[] =
    {
        {"help", no_argument, 0, 'h'},
        {"input-file", required_argument, 0, 'i'},
        {"output-file", required_argument, 0, 'o'},
        {0, 0, 0, 0}
    };
#endif

    /* Parse command-line options */
    
#if HAVE_GETOPT_H == 1
    while((c = getopt_long(argc, argv, "i:o:h", long_options, &option_index)) != -1)
#else
    while((c = getopt(argc, argv, "i:o:h")) != -1)
#endif
    {
        switch(c)
        {
            case 'i':
                inputFilename = optarg;
                break;
            case 'o':
                outputFilename = optarg;
                break;
            case 'h':
            case '?':
                printUsage(argv[0]);
                return 0;
        }
    }
#endif
    /* Check parameters */

    if(inputFilename == NULL && outputFilename == NULL)
    {
        fprintf(stderr, "ERROR: At least an input file or output file must be specified!\n");
        return 1;
    }
    else
        return ILBM_ILBMtoACBM(inputFilename, outputFilename);
}
