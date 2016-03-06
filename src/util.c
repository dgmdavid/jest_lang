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
		fseek( f, 0, SEEK_END );
		size_t file_size = ftell( f );
		fseek( f, 0, SEEK_SET );
		char *text = (char*)mem_alloc( file_size+1 );
		fread( text, 1, file_size, f );
		text[file_size] = '\0';
		fclose( f );
		return text;
	} 
	return NULL;
}

//EOF
