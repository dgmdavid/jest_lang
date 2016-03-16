/*
	Jest Language

	Toy language that compiles to C.

	by David G. Maziero
	08/03/2016
*/

//Parse
static bool8 Parse( Tokenizer *tokenizer, char *file_name )
{
	if( !InitTokenizer( tokenizer, file_name ) ) return FALSE;

	while( TRUE )
	{
		Token token = GetToken( tokenizer );
		if( token.type==TK_END_OF_STREAM ) break;
		printf( "(%.3d,%.2d) type(%13s): [%.*s]\n", token.line, token.col, Token_Type_Desc[token.type], token.length, token.text );
	}

	return TRUE;
}

//EOF
