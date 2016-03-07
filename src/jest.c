/*
	Jest Language

	Toy language that compiles to C.

	by David G. Maziero
	06/03/2016
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char bool8;
typedef unsigned char byte;
typedef unsigned short word;

#define TRUE	1
#define FALSE	0

#define ERRLVL_SUCCESS			0
#define ERRLVL_PARSE_ERROR		1
#define ERRLVL_NO_INPUT_FILE	2
#define ERRLVL_UNKNOWN_OPTION	3
#define ERRLVL_ERROR_OTHER		4
#define ERRLVL_ERROR_LOADING	5

//compiler modes
bool8 CM_QUIET = FALSE;
bool8 CM_COMPILE_ONLY = FALSE;

#include "memory.c"
#include "util.c"
#include "lexer.c"

//main
int main( int argc, char **argv )
{
	printf( "Jest Compiler v0.1\n");
	printf( "Copyleft (c) 2016 by David G. Maziero\n\n" );

	//no arguments?
	if( argc==1 )
	{
		printf( "Usage:\n\tjest source.jst {-options}\n\n" );
		printf( "\t-q \tquiet mode (only prints error messages)\n" );
		printf( "\t-c \tcompile only (do not generate executable file)\n\n" );
		return 0;
	}

	char *input_file = 0;
	for( int i=1; i<argc; ++i )
	{
		//an option
		if( argv[i][0]=='-' )
		{
			if( strcmp( argv[i], "-q" )==0 || strcmp( argv[i], "-Q" )==0 )
			{
				CM_QUIET = TRUE;
			} else
			if( strcmp( argv[i], "-c" )==0 || strcmp( argv[i], "-C" )==0 )
			{
				CM_COMPILE_ONLY = TRUE;
			} else {
				printf( "\tUnknown option: \"%s\"\n\n", argv[i]+1 );
				return ERRLVL_UNKNOWN_OPTION;
			}
		} else {
			//an input file
			if( input_file )
			{
				printf( "\tOnly one input file is allowed for now.\n\n" );
				return ERRLVL_ERROR_OTHER;
			}
			size_t ifn_size = strlen( argv[i] );
			input_file = (char*)mem_alloc( ifn_size+1 );
			strcpy( input_file, argv[i] );
			input_file[ifn_size] = 0;
		}
		
	}

	//no input file?
	if( !input_file )
	{
		printf( "\tNo input file specified!\n\n" );
		return ERRLVL_NO_INPUT_FILE;
	}

	Tokenizer tokenizer;
	if( !Parse( &tokenizer, input_file ) ) return ERRLVL_PARSE_ERROR;

	return ERRLVL_SUCCESS;
}

//EOF
