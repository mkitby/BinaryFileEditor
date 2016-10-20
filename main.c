//binary file editor(bfe)
/* bfe supports the following command-line arguments:
 *
 * Syntax: bfe <options> INFILE, [] means it's optional
 *
 * Get a sub file, -s option
 * bfe.exe -s [-b begin] [-e end] -o OUTFILE INFILE
 *
 *      -b begin     Address to begin at in input file(decimal or hex(0x)), default = 0.
 *      -e end       Address to end at in input file (decimal or hex(0x)), default = end of file.
 *
 * Fill a part with value, -f option
 * bfe.exe -f -b begin -l length -v value [-o OUTFILE] INFILE
 *
 *      -b begin     Address to begin at in input file (decimal or hex(0x)), default = 0.
 *      -l length    Length of part to be filled, bytes.
 *      -v value     Fill value.
 *      -o OUTFILE   Output file name, it's optional, if no, overwrite input file
 *
 * Append a part to file, -a option
 * bfe.exe -a [-w] -l length -v vaule [-o OUTFILE] INFILE
 *
 *      -w           whole length of file option, if no, means append length, if has, means whole file length
 *      -l length    Length of part to be appended or whole file length(-w), bytes.
 *      -v value     Append value.
 *      -o OUTFILE   Output file name, it's optional, if no, overwrite input file
 *
 * Show helps. -h option
 * bfe.exe -h
 */

// To do list
// 1. Support insert a file or value feature
// 2. Error handling and parameter checking
// 3. Support -b and -e options in -f feature
// 4. Support -b and -l options in -s feature

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __GNUC__
#include <unistd.h>
#else
#include "getopt.h"
#endif


#define BFE_VERSION_NUMBER      "v1.0"

#define BFE_HELP_STRING "\n\
 *                                                                          \n\
 * bfe supports the following command-line arguments:                       \n\
 *                                                                          \n\
 * Syntax: bfe <options> INFILE, [] means it's optional                     \n\
 *                                                                          \n\
 * Get a sub file, -s option                                                \n\
 * bfe.exe -s [-b begin] [-e end] -o OUTFILE INFILE                         \n\
 *                                                                          \n\
 * -b begin     Address to begin at in input file,                          \n\
 *              decimal or hex(0x), default = 0                             \n\
 * -e end       Address to end at in input file,                            \n\
 *              decimal or hex(0x), default = end of file                   \n\
 *                                                                          \n\
 * Fill a part with value, -f option                                        \n\
 * bfe.exe -f -b begin -l length -v value [-o OUTFILE] INFILE               \n\
 *                                                                          \n\
 * -b begin     Address to begin at in input file,                          \n\
 *              decimal or hex(0x), default = 0                             \n\
 * -l length    Length of part to be filled, bytes                          \n\
 * -v value     Fill value                                                  \n\
 * -o OUTFILE   Output file name, it's optional, if no, overwrite input file\n\
 *                                                                          \n\
 * Append a part to file, -a option                                         \n\
 * bfe.exe -a [-w] -l length -v vaule [-o OUTFILE] INFILE                   \n\
 *                                                                          \n\
 * -w           whole length of file option, if no, means append length,    \n\
 *              if has, means whole file length                             \n\
 * -l length    Length of part to be appended or whole file length(-w)      \n\
 * -v value     Append value                                                \n\
 * -o OUTFILE   Output file name, it's optional, if no, overwrite input file\n\
 *                                                                          \n\
 * Show helps, -h option                                                    \n\
 * bfe.exe -h                                                               \n\
 */"

struct globalArgs_t {
    int action;              /* -s -f -i -a options */
    int beginFlag;           /* -b option */
    unsigned long begin;     /* -b option parameter */
    int endFlag;             /* -e option */
    unsigned long end;       /* -e option parameter */
    unsigned long length;    /* -l option */
    unsigned char value;	 /* -v option */
    int whole;               /* -w option */
    char *outputFileName;    /* -o option */
    char *inputFileName;     /* last argument */

} globalArgs;

static const char *optString = "sfiawhb:e:l:v:o:";


static int bfe_action(void);
static int bfe_GetSubFile(void);
static int bfe_FillFile(void);
static int bfe_InsertFile(void);
static int bfe_AppendFile(void);
static int bfe_ShowHelp(void);
//static void printf_args(void);

int main(int argc, char *argv[])
{
    int opt = 0;

    /* Initialize globalArgs before we get to work. */
    memset (&globalArgs, 0, sizeof(globalArgs));

    while ((opt = getopt(argc, argv, optString)) != -1) {
        switch (opt) {
        case 's':
            globalArgs.action = 's';
            break;

        case 'f':
            globalArgs.action = 'f';
            break;

        case 'i':
            globalArgs.action = 'i';
            break;

        case 'a':
            globalArgs.action = 'a';
            break;

        case 'b':
            globalArgs.begin = strtol(optarg, NULL, 0);
            globalArgs.beginFlag = 1;
            break;

        case 'e':
            globalArgs.end = strtol(optarg, NULL, 0);
            globalArgs.endFlag = 1;
            break;

        case 'l':
            globalArgs.length = strtol(optarg, NULL, 0);
            break;

        case 'v':
            globalArgs.value = strtol(optarg, NULL, 0);
            break;

        case 'o':
            globalArgs.outputFileName = optarg;
            break;

        case 'w':
            globalArgs.whole = 1;
            break;

        case 'h':
            globalArgs.action = 'h';
            break;

        default:
            /* You won't actually get here. */
            break;
        }
    }

    globalArgs.inputFileName = argv[optind];

    //printf_args();

    return bfe_action();
}

