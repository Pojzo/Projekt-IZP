#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "helper.c"

#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a > b) ? a : b)

// konstanty k normalnej ulohe
#define DEFAULT_ARGUMENTS        3	// pocet argumentov bez --stats
#define MAX_ARGUMENTS            4	// maximalny pocet argumentov na vstupe
#define MAX_PASSWORD_LENGTH      100	// maximalna dlzka hesla na vstupe
#define BUFFER_LENGTH            102	// velkost buffera pre ukladanie hesla
#define MIN_LEVEL                1	// minimalna hodnota argumentu 'level'
#define MAX_LEVEL                4	// maximalna hodnota argumentu 'level'
#define MIN_PARAM                0	// minimalna hodnota argumentu 'param'
#define MAX_PARAM                100	// maximalna hodnota argumentu 'param'
#define STATS_FLAG               "--stats"	// volitelny argument
#define NUM_ASCII_CHARACTERS     126	// pocet charakterov v ascii tabulke s ktorymi pracujeme
#define SPECIAL_CHARACTERS_START 32	// znak ktorym sa zacinaju specialne charaktery
#define ERROR_MSG_LEN            50	// miesto vyhradene pre chybovu spravu

// konstanty k premiovej ulohe 
#define LEVEL_SWITCH "-l"	// volitelny prepinac v premiovej ulohe
#define PARAM_SWITCH "-p"	// volitelny argument v premiovej ulohe
#define DEFAULT_LEVEL  1	// defaultna hodnota, ak nie je zadany prepinac --level
#define DEFAULT_PARAM  1	// defaultna hodnota, ak nie je zadany prepinac --param

typedef unsigned int uint;
typedef unsigned long ulong;

// struct ktory sluzi na ulozenie argumentov z prikazoveho riadku

struct Level {
    uint value;
    uint min_value;
    uint max_value;
};

struct Param {
    ulong value;
    uint min_value;
    ulong max_value;
};

// struct ktory sluzi 
struct Stats {
    bool print;
    uint min_length;
    uint num_words;
    uint total_length;
    uint distinct_chars;
    float average_length;
    bool chars[NUM_ASCII_CHARACTERS];
    /* chars je pole boolov, ktore ma na zaciatku na vsetkych indexoch false
       pocas behu programu nastavujeme chars[char] = true, kde char je charakter zo zadaneho hesla
napriklad: heslo, vo funkci UpdateStats sa stane -> stats.chars[104] = true;  'h'
stats.chars[101] = true;  'e'
stats.chars[115] = true;  's' 
stats.chars[108] = true;  'l'
stats.chars[111] = true;  'o'

na konci iba spocitame kolko boolov z stats.chars je true - pocet roznych znakov v hesle
*/
};

struct Arguments {
    struct Level level;
    struct Param param;
    struct Stats stats;
    bool correct;
    bool optional;
};
// prototypy funkcii

// Kontrola argumentov z prikazoveho riadku
bool CheckArguments(struct Arguments arguments);	// funkcia vrati true ak su argumenty v spravnom tvare
struct Arguments LoadArguments();	// funkcia nacita argumenty 

struct Level CreateDefaultLevel();	// funkcia vrati struct Level s defaultnymi hodnotami
struct Param CreateDefaultParam();	// funkcia vrati struct Param s defaultnymi hodnotami
struct Stats CreateDefaultStats();
struct Arguments IncorrectArguments(struct Arguments arguments, const char *message);	// funckia vrati struct arguments s atributom correct = false a vypise varovnu spravu
struct Arguments CreateArguments(int, const char **);

// Pomocne funkcie 
uint StringLength(const char *string);	// funkcia vrati dlzku vstupneho stringu
bool StringsEqual(const char *string1, const char *string2);	// funkcia vrati true ak sa dva vstupne stringy rovnaju
void RemoveNewLine(char *string, uint string_length);	// funkcia vrati string bez posledneho charakteru
void StringCopy(const char *source, char *dest, uint start, uint length);	// funkcia skopiruje substring dlzky 'length' zacinajucim na 'start' indexe' zo source do dest
void GetSubstrings(const char *password, uint password_len, char substrings[][BUFFER_LENGTH], uint substring_len);	// funkcia do 2d pola substrings nacita vsetky substringy 'password'
bool IsNumber(const char *string);	// funkcia vrati true ak string na vstupe je cislo
const char *StringSubstring(const char *string, uint start, uint end);	// funkcia vrati substring stringu od 'start' po 'end'

