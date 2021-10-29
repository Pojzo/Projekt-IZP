#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

// made by Peter Kovac
// indented using a tool call indent with `indent pwcheck.c --linux`

#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a > b) ? a : b)

// Constants
#define DEFAULT_ARGUMENTS        3	// number of arguments without --stats
#define MAX_ARGUMENTS            4	// maximum number of arguments
#define MAX_PASSWORD_LENGTH      100	// maximum password length
#define BUFFER_LENGTH            102	// buffer lengths for storing password
#define MIN_LEVEL                1	// minimum value for argument 'level'
#define MAX_LEVEL                4	// maximum value for argument 'level'
#define MIN_PARAM                1	// minimum value for argument 'param'
#define STATS_FLAG               "--stats"	// optional argument
#define NUM_ASCII_CHARACTERS     126	// number of characters in ascii table we are working with
#define SPECIAL_CHARACTERS_START 32	// character that begins special characters according to the definition
#define ERROR_MSG_LEN            50	// length of error message

// contants for premium excercise
#define LEVEL_SWITCH "-l"	// optional switch 'level'
#define PARAM_SWITCH "-p"	// optional switch 'param'
#define DEFAULT_LEVEL  1	// default value for argument 'level'
#define DEFAULT_PARAM  1	// default value for argument 'param'

typedef unsigned int uint;
typedef unsigned long ulong;

struct Level {
	int value;
	int min_value;
	int max_value;
};

struct Param {
	long value;
	int min_value;
};

struct Stats {
	bool print;
	uint min_length;
	uint num_words;
	uint total_length;
	uint distinct_chars;
	float average_length;
	bool chars[NUM_ASCII_CHARACTERS];
	/* chars is a bool of chars, that are all intitialized with value false
	   while the program is running, we're settings chars[char] true, where char is character from password
	   for example: 'heslo'; function updateStats does-> stats.chars[104] = true;  'h'
	   stats.chars[101] = true;  'e'
	   stats.chars[115] = true;  's' 
	   stats.chars[108] = true;  'l'
	   stats.chars[111] = true;  'o'

	   at the end we calculate how many members of chars are true - number of distinc characters
	 */
};

struct Arguments {
	struct Level level;
	struct Param param;
	bool correct;
	bool print_stats;
};
// function prototypes

// Checking arguments from command line
struct Arguments CheckArguments(int argc, const char **argv);	// function loads arguments from command line

struct Level CreateDefaultLevel();	// function returns struct Level with default values
struct Param CreateDefaultParam();	// function returns struct Param with default values
struct Arguments IncorrectArguments(struct Arguments arguments, const char *message);	// function returns struct Arguments with attribute correct = false and prints a warning message

// Helper functions
uint StringLength(const char *string);	// functions returns length of string
bool StringsEqual(const char *string1, const char *string2);	// function returns true if strings are equal
void RemoveNewLine(char *string, uint string_length);	// function removes last character from string
void StringCopy(const char *source, char *dest, uint start, uint length);	// function copies substrings starting at 'start' of length 'length' from 'source' to 'dest'
void GetSubstrings(const char *password, uint password_len, char substrings[][BUFFER_LENGTH], uint substring_len);	// function loads all substrings of a string to a 2D array
bool IsNumber(const char *string);	// function returns true if given string is a number
const char *StringSubstring(const char *string, uint start, uint end);	// function returns substring of 'string' from 'start' to 'end'

// Checking individual characters
bool CharUpper(char c);		// function returns true if char is uppercase
bool CharLower(char c);		// function returns true if char is lowercase
bool IsDigit(char c);		// function returns true if char is a digit
bool IsSpecialCharacter(char c);	// function returns true if char is a special character according to the definition

