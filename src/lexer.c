/*
	Jest Language

	Toy language that compiles to C.

	by David G. Maziero
	06/03/2016
*/

//MSVC hack
#ifndef inline
#define inline __inline
#endif

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
	TK_NUMERIC,
	//TODO: keywords?
	TK_KEYWORD,
	//TODO: traits?
	TK_TRAIT,
	TK_END_OF_STREAM
};

static const char *Token_Type_Names[TK_END_OF_STREAM] = {
	"unknown",
	"identifier",
	"equal sign",
	"plus sign",
	"minus sign",
	"asterisk",
	"slash",
	"colon",
	"semi-colon",
	"comma",
	"open parenthesis",
	"close parenthesis",
	"open brace",
	"close brace",
	"open bracket",
	"close bracket",
	"string",
	"numeric",
	"keyword",
	"trait"
};

#define KEYWORD_COUNT 17
static const char *Keywords[KEYWORD_COUNT] = {
	"void", "s8",  "u8", "s16", "u16", "s32", "u32", "s64", "u64", "f32", "f64",
	"struct", "while", "for", "return", "if", "use"
};

typedef struct Tokenizer
{
	int line;
	char *pos;
	char *file_name;
	//TODO: introduce "kind"? to specify KEYWORDS, integer, floating etc?
} Tokenizer;

typedef struct Token
{
	byte type;	
	int length;
	char *text;
} Token;


//InitTokenizer
static inline bool8 InitTokenizer( Tokenizer *tokenizer, const char *file_name )
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
static void EatWhiteSpaces( Tokenizer *tokenizer )
{
	while( tokenizer->pos[0] )
	{
		if( tokenizer->pos[0]==' '  ||
			tokenizer->pos[0]=='\t' ||
			tokenizer->pos[0]=='\r' )
		{
			tokenizer->pos++;
		} else
		if( tokenizer->pos[0]=='\n' )
		{
			tokenizer->line++;
			tokenizer->pos++;
		} else
		if( tokenizer->pos[0]=='/' )
		{
			tokenizer->pos++;
			if( tokenizer->pos[0] )
			{
				//single-line comment
				if( tokenizer->pos[0]=='/' )
				{
					tokenizer->pos++;
					while( tokenizer->pos[0] )
					{
						if( tokenizer->pos[0]=='\n' || tokenizer->pos[1]=='\r' )
						{
							tokenizer->pos++;
							break;
						}
						tokenizer->pos++;
					}
				} else
				//multi-line comment
				if( tokenizer->pos[0]=='*' )
				{
					tokenizer->pos++;
					while( tokenizer->pos[0] )
					{
						if( tokenizer->pos[0]=='*' )
						{
							if( tokenizer->pos[1] && tokenizer->pos[1]=='/' )
							{
								tokenizer->pos++;
								break;
							}
						}
						tokenizer->pos++;
					}
				}
			} 
		} else break;
	}
}

//IsAlpha
static inline bool8 IsAlpha( char c )
{
	return ( (c>='a' && c<='z') || (c>='A' && c<='Z') );
}

//IsDigit
static inline bool8 IsDigit( char c )
{
	return ( c>='0' && c<='9' );
}

//CompareString
static bool8 CompareString( char *token, int length, const char *compare_to )
{
	for( int i=0; i<length; i++ )
	{
		if( compare_to[i]==0 || token[i]!=compare_to[i] ) return FALSE;
	}
	return TRUE;
}

//IsKeyword
static inline bool8 IsKeyword( char *str, int length )
{
	for( int i=0; i<KEYWORD_COUNT; ++i )
		if( CompareString( str, length, Keywords[i] ) ) return TRUE;
	return FALSE;
}

//GetToken
static Token GetToken( Tokenizer *tokenizer )
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

		//TODO: single quotes?

		//strings
		case '"':
		{
			//TODO: consider ""
			token.type = TK_STRING;
			tokenizer->pos++;
			token.text = tokenizer->pos;
			int length = 1;
			while( tokenizer->pos[0] )
			{
				if( tokenizer->pos[0]=='"' ) break;
				else if( tokenizer->pos[0]=='\\' )
				{
					if( tokenizer->pos[1] )	tokenizer->pos++;
					//else ERROR
				}
				tokenizer->pos++;
				length++;
			}
			if( tokenizer->pos[0]!='"' )
			{
				//TODO: ERROR, unfinished string literal
			}
			token.length = length;
		} break;

		default:
		{
			//identifier
			if( IsAlpha( tokenizer->pos[0] ) )
			{
				token.type = TK_IDENTIFIER;
				tokenizer->pos++;
				int length = 1;
				while( tokenizer->pos[0] )
				{
					if( !( IsAlpha(tokenizer->pos[0]) || IsDigit(tokenizer->pos[0]) || tokenizer->pos[0]=='_' || tokenizer->pos[0]=='.' ) ) 
					{
						tokenizer->pos--;
						break;
					}
					tokenizer->pos++;
					length++;
				}
				token.length = length;
				//check if it's a keyword
				if( IsKeyword( token.text, token.length ) ) token.type = TK_KEYWORD;
			} else
			//number?
			if( IsDigit( tokenizer->pos[0] ) || tokenizer->pos[0]=='.' )
			{
				//TODO: hex, binary, float etc...
				token.type = TK_NUMERIC;
				tokenizer->pos++;
				int length = 1;
				while( tokenizer->pos[0] )
				{
					if( !IsDigit( tokenizer->pos[0] ) ) 
					{
						tokenizer->pos--;
						break;
					}
					tokenizer->pos++;
					length++;
				}
				token.length = length;
			}
		} break;
	}
	if( tokenizer->pos[0] ) tokenizer->pos++;

	return token;
}

//Parse
static bool8 Parse( Tokenizer *tokenizer, const char *file_name )
{
	if( !InitTokenizer( tokenizer, file_name ) ) return FALSE;

	while( TRUE )
	{
		Token token = GetToken( tokenizer );
		if( token.type==TK_END_OF_STREAM ) break;
		printf( "line(%.5d) type(%20s): [%.*s]\n", tokenizer->line, Token_Type_Names[token.type], token.length, token.text );
	}

	return TRUE;
}

//EOF
