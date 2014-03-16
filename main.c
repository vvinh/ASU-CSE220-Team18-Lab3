/********************************************
 *
 * CSE220 LS 12166 Lab3 - Team18
 * Nicholas Murray
 * Vivian Vinh
 * Timothy Zamora
 *
 **********************************************/

#include <stdio.h>
#include "common.h"
#include "print.h"
#include "scanner.h"
#include <ctype.h>

FILE *init_lister(const char *name, char source_file_name[], char dte[]);
void quit_scanner(FILE *src_file, Token *list);
void add_token_to_list(Token *list, Token *new_token);

int main(int argc, const char * argv[]) {
	Token *token;
	Token *token_list; //This needs to be implemented as a linked list in scanner.h.
	char source_name[MAX_FILE_NAME_LENGTH];
	char date[DATE_STRING_LENGTH];

	FILE *source_file = init_lister(argv[1], source_name, date);
	init_scanner(source_file, source_name, date);
	init_scanner2(source_name, date);
	token_list = malloc(sizeof(Token));
	token_list->nextptr = NULL;
	do {

		token = get_token();
		add_token_to_list(token_list, token);
		print_token(token);
	} while (token->token_string[0] != '.'); //??? What is the sentinal value that ends this loop?

	quit_scanner(source_file, token_list);
	return 0;
}

void add_token_to_list(Token *list, Token *new_token) {
	// Add new_token to the list knowing that list is a linked list.

	if (list->nextptr == NULL) {

		list->nextptr = new_token;
	} else {

		new_token->nextptr = list->nextptr;
		list->nextptr = new_token;

	}

}

void quit_scanner(FILE *src_file, Token *list) {
	//write code to free all of the memory for the token list
	fclose(src_file);
	Token *delPtr;

	while (list != NULL) {
		delPtr = list;
		list = list->nextptr;
		free(delPtr);
	} // end while
}

FILE *init_lister(const char *name, char source_file_name[], char dte[]) {

	time_t timer;
	FILE *file;
	strcpy(source_file_name, name);
	file = fopen(source_file_name, "r");
	time(&timer);
	strcpy(dte, asctime(localtime(&timer)));
	return file;

}