// Checking password
bool ContainsUpperCase(const char *string, uint string_length);	// function returns true if string contains an uppercase letter
bool ContainsLowerCase(const char *string, uint string_length);	// function returns true if string contains a lowercase letter
bool ContainsDigit(const char *string, uint string_length);	// function returns true if string contains a digit
bool ContainsSpecialCharacter(const char *string, uint string_length);	// function returns true if string contains a special character according to the definition
bool NRepeatingCharacters(const char *string, uint string_length, uint x);	// function returns true if strings contains a repeating sequence of x characters
bool ContainsDuplicateSubstring(char substring[][BUFFER_LENGTH], uint num_substring);	// function returns true if array of strings contains a duplicate
bool PasswordPassed(const char *password, uint password_length, uint level, uint param);	// funkcia vrati true ak heslo preslo pravidlami

// Checking rules
bool FirstRule(const char *password, uint password_length);
bool SecondRule(const char *password, uint password_length, uint param);
bool ThirdRule(const char *password, uint password_length, uint param);
bool FourthRule(const char *password, uint password_length, uint param);

// Stats
struct Stats CreateDefaultStats();
void UpdateStats(struct Stats *, const char *password, uint password_length);	// function upadates stats with a new password
void CountDistinctCharacters(struct Stats *);	// function calculates number of distinct characters
void PrintStats(struct Stats);	// function prints stats

// Main loop
int CheckPasswordsWStats(struct Arguments);	// function loads passwords one by one while updating stats
int CheckPasswordsWOStats(struct Arguments);	// function loads passwords one by one without updating stats

int main(int argc, const char **argv)
{
	struct Arguments arguments = CheckArguments(argc, argv);
	// if something is wrong with arguments, terminate program
	if (!arguments.correct) {
		return 1;
	}

	if (arguments.print_stats) {
		return CheckPasswordsWStats(arguments);
	}

	return CheckPasswordsWOStats(arguments);
}

bool CorrectLevelArgument(const char *string_level, struct Level level)
{
	// if argument 'level' is not a number, it cant be correct
	if (!IsNumber(string_level)) {
		return false;
	}
	if (StringLength(string_level) > 1) {
		return false;
	}

	int level_int = atoi(string_level);
	// if argument is not in valid range, its not correct
	if ((level_int < level.min_value) || (level_int > level.max_value)) {
		return false;
	}
	return true;
}

bool CorrectParamArgument(const char *string_param, struct Param param)
{
	// if argument 'param' is not a number, it cant be correct
	if (!IsNumber(string_param)) {
		return false;
	}
	int param_int = atoi(string_param);
	if (param_int < param.min_value) {
		return false;
	}
	return true;
}

bool CorrectStatsArgument(const char *string_param)
{
	return StringsEqual(string_param, STATS_FLAG);
}

struct Level CreateDefaultLevel()
{
	struct Level level;
	level.value = DEFAULT_LEVEL;
	level.min_value = MIN_LEVEL;
	level.max_value = MAX_LEVEL;

	return level;
}

struct Param CreateDefaultParam()
{
	struct Param param;
	param.value = DEFAULT_PARAM;
	param.min_value = MIN_PARAM;

	return param;
}

struct Stats CreateDefaultStats()
{
	struct Stats stats;
	stats.min_length = MAX_PASSWORD_LENGTH;
	stats.print = false;
	stats.num_words = 0;
	stats.total_length = 0;
	stats.distinct_chars = 0;

	// at the beginning, there are no charactes, so each of them is false
	for (int i = SPECIAL_CHARACTERS_START; i < NUM_ASCII_CHARACTERS; i++) {
		stats.chars[i] = false;
	}
	return stats;
}

struct Arguments IncorrectArguments(struct Arguments arguments,
				    const char *message)
{
	arguments.correct = false;
	fprintf(stderr, "%s\n", message);

	return arguments;
}

