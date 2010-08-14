

#if defined _string_included
#endinput
#endif

#define _string_included


native string_length(const string[]);


stock bool: string_is_packed(const string[])
{ return !(0 <= string[0] <= ucharmax); }


native string_compare(const string1[], const string2[], bool: ignorecase=false, length=cellmax);


native string_concat(dest[], const source[], length = sizeof dest);


native string_copy(dest[], const source[], length = sizeof dest);


native string_printf(dest[], size=sizeof dest, bool: pack=false, const format[], ...);


native string_tokenize(const string[], const delimiters[], ...);


native itoa(number, base, buffer[], size = sizeof buffer);


native atoi(const buffer[]);
