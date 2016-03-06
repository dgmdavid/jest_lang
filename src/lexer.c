/*
	Jest Language

	Toy language that compiles to C.

	by David G. Maziero
	06/03/2016
*/

enum TokenType
{
	TK_UNKNOWN,
	TK_IDENTIFIER,
	TK_EQUALS,
	TK_PLUS,
	TK_MINUS,
	TK_STAR,
	TK_SLASH,
	TK_COLON,
	TK_SEMI_COLON,
	TK_COMMA,
	TK_OPEN_PAREN,
	TK_CLOSE_PAREN,
	TK_OPEN_BRACE,
	TK_CLOSE_BRACE,
	TK_OPEN_BRACKET,
	TK_CLOSE_BRACKET,
	TK_STRING,
	TK_INTEGER,
	TK_FLOATING,
	//TODO: keywords
	TK_END_OF_STREAM
};

typedef struct Tokenizer
{
	int line;
	char *pos;
	char *file_name;
} Tokenizer;

typedef struct Token
{
	byte type;	
	int length;
	char *text;
} Token;


//InitTokenizer
BOOL InitTokenizer( Tokenizer *tokenizer, const char *file_name )
{
	char *input_file_buffer = LoadFileToMemory( file_name );
	if( !input_file_buffer )
	{
		printf( "\tCouldn't read input file: \"%s\"\n\n", file_name );
		return FALSE;
	}

	tokenizer->line = 1;
	tokenizer->pos = input_file_buffer;
	tokenizer->file_name = (char*)file_name;

	return TRUE;
}

//EatWhiteSpaces
void EatWhiteSpaces( Tokenizer *tokenizer )
{
	while( tokenizer->pos[0] )
	{
		if( tokenizer->pos[0]==' '  ||
			tokenizer->pos[0]=='\t' ||
			tokenizer->pos[0]=='\r' )
		{
			tokenizer->pos++;
		} else {
			if( tokenizer->pos[0]=='\n' )
			{
				tokenizer->line++;
				tokenizer->pos++;
			} else break;
		}
	}
}

//IsAlpha
BOOL IsAlpha( char c )
{
	if( (c>='a' && c<='z') ||
		(c>='A' && c<='Z') ||
		c=='_' ) return TRUE;
	return FALSE;
}

//IsDigit
BOOL IsDigit( char c )
{
	if( c>='0' && c<='9' ) return TRUE;
	return FALSE;
}

//GetToken
Token GetToken( Tokenizer *tokenizer )
{
	EatWhiteSpaces( tokenizer );

	Token token;
	token.type = TK_UNKNOWN;
	token.text = tokenizer->pos;
	token.length = 1;

	if( tokenizer->pos[0]==0 )
	{
		token.type = TK_END_OF_STREAM;
		return token;
	}

	switch( tokenizer->pos[0] )
	{
		case '=': token.type = TK_EQUALS; break;
		case '+': token.type = TK_PLUS; break;
		case '-': token.type = TK_MINUS; break;
		case '*': token.type = TK_STAR; break;
		case '/': token.type = TK_SLASH; break;
		case ':': token.type = TK_COLON; break;
		case ';': token.type = TK_SEMI_COLON; break;
		case ',': token.type = TK_COMMA; break;
		case '(': token.type = TK_OPEN_PAREN; break;
		case ')': token.type = TK_CLOSE_PAREN; break;
		case '[': token.type = TK_OPEN_BRACKET; break;
		case ']': token.type = TK_CLOSE_BRACKET; break;
		case '{': token.type = TK_OPEN_BRACE; break;
		case '}': token.type = TK_CLOSE_BRACE; break;
		default:
		{
			//TODO: I think some work is needed here :P
		} break;
	}
	tokenizer->pos++;

	return token;
}

//Parse
BOOL Parse( Tokenizer *tokenizer, const char *file_name )
{
	if( !InitTokenizer( tokenizer, file_name ) ) return FALSE;

	while( TRUE )
	{
		Token token = GetToken( tokenizer );
		if( token.type==TK_END_OF_STREAM ) break;
		printf( "(%d)%d: %.*s\n", tokenizer->line, token.type, token.length, token.text );
	}

	return TRUE;
}

//EOF
