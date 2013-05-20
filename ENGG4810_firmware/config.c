#include "config.h"
#include "third_party/fatfs/src/ff.h"

// Constants
static const char *configFilePath = "0:1.dat";

// Force creation of external symbol
volatile config cfg;

BYTE buf[30];

void config_init( void )
{
	FIL fp;
	WORD br;
	int i = sizeof cfg;
	i++;
	FRESULT j =  f_open(&fp, configFilePath, FA_OPEN_EXISTING | FA_READ);
    FRESULT res = f_read(&fp, buf, sizeof buf, &br);
}
