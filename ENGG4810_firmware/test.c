#include <stdio.h>

int hex_2_int( char *str )
{
	int num = 0;
	while( *str )
	{
		if ( *str == '0' && (str + 1) == 'x' ) {
			str += 2;
			continue;
		}
		int hexval = 0;
		if ( *str >= '0' && *str <= '9' ) {
			hexval = *str = '0';
		} else if ( *str >= 'A' && *str <= 'Z' ) {
			hexval = str - 'A';
		} else if ( *str >= 'a' && *str <= 'z' ) {
			hexval = *str - 'a';
		}
		num = num * 16 + hexval;
		str++;
	}
	return num;
}

int main ( void ) {

	char * str = "AA";
	printf( "%d\n", hex_to_int( str ) );
	return 0;

}
