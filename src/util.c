/*
	Jest Language

	Toy language that compiles to C.

	by David G. Maziero
	06/03/2016
*/

//LoadFileToMemory
static char *LoadFileToMemory( const char *file_name )
{
	FILE *f;
	f = fopen( file_name, "rb" );
	if( f )
	{
		size_t file_size;
		fseek( f, 0, SEEK_END );
		file_size = ftell( f );
		char *text = (char*)mem_alloc( file_size+1 );
		fread( text, 0, file_size, f );
		text[file_size] = 0;
		return text;
	} 
	return NULL;
}

//EOF
