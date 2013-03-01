#ifndef __PARSE_UTILS_H_
#define __PARSE_UTILS_H_

/* 
  author: dr. srinidhi varadarajan
  copyright (C) 2005 

  this file contains a thread-safe string tokenization function that can be
  used for general purpose parsing. 
*/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>


/* parses the token in_str delimited by the characters in delim. it returns
   an output string out_str, which contains the remaining elements of
   the string after the first token. this can be used to parse all tokens in
   a line by recursively calling the parse function.
   
   it also returns the actual delimiting character in delim_char, which can 
   be used to define numerical ranges. if this is not desired, delim_char may be 
   set to NULL.

   it returns the first token delimited by characters in delim or NULL, 
   if no such token is found.

   this is a thread safe implementation 
*/
static inline char *parse_token(char *in_str, char *delim, char **out_str, char *delim_char)
{
  int len;
  char *ptr, *tptr, *token;

  /* Bypass leading whitespace delimiters */
  len = strspn(in_str, delim);
  ptr = (in_str + len);
  
  /* Get end of token */
  tptr = strpbrk(ptr, delim);
  if (tptr == NULL) return(NULL);
  len = tptr - ptr;

  if (delim_char != NULL) *delim_char = *tptr;
  
  /* Create output string */
  *out_str = tptr + 1; /* go past the delimiter */
  
  /* Create token */
  token = (char *) malloc(len + 1);
  if (token == NULL) return(NULL);
  memcpy(token, ptr, len);
  token[len] = (char) 0;
  return(token);
}


#endif 
