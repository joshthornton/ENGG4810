#include "config.h"
#include "third_part/fatfs/src/ff.h"

// Constants
static const char[] configFilePath = "config.engg4810";
static const char[][] buttonWavPaths = {
	"b0.dat",
	"b1.dat",
	"b2.dat",
	"b3.dat",
	"b4.dat",
	"b5.dat",
	"b5.dat",
	"b6.dat",
	"b7.dat",
	"b8.dat",
	"b9.dat",
	"b10.dat",
	"b11.dat",
	"b12.dat",
	"b13.dat",
	"b14.dat",
	"b15.dat",
};

void config_init( config *c )
{

	FIL fp;
	FRESULT res;

	// init each button buffer
	for ( int i = 0; i < NUM_BUTTONS; ++i )
		buffer_init( &(c->buttons[i].b) );

	

}



