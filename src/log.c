/*
	Jest Language

	Toy language that compiles to C.

	by David G. Maziero
	08/03/2016
*/

//Panic
static void Panic( char *message, ... )
{
	va_list	ap;
	va_start( ap, message );
	vsprintf( str_tmp_buffer, message, ap );
	va_end( ap );
	fprintf( stderr, "PANIC: %s\n", str_tmp_buffer );
	exit( ERRLVL_PANIC );
}

//Error
static void Error( char *file, int line, int col, char *message, ... )
{
	va_list	ap;
	va_start( ap, message );
	vsprintf( str_tmp_buffer, message, ap );
	va_end( ap );
	fprintf( stderr, "%s(%d,%d): ERROR: %s\n", file, line, col, str_tmp_buffer );
}

//TODO: Warning?

//EOF