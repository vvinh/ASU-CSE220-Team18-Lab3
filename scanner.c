/********************************************
 *
 * CSE220 LS 12166 Lab3 - Team18
 * Nicholas Murray
 * Vivian Vinh
 * Timothy Zamora
 *
 **********************************************/
#include <stdio.h>
#include <ctype.h>
#include "scanner.h"
#include <string.h>

const char* const SYMBOL_STRINGS2[] = { "<no token>", "<IDENTIFIER>",
		"<NUMBER>", "<STRING>", "^", "*", "(", ")", "-", "+", "=", "[", "]",
		":", ";", "<", ">", ",", ".", "/", ":=", "<=", ">=", "<>", "..",
		"<END OF FILE>", "<ERROR>", "AND", "ARRAY", "BEGIN", "CASE", "CONST",
		"DIV", "DO", "DOWNTO", "ELSE", "END", "FILE", "FOR", "FUNCTION", "GOTO",
		"IF", "IN", "LABEL", "MOD", "NIL", "NOT", "OF", "OR", "PACKED",
		"PROCEDURE", "PROGRAM", "RECORD", "REPEAT", "SET", "THEN", "TO", "TYPE",
		"UNTIL", "VAR", "WHILE", "WITH", };

/*******************
 Static functions needed for the scanner
 You need to design the proper parameter list and 
 return types for functions with ???.
 ******************/
static char get_char(char token_string[]);
static size_t skip_comment(char source_buffer[], size_t j);
static size_t skip_blanks(char source_buffer[], size_t j);
static Token *get_word(char token_string[], Token *token2);
static Token *get_number(char token_string[], Token * token2);
static Token *get_string(char token_string[], Token *token2);
static Token *get_special(char token_string[], Token *token2);
static char * downshift_word(char token_string[]);
static BOOLEAN is_reserved_word(char token_string[], Token *token2);
static size_t skipToNonBank_FromNonBlank(char source_buffer[], size_t currIndex);
static size_t buildToken(char source_buffer[], char token_string[], size_t j);
static size_t skipToBlank_fromNonBlank(char source_buffer[], size_t j);
static size_t skipToNonBlank_FromBlank(char source_buffer[], size_t j);
static void strToUpper(char source[], char target[]);
static size_t getNextTokenFirstIndexForNumber(char source_buffer[],
		size_t index);

typedef enum {
	LETTER, DIGIT, QUOTE, SPECIAL, EOF_CODE,
} CharCode;

/*********************
 Static Variables for Scanner
 Must be initialized in the init_scanner function.
 *********************/
static FILE *src_file;
static char src_name[MAX_FILE_NAME_LENGTH];
static char todays_date[DATE_STRING_LENGTH];
static CharCode char_table[256];

typedef struct {
	char *string;
	TokenCode token_code;
} RwStruct;

const RwStruct rw_table[9][10] = { { { "do", DO }, { "if", IF }, { "in", IN }, {
		"of", OF }, { "or", OR }, { "to", TO }, { NULL, 0 } }, /*/Reserved words of size 2 */
		{ { "and", AND }, { "div", DIV }, { "end", END }, { "for", FOR }, {
				"mod", MOD }, { "nil", NIL }, { "not", NOT }, { "set", SET }, {
				"var", VAR }, { NULL, 0 } }, /*/Reserved words of size 3 */
		{ { "case", CASE }, { "else", ELSE }, { "file", FFILE },
				{ "goto", GOTO }, { "then", THEN }, { "type", TYPE }, { "with",
						WITH }, { NULL, 0 } }, /*/Reserved words of size 4 */
		{ { "array", ARRAY }, { "begin", BEGIN }, { "const", CONST }, { "label",
				LABEL }, { "until", UNTIL }, { "while", WHILE }, { NULL, 0 } }, /*/Reserved words of size 5 */
		{ { "downto", DOWNTO }, { "packed", PACKED }, { "record", RECORD }, {
				"repeat", REPEAT }, { NULL, 0 } },  /*/ Reserved words of size 6 */
		{ { "program", PROGRAM }, { NULL, 0 } }, /*/ Reserved words of size 7 */
		{ { "function", FUNCTION }, { NULL, 0 } }, /*/ Reserved words of size 8 */
		{ { "procedure", PROCEDURE }, { NULL, 0 } }  /*/ Reserved words of size 9 */
};

void init_scanner(FILE *source_file, char source_name[], char date[]) {
	src_file = source_file;
	strcpy(src_name, source_name);
	strcpy(todays_date, date);

	/*******************
	 initialize character table, this table is useful for identifying what type of character
	 we are looking at by setting our array up to be a copy the ascii table.  Since C thinks of
	 a char as like an int you can use ch in get_token as an index into the table.
	 *******************/
}