struct Arguments HandlePositionalArguments(struct Arguments arguments, int argc,
					   const char **argv)
{
	// first checking if there is enough arguments, if yes, check if level and param are correct

	char error_message[ERROR_MSG_LEN];
	// too few arguments
	if (argc < DEFAULT_ARGUMENTS) {
		return IncorrectArguments(arguments,
					  "Insufficient number of arguments");
	}
	//too many arguments
	else if (argc > MAX_ARGUMENTS) {
		return IncorrectArguments(arguments, "Too many arguments");
	}

	if (CorrectLevelArgument(argv[1], arguments.level)) {
		arguments.level.value = atoi(argv[1]);
	} else {
		return IncorrectArguments(arguments,
					  "Incorrect argument level, valid values are 1 - 4");
	}
	if (CorrectParamArgument(argv[2], arguments.param)) {
		arguments.param.value = atoi(argv[2]);
	} else {
		return IncorrectArguments(arguments,
					  "Incorrect argument param, only positive values are valid");
	}
	if (argc == MAX_ARGUMENTS) {
		if (CorrectStatsArgument(argv[3])) {
			arguments.print_stats = true;
		} else {
			sprintf(error_message, "Neznamy argument %s", argv[3]);
			return IncorrectArguments(arguments, error_message);
		}
	}
	return arguments;
}

struct Arguments HandleOptionalArguments(struct Arguments arguments, int argc,
					 const char **argv)
{
	// iterating over every argument, if we see a switch (-l, -p) we check whether the next argument is a valid value
	// for that switch, if not, return IncorrectArguments with a warning message

	char error_message[100];
	for (int i = 1; i < argc; i++) {
		if (CorrectStatsArgument(argv[i])) {
			arguments.print_stats = true;

		} else {
			if (StringsEqual(argv[i], LEVEL_SWITCH)
			    &&
			    (CorrectLevelArgument
			     (argv[i + 1], arguments.level))) {
				if (IsNumber(argv[i + 1])) {
					arguments.level.value =
					    atoi(argv[i + 1]);
					// we have to increment by one, since we used argv[i + 1]
					i++;
				} else {
					return IncorrectArguments(arguments,
								  "Incorrect argument -l\n");
				}
			} else if (StringsEqual(argv[i], PARAM_SWITCH)
				   &&
				   (CorrectParamArgument
				    (argv[i + 1], arguments.param))) {
				if (IsNumber(argv[i + 1])) {
					arguments.param.value =
					    atoi(argv[i + 1]);
					// we have to increment by one, since we used argv[i + 1]
					i++;
				} else {
					return IncorrectArguments(arguments,
								  "Incorrect argument -p\n");
				};
			} else {
				sprintf(error_message, "Unknown argument %s\n",
					argv[i]);
				return IncorrectArguments(arguments,
							  error_message);
			}
		}
	}
	return arguments;
}

struct Arguments CheckArguments(int argc, const char **argv)
{
	struct Arguments arguments;
	arguments.correct = true;

	struct Level level = CreateDefaultLevel();
	struct Param param = CreateDefaultParam();
	struct Stats stats = CreateDefaultStats();

	arguments.level = level;
	arguments.param = param;
	stats = stats;

	char error_message[ERROR_MSG_LEN];

	// if there are no arguments, return arguments with default values
	if (argc == 1) {
		return arguments;
	} else if (argc == 2) {
		// if number of arguments is two and argument is --stats, return arguments with default values
		if (StringsEqual(argv[1], STATS_FLAG)) {
			stats.print = true;
		} else {
			sprintf(error_message, "Incorrect argument %s",
				argv[1]);
			return IncorrectArguments(arguments, error_message);
		}
		return arguments;
	}
	// if the first argument starts with '-', check optional arguments
	if (argv[1][0] == '-') {
		return HandleOptionalArguments(arguments, argc, argv);

		// if its a number, check positional arguments
	} else if (IsNumber(argv[1])) {
		return HandlePositionalArguments(arguments, argc, argv);

	} else {
		sprintf(error_message, "Unknown argument %s", argv[1]);
		return IncorrectArguments(arguments, error_message);
	}
}

uint StringLength(const char *string)
{
	uint length = 0;
	while (*string) {
		++length;
		++string;
	}
	return length;
}

bool StringsEqual(const char *s1, const char *s2)
{
	while (*(s1) && *(s2)) {
		if (*s1 != *s2) {
			return false;
		}
		s1++;
		s2++;
	}
	// if lengths of both strings are same, they should have the same value in the end 
	return *s1 == *s2;
}

