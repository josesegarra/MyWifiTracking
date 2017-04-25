
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// MACRO to check if two MAC addresses are the SAME
#define SAME_VENDOR_MAC(m0,m1)  ((m0[0] == m1[0] && m0[1] == m1[1] && m0[2] == m1[2]) ? 1 : 0)

static char* module = __FILE__; // "vendors.c";


typedef struct {
    unsigned char b[3];
    char name[50];
} t_vendor;

static t_vendor*    vendors;
static int          nvendors = 0;

void vendor_namecp(char* d, char *s)
{
    int i = 48;
    while (i && *s >= ' ')
    {
        *d = *s;
        d++;
        s++;
        i--;
    }
    *s = '\0';
}

void vendors_init(char* fileName)
{
    FILE * fp;
    unsigned char b[3];
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen(fileName, "r");																											// Open vendors file
    if (!fp)
    {
        utils_log(module, "Could not open vendors file %s",fileName);
        return;
    }
	utils_log(module, "Reading vendors file %s", fileName);
    nvendors = 0;
    while ((read = getline(&line, &len, fp)) != -1) if (read > 15 && line[2] == '-' && line[5] == '-') nvendors++;						// Read a line, if it has more than 15 chars, and char2 is : and char 5 is : then this is a vender
    vendors = malloc(nvendors*sizeof(t_vendor));																						// Alloc memory to store vendors
    memset(vendors, 0, nvendors*sizeof(t_vendor));																						// Set vendors memory to zero
    t_vendor* cur = vendors;																											// Current vender is the first
	fseek(fp, 0, SEEK_SET);																												// Move to the beginning
	while ((read = getline(&line, &len, fp)) != -1)																						// Loop all file again
    {
        if (read > 15 && line[2] == '-' && line[5] == '-')																				// If this is a vendor line
        {   
                b[0] = line[0]; b[1] = line[1]; b[2] = '\0'; cur->b[0]= (unsigned char)strtol(b,NULL, 16);								// Read in cur the three bytes as numbers
                b[0] = line[3]; b[1] = line[4]; b[2] = '\0'; cur->b[1] = (unsigned char)strtol(b, NULL, 16);
                b[0] = line[6]; b[1] = line[7]; b[2] = '\0'; cur->b[2] = (unsigned char)strtol(b, NULL, 16);
                utils_trimcpy(&cur->name, &line[9], 48);																				// And copy the name
                cur++;
        }
    }
	utils_log(module, "Done reading vendors file: %i MAC vendors",nvendors);
	fclose(fp);
    if (line) free(line);
}

void vendors_log()
{
    int i = 0;
    t_vendor* cur = vendors;
    utils_log(module, "Vendors total: %i ", nvendors);
    while (i < nvendors)
    {
        utils_log(module, "Vendor %02X:%02X:%02X %s", cur->b[0], cur->b[1], cur->b[2], cur->name);
        i++;
        cur++;
    }
}

char* vendor_name(char p[])
{
    int i = 0;
    t_vendor* cur = vendors;
    while (i < nvendors)
    {
        if (SAME_VENDOR_MAC(cur->b, p)) return cur->name;
        i++;cur++;
    }
    return "-- no vendor";
}