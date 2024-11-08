/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "npr.y"

#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

int row = 0, col = 0;

struct Atributes {
  vector<string> c;

  int row = 0, col = 0;
  int counter = 0;
  vector<string> default_value;

  void clear() {
    default_value.clear();
    c.clear();
    row = 0;
    col = 0;
    counter = 0;
  }
};

enum var_types { Let = 1, Const, Var};
map<var_types, string> vatTypeName = {
  {Let, "let"},
  {Const, "const"},
  {Var, "var"}
};

struct symbol {
    var_types type;
    int row;
    int col;
    int n_args;
};

vector<map<string, symbol>> table = {map<string, symbol>{}};

vector<string> functions;

vector<string> declare_var(var_types type, string name, int row, int col);
void check_symbol(string name, bool allowMod);

string get_var(string &name, bool allowMod);
string get_field(string &name);

#define YYSTYPE Atributes

void erro( string msg );
void print( string st );

extern "C" int yylex();
int yyparse();
void yyerror( const char* );

vector<string> concatena( vector<string> a, vector<string> b ) {
  a.insert( a.end(), b.begin(), b.end() );
  return a;
}

std::string cleanQuotes(const std::string& str) {
    if (str.length() >= 2) {
        return str.substr(1, str.length() - 2);
    } else {
        return "";
    }
}

vector<string> operator+( vector<string> a, vector<string> b ) {
  return concatena( a, b );
}

vector<string> operator+( vector<string> a, string b ) {
  a.push_back( b );
  return a;
}

vector<string> operator+( string a, vector<string> b ) {
  return vector<string>{ a } + b;
}

vector<string> solve_address(vector<string> entrada) {
    map<string, int> label;
    vector<string> saida;
    for (int i = 0; i < entrada.size(); i++) {
        if (entrada[i][0] == ':') {
            label[entrada[i].substr(1)] = saida.size();
        }
        else {
            saida.push_back(entrada[i]);
        }
    };

    for (int i = 0; i < saida.size(); i++) {
      // cout << saida[i] << " " << i << endl;
        if (label.count(saida[i]) > 0) {
            saida[i] = to_string(label[saida[i]]);
        };
    };
    
    return saida;
}

string gera_label( string prefixo ) {
  static int n = 0;
  return prefixo + "_" + to_string( ++n ) + ":";
}

void print( vector<string> codigo ) {
  for( string s : codigo ){
    if (s == "\n" || s == "^") {
      cout << s << endl;;
    }

    else if (s == ".") {
      cout << endl << s << endl;
    }
    // else if(s[0] == '"' || s[0] == '\'') {
    //   cout << cleanQuotes(s) << " ";
    // }
    else {
      cout << s << " ";
    }
  }
  cout << endl;  
}


