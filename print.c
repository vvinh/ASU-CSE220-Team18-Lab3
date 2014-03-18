/********************************************
 *
 * CSE220 LS 12166 Lab3 - Team18
 * Nicholas Murray
 * Vivian Vinh
 * Timothy Zamora
 *
 **********************************************/

#include "print.h"

static char src_name[MAX_FILE_NAME_LENGTH];
static char todays_date[DATE_STRING_LENGTH];

const char* const SYMBOL_STRINGS[] = { "<no token>", "<IDENTIFIER>", "<NUMBER>",
		"<STRING>", "^", "*", "(", ")", "-", "+", "=", "[", "]", ":", ";", "<",
		">", ",", ".", "/", ":=", "<=", ">=", "<>", "..", "<END OF FILE>",
		"<ERROR>", "AND", "ARRAY", "BEGIN", "CASE", "CONST", "DIV", "DO",
		"DOWNTO", "ELSE", "END", "FILE", "FOR", "FUNCTION", "GOTO", "IF", "IN",
		"LABEL", "MOD", "NIL", "NOT", "OF", "OR", "PACKED", "PROCEDURE",
		"PROGRAM", "RECORD", "REPEAT", "SET", "THEN", "TO", "TYPE", "UNTIL",
		"VAR", "WHILE", "WITH", };

static void print_page_header(char source_name[], char date[]);

void print_line(char line[], char source_name_to_print[], char date_to_print[]) {
	char save_ch;
	char *save_chp = NULL;
	static int line_count = MAX_LINES_PER_PAGE;

	if (++line_count > MAX_LINES_PER_PAGE) {
		print_page_header(source_name_to_print, date_to_print);
		line_count = 1;
	}
	if (strlen(line) > MAX_PRINT_LINE_LENGTH) {
		save_chp = &line[MAX_PRINT_LINE_LENGTH];
	}
	if (save_chp) {
		save_ch = *save_chp;
		*save_chp = '\0';
	}
	printf("%s", line);
	fflush(stdout);
	if (save_chp) {
		*save_chp = save_ch;
	}
}

static void print_page_header(char source_name[], char date[]) {
	static int page_number = 0;

	putchar(FORM_FEED_CHAR);
	printf("Page    %d  %s  %s\n\n", ++page_number, source_name, date);
}

void print_token(Token *token) {

	/*/`Missing code goes here */

	char print_buffer[MAX_SOURCE_LINE_LENGTH + 9];






	if (token->type == INTEGER_LIT) {

		sprintf(print_buffer, ">> %s\t    %s\n", SYMBOL_STRINGS[token->code],
				token->token_string);
		print_line(print_buffer, src_name, todays_date);

	} else if (token->type == REAL_LIT) {


		sprintf(print_buffer, ">> %s\t    %s\n", SYMBOL_STRINGS[token->code],
				token->token_string);
		print_line(print_buffer, src_name, todays_date);

	} else if (token->code == NO_TOKEN) {

		sprintf(print_buffer, "%s\t    %s\n", "", token->token_string);
		print_line(print_buffer, src_name, todays_date);

	} else {

		sprintf(print_buffer, ">> %s\t    %s\n",
				SYMBOL_STRINGS[token->code], token->token_string);
		print_line(print_buffer, src_name, todays_date);

	}

}

void init_scanner2(char source_name[], char date[]) {

	strcpy(src_name, source_name);
	strcpy(todays_date, date);

	/*******************
	 initialize character table, this table is useful for identifying what type of character
	 we are looking at by setting our array up to be a copy the ascii table.  Since C thinks of
	 a char as like an int you can use ch in get_token as an index into the table.
	 *******************/
}