void RemoveNewLine(char *string, uint string_len)
{
	string[string_len - 1] = '\0';
}

void StringCopy(const char *source, char *dest, uint start, uint substring_len)
{
	unsigned index = 0;
	for (uint i = start;
	     i < StringLength(source) && (i < start + substring_len); i++) {
		if (index >= substring_len) {
			return;
		}
		dest[index++] = source[i];
	}
	dest[index] = '\0';
}

void GetSubstrings(const char *password, uint password_len,
		   char substrings[][BUFFER_LENGTH], uint substring_len)
{
	uint num_substrings = password_len - substring_len + 1;
	for (uint i = 0; i < num_substrings; i++) {
		StringCopy(password, substrings[i], i, substring_len);
	}
}

bool IsNumber(const char *string)
{
	unsigned length = StringLength(string);
	if (!length) {
		return false;
	}
	for (uint i = 0; i < length; i++) {
		if (!IsDigit(string[i])) {
			return false;
		}
	}
	return true;
}

bool CharUpper(char c)
{
	return ((c >= 'A') && (c <= 'Z'));
}

bool CharLower(char c)
{
	return ((c >= 'a') && (c <= 'z'));
}

bool IsDigit(char c)
{
	return ((c >= '0') && (c <= '9'));
}

bool IsSpecialCharacter(char c)
{
	return (((c >= SPECIAL_CHARACTERS_START) && (c < '0'))
		|| ((c > '9') && (c < 'A')) || ((c > 'Z') && (c < 'a'))
		|| ((c > 'z')));
}

bool ContainsUpperCase(const char *string, uint str_len)
{
	for (uint i = 0; i < str_len; i++) {
		if (CharUpper(string[i])) {
			return true;
		}
	}
	return false;
}

bool ContainsLowerCase(const char *string, uint str_len)
{
	for (uint i = 0; i < str_len; i++) {
		if (CharLower(string[i])) {
			return true;
		}
	}
	return false;
}

bool ContainsDigit(const char *string, uint str_len)
{
	for (uint i = 0; i < str_len; i++) {
		if (IsDigit(string[i])) {
			return true;
		}
	}
	return false;
}

bool ContainsSpecialCharacter(const char *string, uint str_len)
{
	for (uint i = 0; i < str_len; i++) {
		if (IsSpecialCharacter(string[i])) {
			return true;
		}
	}
	return false;
}

bool NRepeatingCharacters(const char *string, uint str_len, uint x)
{
	if (str_len == 0) {
		return false;
	}
	if (x == 1) {		// if 'x' is 1, all password are insecure
		return true;
	}
	uint longest_seq = 1;
	char current_char = string[0];

	for (uint i = 1; i < str_len; i++) {
		if (string[i] == current_char) {
			++longest_seq;	// if current characters is equal to the previous one, increment longest sequence
		} else {
			longest_seq = 1;	// if its not equal, reset both variables
			current_char = string[i];
		}
		if (longest_seq >= x) {	// if a sequence of length x has been found, return true
			return true;
		}
	}
	return false;
}

bool ContainsDuplicateSubstring(char substrings[][BUFFER_LENGTH],
				uint num_substrings)
{
	// brute force approach
	for (uint i = 0; i < num_substrings; i++) {
		for (uint j = i; j < num_substrings; j++) {
			if (i == j) {
				continue;
			}
			if (StringsEqual(substrings[i], substrings[j])) {
				return true;
			}
		}
	}
	return false;
}

bool FirstRule(const char *password, uint password_len)
{				// password must contains at least one lowercase and one uppercase letter
	return (ContainsUpperCase(password, password_len)
		&& ContainsLowerCase(password, password_len));
}

bool SecondRule(const char *password, uint password_len, uint x)
{				// password must meet at least x conditions specified

	x = min(x, 4);		// if x is greater than 4, it wouldnt make sense
	uint passed = 0;
	// check all conditions
	passed =
	    ContainsUpperCase(password,
			      password_len) + ContainsLowerCase(password,
								password_len) +
	    ContainsDigit(password,
			  password_len) + ContainsSpecialCharacter(password,
								   password_len);

	// if password meets at least x conditions, it passed the second rule
	return passed >= x;
}