BOOLEAN get_source_line(char source_buffer[]) {
	char print_buffer[MAX_SOURCE_LINE_LENGTH + 9];

	static int line_number = 0;

	if (fgets(source_buffer, MAX_SOURCE_LINE_LENGTH, src_file) != NULL) {
		++line_number;

		sprintf(print_buffer, "%4d: %s", line_number, source_buffer);

		print_line(print_buffer, src_name, todays_date);

		return (TRUE);
	} else {
		return (FALSE);
	}
}

Token * get_token() {

	char ch; /*/This can be the current character you are examining during scanning. */
	char token_string[MAX_TOKEN_STRING_LENGTH]; /*Store your token here as you build it. */

	Token *token2 = malloc(sizeof(Token));



	ch = get_char(token_string);
	char comp = '\'';

	/*/1.  Skip past all of the blanks */
	/*/2.  figure out which case you are dealing with LETTER, DIGIT, QUOTE, EOF, or special, by examining ch */
	/*/decides to get_word()(if it is a letter), get_number()(if it is a number), get_string()(if it is a quote ‘), if it is the end of the file, */
	/*/or else it must have a special so it can get_special(). */

	/*/ isalpha does not work with lower case */
	if (isalpha(ch)!=0) {

		token2 = get_word(token_string, token2);  /*/(if it is a LETTER) */
	} else if (isdigit(ch) ) {

		token2 = get_number(token_string, token2); /*/(if it is a DIGIT) */

	} else if (ch == comp) {

		token2 = get_string(token_string, token2); /*/(if it is a quote ‘) */

	} else if (ch == EOF) {

		return NULL;

	} else if (ch == '\n') {

		token2->type = STRING_LIT;
		token2->nextptr = NULL;

		strcpy(token2->token_string, " "); /*/copy source_buffer into tmp_buffer */

		token2->code = NO_TOKEN;

	} else {

		token2 = get_special(token_string, token2);   /*/ Special character */
	}

	return token2;
}

static Token *get_special(char token_string[], Token *token2) {
	/*
	 Write some code to Extract the special token.  Most are single-character
	 some are double-character.  Set the token appropriately.
	 */
	size_t i;

	token2->type = STRING_LIT;
	token2->nextptr = NULL;

	/*/The fifth through the twenty fifth elements are all the special characters get_special() looks for */
	for (i = 4; i < 25; ++i) {

		if (strcmp(SYMBOL_STRINGS2[i], &token_string[0]) == 0) {
			strcpy(token2->token_string, token_string); /*copy source_buffer into tmp_buffer */
			token2->code = (TokenCode) i;

			break;

		}

	}

	return token2;

}

static BOOLEAN is_reserved_word(char token_string[], Token *token) {
	/*
	 Examine the reserved word table and determine if the function input is a reserved word.
	 */

	size_t i; /* row counter       9 */

	char tmp[MAX_SOURCE_LINE_LENGTH];
	strToUpper(token_string, tmp);

	for (i = 0; i < 61; ++i) {

		if (strcmp(SYMBOL_STRINGS2[i], &tmp[0]) == 0) {

			token->code = (TokenCode) i;

			return TRUE;

		}

	}

	token->code = IDENTIFIER; /*/ if not a reserve word then identifier */
	return FALSE;

}

static void strToUpper(char source[], char target[]) {

	size_t i;

	for (i = 0; i < MAX_TOKEN_STRING_LENGTH && source[i] != '\0'; ++i) {
		target[i] = toupper(source[i]);
	}
	target[i] = '\0';



}

static Token *get_word(char token_string[], Token *token2) {
	/*
	 Write some code to Extract the word

	 Downshift the word, to make it lower case

	 Write some code to Check if the word is a reserved word.
	 if it is not a reserved word its an identifier.
	 */

	downshift_word(token_string);

	token2->nextptr = NULL;
	token2->type = STRING_LIT;

	strcpy(token2->token_string, token_string);

	/*
	 >> Write some code to Check if the word is a reserved word.  if it is not a reserved word its an identifier
	 >> use the arrays provided in the source code I gave out to find out if something is a reserved word or not. If not then this token is an identifier
	 */

	is_reserved_word(token_string, token2);

	return token2;
}

static Token *get_string(char token_string[], Token *token2) {
	/*
	 Write some code to Extract the string
	 */

	token2->code = STRING;
	token2->type = STRING_LIT;
	token2->nextptr = NULL;

	strcpy(token2->token_string, token_string);

	return token2;
}