#line 204 "npr.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ID = 258,                      /* ID  */
    IF = 259,                      /* IF  */
    ELSE = 260,                    /* ELSE  */
    LET = 261,                     /* LET  */
    CONST = 262,                   /* CONST  */
    VAR = 263,                     /* VAR  */
    FOR = 264,                     /* FOR  */
    FUNCTION = 265,                /* FUNCTION  */
    ASM = 266,                     /* ASM  */
    RETURN = 267,                  /* RETURN  */
    STRING = 268,                  /* STRING  */
    NUM = 269,                     /* NUM  */
    V_TRUE = 270,                  /* V_TRUE  */
    V_FALSE = 271,                 /* V_FALSE  */
    DOUBLE_EQUAL = 272,            /* DOUBLE_EQUAL  */
    DELIMITER = 273,               /* DELIMITER  */
    PLUS_EQUAL = 274,              /* PLUS_EQUAL  */
    DOUBLE_PLUS = 275,             /* DOUBLE_PLUS  */
    GT = 276,                      /* GT  */
    ST = 277                       /* ST  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);



/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ID = 3,                         /* ID  */
  YYSYMBOL_IF = 4,                         /* IF  */
  YYSYMBOL_ELSE = 5,                       /* ELSE  */
  YYSYMBOL_LET = 6,                        /* LET  */
  YYSYMBOL_CONST = 7,                      /* CONST  */
  YYSYMBOL_VAR = 8,                        /* VAR  */
  YYSYMBOL_FOR = 9,                        /* FOR  */
  YYSYMBOL_FUNCTION = 10,                  /* FUNCTION  */
  YYSYMBOL_ASM = 11,                       /* ASM  */
  YYSYMBOL_RETURN = 12,                    /* RETURN  */
  YYSYMBOL_STRING = 13,                    /* STRING  */
  YYSYMBOL_NUM = 14,                       /* NUM  */
  YYSYMBOL_V_TRUE = 15,                    /* V_TRUE  */
  YYSYMBOL_V_FALSE = 16,                   /* V_FALSE  */
  YYSYMBOL_DOUBLE_EQUAL = 17,              /* DOUBLE_EQUAL  */
  YYSYMBOL_DELIMITER = 18,                 /* DELIMITER  */
  YYSYMBOL_PLUS_EQUAL = 19,                /* PLUS_EQUAL  */
  YYSYMBOL_DOUBLE_PLUS = 20,               /* DOUBLE_PLUS  */
  YYSYMBOL_21_ = 21,                       /* '='  */
  YYSYMBOL_GT = 22,                        /* GT  */
  YYSYMBOL_ST = 23,                        /* ST  */
  YYSYMBOL_24_ = 24,                       /* '-'  */
  YYSYMBOL_25_ = 25,                       /* '+'  */
  YYSYMBOL_26_ = 26,                       /* '*'  */
  YYSYMBOL_27_ = 27,                       /* '/'  */
  YYSYMBOL_28_ = 28,                       /* '%'  */
  YYSYMBOL_29_ = 29,                       /* '{'  */
  YYSYMBOL_30_ = 30,                       /* '}'  */
  YYSYMBOL_31_ = 31,                       /* ','  */
  YYSYMBOL_32_ = 32,                       /* '['  */
  YYSYMBOL_33_ = 33,                       /* ']'  */
  YYSYMBOL_34_ = 34,                       /* '('  */
  YYSYMBOL_35_ = 35,                       /* ')'  */
  YYSYMBOL_36_ = 36,                       /* '.'  */
  YYSYMBOL_YYACCEPT = 37,                  /* $accept  */
  YYSYMBOL_FIM = 38,                       /* FIM  */
  YYSYMBOL_S = 39,                         /* S  */
  YYSYMBOL_CMDs = 40,                      /* CMDs  */
  YYSYMBOL_CMD = 41,                       /* CMD  */
  YYSYMBOL_CMD_LET = 42,                   /* CMD_LET  */
  YYSYMBOL_LET_VARs = 43,                  /* LET_VARs  */
  YYSYMBOL_LET_VAR = 44,                   /* LET_VAR  */
  YYSYMBOL_CMD_CONST = 45,                 /* CMD_CONST  */
  YYSYMBOL_CONST_VARs = 46,                /* CONST_VARs  */
  YYSYMBOL_CONST_VAR = 47,                 /* CONST_VAR  */
  YYSYMBOL_CMD_VAR = 48,                   /* CMD_VAR  */
  YYSYMBOL_VAR_VARs = 49,                  /* VAR_VARs  */
  YYSYMBOL_VAR_VAR = 50,                   /* VAR_VAR  */
  YYSYMBOL_CMD_IF = 51,                    /* CMD_IF  */
  YYSYMBOL_EMPILHA_TABLE = 52,             /* EMPILHA_TABLE  */
  YYSYMBOL_DESEMPILHA_TABLE = 53,          /* DESEMPILHA_TABLE  */
  YYSYMBOL_CMD_FUNC = 54,                  /* CMD_FUNC  */
  YYSYMBOL_55_1 = 55,                      /* $@1  */
  YYSYMBOL_PARAMETERS = 56,                /* PARAMETERS  */
  YYSYMBOL_PARAMs = 57,                    /* PARAMs  */
  YYSYMBOL_PARAM = 58,                     /* PARAM  */
  YYSYMBOL_ARGUMENTS = 59,                 /* ARGUMENTS  */
  YYSYMBOL_ARGs = 60,                      /* ARGs  */
  YYSYMBOL_ARG = 61,                       /* ARG  */
  YYSYMBOL_LVALUE = 62,                    /* LVALUE  */
  YYSYMBOL_LVALUEPROP = 63,                /* LVALUEPROP  */
  YYSYMBOL_Expr = 64                       /* Expr  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   445

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  37
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  28
/* YYNRULES -- Number of rules.  */
#define YYNRULES  84
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  148

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   277


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    28,     2,     2,
      34,    35,    26,    25,    31,    24,    36,    27,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    21,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    32,     2,    33,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    29,     2,    30,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    22,    23
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   146,   146,   148,   151,   152,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   167,   170,   171,   174,
     175,   176,   177,   180,   183,   184,   187,   188,   189,   190,
     195,   198,   199,   202,   203,   204,   205,   208,   217,   228,
     231,   233,   233,   260,   261,   264,   268,   271,   272,   276,
     277,   279,   280,   283,   285,   288,   289,   291,   292,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ID", "IF", "ELSE",
  "LET", "CONST", "VAR", "FOR", "FUNCTION", "ASM", "RETURN", "STRING",
  "NUM", "V_TRUE", "V_FALSE", "DOUBLE_EQUAL", "DELIMITER", "PLUS_EQUAL",
  "DOUBLE_PLUS", "'='", "GT", "ST", "'-'", "'+'", "'*'", "'/'", "'%'",
  "'{'", "'}'", "','", "'['", "']'", "'('", "')'", "'.'", "$accept", "FIM",
  "S", "CMDs", "CMD", "CMD_LET", "LET_VARs", "LET_VAR", "CMD_CONST",
  "CONST_VARs", "CONST_VAR", "CMD_VAR", "VAR_VARs", "VAR_VAR", "CMD_IF",
  "EMPILHA_TABLE", "DESEMPILHA_TABLE", "CMD_FUNC", "$@1", "PARAMETERS",
  "PARAMs", "PARAM", "ARGUMENTS", "ARGs", "ARG", "LVALUE", "LVALUEPROP",
  "Expr", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-99)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-51)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -99,     3,   -99,     8,   -99,   -99,   -29,     4,    27,    31,
      33,   194,   -99,   -99,   -99,   -99,    35,    25,    11,    32,
     216,   -99,    48,    49,    58,   -99,    59,   106,   -99,   323,
     194,    60,   -99,    52,    79,   -99,    57,    81,   -99,    75,
     -99,    11,   343,   -99,   -99,   -99,   -99,   -99,    77,    69,
     -99,   -99,   -99,   -99,   194,   -99,   238,    92,   194,   -99,
     194,   194,   194,   194,   194,   194,   194,   194,    55,    35,
     359,   260,     4,   282,    27,   304,    31,    83,   -99,   108,
      78,   -99,   393,   100,   393,   -99,   393,   409,   409,    46,
      46,    -7,    -7,    -7,   375,    96,   102,   -99,   393,   113,
     172,   105,   103,   393,   -99,   109,   112,   393,   -99,   111,
     116,   393,   -99,   -99,   121,   -99,   -99,   136,   -99,   194,
     194,   153,   -14,    -5,    13,    15,    30,    67,   156,   -99,
     194,   -99,   393,   172,   141,   126,   132,   -99,   393,   -99,
     194,   137,   156,   393,   -99,   -99,   140,   -99
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       5,     0,     2,    84,     1,    54,     0,     0,     0,     0,
       0,    84,    75,    72,    82,    83,     0,     0,    39,     0,
      84,     4,     0,     0,     0,     9,    13,    76,    77,     0,
      84,    19,    16,    18,    26,    23,    25,    33,    30,    32,
      41,     0,     0,    70,    73,    80,     5,    81,     0,     0,
       6,     7,     8,    12,    84,    71,    84,     0,    84,    11,
      84,    84,    84,    84,    84,    84,    84,    84,    84,     0,
       0,    84,     0,    84,     0,    84,     0,     0,    15,    84,
      80,    78,    69,     0,    60,    14,    68,    62,    61,    64,
      63,    65,    66,    67,     0,     0,    49,    52,    53,    58,
      84,     0,     0,    22,    17,     0,     0,    29,    24,     0,
       0,    36,    31,    39,     0,    79,    59,    55,    74,    84,
      84,    38,    80,    81,    80,    81,    80,    81,    44,    10,
      84,    51,    57,    84,    47,     0,    43,    46,    56,    37,
      84,     0,     0,    48,     5,    45,    84,    42
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -99,   -99,   -99,   -45,   -98,   -99,    93,   -99,   -99,    94,
     -99,   -99,    91,   -99,   -99,    64,   -99,   -99,   -99,   -99,
     -99,    29,   -99,   -99,    54,   -10,   -99,   -11
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     1,     2,     3,    21,    22,    32,    33,    23,    35,
      36,    24,    38,    39,    25,    46,   114,    26,    77,   135,
     136,   137,    95,    96,    97,    27,    28,    29
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      42,    79,   121,     4,   -20,    30,    43,    31,    -3,    49,
      58,     5,     6,   -21,     7,     8,     9,   -20,    10,    70,
      11,    12,    13,    14,    15,    67,   -21,    68,    16,    69,
      34,   -27,    17,   -28,    37,   139,    40,    18,     5,    44,
      19,    45,    20,    82,   -27,    84,   -28,    86,   -34,    87,
      88,    89,    90,    91,    92,    93,    94,    98,     5,    99,
     103,   -34,   107,    58,   111,    47,    50,    51,    12,    13,
      14,    15,    64,    65,    66,    16,    52,    53,    67,    17,
      68,    71,    69,    72,    41,   -35,    58,    19,    74,    20,
     -50,    60,    61,    62,    63,    64,    65,    66,   -35,   146,
      73,    67,    75,    68,    81,    69,    76,    80,    98,   132,
      85,     5,     6,   115,     7,     8,     9,   113,    10,   138,
      11,    12,    13,    14,    15,    54,    55,    56,    16,   143,
     116,   118,    17,   119,   120,   122,   123,    18,   -40,   124,
      19,   126,    20,     5,     6,   125,     7,     8,     9,   127,
      10,   129,    11,    12,    13,    14,    15,   130,   133,   134,
      16,   141,   140,   142,    17,   104,   144,   112,   108,    18,
     147,   145,    19,   131,    20,     5,     6,   128,     7,     8,
       9,     0,    10,     0,    11,    12,    13,    14,    15,     0,
       0,     0,    16,     0,     0,     0,    17,     5,     0,     0,
       0,    18,     0,     0,    19,     0,    20,    12,    13,    14,
      15,     0,     0,     0,    16,     0,     0,     0,    17,     5,
       0,     0,     0,    41,     0,     0,    19,     0,    20,    12,
      13,    14,    15,     0,     0,     0,    16,     0,     0,     0,
      17,     5,     0,     0,     0,    48,     0,     0,    19,     0,
      20,    12,    13,    14,    15,     0,     0,     0,    16,     0,
       0,     0,    17,     5,     0,     0,     0,    83,     0,     0,
      19,     0,    20,    12,    13,    14,    15,     0,     0,     0,
      16,     0,     0,     0,    17,     5,     0,     0,     0,   101,
       0,     0,   102,     0,    20,    12,    13,    14,    15,     0,
       0,     0,    16,     0,     0,     0,    17,     5,     0,     0,
       0,   105,     0,     0,   106,     0,    20,    12,    13,    14,
      15,     0,     0,     0,    16,     0,     0,     0,    17,     0,
       0,     0,     0,   109,    57,     0,   110,     0,    20,     0,
      58,    59,     0,     0,     0,    60,    61,    62,    63,    64,
      65,    66,     0,     0,     0,    67,     0,    68,     0,    69,
      58,    78,     0,     0,     0,    60,    61,    62,    63,    64,
      65,    66,     0,     0,     0,    67,    58,    68,     0,    69,
       0,    60,    61,    62,    63,    64,    65,    66,     0,     0,
       0,    67,    58,    68,   100,    69,     0,    60,    61,    62,
      63,    64,    65,    66,     0,     0,     0,    67,   117,    68,
      58,    69,     0,     0,     0,    60,    61,    62,    63,    64,
      65,    66,     0,     0,     0,    67,    58,    68,     0,    69,
       0,   -51,   -51,    62,    63,    64,    65,    66,     0,     0,
       0,    67,     0,    68,     0,    69
};