// Kontrola individualnych charakterov
bool CharUpper(char c);		// funkcia vrati true ak vstupny char je velke pismeno
bool CharLower(char c);		// funkcia vrati true ak vstupny char je male pismeno
bool IsDigit(char c);		// funkcia vrati true ak vstupny char je cislica
bool IsSpecialCharacter(char c);	// funkcia vrati true ak char je specialny charakter (podla definicie v zadani)

// Kontrola hesla na vstupe
bool ContainsUpperCase(const char *string, uint string_length);	// funkcia vrati true ak string obsahuje velke pismeno
bool ContainsLowerCase(const char *string, uint string_length);	// funkcia vrati true ak string obsahuje male pismeno
bool ContainsDigit(const char *string, uint string_length);	// funkcia vrati true ak string obsahuje cislicu
bool ContainsSpecialCharacter(const char *string, uint string_length);	// funkcia vrati true ak string obsahuje specialny charakter (podla definicie v zadani)
bool NRepeatingCharacters(const char *string, uint string_length, uint x);	// funkcia vrati true ak string neobsahuje sekvenciu rovnakych znakov dlzky aspon x
bool ContainsDuplicateSubstring(char substring[][BUFFER_LENGTH], uint num_substring);	// funkcia vrati true ak 2d pole substring obsahuje duplikat

// Kontrola jednotlivych pravidiel
bool FirstRule(const char *password, uint password_length);
bool SecondRule(const char *password, uint password_length, uint param);
bool ThirdRule(const char *password, uint password_length, uint param);
bool FourthRule(const char *password, uint password_length, uint param);

// Statistika
struct Stats CreateStats();	// funkcia vrati struct stats s inicializovanymi hodnotami
void UpdateStats(struct Stats *, const char *password, uint password_length);	// funkcia aktualizuje statistiku s novym heslom
void CountDistinctCharacters(struct Stats *);	// funkcia spocita pocit roznych charakterov zo vsetkych hesiel a updatne struct stats
void PrintStats(struct Stats);	// funkcia vypise statistiky

// Hlavny beh programu
bool PasswordPassed(const char *password, uint password_length, uint level, uint param);	// funkcia vrati true ak heslo preslo pravidlami
int CheckPasswordsWStats(struct Arguments);	// funkcia nacita po jednom hesla zo vstupu a updatuje pri tom statistiky 
int CheckPasswordsWOStats(struct Arguments);	// funckia natica po jednom hesla zo vstupu bez toho aby robil statistiky

int main(int argc, const char **argv)
{
    // Ak nieco nie je v poriadku s argumentami z prikazoveho riadku, program ukoncime
    struct Arguments arguments = LoadArguments(argc, argv);
    if (!arguments.correct && CheckArguments(arguments)) {
        return 1;

    } else {
        /*
           printf("Level  value: %d\n", arguments.level.value);
           printf("Param  value: %d\n", arguments.param.value);
           printf("Stats  value: %s\n",
           arguments.stats.print ? "True" : "False");
           */
    }
    if (arguments.stats.print) {
        return CheckPasswordsWStats(arguments);
    }
    return CheckPasswordsWOStats(arguments);
}

bool CorrectLevelArgument(const char *string_level, struct Level level)
{
    // ak argument level nie je cislo, nie je spravny
    if (!IsNumber(string_level)) {
        //fprintf(stderr, "Argument 'level' musi mat hodnotu 1 - %d\n", level.max_value);
        return false;
    }
    if (StringLength(string_level) > 1) {
        //fprintf(stderr, "Argument 'level' musi mat hodnotu 1 - %d\n", level.max_value);
        return false;
    }

    int level_int = atoi(string_level);
    // ak sa argument level nachadza v danom rozmedzi, tak je validny
    if ((level_int < level.min_value) || (level_int > level.max_value)) {
        //fprintf(stderr, "Argument 'level' musi mat hodnotu 1 - %d\n", level.max_value);
        return false;
    }

    return true;
}

