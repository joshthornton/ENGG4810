#include "config.h"
#include "third_party/fatfs/src/ff.h"

// Constants
static const char *configFilePath = "config.engg4810";

// Force creation of external symbol
config cfg;

void config_init( config *c )
{

	FIL fp;
	WORD br;
	int i;

	f_open( &fp, configFilePath, FA_OPEN_EXISTING | FA_READ );
	f_read( &fp, &cfg, sizeof( cfg ), &br );

	// init each button buffer
	for ( i = 0; i < NUM_BUTTONS; ++i )
		buffer_init( &(c->buttons[i].buf) );
}
