#include "config.h"
#include "third_party/fatfs/src/ff.h"

// Constants
static const char *configFilePath = "cfg.cfg";

// Force creation of external symbol
volatile config cfg;

void config_init( void )
{

	FIL fp;
	WORD br;
	f_open( &fp, configFilePath, FA_OPEN_EXISTING | FA_READ );
	f_read( &fp, (void *) &cfg, sizeof( cfg ), &br );
}
