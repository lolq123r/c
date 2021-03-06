/*	5-19: Modify undcl so that it doesn't add redundant parentheses to declarations.

	After looking at a few examples it looks like we only need to add extra parentheses when
	we have a pointer to an array or to a function. That means we have to add parentheses
	only when the last token was a pointer. This is pretty easy to add.
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define	MAXTOKEN 100
#define BUFSIZE 100

enum {NAME, PARENS, BRACKETS};

void dcl(void);
void dirdcl(void);

int gettoken(void);
int tokentype;		// type of last token
char token[MAXTOKEN];	// last token string
char name[MAXTOKEN];	// identifier name
char datatype[MAXTOKEN];// datatype = char, int, etc.
char out[1000];		// output string

char buf[BUFSIZE];	//buffer for ungetch
int bufp;		//next free position in buf

//undcl: convert word descriptions to declarations
int main(void)
{
	int type, ltype = 0;
	char temp[MAXTOKEN];
	
	while(gettoken() != EOF) {
		strcpy(out, token);
		while ((type = gettoken()) != '\n')
			if (type == PARENS || type == BRACKETS){
				if(ltype == '*'){
					sprintf(temp, "(%s)", out);
					strcpy(out, temp);
					strcat(out, token);
				}
				else
					strcat (out, token);
				ltype = type;
			}
			else if (type == '*') {
				sprintf(temp, "*%s", out);
				strcpy(out, temp);
				ltype = type;				
			} else if (type == NAME) {
				sprintf(temp, "%s %s", token, out);
				strcpy(out, temp);
				ltype = type;
			} else
				printf("invalid input at %s\n", token);
		printf("%s\n", out);
	}
	return 0;
}
	
void dcl(void)
{
	int ns;
	
	for (ns = 0; gettoken() == '*';)	//count *'s
		ns++;
	dirdcl();
	while (ns-- > 0)
		strcat(out, " pointer to");
}

//dirdcl: parse a direct declarator
void dirdcl(void)
{
	int type;
	
	if (tokentype == '(') {	//dcl
		dcl();
		if (tokentype != ')')
			printf("error: missing )\n");
	} else if (tokentype == NAME)	//variable name
		strcpy(name, token);
	else
		printf("error: expected name or (dcl)\n");
	while ((type = gettoken()) == PARENS || type == BRACKETS){
		if (type == PARENS)
			strcat(out, " function returning");
		else {
			strcat(out, " array");
			strcat(out, token);
			strcat(out, " of");
		}
	}
}

int gettoken(void)	//return next token
{
	int c, getch(void);
	void ungetch(int);
	char *p = token;
	
	while ((c = getch()) == ' ' || c == '\t')
		;
	if (c == '(') {
		if ((c = getch()) == ')') {
			strcpy(token, "()");
			return tokentype = PARENS;
		} else {
			ungetch(c);
			return tokentype = '(';
		}
	} else if (c == '[') {
		for (*p++ = c; (*p++ = getch()) != ']';)
			;
		*p = '\0';
		return tokentype = BRACKETS;
	} else if (isalpha(c)) {
		for (*p++ = c; isalnum(c = getch()); )
			*p++ = c;
		*p = '\0';
		ungetch(c);
		return tokentype = NAME;
	} else
		return tokentype = c;
}

int getch(void)
{
	return (bufp > 0) ? buf[--bufp] : getchar();
}

void ungetch(int c) // push character back to input
{
	if (bufp >= BUFSIZE)
		printf("ungetch: too many characters\n");
	else
		buf[bufp++] = c;
}
