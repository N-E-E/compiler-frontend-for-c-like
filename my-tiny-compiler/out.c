#include <stdio.h> 
#define TEST 1 
int a ; 
long b ; 
char _d , _e ; 
double c , h ; 
int num[10] ; 
int i ; 
int func ( int a , int b ) { 
	int test ; 
	a = 1 ; 
	b = 1 ; 
	c = .14 ; 
	h = 3.13f ; 
	_d = '\x22' ; 
	_e = '\t' ; 
	a = 1 + b ; 
	b = 2 + b ; 
	b = c ; 
	if ( a == b ) a = a + b ; 
	else if ( a < b ) { 
		a = a - b ; 
		num[10] = "string" ; 
		if ( a > b ) { 
			a = a + 1 ; 
		} 
	} 
	else { 
		a = a / b ; 
		a = a + 1 ; 
	} 
	if ( a > b ) a = a && b ; 
	if ( a > b ) { 
		a = ( a + 1 ) * 3 ; 
		b = b + c + 2 ; 
	} 
	while ( 1 ) { 
		if ( a > b ) { 
			a = ( a + 1 ) * 3 ; 
			b = b + c + 2 ; 
			break ; 
		} 
		if ( a < b ) { 
			continue ; 
		} 
		while ( a == 1 ) { 
			a = a || b ; 
		} 
	} 
	for ( i = 0 ; i < a ; i = i + 1 ) { 
		a = a + 1 ; 
	} 
	for ( ; ; i = i - 1 ) { 
		a = ( a + 1 ) * 2 ; 
	} 
	return a + b ; 
} 
void def ( ) ; 