static char get_char(char token_string[]) {
	/*
	 If at the end of the current line (how do you check for that?),
	 we should call get source line.  If at the EOF (end of file) we should
	 set the character ch to EOF and leave the function.

	 Write some code to set the character ch to the next character in the buffer

	 checks the current state of the source_buffer and
	 gets a new line of code if it is at the end of a line.
	 If it sees a Pascal Comment it skips the comment (a pascal comment is anything between ‘{‘ and ‘}’).
	 */

	static char source_buffer[MAX_SOURCE_LINE_LENGTH];
	static size_t i = 0;
	size_t nextIndex = 0;

	char ch;

	if ((source_buffer[i] == '\0') || (source_buffer[i] == '\n')) {
		i = 0;

		BOOLEAN ret = get_source_line(source_buffer);

		if (ret == FALSE) {
			ch = EOF;
			return ch;
		}

		else {

			i = skip_blanks(source_buffer, i);


			i = skip_comment(source_buffer, i);

			ch = source_buffer[i];

		}

	} else {
		ch = source_buffer[i];

	}

	nextIndex = buildToken(source_buffer, token_string, i);

	i = nextIndex;

	return ch;

}

static size_t getNextTokenFirstIndexForNumber(char source_buffer[],
		size_t index) {

	size_t i;
	char tmp = 'E';
	char tmp2;
	char end = ';';

	for (i = index; i < MAX_SOURCE_LINE_LENGTH; ++i) {

		/*/skip until blank is found */
		if (isblank(source_buffer[i])) {

			i = skipToNonBlank_FromBlank(source_buffer, i);
			return i;

		}

		/*/if is a letter and not e */
		if (isalpha(source_buffer[i])) {

			tmp2 = toupper(source_buffer[i]);

			if (strcmp(&tmp2, &tmp)) {
				return i;
			}

		}

		/*/if not leter but one of math operation */
		if (!isalpha(source_buffer[i])) {

			if (strcmp(&source_buffer[i], &end)) {
				return i;
			}

		}

	}

	return index;

}

static size_t getCurTokenLastIndexForNumber(char source_buffer[],
		size_t currIndex) {

	size_t i;

	/*/ skip to next non blank */
	for (i = currIndex; i < MAX_SOURCE_LINE_LENGTH; ++i) {

		/*/ return if it is a blank*/
		if (isblank(source_buffer[i])) {
			currIndex = i - 1;
			break;
		}

		/*/return if end of line */
		if (source_buffer[i] == ';') {
			currIndex = i - 1;
			break;
		}

		/*/return if ':' */
		if (source_buffer[i] == ':') {
			currIndex = i - 1;
			break;
		}

		/*/return if ',' */
		if (source_buffer[i] == ',') {
			currIndex = i - 1;
			break;
		}

		if (source_buffer[i] == ')') {
			currIndex = i - 1;
			break;
		}

		if (source_buffer[i] == '*') {
			currIndex = i - 1;
			break;
		}


	}

	return currIndex;

}

static size_t getIndexBySymbol(char source_buffer[], size_t index, char sym) {

	size_t i;

	/*/ skip to next non blank */
	for (i = index + 1; i < MAX_SOURCE_LINE_LENGTH; ++i) {
		/*/ skip until non blank is found */
		if ((source_buffer[i - 1] != '\\') && (source_buffer[i] == sym)) {
			return i;

		}

	}

	return index;

}

static size_t getNextTokenFirstIndexForString(char source_buffer[],
		size_t index) {

	size_t i;

	for (i = index + 1; i < MAX_SOURCE_LINE_LENGTH; ++i) {

		/*/skip until blank is found */
		if (isblank(source_buffer[i])) {

			i = skipToNonBlank_FromBlank(source_buffer, i);
			return i;

		}

		if (!isblank(source_buffer[i])) {


			return i;

		}

	}

	return index;

}