static const yytype_int16 yycheck[] =
{
      11,    46,   100,     0,    18,    34,    16,     3,     0,    20,
      17,     3,     4,    18,     6,     7,     8,    31,    10,    30,
      12,    13,    14,    15,    16,    32,    31,    34,    20,    36,
       3,    18,    24,    18,     3,   133,     3,    29,     3,    14,
      32,    30,    34,    54,    31,    56,    31,    58,    18,    60,
      61,    62,    63,    64,    65,    66,    67,    68,     3,    69,
      71,    31,    73,    17,    75,    33,    18,    18,    13,    14,
      15,    16,    26,    27,    28,    20,    18,    18,    32,    24,
      34,    21,    36,    31,    29,    18,    17,    32,    31,    34,
      35,    22,    23,    24,    25,    26,    27,    28,    31,   144,
      21,    32,    21,    34,    35,    36,    31,    30,   119,   120,
      18,     3,     4,    35,     6,     7,     8,    34,    10,   130,
      12,    13,    14,    15,    16,    19,    20,    21,    20,   140,
      30,    35,    24,    31,    21,    30,    33,    29,    30,    30,
      32,    30,    34,     3,     4,    33,     6,     7,     8,    33,
      10,    30,    12,    13,    14,    15,    16,    21,     5,     3,
      20,    35,    21,    31,    24,    72,    29,    76,    74,    29,
      30,   142,    32,   119,    34,     3,     4,   113,     6,     7,
       8,    -1,    10,    -1,    12,    13,    14,    15,    16,    -1,
      -1,    -1,    20,    -1,    -1,    -1,    24,     3,    -1,    -1,
      -1,    29,    -1,    -1,    32,    -1,    34,    13,    14,    15,
      16,    -1,    -1,    -1,    20,    -1,    -1,    -1,    24,     3,
      -1,    -1,    -1,    29,    -1,    -1,    32,    -1,    34,    13,
      14,    15,    16,    -1,    -1,    -1,    20,    -1,    -1,    -1,
      24,     3,    -1,    -1,    -1,    29,    -1,    -1,    32,    -1,
      34,    13,    14,    15,    16,    -1,    -1,    -1,    20,    -1,
      -1,    -1,    24,     3,    -1,    -1,    -1,    29,    -1,    -1,
      32,    -1,    34,    13,    14,    15,    16,    -1,    -1,    -1,
      20,    -1,    -1,    -1,    24,     3,    -1,    -1,    -1,    29,
      -1,    -1,    32,    -1,    34,    13,    14,    15,    16,    -1,
      -1,    -1,    20,    -1,    -1,    -1,    24,     3,    -1,    -1,
      -1,    29,    -1,    -1,    32,    -1,    34,    13,    14,    15,
      16,    -1,    -1,    -1,    20,    -1,    -1,    -1,    24,    -1,
      -1,    -1,    -1,    29,    11,    -1,    32,    -1,    34,    -1,
      17,    18,    -1,    -1,    -1,    22,    23,    24,    25,    26,
      27,    28,    -1,    -1,    -1,    32,    -1,    34,    -1,    36,
      17,    18,    -1,    -1,    -1,    22,    23,    24,    25,    26,
      27,    28,    -1,    -1,    -1,    32,    17,    34,    -1,    36,
      -1,    22,    23,    24,    25,    26,    27,    28,    -1,    -1,
      -1,    32,    17,    34,    35,    36,    -1,    22,    23,    24,
      25,    26,    27,    28,    -1,    -1,    -1,    32,    33,    34,
      17,    36,    -1,    -1,    -1,    22,    23,    24,    25,    26,
      27,    28,    -1,    -1,    -1,    32,    17,    34,    -1,    36,
      -1,    22,    23,    24,    25,    26,    27,    28,    -1,    -1,
      -1,    32,    -1,    34,    -1,    36
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    38,    39,    40,     0,     3,     4,     6,     7,     8,
      10,    12,    13,    14,    15,    16,    20,    24,    29,    32,
      34,    41,    42,    45,    48,    51,    54,    62,    63,    64,
      34,     3,    43,    44,     3,    46,    47,     3,    49,    50,
       3,    29,    64,    62,    14,    30,    52,    33,    29,    64,
      18,    18,    18,    18,    19,    20,    21,    11,    17,    18,
      22,    23,    24,    25,    26,    27,    28,    32,    34,    36,
      64,    21,    31,    21,    31,    21,    31,    55,    18,    40,
      30,    35,    64,    29,    64,    18,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    59,    60,    61,    64,    62,
      35,    29,    32,    64,    43,    29,    32,    64,    46,    29,
      32,    64,    49,    34,    53,    35,    30,    33,    35,    31,
      21,    41,    30,    33,    30,    33,    30,    33,    52,    30,
      21,    61,    64,     5,     3,    56,    57,    58,    64,    41,
      21,    35,    31,    64,    29,    58,    40,    30
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    37,    38,    39,    40,    40,    41,    41,    41,    41,
      41,    41,    41,    41,    41,    41,    42,    43,    43,    44,
      44,    44,    44,    45,    46,    46,    47,    47,    47,    47,
      48,    49,    49,    50,    50,    50,    50,    51,    51,    52,
      53,    55,    54,    56,    56,    57,    57,    58,    58,    59,
      59,    60,    60,    61,    62,    63,    63,    63,    63,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64,    64,    64,    64,    64,    64,
      64,    64,    64,    64,    64
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     0,     2,     2,     2,     1,
       5,     2,     2,     1,     3,     3,     2,     3,     1,     1,
       4,     4,     3,     2,     3,     1,     1,     4,     4,     3,
       2,     3,     1,     1,     4,     4,     3,     7,     5,     0,
       0,     0,    10,     1,     0,     3,     1,     1,     3,     1,
       0,     3,     1,     1,     1,     4,     6,     5,     3,     4,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     2,     1,     2,     4,     1,     1,     1,     3,     4,
       2,     2,     1,     1,     0
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* FIM: S  */
#line 146 "npr.y"
        {yyval.c = solve_address(yyvsp[0].c + "." + functions); print(yyval.c);}
#line 1465 "npr.tab.c"
    break;

  case 4: /* CMDs: CMDs CMD  */
#line 151 "npr.y"
                { yyval.c = yyvsp[-1].c + yyvsp[0].c; }
#line 1471 "npr.tab.c"
    break;

  case 5: /* CMDs: %empty  */
#line 152 "npr.y"
                { yyval.clear();}
#line 1477 "npr.tab.c"
    break;

  case 6: /* CMD: CMD_LET DELIMITER  */
#line 155 "npr.y"
                        { yyval.c = yyval.c; }
#line 1483 "npr.tab.c"
    break;

  case 7: /* CMD: CMD_CONST DELIMITER  */
#line 156 "npr.y"
                          { yyval.c = yyval.c; }
#line 1489 "npr.tab.c"
    break;

  case 8: /* CMD: CMD_VAR DELIMITER  */
#line 157 "npr.y"
                        { yyval.c = yyval.c; }
#line 1495 "npr.tab.c"
    break;

  case 10: /* CMD: '{' EMPILHA_TABLE CMDs DESEMPILHA_TABLE '}'  */
#line 159 "npr.y"
                                                   { yyval.c = "<{" + yyvsp[-2].c + "}>"; }
#line 1501 "npr.tab.c"
    break;

  case 11: /* CMD: Expr DELIMITER  */
#line 160 "npr.y"
                      { yyval.c = yyvsp[-1].c + "^"; }
#line 1507 "npr.tab.c"
    break;

  case 14: /* CMD: Expr ASM DELIMITER  */
#line 163 "npr.y"
                         {yyval.c = yyvsp[-2].c + yyvsp[-1].c + "^"; }
#line 1513 "npr.tab.c"
    break;

  case 15: /* CMD: RETURN Expr DELIMITER  */
#line 164 "npr.y"
                            {yyval.c = yyvsp[-1].c + "'&retorno'" + "@" + "~"; }
#line 1519 "npr.tab.c"
    break;

  case 16: /* CMD_LET: LET LET_VARs  */
#line 167 "npr.y"
                       { yyval.c = yyvsp[0].c;  }
#line 1525 "npr.tab.c"
    break;

  case 17: /* LET_VARs: LET_VAR ',' LET_VARs  */
#line 170 "npr.y"
                               { yyval.c = yyvsp[-2].c + yyvsp[0].c; }
#line 1531 "npr.tab.c"
    break;

  case 19: /* LET_VAR: ID  */
#line 174 "npr.y"
             { yyval.c = declare_var( Let, yyvsp[0].c[0], yyvsp[0].row, yyvsp[0].col ); }
#line 1537 "npr.tab.c"
    break;

  case 20: /* LET_VAR: ID '=' '{' '}'  */
#line 175 "npr.y"
                         { yyval.c = declare_var(Let, yyvsp[-3].c[0],  yyvsp[-3].row, yyvsp[-3].col) + yyvsp[-3].c + "{}" + "=" + "^"; }
#line 1543 "npr.tab.c"
    break;

  case 21: /* LET_VAR: ID '=' '[' ']'  */
#line 176 "npr.y"
                         { yyval.c = declare_var(Let, yyvsp[-3].c[0],  yyvsp[-3].row, yyvsp[-3].col) + yyvsp[-3].c + "[]" + "=" + "^"; }
#line 1549 "npr.tab.c"
    break;

  case 22: /* LET_VAR: ID '=' Expr  */
#line 177 "npr.y"
                      { yyval.c = declare_var( Let, yyvsp[-2].c[0], yyvsp[-2].row, yyvsp[-2].col ) + yyvsp[-2].c + yyvsp[0].c + "=" + "^"; }
#line 1555 "npr.tab.c"
    break;

  case 23: /* CMD_CONST: CONST CONST_VARs  */
#line 180 "npr.y"
                             { yyval.c = yyvsp[0].c;  }
#line 1561 "npr.tab.c"
    break;

  case 24: /* CONST_VARs: CONST_VAR ',' CONST_VARs  */
#line 183 "npr.y"
                                     { yyval.c = yyvsp[-2].c + yyvsp[0].c; }
#line 1567 "npr.tab.c"
    break;

  case 26: /* CONST_VAR: ID  */
#line 187 "npr.y"
               { yyval.c = declare_var( Let, yyvsp[0].c[0], yyvsp[0].row, yyvsp[0].col ); }
#line 1573 "npr.tab.c"
    break;

  case 27: /* CONST_VAR: ID '=' '{' '}'  */
#line 188 "npr.y"
                         { yyval.c = declare_var(Const, yyvsp[-3].c[0],  yyvsp[-3].row, yyvsp[-3].col) + yyvsp[-3].c + "{}" + "=" + "^"; }
#line 1579 "npr.tab.c"
    break;

  case 28: /* CONST_VAR: ID '=' '[' ']'  */
#line 189 "npr.y"
                         { yyval.c = declare_var(Const, yyvsp[-3].c[0],  yyvsp[-3].row, yyvsp[-3].col) + yyvsp[-3].c + "[]" + "=" + "^"; }
#line 1585 "npr.tab.c"
    break;

  case 29: /* CONST_VAR: ID '=' Expr  */
#line 190 "npr.y"
                      { yyval.c = declare_var( Const, yyvsp[-2].c[0], yyvsp[-2].row, yyvsp[-2].col ) + yyvsp[-2].c + yyvsp[0].c + "=" + "^"; }
#line 1591 "npr.tab.c"
    break;

  case 30: /* CMD_VAR: VAR VAR_VARs  */
#line 195 "npr.y"
                       { yyval.c = yyvsp[0].c;  }
#line 1597 "npr.tab.c"
    break;

  case 31: /* VAR_VARs: VAR_VAR ',' VAR_VARs  */
#line 198 "npr.y"
                               { yyval.c = yyvsp[-2].c + yyvsp[0].c; }
#line 1603 "npr.tab.c"
    break;

  case 33: /* VAR_VAR: ID  */
#line 202 "npr.y"
             { yyval.c = declare_var( Let, yyvsp[0].c[0], yyvsp[0].row, yyvsp[0].col ); }
#line 1609 "npr.tab.c"
    break;

  case 34: /* VAR_VAR: ID '=' '{' '}'  */
#line 203 "npr.y"
                         { yyval.c = declare_var(Var, yyvsp[-3].c[0],  yyvsp[-3].row, yyvsp[-3].col) + yyvsp[-3].c + "{}" + "=" + "^"; }
#line 1615 "npr.tab.c"
    break;

  case 35: /* VAR_VAR: ID '=' '[' ']'  */
#line 204 "npr.y"
                         { yyval.c = declare_var(Var, yyvsp[-3].c[0],  yyvsp[-3].row, yyvsp[-3].col) + yyvsp[-3].c + "[]" + "=" + "^"; }
#line 1621 "npr.tab.c"
    break;

  case 36: /* VAR_VAR: ID '=' Expr  */
#line 205 "npr.y"
                      { yyval.c = declare_var( Var, yyvsp[-2].c[0], yyvsp[-2].row, yyvsp[-2].col ) + yyvsp[-2].c + yyvsp[0].c + "=" + "^"; }
#line 1627 "npr.tab.c"
    break;

  case 37: /* CMD_IF: IF '(' Expr ')' CMD ELSE CMD  */
#line 209 "npr.y"
        {
            string lbl_true = gera_label( "lbl_true" );
            string lbl_fim_if = gera_label( "lbl_fim_if" );
            string definicao_lbl_true = ":" + lbl_true;
            string definicao_lbl_fim_if = ":" + lbl_fim_if;
            yyval.c = yyvsp[-4].c + lbl_true + "?" + yyvsp[0].c + lbl_fim_if 
            + "#" + definicao_lbl_true + yyvsp[-2].c + definicao_lbl_fim_if;
        }
#line 1640 "npr.tab.c"
    break;

  case 38: /* CMD_IF: IF '(' Expr ')' CMD  */
#line 218 "npr.y"
        {
          string lbl_true = gera_label( "lbl_true" );
          string lbl_fim_if = gera_label( "lbl_fim_if" );
          string definicao_lbl_true = ":" + lbl_true;
          string definicao_lbl_fim_if = ":" + lbl_fim_if;
          yyval.c = yyvsp[-2].c + lbl_true + "?" + lbl_fim_if + "#" + definicao_lbl_true + yyvsp[0].c + definicao_lbl_fim_if;
          //  $$.c = $3.c + lbl_true + "?" + $5.c + definicao_lbl_true ;
        }
#line 1653 "npr.tab.c"
    break;

  case 39: /* EMPILHA_TABLE: %empty  */
#line 228 "npr.y"
                { table.push_back( map<string, symbol>{}); }
#line 1659 "npr.tab.c"
    break;

  case 40: /* DESEMPILHA_TABLE: %empty  */
#line 231 "npr.y"
                   { table.pop_back(); }
#line 1665 "npr.tab.c"
    break;

  case 41: /* $@1: %empty  */
#line 233 "npr.y"
                       { declare_var(Var, yyvsp[0].c[0], yyvsp[0].row, yyvsp[0].col); }
#line 1671 "npr.tab.c"
    break;

  case 42: /* CMD_FUNC: FUNCTION ID $@1 '(' EMPILHA_TABLE PARAMETERS ')' '{' CMDs '}'  */
#line 235 "npr.y"
          {
            string lbl_function_address = gera_label("func_" + yyvsp[-8].c[0]);
            string def_function_address = ":" + lbl_function_address;

            yyval.c = yyvsp[-8].c + "&" + yyvsp[-8].c + "{}" + "=" + "'&funcao'" + lbl_function_address
            + "[=]" + "^";
            functions = functions + def_function_address + yyvsp[-4].c + yyvsp[-1].c + "undefined"
            + "@" + "'&retorno'" + "@" + "~";
            table.pop_back();
          }
#line 1686 "npr.tab.c"
    break;

  case 44: /* PARAMETERS: %empty  */
#line 261 "npr.y"
              { yyval.c.clear(); }
#line 1692 "npr.tab.c"
    break;

  case 45: /* PARAMs: PARAMs ',' PARAM  */
#line 265 "npr.y"
            { yyval.c = yyvsp[-2].c + yyvsp[0].c + "&" + yyvsp[0].c + "arguments" + "@" + to_string(yyvsp[-2].counter) +"[@]" + "=" + "^" ;
              yyval.counter = yyval.counter + yyvsp[-2].counter + yyvsp[0].counter;      
            }
#line 1700 "npr.tab.c"
    break;

  case 46: /* PARAMs: PARAM  */
#line 268 "npr.y"
                  { yyval.c = yyvsp[0].c + "&" + yyvsp[0].c + "arguments" + "@" + to_string(yyvsp[0].counter) + "[@]" + "=" + "^"; yyval.counter = yyvsp[0].counter + 1;}
#line 1706 "npr.tab.c"
    break;

  case 47: /* PARAM: ID  */
#line 271 "npr.y"
              {yyval.c = yyvsp[0].c; yyval.counter = 0; yyval.default_value.clear(); declare_var(Let, yyvsp[0].c[0], yyvsp[0].row, yyvsp[0].col); }
#line 1712 "npr.tab.c"
    break;

  case 48: /* PARAM: ID '=' Expr  */
#line 272 "npr.y"
                      { yyval.c = yyvsp[-2].c; yyval.counter = 0; 
                        yyval.default_value = yyvsp[0].c; 
                        declare_var(Let, yyvsp[-2].c[0],  yyvsp[-2].row, yyvsp[-2].col); }
#line 1720 "npr.tab.c"
    break;

  case 50: /* ARGUMENTS: %empty  */
#line 277 "npr.y"
            {yyval.c.clear();}
#line 1726 "npr.tab.c"
    break;

  case 51: /* ARGs: ARGs ',' ARG  */
#line 279 "npr.y"
                    {yyval.c = yyvsp[-2].c + yyvsp[0].c; yyval.counter = yyvsp[-2].counter + yyvsp[0].counter; }
#line 1732 "npr.tab.c"
    break;

  case 52: /* ARGs: ARG  */
#line 280 "npr.y"
           {yyval.c = yyvsp[0].c; yyval.counter = yyvsp[0].counter; }
#line 1738 "npr.tab.c"
    break;

  case 53: /* ARG: Expr  */
#line 283 "npr.y"
           {yyval.c = yyvsp[0].c; yyval.counter = 1; }
#line 1744 "npr.tab.c"
    break;

  case 55: /* LVALUEPROP: Expr '[' Expr ']'  */
#line 288 "npr.y"
                               {yyval.c = yyvsp[-3].c + yyvsp[-1].c + "[@]"; }
#line 1750 "npr.tab.c"
    break;

  case 56: /* LVALUEPROP: Expr '[' Expr ']' '=' Expr  */
#line 289 "npr.y"
                                        {yyval.c = yyvsp[-5].c + yyvsp[-3].c + yyvsp[0].c + "[=]" ; }
#line 1756 "npr.tab.c"
    break;

  case 57: /* LVALUEPROP: Expr '.' LVALUE '=' Expr  */
#line 291 "npr.y"
                                      {yyval.c = yyvsp[-4].c + yyvsp[-2].c + yyvsp[0].c + "[=]" ;}
#line 1762 "npr.tab.c"
    break;

  case 58: /* LVALUEPROP: Expr '.' LVALUE  */
#line 292 "npr.y"
                             { yyval.c = yyvsp[-2].c + yyvsp[0].c + "[@]"; }
#line 1768 "npr.tab.c"
    break;

  case 59: /* Expr: LVALUE '=' '{' '}'  */
#line 295 "npr.y"
                           { check_symbol( yyvsp[-3].c[0], true ); yyval.c = yyvsp[-3].c + "{}" + "="; }
#line 1774 "npr.tab.c"
    break;

  case 60: /* Expr: LVALUE '=' Expr  */
#line 296 "npr.y"
                        { check_symbol( yyvsp[-2].c[0], true ); yyval.c = yyvsp[-2].c + yyvsp[0].c + "="; }
#line 1780 "npr.tab.c"
    break;

  case 61: /* Expr: Expr ST Expr  */
#line 297 "npr.y"
                     { yyval.c = yyvsp[-2].c + yyvsp[0].c + yyvsp[-1].c; }
#line 1786 "npr.tab.c"
    break;

  case 62: /* Expr: Expr GT Expr  */
#line 298 "npr.y"
                     { yyval.c = yyvsp[-2].c + yyvsp[0].c + yyvsp[-1].c; }
#line 1792 "npr.tab.c"
    break;

  case 63: /* Expr: Expr '+' Expr  */
#line 299 "npr.y"
                      { yyval.c = yyvsp[-2].c + yyvsp[0].c + yyvsp[-1].c; }
#line 1798 "npr.tab.c"
    break;

  case 64: /* Expr: Expr '-' Expr  */
#line 300 "npr.y"
                      { yyval.c = yyvsp[-2].c + yyvsp[0].c + yyvsp[-1].c; }
#line 1804 "npr.tab.c"
    break;

  case 65: /* Expr: Expr '*' Expr  */
#line 301 "npr.y"
                      { yyval.c = yyvsp[-2].c + yyvsp[0].c + yyvsp[-1].c; }
#line 1810 "npr.tab.c"
    break;

  case 66: /* Expr: Expr '/' Expr  */
#line 302 "npr.y"
                      { yyval.c = yyvsp[-2].c + yyvsp[0].c + yyvsp[-1].c; }
#line 1816 "npr.tab.c"
    break;

  case 67: /* Expr: Expr '%' Expr  */
#line 303 "npr.y"
                      { yyval.c = yyvsp[-2].c + yyvsp[0].c + yyvsp[-1].c; }
#line 1822 "npr.tab.c"
    break;

  case 68: /* Expr: Expr DOUBLE_EQUAL Expr  */
#line 304 "npr.y"
                               { yyval.c = yyvsp[-2].c + yyvsp[0].c + yyvsp[-1].c; }
#line 1828 "npr.tab.c"
    break;

  case 69: /* Expr: LVALUE PLUS_EQUAL Expr  */
#line 305 "npr.y"
                               { yyval.c = yyvsp[-2].c + get_var(yyvsp[-2].c[0], true) + "@" + yyvsp[0].c + "+" + "="; }
#line 1834 "npr.tab.c"
    break;

  case 70: /* Expr: DOUBLE_PLUS LVALUE  */
#line 306 "npr.y"
                           { vector<string> vec; yyval.c = vec + get_var(yyvsp[0].c[0], true) + "@" + "1" + "+" + "="; }
#line 1840 "npr.tab.c"
    break;

  case 71: /* Expr: LVALUE DOUBLE_PLUS  */
#line 307 "npr.y"
                           { vector<string> vec; yyval.c = vec + get_var(yyvsp[-1].c[0], true) + "@"; }
#line 1846 "npr.tab.c"
    break;

  case 73: /* Expr: '-' NUM  */
#line 309 "npr.y"
                {yyval.c = "0" + yyvsp[0].c + "-";}
#line 1852 "npr.tab.c"
    break;

  case 74: /* Expr: Expr '(' ARGUMENTS ')'  */
#line 310 "npr.y"
                               {yyval.c = yyvsp[-1].c + to_string(yyvsp[-1].counter) + yyvsp[-3].c + "$"; }
#line 1858 "npr.tab.c"
    break;

  case 76: /* Expr: LVALUE  */
#line 312 "npr.y"
                { vector<string> vec; yyval.c = vec + get_var(yyvsp[0].c[0], false) + "@"; }
#line 1864 "npr.tab.c"
    break;

  case 78: /* Expr: '(' Expr ')'  */
#line 314 "npr.y"
                     { yyval.c = yyvsp[-1].c; }
#line 1870 "npr.tab.c"
    break;

  case 79: /* Expr: '(' '{' '}' ')'  */
#line 315 "npr.y"
                        { yyval.c = vector<string>{"{}"}; }
#line 1876 "npr.tab.c"
    break;

  case 80: /* Expr: '{' '}'  */
#line 316 "npr.y"
                {yyval.c = vector<string>{"{}"}; }
#line 1882 "npr.tab.c"
    break;

  case 81: /* Expr: '[' ']'  */
#line 317 "npr.y"
                {yyval.c = vector<string>{"[]"}; }
#line 1888 "npr.tab.c"
    break;


#line 1892 "npr.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 324 "npr.y"


#include "lex.yy.c"

vector<string> declare_var( var_types type, string name, int row, int col) {
    /* cerr << "insert_symbol( " << type << ", " << name << ", " << row << ", " << col << ")" << endl; */

    auto& topo = table.back();

    if (topo.count(name) == 0) {
        topo[name] = symbol{type, row, col};
        return vector<string>{ name } + "&";
    }
    else if (type == Var && topo[name].type == Var) {
        topo[name] = symbol{type, row, col};
        return vector<string>{ };
    }
    else {
        cerr << "Erro: a variável '" << name << "' já foi declarada na linha " << topo[name].row + 1 << "." << endl;
        exit(1);
    }
}

string get_var (string &name, bool allowMod) {
  check_symbol(name, allowMod);
  string s = name;
  return s;
}

string get_field (string &name){
  string s = name;
  return s;
}

void check_symbol(string name, bool allowMod) {
    for (int i = table.size() - 1; i >= 0; i--) {
      auto& atual = table[i];
      if (atual.count(name) > 0) {
        if (allowMod && atual[name].type == Const) {
            cerr << "Variavel '" << name << "' não pode ser modificada." << endl;
            exit(1);
        }
        else {
          return;
        }
      }
    }
    cerr << "Erro: a variável '" << name << "' não foi declarada." << endl;
    exit(1);

}

void yyerror( const char* st ) {
    cerr << st << endl;
    cerr << "Perto de: " << yytext << ":" << row + 1 << ":" << col <<endl;
    exit(1);
}

int main() {
    yyparse();
  
    return 0;
}