static int bfe_action(void)
{
    int result = -1;

    switch (globalArgs.action) {
    case 's':
        result = bfe_GetSubFile();
        break;

    case 'f':
        result = bfe_FillFile();
        break;

    case 'i':
        result = bfe_InsertFile();
        break;

    case 'a':
        result = bfe_AppendFile();
        break;

    case 'h':
        result = bfe_ShowHelp();
        break;
    default:
        /* You won't actually get here. */
        break;
    }

    return result;
}

// bfe.exe -s [-b begin] [-e end] -o OUTFILE INFILE
static int bfe_GetSubFile(void)
{
    FILE *fpIn, *fpOut;
    char tmpBuf[1024];
    unsigned long length;
    unsigned long fileSize;

    if ((fpIn = fopen(globalArgs.inputFileName, "rb"))==NULL) {
        printf("Can not open file %s \n", globalArgs.inputFileName);
        return -1;
    }

    if (globalArgs.beginFlag == 0) {
        globalArgs.begin = 0;
    }

    if (globalArgs.endFlag == 0) { // to end of file
        fseek(fpIn, 0L, SEEK_END);
        fileSize = ftell(fpIn);

        if (fileSize <= globalArgs.begin) {
            printf("Error -e parameter \n");
            fclose (fpIn);
            return -1;
        }
        length = fileSize - globalArgs.begin;
    } else {
        if (globalArgs.end < globalArgs.begin) {
            printf("Error -b -e parameters \n");
            fclose (fpIn);
            return -1;
        }
        length = globalArgs.end - globalArgs.begin + 1;
    }

    fseek(fpIn, globalArgs.begin, SEEK_SET);

    if ((fpOut = fopen(globalArgs.outputFileName, "ab+"))==NULL) {
        printf("Can not open file %s \n", globalArgs.outputFileName);
        fclose (fpIn);
        return -1;
    }

    while (length > 0) {
        if (length <= 1024) {
            fread(tmpBuf, sizeof(char), length, fpIn);
            fwrite(tmpBuf, sizeof(char), length, fpOut);
            length -= length;
        } else {
            fread(tmpBuf, sizeof(char), 1024, fpIn);
            fwrite(tmpBuf, sizeof(char), 1024, fpOut);
            length -= 1024;
        }
    }

    fclose (fpIn);
    fclose (fpOut);

    return 0;
}

// bfe.exe -f -b begin -l length -v value [-o OUTFILE] INFILE
static int bfe_FillFile(void)
{
    FILE *fpIn, *fpOut;
    char tmpBuf[1024];
    unsigned long length;
    unsigned long fileSize;
    int i;

    if ((fpIn = fopen(globalArgs.inputFileName, "rb+"))==NULL) {
        printf("Can not open file %s \n", globalArgs.inputFileName);
        return -1;
    }

    if (globalArgs.outputFileName) {
        if ((fpOut = fopen(globalArgs.outputFileName, "ab+"))==NULL) {
            printf("Can not open file %s \n", globalArgs.outputFileName);
            fclose (fpIn);
            return -1;
        }

        //if (globalArgs.beginFlag == 0)
        //{
        //    globalArgs.begin = 0; gong
        //}

        length = globalArgs.length;

        fseek(fpIn, 0L, SEEK_END);
        fileSize = ftell(fpIn);
        fseek(fpIn, 0, SEEK_SET);

        length = globalArgs.begin;
        while (length > 0) {
            if (length <= 1024) {
                fread(tmpBuf, sizeof(char), length, fpIn);
                fwrite(tmpBuf, sizeof(char), length, fpOut);
                length -= length;
            } else {
                fread(tmpBuf, sizeof(char), 1024, fpIn);
                fwrite(tmpBuf, sizeof(char), 1024, fpOut);
                length -= 1024;
            }
        }

        for (i = 0; i < 1024; i++) {
            tmpBuf[i] = globalArgs.value;
        }

        length = globalArgs.length;
        while (length > 0) {
            if (length <= 1024) {
                fwrite(tmpBuf, sizeof(char), length, fpOut);
                length -= length;
            } else {
                fwrite(tmpBuf, sizeof(char), 1024, fpOut);
                length -= 1024;
            }
        }

        fseek(fpIn, globalArgs.begin + globalArgs.length, SEEK_SET);

        length = fileSize - globalArgs.length - globalArgs.begin;
        while (length > 0) {
            if (length <= 1024) {
                fread(tmpBuf, sizeof(char), length, fpIn);
                fwrite(tmpBuf, sizeof(char), length, fpOut);
                length -= length;
            } else {
                fread(tmpBuf, sizeof(char), 1024, fpIn);
                fwrite(tmpBuf, sizeof(char), 1024, fpOut);
                length -= 1024;
            }
        }

        fclose (fpIn);
        fclose (fpOut);
    } else { // No -o option, overwrite input file
        for (i = 0; i < 1024; i++) {
            tmpBuf[i] = globalArgs.value;
        }

        fseek(fpIn, globalArgs.begin, SEEK_SET);

        length = globalArgs.length;
        while (length > 0) {
            if (length <= 1024) {
                fwrite(tmpBuf, sizeof(char), length, fpIn);
                length -= length;
            } else {
                fwrite(tmpBuf, sizeof(char), 1024, fpIn);
                length -= 1024;
            }
        }

        fclose (fpIn);
    }

    return 0;
}

