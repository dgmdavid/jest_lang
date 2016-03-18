/*
	Jest Language

	Toy language that compiles to C.

	by David G. Maziero
	06/03/2016
*/

//InitTokenizer
static inline bool8 InitTokenizer( Tokenizer *tokenizer, char *file_name )
{
	char *input_file_buffer = LoadFileToMemory( file_name );
	if( !input_file_buffer )
	{
		printf( "\tCouldn't read input file: \"%s\"\n\n", file_name );
		return FALSE;
	}

	tokenizer->line = 1;
	tokenizer->col = 1;
	tokenizer->pos = input_file_buffer;
	tokenizer->file_name = file_name;

	return TRUE;
}

//TkzAdvance
static inline void TkzAdvance( Tokenizer *tokenizer )
{
	if( tokenizer->pos[0]=='\n' ) 
	{
		tokenizer->line++;
		tokenizer->col = 0;
	}	
	tokenizer->pos++;
	if( tokenizer->pos[0]!='\r' ) tokenizer->col++;
}

//EatWhiteSpaces
static void EatWhiteSpaces( Tokenizer *tokenizer )
{
	while( tokenizer->pos[0] )
	{
		if( tokenizer->pos[0]==' '  ||
			tokenizer->pos[0]=='\t' ||
			tokenizer->pos[0]=='\b' ||
			tokenizer->pos[0]=='\r' ||
			tokenizer->pos[0]=='\n' )
		{
			TkzAdvance( tokenizer );
		} 
		else if( tokenizer->pos[0]=='/' )
		{
			TkzAdvance( tokenizer );
			if( tokenizer->pos[0] )
			{
				//single-line comment
				if( tokenizer->pos[0]=='/' )
				{
					TkzAdvance( tokenizer );
					while( tokenizer->pos[0] )
					{
						if( tokenizer->pos[0]=='\n' || tokenizer->pos[1]=='\r' )
						{
							TkzAdvance( tokenizer );
							break;
						}
						TkzAdvance( tokenizer );
					}
				} else
				//multi-line comment
				if( tokenizer->pos[0]=='*' )
				{
					TkzAdvance( tokenizer );
					while( tokenizer->pos[0] )
					{
						if( tokenizer->pos[0]=='*' )
						{
							if( tokenizer->pos[1] && tokenizer->pos[1]=='/' )
							{
								TkzAdvance( tokenizer );
								break;
							}
						}
						TkzAdvance( tokenizer );
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

//IsHex
static inline bool8 IsHex( char c )
{
	return ( (c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F') );
}

//IsOctal
static inline bool8 IsOctal( char c )
{
	return (c>='0' && c<='8');
}

//CompareString
static bool8 CompareString( char *token, int length, char *compare_to )
{
	int i=0;
	for( i=0; i<length; i++ )
	{
		if( compare_to[i]==0 || token[i]!=compare_to[i] ) return FALSE;
	}
	if( compare_to[i]!=0 ) return FALSE;
	return TRUE;
}

//IsKeyword
static inline bool8 IsKeyword( char *str, int length )
{
	for( int i=0; i<KEYWORD_COUNT; ++i )
		if( CompareString( str, length, Keywords[i] ) ) return TRUE;
	return FALSE;
}

//IsBuiltinType
static inline bool8 IsBasicType( char *str, int length )
{
	for( int i=0; i<BASIC_TYPE_COUNT; ++i )
		if( CompareString( str, length, BasicTypes[i] ) ) return TRUE;
	return FALSE;
}

//ParseString
static bool8 ParseString( Tokenizer *tokenizer, Token *token )
{
	bool8 result = TRUE;
	token->type = TK_STRING;
	TkzAdvance( tokenizer );
	token->text = tokenizer->pos;
	int length = 1;
	while( tokenizer->pos[0] )
	{
		if( tokenizer->pos[0]=='"' ) break;
		else if( tokenizer->pos[0]=='\\' )
		{
			if( tokenizer->pos[1] )	
				TkzAdvance( tokenizer ); 
			else 
				break;
		}
		TkzAdvance( tokenizer );
		length++;
	}
	if( tokenizer->pos[0]!='"' )
	{
		Error( tokenizer->file_name, token->line, token->col, "unfinished string literal" );
		result = FALSE;
	}
	token->length = (token->text==tokenizer->pos)?0:length;
	return result;
}

//ParseNumeric
static bool8 ParseNumeric( Tokenizer *tokenizer, Token *token )
{
	//TODO: incomplete giant mess - finish it :)
	bool8 result = TRUE;
	NumericType type = NT_DECIMAL;
	bool8 zero_start = (tokenizer->pos[0]=='0')?TRUE:FALSE;
	bool8 float_ = FALSE;
	bool8 floating_point = FALSE;

	token->type = TK_NUMERIC_INTEGER;
	TkzAdvance( tokenizer );
	int length = 1;

	if( zero_start )
	{
		switch( tokenizer->pos[0] )
		{
			case 'B': case 'b': type = NT_BINARY; break;
			case 'H': case 'h': 
			case 'X': case 'x': type = NT_HEXADECIMAL; break;
			case 'O': case 'o': type = NT_OCTAL; break;
			default:
			{
				if( !IsDigit( tokenizer->pos[0] ) )
				{
					Error( tokenizer->file_name, token->line, token->col+1, "invalid qualifier for numeric type" );
					result = FALSE;
				}
			} break;
		}
		if( type!=NT_DECIMAL )
		{
			TkzAdvance( tokenizer );
			length = 0;
		}
	}

	while( tokenizer->pos[0] )
	{
		if( type==NT_DECIMAL )
		{
			if( tokenizer->pos[0]=='f' || tokenizer->pos[0]=='F' )
			{
				float_ = TRUE;
				TkzAdvance( tokenizer );
				break;
			}
			else
			if( tokenizer->pos[0]=='.' )
			{
				if( !floating_point )
				{
					floating_point = TRUE;
				} else {
					if( result )
					{
						Error( tokenizer->file_name, token->line, token->col, "invalid numeric value" );
						result = FALSE;
					}
				}
			}
			else if( !IsDigit( tokenizer->pos[0] ) ) break;
		}
		else if( type==NT_BINARY )
		{
			if( !(tokenizer->pos[0]=='0' || tokenizer->pos[0]=='1') ) break;
		}
		else if( type==NT_HEXADECIMAL )
		{
			if( !IsHex( tokenizer->pos[0] ) ) break;
		} else if( type==NT_OCTAL )
		{
			if( !IsOctal( tokenizer->pos[0] ) ) break;
		}		
		TkzAdvance( tokenizer );
		length++;
	}
	token->length = length;

	if( IsAlpha( tokenizer->pos[0] ) || IsDigit( tokenizer->pos[0] ) || tokenizer->pos[0]=='_' ) 
	{
		while( IsAlpha( tokenizer->pos[0] ) || IsDigit( tokenizer->pos[0] ) || tokenizer->pos[0]=='_' )
		{
			TkzAdvance( tokenizer );
			length++;
		}
		if( result )
		{
			Error( tokenizer->file_name, token->line, token->col, "invalid numeric value" );
			result = FALSE;
		}
	}

	if( result )
	{
		switch( type )
		{
			case NT_DECIMAL:
			{
				if( floating_point || float_ )
				{
					token->type = TK_NUMERIC_FLOATING;
					token->numeric_floating = atof( "" );
					//TODO:
				} else {
					token->numeric_integer = atoll("");
					if( errno==EINVAL || errno==ERANGE )
					{
						//TODO:
					}
				}
			}
		}
	}

	return result;
}

//GetToken
static Token GetToken( Tokenizer *tokenizer )
{
	EatWhiteSpaces( tokenizer );

	Token token;
	token.type = TK_UNKNOWN;
	token.text = tokenizer->pos;
	token.length = 1;
	token.line = tokenizer->line;
	token.col = tokenizer->col;

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
		case '!': token.type = TK_EXCLAMATION; break;

		//TODO: single quotes?
		case '\'': break;

		//strings
		case '"': ParseString( tokenizer, &token ); break;

		default:
		{
			//identifier
			if( IsAlpha( tokenizer->pos[0] ) )
			{
				token.type = TK_IDENTIFIER;
				TkzAdvance( tokenizer );
				int length = 1;
				while( tokenizer->pos[0] )
				{
					if( !( IsAlpha(tokenizer->pos[0]) || IsDigit(tokenizer->pos[0]) || tokenizer->pos[0]=='_' || tokenizer->pos[0]=='.' ) ) 
					{
						tokenizer->pos--;
						tokenizer->col--;
						break;
					}
					TkzAdvance( tokenizer );
					length++;
				}
				token.length = length;
				//check if it's a keyword
				if( IsKeyword( token.text, token.length ) ) token.type = TK_KEYWORD;
				else if( IsBasicType( token.text, token.length ) ) token.type = TK_BASIC_TYPE;
			}
			//number?
			else if( IsDigit( tokenizer->pos[0] ) )
			{
				ParseNumeric( tokenizer, &token );
			}
		} break;
	}
	if( tokenizer->pos[0] )	TkzAdvance( tokenizer );

	return token;
}

//Peek
static Token Peek( Tokenizer *tokenizer )
{
	Tokenizer previous_state = *tokenizer;
	Token token = GetToken( tokenizer );
	*tokenizer = previous_state;
	return token;
}

//Expect
static bool8 Expect( Tokenizer *tokenizer, byte token_type )
{
	Tokenizer previous_state = *tokenizer;
	Token token = GetToken( tokenizer );
	if( token.type!=token_type )
	{
		Error( tokenizer->file_name, token.line, token.col, "Expected \"%s\", found \"%.*s\".", Token_Type_Names[token_type], token.length, token.text ); 
		*tokenizer = previous_state;
		return FALSE;
	}
	return TRUE;
}

//EOF