bool ThirdRule(const char *password, uint password_len, uint x)
{				// password must not contains a sequence of x repeating characters

	// return negation, if a sequence has been found, the password is insecure 
	return (!NRepeatingCharacters(password, password_len, x));
}

bool FourthRule(const char *password, uint password_len, uint substring_len)
{
	uint num_substrings = password_len - substring_len + 1;
	char substrings[num_substrings][BUFFER_LENGTH];
	GetSubstrings(password, password_len, substrings, substring_len);
	return !ContainsDuplicateSubstring(substrings, num_substrings);
}

bool PasswordPassed(const char *password, uint password_length,
		    uint level, uint param)
{
	bool passed = FirstRule(password, password_length);

	if (level >= 2) {
		passed = passed && SecondRule(password, password_length, param);
	}
	if (level >= 3) {
		passed = passed && ThirdRule(password, password_length, param);
	}
	if (level == MAX_LEVEL) {
		passed = passed && FourthRule(password, password_length, param);
	}
	return passed;
}

void UpdateStats(struct Stats *stats, const char *password,
		 uint password_length)
{
	stats->num_words++;
	stats->total_length += password_length;
	stats->min_length = min(stats->min_length, password_length);
	int index;
	for (uint i = 0; i < password_length; i++) {
		index = password[i];
		stats->chars[index] = true;
	}
}

void CountDistinctCharacters(struct Stats *stats)
{
	for (uint i = SPECIAL_CHARACTERS_START; i < NUM_ASCII_CHARACTERS + 1;
	     i++) {
		stats->distinct_chars += stats->chars[i];
	}
}

void PrintStats(struct Stats stats)
{
	printf("Statistika:\n");
	printf("Ruznych znaku: %d\n", stats.distinct_chars);
	printf("Minimalni delka: %d\n", stats.min_length);
	printf("Prumerna delka: %.1f\n", stats.average_length);
}

int CheckPasswordsWStats(struct Arguments arguments)
{

	char buffer[BUFFER_LENGTH];	// buffer for storing passwords
	uint password_length;
	bool passed;		// variable determining whether a password passed all rules
	struct Stats stats = CreateDefaultStats();

	while (fgets(buffer, BUFFER_LENGTH, stdin)) {
		password_length = StringLength(buffer);
		if (password_length == BUFFER_LENGTH) {
			fprintf(stderr,
				"Password is too long, invalid input\n");
			return 1;
		}

		RemoveNewLine(buffer, password_length);	// removes last character from password - '/'
		// update stats with new password
		// password_length should decrease by one
		UpdateStats(&(stats), buffer, --password_length);	// update stats with new password

		passed = PasswordPassed(buffer, password_length, arguments.level.value, arguments.param.value);	// PasswordPassed determines if password has passed
		if (passed) {	// if password passed, print it 
			printf("%s\n", buffer);
		}
	}

	stats.average_length =
	    (double)stats.total_length / (double)stats.num_words;

	CountDistinctCharacters(&(stats));

	// print stats at the end
	PrintStats(stats);
	return 0;
}

int CheckPasswordsWOStats(struct Arguments arguments)
{
	char buffer[BUFFER_LENGTH];	// buffer for storing passwords
	uint password_length;
	bool passed;		// variable determining whether a password passed all rules

	while (fgets(buffer, BUFFER_LENGTH, stdin)) {
		password_length = StringLength(buffer);
		if (password_length >= MAX_PASSWORD_LENGTH) {
			fprintf(stderr, "Password too long, invalid input\n");
			return 1;
		}
		RemoveNewLine(buffer, password_length);	// remove last character from password - '\n'
		// password_length should decrease by one
		passed = PasswordPassed(buffer, --password_length, arguments.level.value, arguments.param.value);	// PasswordPassed determines if password has passed
		if (passed) {	// if password passed, print it
			printf("%s\n", buffer);
		}
	}

	return 0;
}
