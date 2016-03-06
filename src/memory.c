/*
	Jest Language

	Toy language that compiles to C.

	by David G. Maziero
	06/03/2016
*/

//mem_alloc
static void *mem_alloc( size_t size )
{
	//TODO: use some kind of memory pool?
	void *ptr = malloc( size );
	return ptr;
}

//EOF