static int bfe_InsertFile(void)
{
    printf("Not supported \n");
    return -1;
}

// bfe.exe -a [-w] -l length -v vaule [-o OUTFILE] INFILE
static int bfe_AppendFile(void)
{
    FILE *fpIn, *fpOut;
    char tmpBuf[1024];
    unsigned long appendLength;
    unsigned long fileSize;
    int i;

    if (globalArgs.outputFileName) {
        if ((fpIn = fopen(globalArgs.inputFileName, "rb"))==NULL) {
            printf("Can not open file %s \n", globalArgs.inputFileName);
            return -1;
        }

        fseek(fpIn, 0L, SEEK_END);
        fileSize = ftell(fpIn);
        fseek(fpIn, 0, SEEK_SET);

        if (globalArgs.whole) { // whole length
            if (globalArgs.length < fileSize) {
                printf("Error length \n");
                fclose (fpIn);
                return -1;
            }
            appendLength =  globalArgs.length - fileSize;
        } else { // append length
            appendLength = globalArgs.length;
        }

        if ((fpOut = fopen(globalArgs.outputFileName, "ab+"))==NULL) {
            printf("Can not open file %s \n", globalArgs.outputFileName);
            fclose (fpIn);
            return -1;
        }

        while (fileSize > 0) {
            if (fileSize <= 1024) {
                fread(tmpBuf, sizeof(char), fileSize, fpIn);
                fwrite(tmpBuf, sizeof(char), fileSize, fpOut);
                fileSize -= fileSize;
            } else {
                fread(tmpBuf, sizeof(char), 1024, fpIn);
                fwrite(tmpBuf, sizeof(char), 1024, fpOut);
                fileSize -= 1024;
            }
        }

        for (i = 0; i < 1024; i++) {
            tmpBuf[i] = globalArgs.value;
        }

        while (appendLength > 0) {
            if (appendLength <= 1024) {
                fwrite(tmpBuf, sizeof(char), appendLength, fpOut);
                appendLength -= appendLength;
            } else {
                fwrite(tmpBuf, sizeof(char), 1024, fpOut);
                appendLength -= 1024;
            }
        }

        fclose (fpIn);
        fclose (fpOut);
    } else { // No -o option, overwrite input file
        for (i = 0; i < 1024; i++) {
            tmpBuf[i] = globalArgs.value;
        }

        if ((fpIn = fopen(globalArgs.inputFileName, "ab+"))==NULL) {
            printf("Can not open file %s \n", globalArgs.inputFileName);
            return -1;
        }

        if (globalArgs.whole) { // whole length
            fseek(fpIn, 0L, SEEK_END);
            fileSize = ftell(fpIn);
            fseek(fpIn, 0, SEEK_SET);

            if (globalArgs.length < fileSize) {
                printf("Error length \n");
                fclose (fpIn);
                return -1;
            }
            appendLength =  globalArgs.length - fileSize;
        } else { // append length
            appendLength = globalArgs.length;
        }

        while (appendLength > 0) {
            if (appendLength <= 1024) {
                fwrite(tmpBuf, sizeof(char), appendLength, fpIn);
                appendLength -= appendLength;
            } else {
                fwrite(tmpBuf, sizeof(char), 1024, fpIn);
                appendLength -= 1024;
            }
        }

        fclose(fpIn);
    }

    return 0;
}

static int bfe_ShowHelp(void)
{
    printf("\n/* bfe - binary file editor ");
    printf(BFE_VERSION_NUMBER);
    printf("%s", BFE_HELP_STRING);

    return 0;
}

#if 0
static void printf_args(void)
{
    printf("action=%c\n", globalArgs.action);
    printf("beginFlag=%d\n", globalArgs.beginFlag);
    printf("begin=%ld\n", globalArgs.begin);
    printf("endFlag=%d\n", globalArgs.endFlag);
    printf("end=%ld\n", globalArgs.end);
    printf("length=%ld\n", globalArgs.length);
    printf("value=%d\n", globalArgs.value);
    printf("whole=%d\n", globalArgs.whole);
    printf("outputFileName=%s\n", globalArgs.outputFileName);
    printf("inputFileName=%s\n", globalArgs.inputFileName);
}
#endif
