#include <cs50.h>  // for bool
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint8_t BYTE;
const int BUFFER_SIZE = 512;

void attemptRecovery(FILE *imageFile);
bool getBlock(FILE *imageFile, BYTE buffer[BUFFER_SIZE]);
bool blockHasJPGHeader(BYTE buffer[BUFFER_SIZE]);
FILE *openRecoveryFile(int currentRecoveryFileNum);

// Attempt to recover "lost" JPG files
// from a forensic dump of a camera card
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }

    char *imageFilename = argv[1];
    FILE *imageFile = fopen(imageFilename, "rb");
    if (imageFile == NULL)
    {
        printf("Unable to open input file %s\n", imageFilename);
        return 1;
    }

    attemptRecovery(imageFile);
    fclose(imageFile);
    return 0;
}

// Look for a JPG header aligned on a BUFFER_SIZE (512) byte boundary
// and assume it is the start of a JPG to recover.  Write all further
// blocks until the next assumed JPG header is found.
void attemptRecovery(FILE *imageFile)
{
    BYTE buffer[BUFFER_SIZE];
    bool recovering = false;
    FILE *recoveryFile = NULL;
    int currentRecoveryFileNum = 0;

    while (getBlock(imageFile, buffer))
    {
        if (blockHasJPGHeader(buffer))
        {
            if (recovering)
            {
                // Close off the previous file and start a new one
                if (recoveryFile != NULL)
                {
                    fclose(recoveryFile);
                    recoveryFile = NULL;
                }
            }
            recovering = true;
        }

        if (recovering)
        {
            if (recoveryFile == NULL)
            {
                // open a new recovery file
                recoveryFile = openRecoveryFile(currentRecoveryFileNum++);
                if (recoveryFile == NULL)
                {
                    printf("Unable to open a new recovery file!\n");
                    return;
                }
            }

            if (BUFFER_SIZE != fwrite(buffer, 1, BUFFER_SIZE, recoveryFile))
            {
                printf("Unable to write to recovery file!\n");
                fclose(recoveryFile);
                return;
            }
        }
    }

    // Close any currently open recovery file
    if (recoveryFile != NULL)
    {
        fclose(recoveryFile);
        recoveryFile = NULL;
    }
    return;
}

// Read a block from the forensic image, return true if possible, or false
// if no more blocks are available (presumably because of end of file)
bool getBlock(FILE *imageFile, BYTE buffer[BUFFER_SIZE])
{
    memset(buffer, 0, BUFFER_SIZE);
    if (fread(buffer, 1, BUFFER_SIZE, imageFile) > 0)
    {
        return true;
    }
    return false;
}

// Look for a JPG header using the the signature in the assignment:
// Specifically, the first three bytes of JPEGs are 0xff 0xd8 0xff
// from first byte to third byte, left to right. The fourth byte, meanwhile, is
// either 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
// 0xeb, 0xec, 0xed, 0xee, or 0xef. Put another way, the fourth byte’s first four bits are 1110.
bool blockHasJPGHeader(BYTE buffer[BUFFER_SIZE])
{
    if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff)
    {
        return (buffer[3] >> 4) == 0x0e;
    }
    return false;
}

// Format the name of the recovery file and open it
FILE *openRecoveryFile(int currentRecoveryFileNum)
{
    char filename[8];
    sprintf(filename, "%03i.jpg", currentRecoveryFileNum);
    return fopen(filename, "wb");
}
