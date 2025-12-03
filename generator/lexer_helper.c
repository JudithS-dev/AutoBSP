#include "lexer_helper.h"

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// TODO: keep this list in sync with the keywords defined in the lexer (lexerDefinition.l)
/**
 * @brief list of all keywords recognized by the lexer
 * @note used for finding closest matching keyword
 * @note must be kept in sync with the keywords defined in the lexer
 */
static const char *lexer_keywords[] = {
  "AutoBSP",
  "Controller",
  "OUTPUT",
  "INPUT",
  "name",
  "pin",
  "type",
  "pull",
  "speed",
  "init",
  "active",
  "enable",
  "true",
  "false",
  "high",
  "low",
  "STM32F446RE",
  "pushpull",
  "opendrain",
  "up",
  "down",
  "none",
  "medium",
  "very_high",
  "on",
  "off"
};

// number of keywords in the lexer_keywords array
static const int lexer_keywords_count = sizeof(lexer_keywords) / sizeof(lexer_keywords[0]);

/**
 * @brief returns the minimum of three integer values
 * 
 * @param a first integer
 * @param b second integer
 * @param c third integer
 * @return int minimum value
 */
int min_3_values(int a, int b, int c){
  if(a < b)
    return (a < c) ? a : c;
  else
    return (b < c) ? b : c;
}

/**
 * @brief calulates the case insensitive levenshtein distance between two strings (s1 and s2)
 * 
 * @param s1 first string
 * @param s2 second string
 * @return unsigned int levenshtein distance
 * 
 * @note source: https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
 * 
 * The levenshtein distance is a measure of the difference between two strings.
 * It is defined as the minimum number of single-character edits (insertions, deletions or substitutions)
 * required to change one string (s1) into the other (s2).
 * It has the following properties:
 * - Non-negative, i.e., distance(s1, s2) >= 0
 * - Identity, i.e., distance(s1, s2) == 0 if and only if s1 == s2
 * - Symmetric, i.e., distance(s1, s2) == distance(s2, s1)
 */
unsigned int levenshtein_distance(const char *s1, const char *s2){
  unsigned int len1, len2;
  len1 = strlen(s1);
  len2 = strlen(s2);
  
  unsigned int matrix[len2 + 1][len1 + 1];
  unsigned int i, j;
  for(i = 0; i <= len2; i++) matrix[i][0] = i;
  for(j = 0; j <= len1; j++) matrix[0][j] = j;
  
  for(i = 1; i <= len2; i++){
    for(j = 1; j <= len1; j++){
      // check if characters are the same
      char c1 = (char)tolower(s1[j - 1]);
      char c2 = (char)tolower(s2[i - 1]);
      unsigned int cost = (c1 == c2) ? 0 : 1;
      // calculate next matrix cell value
      matrix[i][j] = min_3_values(
        matrix[i - 1][j] + 1,
        matrix[i][j - 1] + 1,
        matrix[i - 1][j - 1] + cost
      );
    }
  }
  return matrix[len2][len1];
}

/**
 * @brief prints the closest matching keyword(s) to the provided word
 * 
 * @param word input string to compare against keywords
 * @note only suggests keywords if the distance is below a dynamic threshold (floor(strlen(word) / 2))
 * @note uses the levenshtein distance to find the closest match
 */
void print_closest_keywords(const char* word){
  unsigned int keyword_distances[lexer_keywords_count];
  unsigned int cur_best_distance = UINT_MAX;
  
  // iterate through all keywords and calculate their distance to the input word
  for(int i = 0; i < lexer_keywords_count; i++){
    keyword_distances[i] = levenshtein_distance(word, lexer_keywords[i]);
    if(keyword_distances[i] < cur_best_distance){
      cur_best_distance = keyword_distances[i];
    }
  }
  
  // only suggest keywords if the distance is below dynamic threshold (floor(strlen(word) / 2))
  unsigned int distance_threshold = (strlen(word) / 2);
  if(cur_best_distance <= distance_threshold){
    fprintf(stderr, "       Did you mean ");
    bool first_word = true;
    for(int i = 0; i < lexer_keywords_count; i++){
      if(keyword_distances[i] == cur_best_distance){
        if(!first_word){
          fprintf(stderr, " or ");
        }
        fprintf(stderr, "'%s'", lexer_keywords[i]);
        first_word = false;
      }
    }
    fprintf(stderr, "?\n");
  }
}