/*
	Jest Language

	Toy language that compiles to C.

	by David G. Maziero
	06/03/2016
*/

enum TokenType
{
	TK_IDENTIFIER,
	TK_OPEN_PAREN,
	TK_CLOSE_PAREN,
	TK_OPEN_BRACE,
	TK_CLOSE_BRACE,
	TK_STRING,
	TK_INTEGER,
	TK_FLOATING,
	TK_END_OF_STREAM
};

struct Tokenizer
{
	char *pos;
};

struct Token
{
	byte type;	
	int length;
	char *text;
};

//EOF