bool CorrectParamArgument(const char *string_param, struct Param param)
{
    if (!IsNumber(string_param)) {
        //fprintf(stderr, "argument 'param' musi byt kladne cislo");
        return false;
    }
    uint param_int = atoi(string_param);
    if (param_int < 1) {
        //fprintf(stderr, "argument 'param' musi byt kladne cislo");
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
    param.max_value = MAX_PARAM;

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
    // kazdemu charakteru priradime hodnotu false, takze sa nenachadza
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
    char error_message[ERROR_MSG_LEN];
    if (argc < DEFAULT_ARGUMENTS) {
        return IncorrectArguments(arguments,
                "Nedostatocny pocet argumentov");
    }

    else if (argc > MAX_ARGUMENTS) {
        return IncorrectArguments(arguments,
                "Prilis velky pocet argumentov");
    }

    if (CorrectLevelArgument(argv[1], arguments.level)) {
        arguments.level.value = atoi(argv[1]);
    } else {
        return IncorrectArguments(arguments,
                "Zle zadany argument 'level'.. povolene hodnoty su 1 - 4");
    }
    if (CorrectParamArgument(argv[2], arguments.param)) {
        arguments.param.value = atoi(argv[2]);
    } else {
        return IncorrectArguments(arguments,
                "Zle zadany argument 'param'.. povolene hodnoty len kladne cisla");
    }
    if (argc == MAX_ARGUMENTS) {
        // ak je posledny argument stats
        if (CorrectStatsArgument(argv[3])) {
            arguments.stats.print = true;
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
    char error_message[100];
    // dal by sa spravit nejaky genericky algoritmus na nacitavanie volitelnych argumentov ale pre tuto ulohu to staci takto
    for (int i = 1; i < argc; i++) {
        if (CorrectStatsArgument(argv[i])) {
            arguments.stats.print = true;

        } else {
            // ak sa argument rovna -l a zaroven je jeho hodnota spravna
            if (StringsEqual(argv[i], LEVEL_SWITCH)
                    &&
                    (CorrectLevelArgument
                     (argv[i + 1], arguments.level))) {
                if (IsNumber(argv[i + 1])) {
                    arguments.level.value =
                        atoi(argv[i + 1]);
                    i++;
                } else {
                    return IncorrectArguments(arguments,
                            "Nespravne zadany argument -l\n");
                }
            } else if (StringsEqual(argv[i], PARAM_SWITCH)
                    &&
                    (CorrectParamArgument
                     (argv[i + 1], arguments.param))) {
                if (IsNumber(argv[i + 1])) {
                    arguments.param.value =
                        atoi(argv[i + 1]);
                    i++;
                } else {
                    return IncorrectArguments(arguments,
                            "Nespravne zadany argument -p\n");
                };
            } else {
                sprintf(error_message, "Neznamy argument %s\n",
                        argv[i]);
                return IncorrectArguments(arguments,
                        error_message);
            }
        }
    }
    return arguments;
}

bool CheckArguments(struct Arguments arguments)
{
    if (!((arguments.level.value >= arguments.level.min_value)
                && (arguments.level.value <= arguments.level.max_value))) {
        IncorrectArguments(arguments,
                "Argument 'level' musi byt v rozsahu 1 - 4");
        return false;
    }
    if (!((arguments.param.value >= arguments.param.min_value)
                && (arguments.param.value <= arguments.param.max_value))) {
        IncorrectArguments(arguments,
                "argument 'param' musi byt kladne cislo");
        return false;
    }
    return true;
}

struct Arguments LoadArguments(int argc, const char **argv)
{
    struct Arguments arguments;
    arguments.correct = true;

    struct Level level = CreateDefaultLevel();
    struct Param param = CreateDefaultParam();
    struct Stats stats = CreateDefaultStats();

    arguments.level = level;
    arguments.param = param;
    arguments.stats = stats;

    char error_message[ERROR_MSG_LEN];
    // ak je pocet argumentov 0 returneme defaultne argumenty
    if (argc == 1) {
        return arguments;
    } else if (argc == 2) {
        // ak je pocet arugmentov jeden a zaroven sa ten argument rovna --stats
        if (StringsEqual(argv[1], STATS_FLAG)) {
            arguments.stats.print = true;
        } else {
            sprintf(error_message, "Nespravne zadane argumenty",
                    argv[1]);
            return IncorrectArguments(arguments, error_message);
        }
        return arguments;
    }
    // ak prvy argument zacina s '-', tak kontrolujeme volitelne argumenty
    if (argv[1][0] == '-') {
        //printf("Checking optional arguments\n");
        return HandleOptionalArguments(arguments, argc, argv);
    } else if (IsNumber(argv[1])) {
        //printf("Checking positional arguments\n");
        return HandlePositionalArguments(arguments, argc, argv);
    } else {
        sprintf(error_message, "Neznamy argument %s", argv[1]);
        return IncorrectArguments(arguments, error_message);
    }
}

struct Arguments CreateArguments(int argc, const char **argv)
{
    struct Arguments arguments;

    // postupne priradime structu arguments hodnoti z argumentov
    arguments.level.value = atoi(argv[1]);
    arguments.param.value = atoi(argv[2]);
    if (argc == MAX_ARGUMENTS) {
        arguments.stats.print = true;
    } else {
        arguments.stats.print = false;
    }

    return arguments;
}

uint StringLength(const char *string)
{
    uint length = 0;
    // iterujeme cez string, vzdy posuvame pointer na string o jedno dopredu, az kym nie je NULL
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
    // ak maju rovnake dlzky, tak obidva ukazovatele budu rovnake
    return *s1 == *s2;
}

void RemoveNewLine(char *string, uint string_len)
{
    string[string_len - 1] = '\0';
}

void StringCopy(const char *source, char* dest, uint start, uint substring_len)
{
    unsigned index = 0;
    for (uint i = start; i < StringLength(source) && (i < start + substring_len); i++) {
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
        // zaciatok = i, koniec, zaciatok + substring_len
        StringCopy(password, substrings[i], i, substring_len);
    }
}

bool IsNumber(const char *string)
{
    unsigned length = StringLength(string);
    // ak ma string dlzku 0, nemoze byt cislo
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
    // vyselektujeme ktore znaky su specialny charakter
    return (((c >= SPECIAL_CHARACTERS_START) && (c < '0'))
            || ((c > '9') && (c < 'A')) || ((c > 'Z') && (c < 'a'))
            || ((c > 'z')));
}

bool ContainsUpperCase(const char *string, uint str_len)
{
    // iterujeme cez string na kontrolu kazdeho charakteru
    for (uint i = 0; i < str_len; i++) {
        if (CharUpper(string[i])) {
            return true;
        }
    }
    return false;
}

bool ContainsLowerCase(const char *string, uint str_len)
{
    // iterujeme cez string na kontrolu kazdeho charakteru
    for (uint i = 0; i < str_len; i++) {
        if (CharLower(string[i])) {
            return true;
        }
    }
    return false;
}

bool ContainsDigit(const char *string, uint str_len)
{
    // iterujeme cez string na kontrolu kazdeho charakteru
    for (uint i = 0; i < str_len; i++) {
        if (IsDigit(string[i])) {
            return true;
        }
    }
    return false;
}

bool ContainsSpecialCharacter(const char *string, uint str_len)
{
    // iterujeme cez string na kontrolu kazdeho charakteru
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
    if (x == 1) {		// ak je parameter 1, tak vsetky hesla, ktore maju dlzku > 0 nie su bezpecne
        return true;
    }
    uint longest_seq = 1;	// najdlhsia sekvencia na zaciatku musi byt jedna, lebo kontrolujeme od drueho charakteru
    char current_char = string[0];	// aktualny charakter
    // iterujeme cez string na kontrolu kazdeho charakteru
    for (uint i = 1; i < str_len; i++) {
        if (string[i] == current_char) {
            ++longest_seq;	// ak s aktualny znak rovna tomu predchadzajucemu, zvacsime najdlhsiu sekvenciu o 1
        } else {
            longest_seq = 1;	// ak sa nerovna, resetujeme obidve premenne
            current_char = string[i];
        }
        if (longest_seq >= x) {
            return true;
        }
    }
    return false;
}

bool ContainsDuplicateSubstring(char substrings[][BUFFER_LENGTH],
        uint num_substrings)
{
    for (uint i = 0; i < num_substrings; i++) {
        for (uint j = 0; j < num_substrings; j++) {
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
{				// heslo musi obsahovat aspon jedno male a aspon jedno velke pismeno
    return (ContainsUpperCase(password, password_len)
            && ContainsLowerCase(password, password_len));
}

bool SecondRule(const char *password, uint password_len, uint x)
{				// Heslo musi splnat aspon x podmienok specifikovanych v zadani 
    x = min(x, 4);
    uint passed = 0;
    // skontrolujeme vsetky podmienky
    passed =
        ContainsUpperCase(password,
                password_len) + ContainsLowerCase(password,
                    password_len) +
                    ContainsDigit(password,
                            password_len) + ContainsSpecialCharacter(password,
                                password_len);
    // ak hesla splna x alebo viac podmienok, tak preslo druhym pravidlom
    return passed >= x;
}

bool ThirdRule(const char *password, uint password_len, uint x)
{				// heslo nesmie obsahovat sekvenciu opakujucich sa charakterov aspon dlzky x
    // returnujeme negaciu funkcie NRepeatingCharacters.. ak sa tam nachadza sekvencia x charakterov, tak heslo pravidlom nepreslo
    return (!NRepeatingCharacters(password, password_len, x));
}


bool FourthRule(const char *password, uint password_len, uint substring_len)
{
    // toto opravit aby to bol aj argument do funkcie
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

    char buffer[BUFFER_LENGTH];	// buffer pre nacitavanie hesiel 
    uint password_length;	// dlzka nacitaneho hesla
    bool passed;		// premenna urcujuca ci heslo preslo danym poctom pravidiel

    // pokial je na vstupe heslo, nacitame ho do buffera
    while (fgets(buffer, BUFFER_LENGTH, stdin)) {
        password_length = StringLength(buffer);
        if (password_length == BUFFER_LENGTH) {
            fprintf(stderr, "Prilis velka dlzka hesla\n");
            return 1;
        }

        RemoveNewLine(buffer, password_length);	// odstranime z hesla posledny charakter - '\n'

        UpdateStats(&(arguments.stats), buffer, --password_length);	// updateneme statistiky s novym heslom

        passed = PasswordPassed(buffer, password_length, arguments.level.value, arguments.param.value);	// funkcia PasswordPassed nam povie ci heslo preslo pravidlami
        if (passed) {	// ak heslo preslo pravidlami, tak ho vypiseme
            printf("%s\n", buffer);
        }
    }
    // aktualizujeme premiernu dlzku hesla 
    arguments.stats.average_length =
        (double)arguments.stats.total_length /
        (double)arguments.stats.num_words;

    CountDistinctCharacters(&(arguments.stats));

    // na zaver vypiseme statistiku
    PrintStats(arguments.stats);
    return 0;
}

int CheckPasswordsWOStats(struct Arguments arguments)
{
    char buffer[BUFFER_LENGTH];	// buffer pre nacitavanie hesiel 
    uint password_length;	// dlzka nacitaneho hesla
    bool passed;		// premenna urcujuca ci heslo preslo danym poctom pravidiel

    while (fgets(buffer, BUFFER_LENGTH, stdin)) {	// nacitame heslo do buffera
        password_length = StringLength(buffer);
        if (password_length >= MAX_PASSWORD_LENGTH) {
            fprintf(stderr, "Prilis velka dlzka hesla\n");
            return 1;
        }
        RemoveNewLine(buffer, password_length);	// odstranime z hesla posledny charakter - '\n'

        passed = PasswordPassed(buffer, --password_length, arguments.level.value, arguments.param.value);	// funkcia PasswordPassed nam povie ci heslo preslo pravidlami
        if (passed) {	// ak heslo preslo pravidlami, tak ho vypiseme
            printf("%s\n", buffer);
        }
    }
    return 0;
}