static size_t buildToken(char source_buffer[], char token_string[], size_t j) {

	size_t i, n, lastIndex;
	n = 0;

	size_t k, nextIndex;

	if (source_buffer[j] == ':') {

			lastIndex = getIndexBySymbol(source_buffer, j, '=');

			for (i = j ; i <= lastIndex; ++i) {

				token_string[n] = source_buffer[i];
				n++;

			}
			token_string[n] = '\0';

			nextIndex = getNextTokenFirstIndexForString(source_buffer, lastIndex);

		} else

	/*/ if string */
	if (source_buffer[j] == '\'') {

		lastIndex = getIndexBySymbol(source_buffer, j, '\'');

		for (i = j + 1; i < lastIndex; ++i) {

			token_string[n] = source_buffer[i];
			n++;

		}
		token_string[n] = '\0';

		nextIndex = getNextTokenFirstIndexForString(source_buffer, lastIndex);

	} else
	/*if it is a number */
	if (isdigit(source_buffer[j])) {
		lastIndex = getCurTokenLastIndexForNumber(source_buffer, j);

		for (i = j; i <= lastIndex; ++i) {

			token_string[n] = source_buffer[i];
			n++;

		}
		token_string[n] = '\0';

		nextIndex = getNextTokenFirstIndexForNumber(source_buffer,
				lastIndex + 1);

	} else

	/*if not alpha - that means special character */
	if ((isalpha(toupper(source_buffer[j])) == 0)) {	/*1=true */

		token_string[n] = source_buffer[j];
		token_string[++n] = '\0';
		/*return token_string; */

		nextIndex = skipToNonBank_FromNonBlank(source_buffer, j);

	} else {

		k = skipToBlank_fromNonBlank(source_buffer, j);

		/* skip to next blank */
		for (i = j; i < k; ++i) {

			token_string[n] = source_buffer[i];
			n++;

		}
		token_string[n] = '\0';

		nextIndex = skipToBlank_fromNonBlank(source_buffer, j);

		nextIndex = skipToNonBlank_FromBlank(source_buffer, nextIndex);

	}

	return nextIndex;

}

static size_t skipToNonBlank_FromBlank(char source_buffer[], size_t j) {

	size_t i;

	/* skip to next non blank */
	for (i = j; i < MAX_SOURCE_LINE_LENGTH; ++i) {
		/* skip until non blank is found */
		if (isblank(source_buffer[i]) == 0) {

			return i;
			/*break; */
		}

	}

	return j;

}

static size_t skipToBlank_fromNonBlank(char source_buffer[], size_t j) {

	size_t i;
	/* skip to next blank */
	for (i = j; i < MAX_SOURCE_LINE_LENGTH; ++i) {
		/*return if not a letter */
		if (isalpha(source_buffer[i]) == 0) {

			return i;

		}

		/*skip until blank is found */
		if (isblank(source_buffer[i]) != 0) {

			return i;

		}

		if (source_buffer[i] == '\n') {

					return i;

				}


	}

	return j;

}

static size_t skipToNonBank_FromNonBlank(char source_buffer[], size_t currIndex) {

	size_t i;

	/* skip to next non blank */
	for (i = currIndex + 1; i < MAX_SOURCE_LINE_LENGTH; ++i) {
		/* skip until non blank is found */
		if (isblank(source_buffer[i]) == 0) {
			currIndex = i;
			break;
		}

	}

	return currIndex;

}

static size_t skip_blanks(char source_buffer[], size_t j) {
	/*
	 Write some code to skip past the blanks in the program and return a pointer
	 to the first non blank character
	 */

	size_t i;

	for (i = j; i < MAX_SOURCE_LINE_LENGTH; ++i) {

		if (isblank(source_buffer[i]) == 0) {
			return i; /* if not blank then return the address of non-blank */
		}

	}
	return j;

}

static size_t skip_comment(char source_buffer[], size_t j) {
	/*
	 Write some code to skip past the comments in the program and return a pointer
	 to the first non blank character.  Watch out for the EOF character.
	 */

	size_t i;

	char *s1 = "{";
	char *s2 = "}";

	char t1 = source_buffer[j];
	char t2;

	if (strncmp(&t1, s1, 1) == 0) {
		for (i = j; i < MAX_SOURCE_LINE_LENGTH; ++i) {
			t2 = source_buffer[i];
			if (strncmp(&t2, s2, 1) == 0) {
				return i + 1;
			}
		}
	}

	return j;
}

static char * downshift_word(char token_string[]) {
	/*
	 Make all of the characters in the incoming word lower case.
	 */

	size_t i; /* counter */
	char s1[MAX_TOKEN_STRING_LENGTH];

	/* extract the source file name */
	for (i = 0; i < MAX_TOKEN_STRING_LENGTH && token_string[i] != '\0'; ++i) {
		s1[i] = tolower(token_string[i]);
	} /* end for */
	s1[i] = '\0';

	strcpy(token_string, s1);

	return token_string;
}

static Token *get_number(char token_string[], Token * token2) {
	/*
	 Write some code to Extract the number and convert it to a literal number.
	 */
	char *stringPtr; /* create char pointer */
	strtod(token_string, &stringPtr);

	token2->code = NUMBER;
	token2->type = INTEGER_LIT;
	token2->nextptr = NULL;

	strcpy(token2->token_string, token_string); /*copy source_buffer into tmp_buffer */
	return token2;

}

