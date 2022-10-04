# include <iostream>
# include <stdio.h>
# include <vector>
# include <map>
# include <stdlib.h>
# include <string>
# include <stack>
# include <cmath> 
# include <sstream> // stringstream

using namespace std ;
int gTestNum = 0 ;
int gCountWhile = 0 ;
int gCountLine = 0 ;

# define DEFINE_ID_MIN_SIZE 3
# define ID_POSITION 0

enum Tokentype {
  IDENT, CONSTANT, 
  INT, FLOAT, CHAR, BOOL, STRING,
  VOID, IF, ELSE, WHILE, DO, RETURN, DONE, CIN, COUT,
  L_PARENTHESES, R_PARENTHESES, L_SQUARE, R_SQUARE, L_CURLY, R_CURLY, // () [] { }
  ADD, SUB, MULT, DIVIDE, MOD,       // + - * / %
  XOR, GREATER_THAN, LESS_THAN,      // ^ > <  
  GE, LE, EQ, NEQ,                   // ">="  "<="  "=="  "!="
  BITAND, BITOR, EQUAL, NOT,         // & | = !
  AND, OR, PE, ME, TE, DE, RE,       // "&&"  "||"  "+="  "-="  "*="  "/="  "%="  
  PP, MM, RS, LS,                    // "++"  "--"  ">>"  "<<"
  SEMICOLON, COMMA, QUESTION, COLON, // ; , ? : 
  COMMENT, UNRECOGNIZED
} ;

enum LineType { DEFINITION, STATEMENT } ;

enum ErrorType { UNEXPECTED_TOKEN, UNDEFINED_TOKEN, END_OF_PROGRAM, DIVIDEND_ZERO } ;

enum PrimitiveType { T_INT, T_FLOAT, T_CHAR, T_BOOL, T_STRING } ;

struct Token{
  string name ;
  Tokentype type ;
  int line ;
} ;

struct RefInfo{
  string name ;
  Tokentype type ;
  bool needReturn ;
} ;

struct DefinedIDInfo{
  Tokentype type ;
  string content ;
  int arraySize ;
  vector<string> *array ;
  string value ;
  int stackLayer ;
} ;

struct DefinedFunctionInfo{
  Tokentype type ;  // function的型別
  vector<RefInfo> *refnames ;  // 參數名稱&順序
  vector<Token> *content ;   // {...} 中的內容
  map< string, DefinedIDInfo > localIDTable ;   // function變數+參數列表 
} ;

vector< map< string, DefinedIDInfo > > * gStackOfID ;
map< string, DefinedIDInfo >::iterator gIterID ;
map< string, DefinedFunctionInfo > gFunctionTable ;
map< string, DefinedFunctionInfo >::iterator gIterF ;

class Parser {
  public:
  Token mBuffer ;
  vector<Token> mTokenList ;
  bool FindDefinedToken( string name ) ;
  bool FindDefinedFunction( string name ) ;
  bool IsReservedWords( string name ) ;
  void DefineGlobalVariables() ;
  void DefineGlobalFunction() ;
  bool SystemFunctions() ;
  bool User_Input() ;
  bool Definition() ;
  bool Type_Specifier() ; 
  bool Function_Definition_or_Declarators() ;
  bool Rest_of_Declarators() ;
  bool Function_Definition_Without_ID() ;
  bool Formal_Parameter_List() ;
  bool Compound_Statement() ;
  bool Declaration() ;
  bool Statement() ;
  bool Expression() ;
  bool Basic_Expression() ;
  bool Rest_of_Identifier_Started_Basic_Exp() ;
  bool Rest_of_PPMM_Identifier_Started_Basic_Exp() ;
  bool Sign() ;
  bool Actual_Parameter_List() ;
  bool Assignment_Operator() ;
  bool Romce_and_Romloe() ;
  bool Rest_of_Maybe_Logical_OR_Exp() ;
  bool Maybe_Logical_AND_Exp() ; 
  bool Rest_of_Maybe_Logical_AND_Exp() ;
  bool Maybe_Bit_OR_Exp() ; 
  bool Rest_of_Maybe_Bit_OR_Exp() ;
  bool Maybe_Bit_Ex_OR_Exp() ; 
  bool Rest_of_Maybe_Bit_Ex_OR_Exp() ;
  bool Maybe_Bit_AND_Exp() ;
  bool Rest_of_Maybe_Bit_AND_Exp() ;
  bool Maybe_Equality_Exp() ;
  bool Rest_of_Maybe_Equality_Exp() ;
  bool Maybe_Relational_Exp() ; 
  bool Rest_of_Maybe_Relational_Exp() ; 
  bool Maybe_Shift_Exp() ;
  bool Rest_of_Maybe_Shift_Exp() ;
  bool Maybe_Additive_Exp() ;
  bool Rest_of_Maybe_Additive_Exp() ; 
  bool Maybe_Mult_Exp() ;
  bool Rest_of_Maybe_Mult_Exp() ; 
  bool Unary_Exp() ;
  bool Signed_Unary_Exp() ;
  bool Unsigned_Unary_Exp() ;

} ;

int gLine = 0 ;
bool gKeepBuffer = false ;

// --------------------------------- GetToken ---------------------------------------------------

bool IsDigit( char ch ) {
  if ( ch >= '0' && ch <= '9' )
    return true ;
  else
    return false ;
} // IsDigit()

bool IsLetter( char ch ) {
  if ( ( ch >= 'a' && ch <= 'z' ) || ( ch >= 'A' && ch <= 'Z' ) )
    return true ;
  else
    return false ;
} // IsLetter()

bool IsDelimiter( char ch ) {
  if ( ch == '(' || ch == ')' || ch == '[' || ch == ']' ||
       ch == '{' || ch == '}' || ch == '+' || ch == '-' ||
       ch == '*' || ch == '/' || ch == '%' || ch == '^' ||
       ch == '>' || ch == '<' || ch == '=' || ch == '!' || 
       ch == '&' || ch == '|' || ch == ';' || ch == ',' ||
       ch == ':' || ch == '?' )
    return true ;
  else
    return false ;
} // IsDelimiter()

Token GetNumConstant() {
  Token token ;
  char ch = '\0' ;
  ch = cin.peek() ;
  if ( ch == '.' ) {                // Constant start with point
    ch = cin.get() ;
    token.name += ch ;
    ch = cin.peek() ;
    if ( IsDigit( ch ) ) {
      while ( IsDigit( ch ) ) {
        ch = cin.get() ;
        token.name += ch ;
        ch = cin.peek() ;
      } // while

      token.type = CONSTANT ;
    } // end if
    else {                          // Case: "."
      token.type = UNRECOGNIZED ;
    } // else
  } // if
  else {                            // Constant start with digit
    bool isPointAppear = false ;

    while ( IsDigit( ch ) || ch == '.' ) {
      if ( ch == '.' ) {
        if ( isPointAppear == true ) { // Duplicate point
          token.type = CONSTANT ;
          return token ;
        } // if
        else {
          isPointAppear = true ;
        } // else
      } // end if

      ch = cin.get() ;
      token.name += ch ;
      ch = cin.peek() ;
    } // end while

    token.type = CONSTANT ;
  } // else

  return token ;
} // GetNumConstant()

// 目前沒有存 double quote，只存 double quote裡的字串   // 改成有存
Token GetStringConstant() {
  Token token ;
  char ch = '\0' ;
  ch = cin.get() ; // get '"' ( double quote )
  token.name += ch ;
  ch = cin.peek() ;
  while ( ch != '\"' ) {
    if ( ch == '\n' )
      gLine++ ;

    ch = cin.get() ;
    token.name += ch ;
    ch = cin.peek() ;
  } // while

  ch = cin.get() ;
  token.name += ch ;
  token.type = CONSTANT ;
  return token ;
} // GetStringConstant()

Token GetChar() {
  Token token ;
  char ch = '\0' ;
  ch = cin.get() ; // get ''' ( single quote )
  token.name += ch ;

  ch = cin.peek() ;
  while ( ch != '\'' ) {
    if ( ch == '\n' )
      gLine++ ;
    ch = cin.get() ;
    token.name += ch ;
    ch = cin.peek() ;
  } // while

  ch = cin.get() ; // get ''' ( single quote )
  token.name += ch ;
  token.type = CONSTANT ;

  return token ; 
} // GetChar()

// Get ID, INT, FLOAT, CHAR, BOOL, STRING, VOID, IF, ELSE, WHILE, DO ,RETURN, 
// CONSTANT( true, flase )
Token GetID() {
  Token token ;
  char ch = '\0' ;
  ch = cin.get() ;
  token.name += ch ;

  ch = cin.peek() ;

  while ( IsLetter( ch ) || IsDigit( ch ) || ch == '_' ) {
    ch = cin.get() ;
    token.name += ch ;
    ch = cin.peek() ;
  } // while

  if ( token.name == "int" )
    token.type = INT ;
  else if ( token.name == "float" )
    token.type = FLOAT ;
  else if ( token.name == "char" ) 
    token.type = CHAR ;
  else if ( token.name == "bool" ) 
    token.type = BOOL ;
  else if ( token.name == "string" ) 
    token.type = STRING ;
  else if ( token.name == "void" ) 
    token.type = VOID ;
  else if ( token.name == "if" ) 
    token.type = IF ;
  else if ( token.name == "else" ) 
    token.type = ELSE ;
  else if ( token.name == "while" ) 
    token.type = WHILE ;
  else if ( token.name == "do" ) 
    token.type = DO ;
  else if ( token.name == "return" ) 
    token.type = RETURN ;
  else if ( token.name == "true" || token.name == "false" ) 
    token.type = CONSTANT ;
  else if ( token.name == "cin" )
    token.type = CIN ;
  else if ( token.name == "cout" ) 
    token.type = IDENT ;
  else if ( token.name == "Done" ) 
    token.type = DONE ;
  else {
    token.type = IDENT ;
  } // else

  return token ;
} // GetID()

Token GetDelimiter() {
  Token token ;
  char ch = '\0' ; 
  ch = cin.peek() ;

  if ( ch == '>' ) {
    ch = cin.get() ;
    token.name += ch ;
    ch = cin.peek() ;
    if ( ch == '=' ) {      // Case: ">="
      ch = cin.get() ;
      token.name += ch ;
      token.type = GE ;
    } // if
    else if ( ch == '>' ) { // Case: ">>"
      ch = cin.get() ;
      token.name += ch ;
      token.type = RS ;
    } // else if
    else {
      token.type = GREATER_THAN ;
    } // end else
  } // if

  else if ( ch == '<' ) { // Case: "<=" or "<<" or "<"
    ch = cin.get() ;
    token.name += ch ;
    ch = cin.peek() ;
    if ( ch == '=' ) {        
      ch = cin.get() ;
      token.name += ch ;
      token.type = LE ;
    } // if
    else if ( ch == '<' ) {  
      ch = cin.get() ;
      token.name += ch ;
      token.type = LS ;
    } // else if
    else {
      token.type = LESS_THAN ;
    } // else
  } // else if

  else if ( ch == '=' ) { // Case: "=="  or "="
    ch = cin.get() ;
    token.name += ch ;
    ch = cin.peek() ;
    if ( ch == '=' ) { 
      ch = cin.get() ;
      token.name += ch ;
      token.type = EQ ;
    } // if
    else {
      token.type = EQUAL ;
    } // else
  } // else if

  else if ( ch == '!' ) { // Case: "!=" or "!"
    ch = cin.get() ;
    token.name += ch ;
    ch = cin.peek() ;
    if ( ch == '=' ) { 
      ch = cin.get() ;
      token.name += ch ;
      token.type = NEQ ;
    } // if
    else {
      token.type = NOT ;
    } // else
  } // else if

  else if ( ch == '&' ) { // Case: "&&" or "&"
    ch = cin.get() ;
    token.name += ch ;
    ch = cin.peek() ;
    if ( ch == '&' ) { 
      ch = cin.get() ;
      token.name += ch ;
      token.type = AND ; 
    } // if
    else {
      token.type = BITAND ;
    } // else
  } // else if

  else if ( ch == '|' ) { // Case: "||" or "|"
    ch = cin.get() ;
    token.name += ch ;
    ch = cin.peek() ;
    if ( ch == '|' ) {
      ch = cin.get() ;
      token.name += ch ;
      token.type = OR ; 
    } // if
    else {
      token.type = BITOR ; 
    } // else
  } // else if
  
  else if ( ch == '+' ) { // Case: "++" or "+=" or "+"
    ch = cin.get() ;
    token.name += ch ;
    ch = cin.peek() ;
    if ( ch == '+' ) {
      ch = cin.get() ;
      token.name += ch ;
      token.type = PP ;
    } // if
    else if ( ch == '=' ) {
      ch = cin.get() ;
      token.name += ch ;
      token.type = PE ;
    } // else if
    else {
      token.type = ADD ;
    } // else
  } // else if

  else if ( ch == '-' ) { // Case: "--" or "-=" or "-"
    ch = cin.get() ;
    token.name += ch ;
    ch = cin.peek() ;
    if ( ch == '-' ) {
      ch = cin.get() ;
      token.name += ch ;
      token.type = MM ;
    } // if
    else if ( ch == '=' ) {
      ch = cin.get() ;
      token.name += ch ;
      token.type = ME ;
    } // else if
    else {
      token.type = SUB ;
    } // else
  } // else if

  else if ( ch == '*' ) { // Case: "*=" or "*"
    ch = cin.get() ;
    token.name += ch ;
    ch = cin.peek() ;
    if ( ch == '=' ) {
      ch = cin.get() ;
      token.name += ch ;
      token.type = TE ;
    } // if
    else {
      token.type = MULT ;
    } // else
  } // else if

  else if ( ch == '/' ) { // Case: "/=" or "//" or "/"
    ch = cin.get() ;
    token.name += ch ;
    ch = cin.peek() ;
    if ( ch == '=' ) {
      ch = cin.get() ;
      token.name += ch ;
      token.type = DE ;
    } // if
    else if ( ch == '/' ) {
      ch = cin.get() ;
      token.name += ch ;
      token.type = COMMENT ;
    } // else if
    else {
      token.type = DIVIDE ;
    } // else
  } // else if

  else if ( ch == '%' ) { // Case: "%=" or "%"
    ch = cin.get() ;
    token.name += ch ;
    ch = cin.peek() ;
    if ( ch == '=' ) {
      ch = cin.get() ;
      token.name += ch ;
      token.type = RE ;
    } // if
    else {
      token.type = MOD ;
    } // else
  } // else if

  else {
    ch = cin.get() ;
    token.name += ch ;
    if ( ch == '(' )
      token.type = L_PARENTHESES ;
    else if ( ch == ')' )
      token.type = R_PARENTHESES ;
    else if ( ch == '[' )
      token.type = L_SQUARE ;
    else if ( ch == ']' )
      token.type = R_SQUARE ;
    else if ( ch == '{' )
      token.type = L_CURLY ;
    else if ( ch == '}' )
      token.type = R_CURLY ;
    else if ( ch == '^' )
      token.type = XOR ;
    else if ( ch == ';' )
      token.type = SEMICOLON ;
    else if ( ch == ',' )
      token.type = COMMA ;
    else if ( ch == '?' )
      token.type = QUESTION ;
    else if ( ch == ':' )
      token.type = COLON ;
  } // else
  

  return token ;
} // GetDelimiter()

Token GetToken() {
  Token token ;
  char ch = '\0' ;
  ch = cin.peek() ;
  
  while ( ch == ' ' || ch == '\n' || ch == '\t' ) { // white space
    if ( ch == '\n' ) 
      gLine++ ;

    ch = cin.get() ;
    ch = cin.peek() ;
  } // while

  if ( IsDigit( ch ) || ch == '.' ) {
    token = GetNumConstant() ;        
  } // if
  else if ( ch == '\"' ) {
    token = GetStringConstant() ;
  } // else if
  else if ( ch == '\'' ) {
    token = GetChar() ;
  } // else if
  else if ( IsLetter( ch ) ) {
    token = GetID() ;
  } // else if
  else if ( IsDelimiter( ch ) ) {
    token = GetDelimiter() ;
    if ( token.type == COMMENT ) {
      ch = cin.peek() ;
      while ( ch != '\n' ) {
        ch = cin.get() ;
        ch = cin.peek() ;
      } // while

      token = GetToken() ;
    } // if
  } // else if
  else {
    ch = cin.get() ;
    token.name += ch ;
    token.type = UNRECOGNIZED ;
  } // else
  
  token.line = gLine ;
  return token ;
} // GetToken()

// Skip line-comment 
void GetGarbage() {
  char ch = '\0' ;
  ch = cin.peek() ;               
  while ( ch != '\n' ) {
    ch = cin.get() ;
    ch = cin.peek() ;
  } // end while

  ch = cin.get() ;                 // read the '\n' ( end of comment )
} // end GetGarbage()


// -------------------------------------------- Evaluate --------------------------------------------

class Evaluator {
  public:

  int OperatorPriority( string op ) { 
    int priority = 0 ;
    if ( op == "(" || op == ")" )
      priority = 1 ;
    else if ( op == "&&" || op == "||" )
      priority = 2 ;
    else if ( op == "==" || op == "!=" || op == ">" || op == "<" || op == ">=" || op == "<=" ) 
      priority = 3 ;
    else if ( op == "+" || op == "-" )
      priority = 4 ;
    else if ( op == "*" || op == "/" || op == "%" )
      priority = 5 ;
    else if ( op == ">>" || op == "<<" )
      priority = 6 ;
      
    return priority ;
  } // OperatorPriority()

  // Check if string is a float
  bool IsFloat( string sstr ) {
    bool isFloat = false ;
    for ( int i = 0 ; i < sstr.size() ; i++ ) {
      if ( sstr[i] == '.' || ( sstr[i] == 'e' && sstr[i-1] == '1' ) ) {
        isFloat = true ;
      } // end if
    } // end for

    return isFloat ;
  } // end IsFloat()

  bool IsBoolean( string sstr ) {
    if ( sstr == "true" || sstr == "false" )
      return true ;
    else
      return false ;
  } // IsBoolean()

  bool FindDefinedFunction( string name ) {  
    // , float &value 
    map< string, DefinedFunctionInfo >::iterator gIterF ;
    gIterF = gFunctionTable.find( name ) ;      // 找ID是否被defined
    if ( gIterF == gFunctionTable.end() )       // 找不到
      return false;
    else          
      return true;

  } // FindDefinedFunction()

  // Convert float to string, return a string
  string FloatToString( float fNum ) {
    string sResult = "\0" ;
    char cTemp[50] ;
    sprintf( cTemp, "%f", fNum ) ;
    for ( int i = 0; i < sizeof( cTemp ) && cTemp[i] != '\0' ; i++ ) {
      sResult = sResult + cTemp[i] ;
    } // end for

    return sResult ;
  } // end FloatToString()

  string IntToString( int iNum ) {
    stringstream ssResult ;
    string sResult = "\0" ;
    ssResult << iNum ;
    sResult = ssResult.str() ;
    return sResult ;
  } // end IntToString()

  // input: float, output: string sResult
  // Check whether the result of a division operation is a float or an integer
  string IntOrFloat( float fNum ) {
    bool isInt = true ;
    stringstream ssResult ;
    string sResult = "\0" ;

    if ( fNum - ( int ) fNum == 0 ) { // is Int
      int iNum = 0 ;
      iNum = ( int ) fNum ;
      ssResult << iNum ;
    } // end if  
    else {                        // is Float
      ssResult << fNum ;
    } // end else

    sResult = ssResult.str() ;

    return sResult ;
  } // end IntOrFloat()

  bool IsOperator( Token token ) {
    if ( token.type == ADD    || token.type == SUB || token.type == MULT || 
         token.type == DIVIDE || token.type == MOD || token.type == XOR  ||
         token.type == GREATER_THAN || token.type == LESS_THAN || token.type == GE ||
         token.type == LE || token.type == EQ || token.type == NEQ || 
         token.type == RS || token.type == LS || token.type == L_PARENTHESES || 
         token.type == COLON || token.type == COMMA || token.type == QUESTION ) 
      return true ;
    else
      return false ;
  } // IsOperator()

  string GetStrContent( string str ) {
    if ( str.size() >= 2 ) {
      str.erase( str.begin() ) ;
      str.erase( str.end()-1 ) ;
    } // if
    
    return str ;
  } // GetStrContent()

  bool IsAssignOpt( string opt ) {
    if ( opt == "=" || opt == "+=" || opt == "-=" || 
         opt == "*=" || opt == "/=" || opt == "%=" )
      return true ;
    else 
      return false ;
  } // IsAssignOpt()

  // Determines the result's type
  PrimitiveType TypePriority( PrimitiveType type1, PrimitiveType type2 ) {
    PrimitiveType priority ;

    if ( type1 == T_STRING || type2 == T_STRING ) 
      priority = T_STRING ;
    else if ( type1 == T_FLOAT || type2 == T_FLOAT )
      priority = T_FLOAT ;
    else 
      priority = type1 ;
    
    return priority ;  
    
  } // TypePriority()

  PrimitiveType WhichType( string value ) {
    PrimitiveType type ;

    if ( value[0] == '\"' )
      type = T_STRING ;
    else if ( value[0] == '\'' )
      type = T_CHAR ;
    else if ( IsFloat( value ) )
      type = T_FLOAT ;
    else if ( IsBoolean( value ) )
      type = T_BOOL ;
    else
      type = T_INT ;
    
    return type ;
  } // WhichType()

  string Calculate( vector<Token> postfix ) {
    PrimitiveType type ;
    string sResult = "\0" ;
    float fResult = 0.0 ;
    float fDifference = 0.0 ; // 誤差
    int iResult = 0 ;
    stack<Token> operationStack ;
    Token operand1, operand2 ;
    Token op ;

    for ( int i = 0 ; i < postfix.size() ; i++ ) { 
      if ( postfix[i].type == CONSTANT || postfix[i].type == IDENT || postfix[i].type == NOT ) {
        operationStack.push( postfix[i] ) ;
      } // end if
      else { // Take the top two operands from the stack
        operand2 = operationStack.top() ;
        operationStack.pop() ;
        operand1 = operationStack.top() ;
        operationStack.pop() ;
        
        if ( postfix[i].type == ADD ) {
          fResult = 0.0 ;
          sResult = "\0" ;
          iResult = 0 ;

          type = TypePriority( WhichType( operand1.name ), WhichType( operand2.name ) ) ;
          if ( type == T_STRING ) {
            if ( WhichType( operand1.name ) == T_STRING || WhichType( operand1.name ) == T_CHAR )
              sResult = "\"" + GetStrContent( operand1.name ) ;
            else
              sResult = "\"" + operand1.name ;
            
            if ( WhichType( operand2.name ) == T_STRING || WhichType( operand2.name ) == T_CHAR )
              sResult = sResult + GetStrContent( operand2.name ) + "\"" ;
            else
              sResult = sResult + operand2.name + "\"" ;     
          } // if
          else if ( type == T_FLOAT ) {
            fResult = atof( operand1.name.c_str() ) + atof( operand2.name.c_str() ) ;
            sResult = FloatToString( fResult ) ;
          } // else if
          else if ( type == T_INT ) {
            iResult = atoi( operand1.name.c_str() ) + atoi( operand2.name.c_str() ) ;
            sResult = IntToString( iResult ) ;                  
          } // else if
          else if ( type == T_CHAR ) {
            sResult = "\'" + GetStrContent( operand1.name ) + GetStrContent( operand2.name ) + "\'" ;
          } // else if
          else { // BOOL, but... does bool have an add operation?
            if ( operand1.name == "true" || operand1.name == "1" )
              iResult++ ;
            
            if ( operand2.name == "true" || operand2.name == "1" )
              iResult++ ;
            
            sResult = IntToString( iResult ) ; 

          } // else   

          op.name = sResult ;
          op.type = CONSTANT ;
          operationStack.push( op ) ;
        } // end if
        else if ( postfix[i].type == SUB ) {
          fResult = 0.0 ;
          sResult = "\0" ;
          iResult = 0 ;
          if ( IsFloat( operand1.name ) || IsFloat( operand2.name ) ) {
            fResult = atof( operand1.name.c_str() ) - atof( operand2.name.c_str() ) ;
            sResult = FloatToString( fResult ) ;
            op.name = sResult ;
            op.type = CONSTANT ;
            operationStack.push( op ) ;
          } // end if
          else {
            iResult = atoi( operand1.name.c_str() ) - atoi( operand2.name.c_str() ) ;
            sResult = IntToString( iResult ) ;
            op.name = sResult ;
            op.type = CONSTANT ;
            operationStack.push( op ) ;
          } // end else
        } // end else if
        else if ( postfix[i].type == MULT ) {
          fResult = 0.0 ;
          sResult = "\0" ;
          iResult = 0 ;   
          if ( ( operand1.type == NOT && ( operand2.name == "true" || operand2.name == "false" ) ) ||
               ( operand2.type == NOT && ( operand1.name == "true" || operand1.name == "false" ) ) ) {
            if ( operand1.type == NOT ) {
              if ( operand2.name == "true" )
                sResult = "false" ;
              else if ( operand2.name == "false" )
                sResult = "true" ;

            } // if
            else {                                  // operand2.type == NOT
              if ( operand1.name == "true" )
                sResult = "flase" ;
              else if ( operand1.name == "false" )
                sResult = "true" ;

            } // else
          } // if    
          else if ( IsFloat( operand1.name ) || IsFloat( operand2.name ) ) {
            fResult = 0.0 ;
            fResult = atof( operand1.name.c_str() ) * atof( operand2.name.c_str() ) ;
            sResult = "\0" ;
            sResult = FloatToString( fResult ) ;
          } // end else if
          else {
            iResult = atoi( operand1.name.c_str() ) * atoi( operand2.name.c_str() ) ;
            sResult = IntToString( iResult ) ;
          } // end else

          op.name = sResult ;
          op.type = CONSTANT ;
          operationStack.push( op ) ;
        } // end else if
        else if ( postfix[i].type == DIVIDE ) {   
          if ( atof( operand2.name.c_str() ) == 0.0 ) {
            throw DIVIDEND_ZERO ; 
          } // end if

          fResult = 0.0 ;
          sResult = "\0" ;
          iResult = 0 ; 

          if ( IsFloat( operand1.name ) || IsFloat( operand2.name ) ) {
            fResult = atof( operand1.name.c_str() ) / atof( operand2.name.c_str() ) ;
            sResult = FloatToString( fResult ) ;
          } // end if
          else {
            iResult = atoi( operand1.name.c_str() ) / atoi( operand2.name.c_str() ) ;
            sResult = IntOrFloat( iResult ) ;
          } // end else

          op.name = sResult ;
          op.type = CONSTANT ;
          operationStack.push( op ) ;
        } // end else if
        else if ( postfix[i].type == MOD ) {  
          if ( atof( operand2.name.c_str() ) == 0.0 ) 
            throw DIVIDEND_ZERO ; 
          sResult = "\0" ;
          iResult = 0 ;        
          iResult = atoi( operand1.name.c_str() ) % atoi( operand2.name.c_str() ) ;
          sResult = IntToString( iResult ) ;
          op.name = sResult ;
          op.type = CONSTANT ;
          operationStack.push( op ) ;
        } // else if
        else if ( postfix[i].type == EQ ) {
          fDifference = 0.0 ;
          fDifference = atof( operand1.name.c_str() ) - atof( operand2.name.c_str() ) ;
          if ( fDifference <= 0.0001 && fDifference >= -0.0001 ) {
            op.name = "true" ;
            op.type = CONSTANT ;
            operationStack.push( op ) ;
          } // end if
          else {
            op.name = "false" ;
            op.type = CONSTANT ;
            operationStack.push( op ) ;
          } // end else
        } // end else if
        else if ( postfix[i].type == NEQ ) {
          fDifference = 0.0 ;
          fDifference = atof( operand1.name.c_str() ) - atof( operand2.name.c_str() ) ;
          if ( fDifference <= 0.0001 && fDifference >= -0.0001 ) {
            op.name = "false" ;
            op.type = CONSTANT ;
            operationStack.push( op ) ;
          } // end if
          else {
            op.name = "true" ;
            op.type = CONSTANT ;
            operationStack.push( op ) ;
          } // end else
        } // end else if
        else if ( postfix[i].type == GREATER_THAN ) {
          fDifference = 0.0 ;
          fDifference = atof( operand1.name.c_str() ) - atof( operand2.name.c_str() ) ;
          if ( fDifference > 0.0001 ) {
            op.name = "true" ;
          } // end if
          else {
            op.name = "false" ;
          } // end else

          op.type = CONSTANT ;
          operationStack.push( op ) ;
        } // end else if
        else if ( postfix[i].type == LESS_THAN ) {
          fDifference = 0.0 ;
          fDifference = atof( operand1.name.c_str() ) - atof( operand2.name.c_str() ) ;
          if ( fDifference < -0.0001 ) {
            op.name = "true" ;
          } // end if
          else {
            op.name = "false" ;
          } // end else

          op.type = CONSTANT ;
          operationStack.push( op ) ;
        } // end else if
        else if ( postfix[i].type == GE ) {
          fDifference = 0.0 ;
          fDifference = atof( operand1.name.c_str() ) - atof( operand2.name.c_str() ) ;
          if ( fDifference >= -0.0001 ) {
            op.name = "true" ;
          } // end if
          else {
            op.name = "false" ;
          } // end else

          op.type = CONSTANT ;
          operationStack.push( op ) ;
        } // end else if
        else if ( postfix[i].type == LE ) {
          fDifference = 0.0 ;
          fDifference = atof( operand1.name.c_str() ) - atof( operand2.name.c_str() ) ;
          if ( fDifference <= 0.0001 ) {
            op.name = "true" ;
          } // end if
          else {
            op.name = "false" ;
          } // end else

          op.type = CONSTANT ;
          operationStack.push( op ) ;
        } // end else if
        else if ( postfix[i].type == AND ) {
          fResult = 0.0 ;
          sResult = "\0" ;
          iResult = 0 ;
          if ( IsBoolean( operand1.name ) || IsBoolean( operand2.name ) ) {
            if ( operand1.name == "true" && operand2.name == "true" )
              sResult = "true" ;
            else
              sResult = "false" ;
          } // if
          else if ( IsFloat( operand1.name ) || IsFloat( operand2.name ) ) {
            if ( atof( operand1.name.c_str() ) == 0 || atof( operand2.name.c_str() ) == 0 ) 
              fResult = 0 ;
            else 
              fResult = 1 ;
            sResult = FloatToString( fResult ) ;
          } // else if
          else {
            if ( atoi( operand1.name.c_str() ) == 0 || atoi( operand2.name.c_str() ) == 0 )
              iResult = 0 ;
            else 
              iResult = 1 ;
            sResult = IntToString( iResult ) ;
          } // end else

          op.name = sResult ;
          op.type = CONSTANT ;
          operationStack.push( op ) ;
        } // end else if
        else if ( postfix[i].type == OR ) {
          fResult = 0.0 ;
          sResult = "\0" ;
          iResult = 0 ;
          if ( IsBoolean( operand1.name ) || IsBoolean( operand2.name ) ) {
            if ( atoi( operand1.name.c_str() ) != 0 || operand1.name == "true" ||
                 atoi( operand2.name.c_str() ) != 0 || operand2.name == "true" )
              sResult = "true" ;
            else
              sResult = "false" ;
          } // if
          else if ( IsFloat( operand1.name ) || IsFloat( operand2.name ) ) {
            if ( atof( operand1.name.c_str() ) != 0 || atof( operand2.name.c_str() ) != 0 ) 
              fResult = 1 ;
            else 
              fResult = 0 ;
            sResult = FloatToString( fResult ) ;
          } // else if
          else {
            if ( atoi( operand1.name.c_str() ) != 0 || atoi( operand2.name.c_str() ) != 0 )
              iResult = 1 ;
            else 
              iResult = 0 ;
            sResult = IntToString( iResult ) ;
          } // end else

          op.name = sResult ;
          op.type = CONSTANT ;
          operationStack.push( op ) ;
        } // end else if
        else if ( postfix[i].type == RS ) {
          sResult = "\0" ;    
          fResult = atof( operand1.name.c_str() ) / pow( 2, atoi( operand2.name.c_str() ) ) ;
          iResult = floor( fResult ) ;   
          sResult = IntToString( iResult ) ;
          op.name = sResult ;
          op.type = CONSTANT ;
          operationStack.push( op ) ;
        } // else if
        else if ( postfix[i].type == LS ) {
          sResult = "\0" ;
          iResult = 0 ;       
          iResult = atoi( operand1.name.c_str() ) * pow( 2, atoi( operand2.name.c_str() ) ) ;
          sResult = IntToString( iResult ) ;
          op.name = sResult ;
          op.type = CONSTANT ;
          operationStack.push( op ) ;
        } // else if
      } // end else
    } // end for

    return operationStack.top().name ;
    
  } // end Calculate()

  void CalDefine( string &result, string opt, string defIDName, string arrayIndex ) {
    string defValue = "\0" ;
    Tokentype defIDType ;
    float fResult = 0.0 ;
    int iResult = 0 ;
    int index = atoi( arrayIndex.c_str() ) ;

    if ( gStackOfID->size() > 0 ) {
      defIDType = gStackOfID->at( gStackOfID->size()-1 )[defIDName].type ;
      if ( index >= 0 )
        defValue = gStackOfID->at( gStackOfID->size()-1 )[defIDName].array->at( index ) ;
      else
        defValue = gStackOfID->at( gStackOfID->size()-1 )[defIDName].value ;
    } // if

    if ( opt == "=" || opt == "<<" ) {
      if ( defIDType == INT ) {
        iResult = atoi( result.c_str() ) ;
        result = IntToString( iResult ) ;
      } // if
      else if ( defIDType == FLOAT ) {
        fResult = atof( result.c_str() ) ;
        result = FloatToString( fResult ) ;
      } // else if
      else if ( defIDType == BOOL ) {
        if ( result == "true" || result == "false" )
          result = result ;
        else {
          iResult = atoi( result.c_str() ) ;
          if ( iResult != 0 )
            result = "1" ;
          else 
            result = "0" ;
        } // else
      } // else if
      else if ( defIDType == STRING )
        result = "\"" + GetStrContent( result ) + "\"" ;
      else if ( defIDType == CHAR )
        result = "\'" + GetStrContent( result ) + "\'" ;

      if ( opt == "<<" ) {
        if ( result[0] == '\"' || result[0] == '\'' )
          result = GetStrContent( result ) ;
        for ( int j = 0 ; j < result.size() ; j++ ) {
          if ( result[j] == '\\' && j + 1 < result.size() ) {
            if ( result[j+1] == 'n' )
              cout << "\n" ;
            else if ( result[j+1] == 't' )
              cout << "\t" ;
            j++ ;
          } // if
          else
            cout << result[j] ;
        } // for
      } // if
    } // if
    else if ( opt == "+=" ) {
      if ( defIDType == INT ) {
        iResult = atoi( defValue.c_str() ) + atoi( result.c_str() ) ;
        result = IntToString( iResult ) ;
      } // if
      else if ( defIDType == FLOAT ) {
        fResult = atof( defValue.c_str() ) + atof( result.c_str() ) ;
        result = FloatToString( fResult ) ;
      } // else if
      else if ( defIDType == STRING )
        result = "\"" + GetStrContent( defValue ) + GetStrContent( result ) + "\"" ;
      else if ( defIDType == CHAR )
        result = "\'" + GetStrContent( defValue ) + GetStrContent( result ) + "\'" ;
    } // else if
    else if ( opt == "-=" ) {
      if ( defIDType == INT ) {
        iResult = atoi( defValue.c_str() ) - atoi( result.c_str() ) ;
        result = IntToString( iResult ) ;
      } // if
      else if ( defIDType == FLOAT ) {
        fResult = atof( defValue.c_str() ) - atof( result.c_str() ) ;
        result = FloatToString( fResult ) ;
      } // else if
    } // else if
    else if ( opt == "*=" ) {
      if ( defIDType == INT ) {
        iResult = atoi( defValue.c_str() ) * atoi( result.c_str() ) ;
        result = IntToString( iResult ) ;
      } // if
      else if ( defIDType == FLOAT ) {
        fResult = atof( defValue.c_str() ) * atof( result.c_str() ) ;
        result = FloatToString( fResult ) ;
      } // else if
    } // else if
    else if ( opt == "/=" ) {
      if ( atof( result.c_str() ) == 0 )
        throw DIVIDEND_ZERO ;
      if ( defIDType == INT ) {
        iResult = atoi( defValue.c_str() ) / atoi( result.c_str() ) ;
        result = IntToString( iResult ) ;
      } // if
      else if ( defIDType == FLOAT ) {
        fResult = atof( defValue.c_str() ) / atof( result.c_str() ) ;
        result = FloatToString( fResult ) ;
      } // else if
    } // else if
    else if ( opt == "%=" ) {
      if ( atoi( result.c_str() ) == 0 )
        throw DIVIDEND_ZERO ;
      if ( defIDType == INT ) {
        iResult = atoi( defValue.c_str() ) / atoi( result.c_str() ) ;
        result = IntToString( iResult ) ;
      } // if
    } // else if

    if ( gStackOfID->size() > 0 ) {
      if ( index >= 0 )
        gStackOfID->at( gStackOfID->size()-1 )[defIDName].array->at( index ) = result ;
      else
        gStackOfID->at( gStackOfID->size()-1 )[defIDName].value = result ; 
    } // if

  } // end CalDefine()

  vector<Token> InfixToPostfix( vector<Token> mTokenVec ) {
    vector<Token> postfix ;
    stack<Token> operatorStack ;
    Token tempToken ;
    string value = "\0" ;
    string result = "\0" ;
    int iValue = 0, tSize = 0 ;
    int arrayIndex = 0 ;
    int countP = 0 ;
    int start = 0, end = 0 ;
    string tName = "\0" ;
    bool isOutPar = true ;    

    for ( int findOP = 0 ; findOP < mTokenVec.size() && isOutPar ; findOP++ ) {
      if ( mTokenVec[findOP].type == L_PARENTHESES )
        countP++ ;
      else if ( mTokenVec[findOP].type == R_PARENTHESES )
        countP-- ;

      if ( countP <= 0 && findOP < mTokenVec.size() - 1 )
        isOutPar = false ;    // count<=0 代表不是最外面的()
    } // for

    if ( isOutPar && mTokenVec[0].type == L_PARENTHESES && 
         mTokenVec[mTokenVec.size()-1].type == R_PARENTHESES ) {
      mTokenVec.erase( mTokenVec.begin() ) ;
      mTokenVec.erase( mTokenVec.end()-1 ) ;
    } // if

    countP = 0 ;
    for ( int i = 0 ; i < mTokenVec.size() ; i++ ) {
      if ( mTokenVec[i].type == L_PARENTHESES )
        countP++ ;
      else if ( mTokenVec[i].type == R_PARENTHESES )
        countP-- ;

      if ( FindDefinedFunction( mTokenVec[i].name ) ) {
        map< string, DefinedFunctionInfo >::iterator tempIterF ;
        map<string, DefinedIDInfo>::iterator tempIterID ;
        RefInfo tempRef ;
        Token tempToken ;
        DefinedIDInfo tempID ;
        vector<Token> refList ; 
        string refValue = "\0" ;
        tempIterF = gFunctionTable.find( mTokenVec[i].name ) ;
        gStackOfID->push_back( gStackOfID->at( gStackOfID->size()-1 ) ) ;

        // 處理參數
        int start = i, countRef = 0 ;
        for ( int j = i ; j+1 < mTokenVec.size() && mTokenVec[j].type != R_PARENTHESES ; j++ ) {
          if ( mTokenVec[j].type == L_PARENTHESES ) 
            start = j+1 ;
          else if ( mTokenVec[j+1].type == COMMA || mTokenVec[j+1].type == R_PARENTHESES ) {
            vector<Token> temp( mTokenVec.begin() + start, mTokenVec.begin() + j + 1 ) ;
            vector<Token> temppostfix = InfixToPostfix( temp ) ;
            if ( temp.size() == 1 )
              refList.push_back( temp[0] ) ;
            tempRef = tempIterF->second.refnames->at( countRef ) ;
            tempID.type = tempRef.type ;
            tempID.stackLayer = gStackOfID->size() - 1 ;
            tempID.arraySize = -1 ;
            tempID.value = Calculate( temppostfix ) ;
            gStackOfID->at( gStackOfID->size() - 1 )[tempRef.name] = tempID ;
            countRef++ ;
            start = j+2 ;
          } // else if
        } // for

        i = start-1 ;

        // 處理執行
        vector<Token> tempTokenList ;
        for ( int j = 0 ; j < tempIterF->second.content->size() ; j++ )
          tempTokenList.push_back( tempIterF->second.content->at( j ) ) ;
        result = Execute_Statement( tempTokenList ) ;

        for ( int j = 0 ; j < tempIterF->second.refnames->size() ; j++ ) {
          if ( tempIterF->second.refnames->at( j ).needReturn ) {
            tempRef = tempIterF->second.refnames->at( j ) ;
            refValue = gStackOfID->at( gStackOfID->size() - 1 )[tempRef.name].value ;
            gStackOfID->at( gStackOfID->size() - 2 )[refList[j].name].value = refValue ; 
          } // if
        } // for

        tempToken.name = result ;
        tempToken.type = CONSTANT ;
        postfix.push_back( tempToken ) ;
        LocalIdTablePopBack() ;
      } // if
      else if ( mTokenVec[i].type == CONSTANT || mTokenVec[i].type == IDENT ) {
        if ( mTokenVec[i].type == IDENT ) {     // Find ID value
          // 找到ID相對應的CONSTANT再存入
          gIterID = gStackOfID->at( gStackOfID->size()-1 ).find( mTokenVec[i].name ) ; 

          if ( gIterID->second.arraySize > 0 ) {   // 是陣列
            int findRSquare = 0 ;
            for ( findRSquare = i + 2 ; findRSquare < mTokenVec.size() && 
                  mTokenVec[findRSquare].type != R_SQUARE ; findRSquare++ )
              ;
              
            start = 0, end = 0 ;
            result = "\0" ;
            start = i + 2 ;
            end = findRSquare ;
            vector<Token> temp( mTokenVec.begin() + start, mTokenVec.begin() + end ) ;
            vector<Token> temppostfix = InfixToPostfix( temp ) ;
            result = Calculate( temppostfix ) ;
            arrayIndex = atoi( result.c_str() ) ;   
            mTokenVec.erase( mTokenVec.begin() + start, mTokenVec.begin() + end );
            tempToken.name =  result ;
            tempToken.type = CONSTANT ;
            mTokenVec.insert( mTokenVec.begin() + i+2, tempToken ) ;    
            gIterID = gStackOfID->at( gStackOfID->size()-1 ).find( mTokenVec[i].name ) ; 
            i += 3 ;
            value = gIterID->second.array->at( arrayIndex ) ;         
          } // if
          else {
            value = gIterID->second.value ;
          } // else
          
          tempToken.name = value ;
          tempToken.type = CONSTANT ;
          postfix.push_back( tempToken ) ;
        } // end if
        else {
          postfix.push_back( mTokenVec[i] ) ;
        } // end else
      } // else if
      else if ( mTokenVec[i].type == PP ) {
        if ( i != 0 && ( mTokenVec[i-1].type == IDENT || mTokenVec[i-1].type == R_SQUARE ) ) { // EX. a++ ;
          if ( mTokenVec[i-1].type == R_SQUARE ) { // Is array


            gIterID = gStackOfID->at( gStackOfID->size()-1 ).find( mTokenVec[i-4].name ) ;          
            // gIterID = gStackOfID->at( gStackOfID->size()-1 ).find( mTokenVec[findLSquare-1].name ) ; 

            int arrayIndex = atoi( mTokenVec[i-2].name.c_str() ) ;
            if ( arrayIndex < gIterID->second.arraySize ) {
              value = gIterID->second.array->at( arrayIndex ) ;
              iValue = atoi( value.c_str() ) + 1 ;         
              tSize = gStackOfID->size() - 1 ;
              gStackOfID->at( tSize )[mTokenVec[i-4].name].array->at( arrayIndex ) = IntToString( iValue ) ;
            } // if
          } // if
          else {
            gIterID = gStackOfID->at( gStackOfID->size()-1 ).find( mTokenVec[i-1].name ) ;
            iValue = atoi( gIterID -> second.value.c_str() ) + 1 ;
            // update IDTable
            gStackOfID->at( gStackOfID->size()-1 )[mTokenVec[i-1].name].value = IntToString( iValue ) ;
          } // else
        } // if
        else {                                          // EX. ++a ;
          gIterID = gStackOfID->at( gStackOfID->size()-1 ).find( mTokenVec[i+1].name ) ;
          if ( gIterID->second.arraySize > 0 ) {     // Is array
            i += 3 ;
            int arrayIndex = atoi( mTokenVec[i].name.c_str() ) ;
            if ( arrayIndex < gIterID->second.arraySize ) {
              value = gIterID->second.array->at( arrayIndex ) ;
              iValue = atoi( value.c_str() ) + 1 ;
              i += 1 ;      // 讀掉']'
              tSize = gStackOfID->size()-1 ;
              tName = "\0" ;
              tName = mTokenVec[i-3].name ;
              // update IDTable
              gStackOfID->at( tSize )[tName].array->at( arrayIndex ) = IntToString( iValue ) ; 
            } // if      
          }  // if 
          else {
            iValue = atoi( gIterID -> second.value.c_str() ) + 1 ;
            gStackOfID->at( gStackOfID->size()-1 )[mTokenVec[i+1].name].value = IntToString( iValue ) ; 
            i += 1 ; // 讀掉後一個IDENT
          } // else  
          
          tempToken.name = IntToString( iValue ) ;
          tempToken.type = IDENT ;
          postfix.push_back( tempToken ) ;
        } // else
        
      } // else if
      else if ( mTokenVec[i].type == MM ) {  // EX. a-- 
        if ( i != 0 && ( mTokenVec[i-1].type == IDENT || mTokenVec[i-1].type == R_SQUARE ) ) { 
          if ( i >= 4 && mTokenVec[i-1].type == R_SQUARE ) { // Is array
            gIterID = gStackOfID->at( gStackOfID->size()-1 ).find( mTokenVec[i-4].name ) ; 
            int arrayIndex = atoi( mTokenVec[i-2].name.c_str() ) ;
            if ( arrayIndex < gIterID->second.arraySize ) {
              value = gIterID->second.array->at( arrayIndex ) ;
              iValue = atoi( value.c_str() ) - 1 ;
              tSize = gStackOfID->size()-1 ;
              gStackOfID->at( tSize )[mTokenVec[i-4].name].array->at( arrayIndex ) = IntToString( iValue ) ;
            } // if
          } // if
          else {
            gIterID = gStackOfID->at( gStackOfID->size()-1 ).find( mTokenVec[i-1].name ) ;
            iValue = atoi( gIterID -> second.value.c_str() ) - 1 ;
            // update IDTable
            gStackOfID->at( gStackOfID->size()-1 )[mTokenVec[i-1].name].value = IntToString( iValue ) ;
          } // else
        } // if
        else {                                          // EX. --a ;
          gIterID = gStackOfID->at( gStackOfID->size()-1 ).find( mTokenVec[i+1].name ) ;
          if ( gIterID->second.arraySize > 0 ) {     // Is array
            i += 3 ;
            int arrayIndex = atoi( mTokenVec[i].name.c_str() ) ;
            if ( arrayIndex < gIterID->second.arraySize ) {
              value = gIterID->second.array->at( arrayIndex ) ;
              iValue = atoi( value.c_str() ) - 1 ;
              i += 1 ;      // 讀掉']'
              tSize = gStackOfID->size()-1 ;
              tName = "\0" ;
              tName = mTokenVec[i-3].name ;
              // update IDTable
              gStackOfID->at( tSize )[tName].array->at( arrayIndex ) = IntToString( iValue ) ;     
            } // if  
          }  // if 
          else {
            iValue = atoi( gIterID -> second.value.c_str() ) - 1 ;
            gStackOfID->at( gStackOfID->size()-1 )[mTokenVec[i+1].name].value = IntToString( iValue ) ; 
            i += 1 ; // 讀掉後一個IDENT
          } // else  
          
          tempToken.name = IntToString( iValue ) ;
          tempToken.type = IDENT ;
          postfix.push_back( tempToken ) ;
          
        } // else
      } // else if
      else if ( mTokenVec[i].type == SUB && ( i == 0 || IsOperator( mTokenVec[i-1] ) ) ) {
        // 兩種情況SUB是負號 (1) index == 0 (2) index-1是運算子
        tempToken.name = "-1" ;
        tempToken.type = CONSTANT ;
        postfix.push_back( tempToken ) ;

        tempToken.name = "\0" ; // initial
        tempToken.name = "*" ;
        tempToken.type = MULT ;
        operatorStack.push( tempToken ) ;
      } // else if
      else if ( mTokenVec[i].type == NOT ) {
        tempToken.name = "-1" ;
        tempToken.type = NOT ;
        postfix.push_back( tempToken ) ;

        tempToken.name = "\0" ; // initial
        tempToken.name = "*" ;
        tempToken.type = MULT ;
        operatorStack.push( tempToken ) ;
      } // else if
      else if ( mTokenVec[i].type == L_PARENTHESES ) {
        start = i ;
        for ( end = start + 1 ; end < mTokenVec.size() && countP != 0 ; end++ ) {
          if ( mTokenVec[end].type == L_PARENTHESES )
            countP++ ;
          else if ( mTokenVec[end].type == R_PARENTHESES )
            countP-- ;
        } // for

        vector<Token> temp( mTokenVec.begin() + start, mTokenVec.begin() + end ) ;
        vector<Token> temppostfix = InfixToPostfix( temp ) ;
        result = Calculate( temppostfix ) ;
        tempToken.name = result ;
        tempToken.type = CONSTANT ;
        postfix.push_back( tempToken ) ;
        i = end - 1 ;
      } // end else if
      else if ( countP == 0 && mTokenVec[i].type == COMMA ) {
        vector<Token> temp( mTokenVec.begin() + start, mTokenVec.begin() + i ) ;
        vector<Token> temppostfix = InfixToPostfix( temp ) ;
        result = Calculate( temppostfix ) ;
        while ( !postfix.empty() )
          postfix.pop_back() ;
        while ( !operatorStack.empty() )
          operatorStack.pop() ;
        tempToken.name = result ;
        tempToken.type = CONSTANT ;
        postfix.push_back( tempToken ) ;
        start = i + 1 ;
      } // else if
      else if ( IsAssignOpt( mTokenVec[i].name ) ) {
        int start = 0, end = 0, countPar = 0 ;
        string tempResult = "\0" ;
        Token tempToken ;
        start = i+1 ;
        for ( end = start, countPar = 0 ; end < mTokenVec.size() && 
              ( countPar != 0 || mTokenVec[end].type != R_PARENTHESES ) ; end++ ) {
          if ( mTokenVec[end].type == L_PARENTHESES )    // "("
            countPar++ ;
          else if ( mTokenVec[end].type == R_PARENTHESES )
            countPar-- ;
        } // for

        vector<Token> temp( mTokenVec.begin() + start, mTokenVec.begin() + end ) ;
        vector<Token> temppostfix = InfixToPostfix( temp ) ;
        tempResult = Calculate( temppostfix ) ;
        if ( i-1 >= 0 && mTokenVec[i-1].type == IDENT )
          CalDefine( tempResult, mTokenVec[i].name, mTokenVec[i-1].name, "-1" ) ;
        else if ( i-4 >= 0 )
          CalDefine( tempResult, mTokenVec[i].name, mTokenVec[i-4].name, mTokenVec[i-2].name ) ;
        tempToken.name = tempResult ;
        tempToken.type = CONSTANT ;
        postfix.pop_back() ;
        postfix.push_back( tempToken ) ;
        i = end-1 ;
      } // else if
      else if ( countP == 0 && mTokenVec[i].type == QUESTION ) {
        string result = "\0" ;
        Token tempToken ;
        int posColon = 0 ;
        vector<Token> temp( mTokenVec.begin(), mTokenVec.begin() + i ) ;
        vector<Token> temppostfix = InfixToPostfix( temp ) ;
        result = Calculate( temppostfix ) ;
        while ( !postfix.empty() )
          postfix.pop_back() ;
        while ( !operatorStack.empty() )
          operatorStack.pop() ;

        for ( int pos = i ; pos < mTokenVec.size() ; pos++ ) {
          if ( mTokenVec[pos].type == L_PARENTHESES )
            countP++ ;
          else if ( mTokenVec[pos].type == R_PARENTHESES )
            countP-- ;
          
          if ( countP == 0 && mTokenVec[pos].type == COLON )
            posColon = pos ;
        } // for

        if ( result == "true" || ( result != "false" && result != "0" ) ) {
          vector<Token> temp( mTokenVec.begin() + i + 1, mTokenVec.begin() + posColon ) ;
          vector<Token> temppostfix = InfixToPostfix( temp ) ;
          result = Calculate( temppostfix ) ;
          i = mTokenVec.size() ;
        } // if
        else {
          vector<Token> temp( mTokenVec.begin() + posColon + 1, mTokenVec.end() ) ;
          vector<Token> temppostfix = InfixToPostfix( temp ) ;
          result = Calculate( temppostfix ) ;
          i = mTokenVec.size() ;
        } // else 

        tempToken.name = result ;
        tempToken.type = CONSTANT ;
        postfix.push_back( tempToken ) ;
      } // else if
      else { // +-*/ boolean
        if ( mTokenVec[i].type == SEMICOLON ) ;
        else if ( operatorStack.empty() ) {
          operatorStack.push( mTokenVec[i] ) ;
        } // end if
        else if ( OperatorPriority( mTokenVec[i].name ) > OperatorPriority( operatorStack.top().name ) ) {
          operatorStack.push( mTokenVec[i] ) ;
        } // end if
        else {

          while ( ( !operatorStack.empty() ) && 
                  OperatorPriority( mTokenVec[i].name ) <= OperatorPriority( operatorStack.top().name ) ) {
            postfix.push_back( operatorStack.top() ) ;
            operatorStack.pop() ;
          } // end while

          operatorStack.push( mTokenVec[i] ) ;
        } // end else
      } // end else
    } // end for

    while ( !operatorStack.empty() ) {
      postfix.push_back( operatorStack.top() ) ;
      operatorStack.pop() ;
    } // end while

    return postfix ;
  } // end InfixToPostfix()

  string Evaluate( vector<Token> tokenList ) {
    string result = "\0" ;
    vector<Token> postfix ;
    int signPosition = 0, start = 0, end = 0 ;
    int countPara = 0 ;   // >0 多"(", <0 多")"
    vector<int> posCOMs ;
    bool isOutPar = true ;    // 只要count<0代表不是完整()
    
    try {
      // find if expression have outer parentheses
      for ( int findOP = 0 ; findOP < tokenList.size() && isOutPar ; findOP++ ) {
        if ( tokenList[findOP].type == L_PARENTHESES )
          countPara++ ;
        else if ( tokenList[findOP].type == R_PARENTHESES )
          countPara-- ;

        if ( countPara <= 0 && findOP < tokenList.size() - 1 )
          isOutPar = false ;
      } // for

      if ( isOutPar && tokenList[0].type == L_PARENTHESES && 
           tokenList[tokenList.size()-1].type == R_PARENTHESES ) {
        tokenList.erase( tokenList.begin() ) ;
        tokenList.erase( tokenList.end()-1 ) ;
      } // if

      // find comma position
      countPara = 0 ;
      for ( int findPos = 0 ; findPos < tokenList.size() ; findPos++ ) {
        if ( tokenList[findPos].type == L_PARENTHESES )
          countPara++ ;
        else if ( tokenList[findPos].type == R_PARENTHESES )
          countPara-- ;
        else if ( countPara == 0 && tokenList[findPos].type == COMMA )
          posCOMs.push_back( findPos ) ;
      } // for

      if ( posCOMs.size() > 0 ) {
        start = 0 ;
        for ( int i = 0 ; i < posCOMs.size() && start < tokenList.size() ; i++ ) {
          vector<Token> temp( tokenList.begin() + start, tokenList.begin() + posCOMs[i] ) ;
          result = Evaluate( temp ) ;
          start = posCOMs[i] + 1 ;
        } // for

        if ( start < tokenList.size() ) {
          vector<Token> temp( tokenList.begin() + start, tokenList.end() ) ;
          result = Evaluate( temp ) ;
        } // if
      } // if
      else {
        vector<Token> temp( tokenList.begin(), tokenList.end() ) ;
        postfix = InfixToPostfix( temp ) ;
        result = Calculate( postfix ) ;
      } // else

    } // end try
    catch( ErrorType message ) {
      if ( message == DIVIDEND_ZERO )
        cout << "Error" << endl ;
    } // end catch

    if ( result[0] == '\"' || result[0] == '\'' )
      result = GetStrContent( result ) ;
    return result ;
  } // end Evaluate()

  void DefineLocalVariable( vector<Token> tokenList ) {
    if ( gStackOfID->size() > 1 ) {                            // 判斷是否在function內
      DefinedIDInfo tempDefinedID ;
      tempDefinedID.arraySize = 0 ;                            // initial
      for ( int i = 0 ; i < tokenList.size() ; i++ ) {
        if ( tokenList[i].type == IDENT ) {
          tempDefinedID.type = tokenList[0].type ;
          tempDefinedID.stackLayer = 0 ;                       // initial
          tempDefinedID.stackLayer = gStackOfID->size() - 1 ;
          if ( i+1 < tokenList.size() && tokenList[i+1].type == L_SQUARE ) {
            tempDefinedID.arraySize = atoi( tokenList[i+2].name.c_str() ) + 1 ;    // 有value的時候記得改
            tempDefinedID.array = new vector< string >() ;
          } // if
          else    
            tempDefinedID.arraySize = -1 ;

          if ( tempDefinedID.arraySize > 0 ) {
            tempDefinedID.array->clear() ;
            for ( int i = 0; i < tempDefinedID.arraySize ; i++ ) {
              if ( tempDefinedID.type == INT )
                tempDefinedID.array->push_back( "0" ) ;
              else if ( tempDefinedID.type == FLOAT )
                tempDefinedID.array->push_back( "0.0" ) ;                              
              else if ( tempDefinedID.type == BOOL )
                tempDefinedID.array->push_back( "false" ) ;                                
              else if ( tempDefinedID.type == CHAR )
                tempDefinedID.array->push_back( "\'\'" ) ;                                  
              else if ( tempDefinedID.type == STRING )
                tempDefinedID.array->push_back( "\"\"" ) ;    
            } // for
          } // if
          else {
            if ( tempDefinedID.type == INT )
              tempDefinedID.value = "0" ;                                           // 定義值為0
            else if ( tempDefinedID.type == FLOAT )
              tempDefinedID.value = "0.0" ;                                           
            else if ( tempDefinedID.type == BOOL )
              tempDefinedID.value = "false" ;                                           
            else if ( tempDefinedID.type == CHAR )
              tempDefinedID.value = "\'\'" ;                                           
            else if ( tempDefinedID.type == STRING )
              tempDefinedID.value = "\"\"" ;               
          } // else                      
          
          tempDefinedID.content = tokenList[0].name + " " + tokenList[i].name ;  
          gStackOfID->at( gStackOfID->size()-1 )[tokenList[i].name] = tempDefinedID ;
        } // if
      } // for
    } // if
  } // end DefineLocalVariable()

  void LocalIdTablePopBack() {
    // 前一層變數更新
    for ( gIterID = gStackOfID->at( gStackOfID->size() - 2 ).begin() ; 
          gIterID != gStackOfID->at( gStackOfID->size() - 2 ).end() ; gIterID++ ) {     // 遍尋前一層
      map<string, DefinedIDInfo>::iterator tempIterID ;
      tempIterID = gStackOfID->at( gStackOfID->size()-1 ).find( gIterID->first ) ;      // 找Local層
      if ( tempIterID != gStackOfID->at( gStackOfID->size()-1 ).end() && 
           tempIterID->second.stackLayer != gStackOfID->size()-1 &&
           tempIterID->first != "cout" ) {
        gIterID->second.value = tempIterID->second.value ;
      } // if
      
    } // for
    
    gStackOfID->pop_back() ;
  } // LocalIdTablePopBack()

  string Execute_Statement( vector<Token> tokenList ) {
    int isCout = 0 ;
    vector<Token> evaluateTokenList ;
    string result = "\0", refValue = "\0" ;
    if ( tokenList[0].name == "cout" ) {
      for ( int i = 0 ; i < tokenList.size() && tokenList[i].type != SEMICOLON ; i++ ) {
        if ( tokenList[i].name == "<<" ) {
          i++ ;
          isCout = 0 ;
          while ( i < tokenList.size() && tokenList[i].type != SEMICOLON && 
                  ( isCout != 0 || tokenList[i].name != "<<" ) ) {
            if ( tokenList[i].type == L_PARENTHESES ) 
              isCout++ ;
            else if ( tokenList[i].type == R_PARENTHESES )
              isCout-- ;

            evaluateTokenList.push_back( tokenList[i] ) ;
            i++ ;
          } // while

          i-- ;
          result = Evaluate( evaluateTokenList ) ;

          for ( int j = 0 ; j < result.size() ; j++ ) {
            if ( result[j] == '\\' && j + 1 < result.size() ) {
              if ( result[j+1] == 'n' )
                cout << "\n" ;
              else if ( result[j+1] == 't' )
                cout << "\t" ;
              j++ ;
            } // if
            else
              cout << result[j] ;
          } // for

        } // if

        if ( gTestNum == 2 && gCountWhile == 2 && gCountLine == 4 )
          cout << "cout" << endl ;
        evaluateTokenList.clear() ;
      } // for

    } // if
    else if ( tokenList[0].type == L_CURLY ) {
      int countCurly = 1 ;    // >0 多"{", <0 多"}"
      int start = 0 ;
      bool newLine = true ;

      for ( int i = 1 ; i < tokenList.size() && countCurly != 0 ; i++ ) {
        if ( tokenList[i].type == R_CURLY ) 
          countCurly-- ;
        else if ( tokenList[i].type == L_CURLY )
          countCurly++ ;

        if ( newLine )
          start = i ;

        newLine = false ;

        if ( countCurly >= 1 && tokenList[i].type == SEMICOLON ) {
          while ( countCurly > 1 && i+1 < tokenList.size() && tokenList[i+1].type == R_CURLY ) {
            countCurly-- ;
            i++ ;
          } // while
          
          if ( countCurly > 1 || ( i+1 < tokenList.size() && tokenList[i+1].type == ELSE ) ) 
            ; // continue until else end
          else {
            vector<Token> temp( tokenList.begin() + start, tokenList.begin() + i + 1 ) ;
            result = Execute_Statement( temp ) ;
            gCountLine++ ;
            newLine = true ;
          } // else
        } // if
      } // for

    } // else if
    else if ( tokenList[0].type == IF ) {
      int countPara = 1 ;    // >0 多"(", <0 多")"
      int countCurly = 0 ;    // >0 多"{", <0 多"}"
      int countIf = 1 ; 
      int i = 0 ;
      for ( i = 2 ; i < tokenList.size() && countPara != 0 ; i++ ) {    // if 的條件判斷
        if ( tokenList[i].type == L_PARENTHESES )
          countPara++ ;
        else if ( tokenList[i].type == R_PARENTHESES ) {
          countPara-- ;
          if ( countPara == 0 ) {
            vector<Token> temp( tokenList.begin() + 2, tokenList.begin() + i ) ;
            result = Evaluate( temp ) ;
          } // if
        } // if
      } // for

      int start = i ;
      for ( i = i ; i < tokenList.size() ; i++ ) {
        if ( tokenList[i].type == L_CURLY )
          countCurly++ ;
        else if ( tokenList[i].type == R_CURLY ) 
          countCurly-- ;
        else if ( tokenList[i].type == IF )
          countIf++ ;

        if ( countCurly == 0 && ( tokenList[i].type == R_CURLY || tokenList[i].type == SEMICOLON ) ) {
          while ( i+2 < tokenList.size() && tokenList[i+1].type == ELSE && 
                  tokenList[i+2].type != L_CURLY && countIf > 1 ) {
            for ( i = i+1 ; i < tokenList.size() && tokenList[i].type != SEMICOLON ; i++ ) 
              ;
            countIf-- ;
          } // while

          if ( result == "true" ) {
            gStackOfID->push_back( gStackOfID->at( gStackOfID->size()-1 ) ) ;
            vector<Token> temp( tokenList.begin() + start, tokenList.begin() + i + 1 ) ;
            result = Execute_Statement( temp ) ;
            i = tokenList.size() ;
            LocalIdTablePopBack() ;
          } // if
          else {
            if ( i+1 < tokenList.size() ) {   // 判斷還有沒有else
              i++ ;
              gStackOfID->push_back( gStackOfID->at( gStackOfID->size()-1 ) ) ;
              i++ ;     // skip ELSE
              vector<Token> temp( tokenList.begin() + i, tokenList.end() ) ;
              result = Execute_Statement( temp ) ;
              i = tokenList.size() ;
              LocalIdTablePopBack() ;
            } // if
          } // else
        } // if
      } // for
    } // else if
    else if ( tokenList[0].type == WHILE ) {
      gStackOfID->push_back( gStackOfID->at( gStackOfID->size()-1 ) ) ;
      bool do_loop = true ;
      gCountWhile++ ;
      
      while ( do_loop == true ) {
        int countPara = 1 ;    // >0 多"(", <0 多")"
        int countCurly = 0 ;    // >0 多"{", <0 多"}"
        int i = 0 ;

        for ( i = 2 ; i < tokenList.size() && countPara != 0 ; i++ ) {
          if ( tokenList[i].type == L_PARENTHESES )
            countPara++ ;
          else if ( tokenList[i].type == R_PARENTHESES ) {
            countPara-- ;
            if ( countPara == 0 ) {
              vector<Token> temp( tokenList.begin() + 2, tokenList.begin() + i ) ;
              result = Evaluate( temp ) ;
            } // if
          } // else if
        } // for

        int start = i ;
        if ( result == "true" ) {
          for ( i = i ; i < tokenList.size() ; i++ ) {
            if ( tokenList[i].type == L_CURLY )
              countCurly++ ;
            else if ( tokenList[i].type == R_CURLY ) 
              countCurly-- ;

            if ( countCurly == 0 && ( tokenList[i].type == R_CURLY || tokenList[i].type == SEMICOLON ) ) {
              vector<Token> temp( tokenList.begin() + start, tokenList.begin() + i + 1 ) ;
              result = Execute_Statement( temp ) ;
            } // if
          } // for
        } // if
        else
          do_loop = false ;
          
      } // while

      LocalIdTablePopBack() ;
    } // else if
    else if ( tokenList[0] .type == RETURN ) {
      vector<Token> temp( tokenList.begin() + 1, tokenList.end() ) ;
      result = Evaluate( temp ) ;
    } // else if
    else if ( FindDefinedFunction( tokenList[0].name ) ) {
      map< string, DefinedFunctionInfo >::iterator tempIterF ;
      map<string, DefinedIDInfo>::iterator tempIterID ;
      RefInfo tempRef ;
      DefinedIDInfo tempID ;
      vector<Token> refList ;
      tempIterF = gFunctionTable.find( tokenList[0].name ) ;
      gStackOfID->push_back( gStackOfID->at( gStackOfID->size()-1 ) ) ;

      // 處理參數
      int start = 0, countRef = 0 ;
      for ( int j = 0 ; j+1 < tokenList.size() && tokenList[j].type != R_PARENTHESES ; j++ ) {
        if ( tokenList[j].type == L_PARENTHESES ) 
          start = j+1 ;
        else if ( tokenList[j+1].type == COMMA || tokenList[j+1].type == R_PARENTHESES ) {
          vector<Token> temp( tokenList.begin() + start, tokenList.begin() + j + 1 ) ;
          vector<Token> temppostfix = InfixToPostfix( temp ) ;
          if ( temp.size() == 1 )
            refList.push_back( temp[0] ) ;
          tempRef = tempIterF->second.refnames->at( countRef ) ;
          tempID.type = tempRef.type ;
          tempID.stackLayer = gStackOfID->size() - 1 ;
          tempID.arraySize = -1 ;
          tempID.value = Calculate( temppostfix ) ;
          gStackOfID->at( gStackOfID->size() - 1 )[tempRef.name] = tempID ;
          countRef++ ;
          start = j+2 ;
        } // else if
      } // for

      // 處理執行
      vector<Token> tempTokenList ;
      for ( int j = 0 ; j < tempIterF->second.content->size() ; j++ )
        tempTokenList.push_back( tempIterF->second.content->at( j ) ) ;
      result = Execute_Statement( tempTokenList ) ;

      for ( int j = 0 ; j < tempIterF->second.refnames->size() ; j++ ) {
        if ( tempIterF->second.refnames->at( j ).needReturn ) {
          tempRef = tempIterF->second.refnames->at( j ) ;
          refValue = gStackOfID->at( gStackOfID->size() - 1 )[tempRef.name].value ;
          gStackOfID->at( gStackOfID->size() - 2 )[refList[j].name].value = refValue ; 
        } // if
      } // for

      LocalIdTablePopBack() ;

    } // else if
    else {
      if ( tokenList[0].type != INT    && tokenList[0].type != CHAR && tokenList[0].type != FLOAT  &&
           tokenList[0].type != STRING && tokenList[0].type != BOOL ) {
        tokenList.erase( tokenList.begin() + tokenList.size()-1 ) ; // 去掉分號
        Evaluate( tokenList ) ;
      } // if
      else {
        tokenList.erase( tokenList.begin() + tokenList.size()-1 ) ;
        DefineLocalVariable( tokenList ) ;
      } // else
    } // else

    return result ;
  } // Execute_Statement()

} ;

// ------------------------------------------ Parser ------------------------------------------

bool Parser::FindDefinedToken( string name ) {  
  // , float &value    
  gIterID = gStackOfID->at( gStackOfID->size()-1 ).find( name ) ;      // 找ID是否被defined
  if ( gIterID == gStackOfID->at( gStackOfID->size()-1 ).end() )       // 找不到
    return false;
  else                   
    return true;

} // Parser::FindDefinedToken()

bool Parser::FindDefinedFunction( string name ) {  
  // , float &value 
  gIterF = gFunctionTable.find( name ) ;      // 找ID是否被defined
  if ( gIterF == gFunctionTable.end() )       // 找不到
    return false;
  else          
    return true;

} // Parser::FindDefinedFunction()
// name == "cin"   || name == "cout"  || name == "Done"
bool Parser::IsReservedWords( string name ) {
  if ( mTokenList.size() == 0 && ( name == "ListAllVariables" || name == "ListAllFunctions" 
                                   || name == "ListVariable" || name == "ListFunction" ) )
    return true ;
  else
    return false ;
} // Parser::IsReservedWords()

void Parser::DefineGlobalVariables() {   // ex. int a, b, c[10];
  DefinedIDInfo tempDefinedID ;
  tempDefinedID.arraySize = 0 ;
  tempDefinedID.stackLayer = 0 ;

  for ( int i = 1 ; i < mTokenList.size() ; i++ ) {
    if ( mTokenList[i].type == IDENT ) {
      if ( mTokenList[i+1].type == L_SQUARE ) {
        tempDefinedID.arraySize = atoi( mTokenList[i+2].name.c_str() ) + 1 ;     // 是陣列
        tempDefinedID.array = new vector< string >() ;
      } // if 
      else
        tempDefinedID.arraySize = -1 ;
      
      tempDefinedID.type = mTokenList[0].type ;                              // 定義變數type
      if ( tempDefinedID.arraySize > 0 ) {
        tempDefinedID.array->clear() ;
        for ( int i = 0; i < tempDefinedID.arraySize ; i++ ) {
          if ( tempDefinedID.type == INT )
            tempDefinedID.array->push_back( "0" ) ;
          else if ( tempDefinedID.type == FLOAT )
            tempDefinedID.array->push_back( "0.0" ) ;                              
          else if ( tempDefinedID.type == BOOL )
            tempDefinedID.array->push_back( "false" ) ;                                
          else if ( tempDefinedID.type == CHAR )
            tempDefinedID.array->push_back( "\'\'" ) ;                                  
          else if ( tempDefinedID.type == STRING )
            tempDefinedID.array->push_back( "\"\"" ) ;    
        } // for
      } // if
      else {
        if ( tempDefinedID.type == INT )
          tempDefinedID.value = "0" ;                                           // 定義值為0
        else if ( tempDefinedID.type == FLOAT )
          tempDefinedID.value = "0.0" ;                                           
        else if ( tempDefinedID.type == BOOL )
          tempDefinedID.value = "false" ;                                           
        else if ( tempDefinedID.type == CHAR )
          tempDefinedID.value = "\'\'" ;                                           
        else if ( tempDefinedID.type == STRING )
          tempDefinedID.value = "\"\"" ;      
      } // else                                     

      if ( tempDefinedID.arraySize < 0 )
        tempDefinedID.content = mTokenList[0].name + " " + mTokenList[i].name ; // 儲存變數內容
      else {
        tempDefinedID.content = mTokenList[0].name + " " + mTokenList[i].name + "[ " ;
        tempDefinedID.content = tempDefinedID.content + mTokenList[i+2].name + " ]" ; // 儲存變數內容
      } // else

      gIterID = gStackOfID->at( 0 ).find( mTokenList[i].name ) ;
      gStackOfID->at( 0 )[mTokenList[i].name] = tempDefinedID ;              // 覆蓋變數內容
      if ( gIterID == gStackOfID->at( 0 ).end() )
        cout << "Definition of " << mTokenList[i].name << " entered ...\n" ;
      else {
        cout << "New definition of " << mTokenList[i].name << " entered ...\n" ;
      } // else
    } // if
  } // for
  
} // Parser::DefineGlobalVariables()

void Parser::DefineGlobalFunction() {    // ex. void f(int &a){int b,c[10];}
  DefinedFunctionInfo tempDefinedFunction ;
  RefInfo tempRef ;
  // tempDefinedFunction.content = new vector< Token >() ;
  tempDefinedFunction.refnames = new vector< RefInfo >() ;
  string tempFunctionName = "\0", space = "\0", tempContent = "\0" ;
  
  tempFunctionName = mTokenList[1].name + "()" ;     // 儲存function名稱
  gIterF = gFunctionTable.find( mTokenList[1].name ) ;   // 判斷是否被定義過
  tempDefinedFunction.type = mTokenList[0].type ;
                                                      // 儲存function區域變數(stack最上面)
  tempDefinedFunction.localIDTable = gStackOfID->at( gStackOfID->size()-1 ) ;   
  gStackOfID->pop_back() ;   // 將stack最上層移除
  int start = 1 ;
  for ( start = 1 ; start < mTokenList.size() && mTokenList[start].type != L_CURLY ; start++ ) {
    if ( mTokenList[start].type == INT || mTokenList[start].type == CHAR ||
         mTokenList[start].type == FLOAT || mTokenList[start].type == STRING || 
         mTokenList[start].type == BOOL ) {
      if ( start+2 < mTokenList.size() && mTokenList[start+1].type == BITAND ) {
        tempRef.name = mTokenList[start+2].name ;
        tempRef.needReturn = true ;
      } // if
      else if ( start+1 < mTokenList.size() ) {
        tempRef.name = mTokenList[start+1].name ;
        tempRef.needReturn = false ;
      } // else if

      tempRef.type = mTokenList[start].type ;
      tempDefinedFunction.refnames->push_back( tempRef ) ;
    } // if

  } // for

  tempDefinedFunction.content = new vector< Token >( mTokenList.begin() + start, mTokenList.end() ) ;
  /*
  space = "" ;
  tempContent = "" ;
  for ( int i = 0 ; i < mTokenList.size() ; i++ ) {      // 儲存function內容，遇到 } 或 ; 換行
    if ( tempContent == "" || 
         ( i > 0 && 
           ( ( i-1 == 1 && mTokenList[i].type == L_PARENTHESES ) 
             || mTokenList[i].type == L_SQUARE || mTokenList[i].type == COMMA 
             || ( mTokenList[i-1].type == L_PARENTHESES && mTokenList[i].type == R_PARENTHESES ) 
             || ( ( mTokenList[i].type == PP || mTokenList[i].type == MM ) 
                  && mTokenList[i-1].type == IDENT ) 
             || ( ( mTokenList[i-1].type == PP || mTokenList[i-1].type == MM ) 
                  && mTokenList[i].type == IDENT ) ) ) )
      tempContent = tempContent + mTokenList[i].name ;   // 存此token前不用加空格
    else
      tempContent = tempContent + " " + mTokenList[i].name ;

    if ( mTokenList[i].type == L_CURLY ) {   // 左大括號, 後面換行+2空格
      tempDefinedFunction.content->push_back( space + tempContent ) ;
      space = space + "  " ;
      tempContent = "" ;
    } // if
    else if ( mTokenList[i].type == R_CURLY ) {   // 右大括號, 刪除2空格後換行
      space.erase( space.size() - 2 );
      tempDefinedFunction.content->push_back( space + tempContent ) ;
      tempContent = "" ;
    } // else if
    else if ( mTokenList[i].type == SEMICOLON ) {   // 分號, 換行
      tempDefinedFunction.content->push_back( space + tempContent ) ;
      tempContent = "" ;
    } // else if
  } // for
  */

  if ( gIterF == gFunctionTable.end() ) {
    gFunctionTable[mTokenList[1].name] = tempDefinedFunction ;
    cout << "Definition of " << tempFunctionName << " entered ...\n" ;
  } // if
  else {
    gFunctionTable[mTokenList[1].name] = tempDefinedFunction ;
    cout << "New definition of " << tempFunctionName << " entered ...\n" ;
  } // else
  
} // Parser::DefineGlobalFunction()

bool Parser::SystemFunctions() {
  if ( mTokenList[0].name == "ListAllVariables" ) {
    for ( gIterID = gStackOfID->at( 0 ).begin() ; gIterID != gStackOfID->at( 0 ).end() ; gIterID++ )
      if ( gIterID->first != "cout" )
        cout << gIterID->first << endl ;
  } // if
  else if ( mTokenList[0].name == "ListAllFunctions" ) {
    for ( gIterF = gFunctionTable.begin() ; gIterF != gFunctionTable.end() ; gIterF++ )
      if ( gIterID->first != "cout" )
        cout << gIterF->first << "()" << endl;
  } // else if
  else if ( mTokenList[0].name == "ListVariable" && mTokenList[1].type == L_PARENTHESES && 
            mTokenList[2].type == CONSTANT && mTokenList[2].name[0] == '\"' ) {   // ex. ListVariable("a");
    if ( FindDefinedToken( mTokenList[2].name.substr( 1, mTokenList[2].name.size()-2 ) ) ) {     // 字串去掉""
      DefinedIDInfo tempID ;
      tempID = gStackOfID->at( 0 )[ mTokenList[2].name.substr( 1, mTokenList[2].name.size()-2 ) ] ;
      cout << tempID.content ;
      cout << " ;\n" ;
    } // if
  } // else if
  else if ( mTokenList[0].name == "ListFunction" && mTokenList[1].type == L_PARENTHESES &&
            mTokenList[2].type == CONSTANT && mTokenList[2].name[0] == '\"' ) {
    if ( FindDefinedFunction( mTokenList[2].name.substr( 1, mTokenList[2].name.size()-2 ) ) ) {     // 字串去掉""
      DefinedFunctionInfo tempFunc ;
      tempFunc.content = new vector< Token >() ;
      tempFunc = gFunctionTable[ mTokenList[2].name.substr( 1, mTokenList[2].name.size()-2 ) ] ;
      for ( int i = 0 ; i < tempFunc.content->size() ; i++ ) {
        cout << tempFunc.content->at( i ).name << endl ;
      } // for
    } // if
  } // else if
  else
    return false ;

  return true ;
} // Parser::SystemFunctions()

bool Parser::User_Input() {
  Evaluator eav ;
  cout << "> " ;
  char ch = cin.peek() ;
  gLine = 0 ;

  if ( !gKeepBuffer )                             // for 'else'
    mBuffer = GetToken() ;
  gKeepBuffer = false ; 

  if ( Definition() ) {
    if ( mTokenList[2].type != L_PARENTHESES )    // Tokens
      DefineGlobalVariables() ;
    else                                          // Function
      DefineGlobalFunction() ;
  } // if
  else if ( Statement() ) {
    if ( !SystemFunctions() )
      eav.Execute_Statement( mTokenList ) ;
    cout << "Statement executed ...\n" ;
  } // else if
  else {
    mBuffer.line = 1 ;
    throw UNEXPECTED_TOKEN ;
  } // else

  return true ;

} // Parser::User_Input()

bool Parser::Definition() {
  if ( mBuffer.type == VOID ) {         // VOID Identifier function_definition_without_ID
    mTokenList.push_back( mBuffer );
    mBuffer = GetToken();

    if ( mBuffer.type == IDENT ) {
      mTokenList.push_back( mBuffer );
      mBuffer = GetToken();  

      if ( Function_Definition_Without_ID() ) ;
      else throw UNEXPECTED_TOKEN ;

    } // if
    else throw UNEXPECTED_TOKEN ;
  } // if
  else if ( Type_Specifier() ) {        // type_specifier Identifier function_definition_or_declarators
    if ( mBuffer.type == IDENT ) {
      mTokenList.push_back( mBuffer );
      mBuffer = GetToken();
      
      if ( Function_Definition_or_Declarators() ) ; // if
      else throw UNEXPECTED_TOKEN;

    } // if
    else throw UNEXPECTED_TOKEN;

  } // else if
  else return false;

  return true;

} // Parser::Definition()

bool Parser::Type_Specifier() {
  // INT | CHAR | FLOAT | STRING | BOOL
  if ( mBuffer.type == INT    || mBuffer.type == CHAR || mBuffer.type == FLOAT  ||
       mBuffer.type == STRING || mBuffer.type == BOOL ) {   
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
  } // if
  else return false;

  return true;

} // Parser::Type_Specifier()

bool Parser::Function_Definition_or_Declarators() {
  if ( Function_Definition_Without_ID() ) ;
  else if ( Rest_of_Declarators() ) ;
  else return false ;

  return true ;
} // Parser::Function_Definition_or_Declarators()

bool Parser::Rest_of_Declarators() {
  int varStart = 0, varEnd = 0 ;
  varStart = mTokenList.size()-2 ;
  if ( mBuffer.type == L_SQUARE ) {
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
    if ( mBuffer.type == CONSTANT ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      if ( mBuffer.type == R_SQUARE ) {
        mTokenList.push_back( mBuffer ) ;
        mBuffer = GetToken() ;
      } // if
      else throw UNEXPECTED_TOKEN ;
    } // if
    else throw UNEXPECTED_TOKEN ;
  } // if

  while ( mBuffer.type == COMMA ) {
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;

    if ( mBuffer.type == IDENT ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      if ( mBuffer.type == L_SQUARE ) {
        mTokenList.push_back( mBuffer ) ;
        mBuffer = GetToken() ;
        if ( mBuffer.type == CONSTANT ) {
          mTokenList.push_back( mBuffer ) ;
          mBuffer = GetToken() ;
          if ( mBuffer.type == R_SQUARE ) {
            mTokenList.push_back( mBuffer ) ;
            mBuffer = GetToken() ;
          } // if

          else throw UNEXPECTED_TOKEN ;
        } // if

        else throw UNEXPECTED_TOKEN ;
      } // if

    } // if

    else throw UNEXPECTED_TOKEN ;
  } // while
  
  if ( mBuffer.type == SEMICOLON ) {
    varEnd = mTokenList.size() ;
    mTokenList.push_back( mBuffer ) ; // END OF COMMAND
  } // if
  else return false ;

  // ------------------宣告function的變數-----------------//
  if ( gStackOfID->size() > 1 ) {    // 判斷是否在function內
    DefinedIDInfo tempDefinedID ;
    tempDefinedID.arraySize = 0 ;
    tempDefinedID.stackLayer = 0 ;
    for ( int i = varStart ; i < varEnd ; i++ ) {
      if ( mTokenList[i].type == IDENT ) {
        tempDefinedID.type = mTokenList[varStart].type ;
        if ( mTokenList[i+1].type == L_SQUARE ) {
          tempDefinedID.arraySize = atoi( mTokenList[i+2].name.c_str() ) + 1 ;    // 有value的時候記得改
          tempDefinedID.array = new vector< string >() ;
        } // if         
        else 
          tempDefinedID.arraySize = -1 ;

        if ( tempDefinedID.arraySize > 0 ) {
          tempDefinedID.array->clear() ;
          for ( int i = 0; i < tempDefinedID.arraySize ; i++ ) {
            if ( tempDefinedID.type == INT )
              tempDefinedID.array->push_back( "0" ) ;
            else if ( tempDefinedID.type == FLOAT )
              tempDefinedID.array->push_back( "0.0" ) ;                              
            else if ( tempDefinedID.type == BOOL )
              tempDefinedID.array->push_back( "false" ) ;                                
            else if ( tempDefinedID.type == CHAR )
              tempDefinedID.array->push_back( "\'\'" ) ;                                  
            else if ( tempDefinedID.type == STRING )
              tempDefinedID.array->push_back( "\"\"" ) ;    
          } // for
        } // if
        else {
          if ( tempDefinedID.type == INT )
            tempDefinedID.value = "0" ;                                           // 定義值為0
          else if ( tempDefinedID.type == FLOAT )
            tempDefinedID.value = "0.0" ;                                           
          else if ( tempDefinedID.type == BOOL )
            tempDefinedID.value = "false" ;                                           
          else if ( tempDefinedID.type == CHAR )
            tempDefinedID.value = "\'\'" ;                                           
          else if ( tempDefinedID.type == STRING )
            tempDefinedID.value = "\"\"" ;               
        } // else                      
        
        tempDefinedID.content = mTokenList[varStart].name + " " + mTokenList[i].name ;  
        gStackOfID->at( gStackOfID->size()-1 )[mTokenList[i].name] = tempDefinedID ;
      } // if
    } // for
  } // if
  // ------------------宣告function的變數-----------------//

  return true ;
} // Parser::Rest_of_Declarators()

bool Parser::Function_Definition_Without_ID() {
  int paraStart = 0, paraEnd = 0 ;

  if ( mBuffer.type == L_PARENTHESES ) {
    gStackOfID->push_back( gStackOfID->at( gStackOfID->size()-1 ) ); 
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
    paraStart = mTokenList.size() ;

    if ( mBuffer.type == VOID ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
    } // if 
    else if ( Formal_Parameter_List() ) 
      ; // else if

    if ( mBuffer.type == R_PARENTHESES ) {
      paraEnd = mTokenList.size() ;
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      // ------------------宣告function的參數-----------------//
      if ( gStackOfID->size() > 1 ) {    // 判斷是否是在function內
        DefinedIDInfo tempDefinedID ;
        string idname = "\0" ;
        tempDefinedID.arraySize = 0 ;
        tempDefinedID.stackLayer = gStackOfID->size() - 1 ;
        for ( int i = paraStart ; i < paraEnd ; i++ ) {
          if ( mTokenList[i].type == INT || mTokenList[i].type == CHAR || mTokenList[i].type == FLOAT  ||
               mTokenList[i].type == STRING || mTokenList[i].type == BOOL ) {
            tempDefinedID.type = mTokenList[i].type ;
            tempDefinedID.content = mTokenList[i].name + " " ;
            
            while ( mTokenList[i].type != COMMA && mTokenList[i].type != R_PARENTHESES ) {      // 直到此變數結束

              if ( mTokenList[i+1].type == L_SQUARE ) {
                tempDefinedID.arraySize = atoi( mTokenList[i+2].name.c_str() ) + 1 ;    // 有value的時候記得改
                tempDefinedID.array = new vector< string >() ;
              } // if                
              else 
                tempDefinedID.arraySize = -1 ;

              if ( tempDefinedID.arraySize > 0 ) {
                tempDefinedID.array->clear() ;
                for ( int i = 0; i < tempDefinedID.arraySize ; i++ ) {
                  if ( tempDefinedID.type == INT )
                    tempDefinedID.array->push_back( "0" ) ;
                  else if ( tempDefinedID.type == FLOAT )
                    tempDefinedID.array->push_back( "0.0" ) ;                              
                  else if ( tempDefinedID.type == BOOL )
                    tempDefinedID.array->push_back( "false" ) ;                                
                  else if ( tempDefinedID.type == CHAR )
                    tempDefinedID.array->push_back( "\'\'" ) ;                                  
                  else if ( tempDefinedID.type == STRING )
                    tempDefinedID.array->push_back( "\"\"" ) ;    
                } // for
              } // if
              else {
                if ( tempDefinedID.type == INT )
                  tempDefinedID.value = "0" ;                                           // 定義值為0
                else if ( tempDefinedID.type == FLOAT )
                  tempDefinedID.value = "0.0" ;                                           
                else if ( tempDefinedID.type == BOOL )
                  tempDefinedID.value = "false" ;                                           
                else if ( tempDefinedID.type == CHAR )
                  tempDefinedID.value = "\'\'" ;                                           
                else if ( tempDefinedID.type == STRING )
                  tempDefinedID.value = "\"\"" ;               
              } // else                      

              // if ( mTokenList[i].type == BITAND )   // 若要回傳, 任務四處理
              if ( mTokenList[i].type == IDENT )
                idname = mTokenList[i].name ;

              i++;
            } // while
            
            tempDefinedID.content = tempDefinedID.content + idname ;
            gStackOfID->at( gStackOfID->size()-1 )[idname] = tempDefinedID ;
          } // if
          
        } // for
      } // if
      // ------------------宣告function的參數-----------------//

      if ( Compound_Statement() ) ;
      else throw UNEXPECTED_TOKEN ;

    } // if
    else throw UNEXPECTED_TOKEN ;

  } // if
  else return false ;

  return true ;
} // Parser::Function_Definition_Without_ID()

bool Parser::Formal_Parameter_List() {
  if ( Type_Specifier() ) {
    if ( mBuffer.type == BITAND ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
    } // if

    if ( mBuffer.type == IDENT ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      if ( mBuffer.type == L_SQUARE ) {
        mTokenList.push_back( mBuffer ) ;
        mBuffer = GetToken() ;
        if ( mBuffer.type == CONSTANT ) {
          mTokenList.push_back( mBuffer ) ;
          mBuffer = GetToken() ;
          if ( mBuffer.type == R_SQUARE ) {
            mTokenList.push_back( mBuffer ) ;
            mBuffer = GetToken() ;
          } // if
          else throw UNEXPECTED_TOKEN ;
        } // if
        else throw UNEXPECTED_TOKEN ;
      } // if
      
      while ( mBuffer.type == COMMA ) {
        mTokenList.push_back( mBuffer ) ;
        mBuffer = GetToken() ;
        if ( Type_Specifier() ) {
          if ( mBuffer.type == BITAND ) {
            mTokenList.push_back( mBuffer ) ;
            mBuffer = GetToken() ;
          } // if

          if ( mBuffer.type == IDENT ) {
            mTokenList.push_back( mBuffer ) ;
            mBuffer = GetToken() ;

            if ( mBuffer.type == L_SQUARE ) {
              mTokenList.push_back( mBuffer ) ;
              mBuffer = GetToken() ;
              if ( mBuffer.type == CONSTANT ) {
                mTokenList.push_back( mBuffer ) ;
                mBuffer = GetToken() ;
                if ( mBuffer.type == R_SQUARE ) {
                  mTokenList.push_back( mBuffer ) ;
                  mBuffer = GetToken() ;
                } // if
                else throw UNEXPECTED_TOKEN ;

              } // if
              else throw UNEXPECTED_TOKEN ;

            } // if

          } // if
          else throw UNEXPECTED_TOKEN ;

        } // if
        else throw UNEXPECTED_TOKEN ;    
      } // while
    } // if
    else throw UNEXPECTED_TOKEN ;

  } // if
  else return false ;

  return true ;
} // Parser::Formal_Parameter_List()

bool Parser::Compound_Statement() {
  if ( mBuffer.type == L_CURLY ) {
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;

    while ( Declaration() || Statement() ) {
      if ( !gKeepBuffer )                             // for 'else'
        mBuffer = GetToken() ;
      gKeepBuffer = false ; 
    } // while

    if ( mBuffer.type == R_CURLY ) {
      mTokenList.push_back( mBuffer ) ;
    } // if
    else throw UNEXPECTED_TOKEN ;
  } // if
  else return false ;

  return true ;

} // Parser::Compound_Statement()

bool Parser::Declaration() {
  if ( Type_Specifier() ) {
    if ( mBuffer.type == IDENT ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Rest_of_Declarators() ) ;
      else throw UNEXPECTED_TOKEN ;

    } // if
    else throw UNEXPECTED_TOKEN ;

  } // if 
  else return false ;

  return true ;
} // Parser::Declaration()

bool Parser::Statement() {
  if ( mBuffer.type == DONE ) {
    mBuffer = GetToken() ;
    if ( mBuffer.type == L_PARENTHESES ) {
      mBuffer = GetToken() ;
      if ( mBuffer.type == R_PARENTHESES ) {
        mBuffer = GetToken() ;
        if ( mBuffer.type == SEMICOLON ) {
          throw END_OF_PROGRAM ;
        } // if
        else throw UNEXPECTED_TOKEN ;
      } // if
      else throw UNEXPECTED_TOKEN ;
    } // if
    else throw UNEXPECTED_TOKEN ;
  } // if
  else if ( mBuffer.type == SEMICOLON ) {    // ';'
    mTokenList.push_back( mBuffer ) ;
  } // else if
  else if ( Expression() ) {            // expression ';' 
    if ( mBuffer.type == SEMICOLON ) {
      mTokenList.push_back( mBuffer ) ;
    } // if
    else throw UNEXPECTED_TOKEN ;

  } // else if
  else if ( mBuffer.type == RETURN ) {  // RETURN [ expression ] ';'
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;

    if ( Expression() ) 
      ; // if

    if ( mBuffer.type == SEMICOLON ) {
      mTokenList.push_back( mBuffer ) ;
    } // if
    else throw UNEXPECTED_TOKEN ;

  } // else if
  else if ( Compound_Statement() ) ;    // compound_statement
  else if ( mBuffer.type == IF ) {      // IF '(' expression ')' statement [ ELSE statement ]
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
    if ( mBuffer.type == L_PARENTHESES ) {    
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Expression() ) {
        if ( mBuffer.type == R_PARENTHESES ) {
          gStackOfID->push_back( gStackOfID->at( gStackOfID->size()-1 ) ); 
          mTokenList.push_back( mBuffer ) ;
          mBuffer = GetToken() ;

          if ( Statement() ) {
            gStackOfID->pop_back() ;
            mBuffer = GetToken() ;
            if ( mBuffer.type == ELSE ) {
              gStackOfID->push_back( gStackOfID->at( gStackOfID->size()-1 ) ); 
              mTokenList.push_back( mBuffer ) ;
              mBuffer = GetToken() ;

              if ( Statement() ) 
                gStackOfID->pop_back() ;
              else throw UNEXPECTED_TOKEN ;
            } // if
            else gKeepBuffer = true ;
          } // if
          else throw UNEXPECTED_TOKEN ;
        } // if
        else throw UNEXPECTED_TOKEN ;
      } // if
      else throw UNEXPECTED_TOKEN ;
    } // if
    else throw UNEXPECTED_TOKEN ;
  } // else if
  else if ( mBuffer.type == WHILE ) {   // WHILE '(' expression ')' statement
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
    if ( mBuffer.type == L_PARENTHESES ) {    
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Expression() ) {
        if ( mBuffer.type == R_PARENTHESES ) {
          gStackOfID->push_back( gStackOfID->at( gStackOfID->size()-1 ) ) ;
          mTokenList.push_back( mBuffer ) ;
          mBuffer = GetToken() ;

          if ( Statement() ) 
            gStackOfID->pop_back() ;
          else throw UNEXPECTED_TOKEN ;

        } // if
        else throw UNEXPECTED_TOKEN ;

      } // if
      else throw UNEXPECTED_TOKEN ;

    } // if
    else throw UNEXPECTED_TOKEN ;

  } // else if
  else if ( mBuffer.type == DO ) {      // DO statement WHILE '(' expression ')' ';'
    gStackOfID->push_back( gStackOfID->at( gStackOfID->size()-1 ) ) ;
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
    if ( Statement() ) {
      mBuffer = GetToken() ;
      if ( mBuffer.type == WHILE ) {  
        mTokenList.push_back( mBuffer ) ;
        mBuffer = GetToken() ;

        if ( mBuffer.type == L_PARENTHESES ) {    
          mTokenList.push_back( mBuffer ) ;
          mBuffer = GetToken() ;

          if ( Expression() ) {
            if ( mBuffer.type == R_PARENTHESES ) {
              mTokenList.push_back( mBuffer ) ;
              mBuffer = GetToken() ;

              if ( mBuffer.type == SEMICOLON ) {
                gStackOfID->pop_back() ;
                mTokenList.push_back( mBuffer ) ;
              } // if
              else throw UNEXPECTED_TOKEN ;
            } // if
            else throw UNEXPECTED_TOKEN ;
          } // if
          else throw UNEXPECTED_TOKEN ;
        } // if
        else throw UNEXPECTED_TOKEN ;
      } // if
      else throw UNEXPECTED_TOKEN ;
    } // if
    else throw UNEXPECTED_TOKEN ;

  } // else if
  else if ( mBuffer.type == CIN ) {
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
    if ( mBuffer.type == RS ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      if ( Basic_Expression() ) {
        if ( mBuffer.type == SEMICOLON ) {
          mTokenList.push_back( mBuffer ) ;
        } // if
        else throw UNEXPECTED_TOKEN ;
      } // if
      else throw UNEXPECTED_TOKEN ;
    } // if
    else throw UNEXPECTED_TOKEN ;

  } // else if
  else if ( mBuffer.name == "cout" ) {
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
    while ( mBuffer.type == LS ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Basic_Expression() ) ;
      else throw UNEXPECTED_TOKEN ;
    } // while

    if ( mBuffer.type == SEMICOLON ) {
      mTokenList.push_back( mBuffer ) ;
    } // if
    else throw UNEXPECTED_TOKEN ;
  } // else if
  else return false;

  return true;

} // Parser::Statement()

bool Parser::Expression() {
  if ( Basic_Expression() ) {           // basic_expression { ',' basic_expression }
    while ( mBuffer.type == COMMA ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Basic_Expression() ) ;
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;
  return true ;
} // Parser::Expression()

bool Parser::Basic_Expression() {
  if ( mBuffer.type == IDENT ) {                   // Identifier rest_of_Identifier_started_basic_exp
    if ( !FindDefinedToken( mBuffer.name ) && !FindDefinedFunction( mBuffer.name ) &&
         !IsReservedWords( mBuffer.name ) ) // !IsReservedWords( mBuffer.name )
      throw UNDEFINED_TOKEN ;
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
    
    if ( Rest_of_Identifier_Started_Basic_Exp() ) ;
    else throw UNEXPECTED_TOKEN ;

  } // if
  else if ( mBuffer.type == PP || mBuffer.type == MM ) { // ( PP | MM ) Identifier rest_of_PPMM_Id
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;

    if ( mBuffer.type == IDENT ) {
      if ( !FindDefinedToken( mBuffer.name ) && !FindDefinedFunction( mBuffer.name ) )
        throw UNDEFINED_TOKEN ;
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Rest_of_PPMM_Identifier_Started_Basic_Exp() ) ;
      else throw UNEXPECTED_TOKEN ;

    } // if
    else throw UNEXPECTED_TOKEN ;
  } // else if
  else if ( Sign() ) {                 // sign { sign } signed_unary_exp romce_and_romloe
    while ( Sign() ) ;  // while

    if ( Signed_Unary_Exp() ) {
      
      if ( Romce_and_Romloe() ) {

      } // if
      else throw UNEXPECTED_TOKEN ;

    } // if
    else throw UNEXPECTED_TOKEN ;

  } // else if
  else if ( mBuffer.type == CONSTANT ) {                                    // Constant romce_and_romloe
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
    if ( Romce_and_Romloe() ) ;
    else throw UNEXPECTED_TOKEN ;

  } // else if
  else if ( mBuffer.type == L_PARENTHESES ) {                 // '(' expression ')' romce_and_romloe
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;

    if ( Expression() ) {
      if ( mBuffer.type == R_PARENTHESES ) {
        mTokenList.push_back( mBuffer ) ;
        mBuffer = GetToken() ;

        if ( Romce_and_Romloe() ) ;
        else throw UNEXPECTED_TOKEN ;

      } // if
      else throw UNEXPECTED_TOKEN ;

    } // if
    else throw UNEXPECTED_TOKEN ;

  } // else if
  else return false ;

  return true ;

} // Parser::Basic_Expression()

bool Parser::Rest_of_Identifier_Started_Basic_Exp() {
  if ( mBuffer.type == L_PARENTHESES ) {   // '(' [ actual_parameter_list ] ')' romce_and_romloe
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;

    if ( Actual_Parameter_List() ) 
      ; // if

    if ( mBuffer.type == R_PARENTHESES ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Romce_and_Romloe() ) {

      } // if
      else throw UNEXPECTED_TOKEN ;

    } // if
    else throw UNEXPECTED_TOKEN ;

  } // if
  else {
    if ( mBuffer.type == L_SQUARE ) {                         // [ '[' expression ']' ]
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Expression() ) {
      
        if ( mBuffer.type == R_SQUARE ) {
          mTokenList.push_back( mBuffer ) ;
          mBuffer = GetToken() ;
        } // if
        else throw UNEXPECTED_TOKEN ;

      } // if
      else throw UNEXPECTED_TOKEN ;

    } // if

    if ( Assignment_Operator() ) {                            // assignment_operator basic_expression
      if ( Basic_Expression() ) { } // if
      else throw UNEXPECTED_TOKEN ;
    } // if
    else if ( mBuffer.type == PP || mBuffer.type == MM ) {    // [ PP | MM ] romce_and_romloe
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Romce_and_Romloe() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // else if
    else if ( Romce_and_Romloe() ) { } // else if              // romce_and_romloe
    else return false ;

  } // else

  return true ;

} // Parser::Rest_of_Identifier_Started_Basic_Exp()

bool Parser::Rest_of_PPMM_Identifier_Started_Basic_Exp() {
  if ( mBuffer.type == L_SQUARE ) {                         // [ '[' expression ']' ]
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;

    if ( Expression() ) {
    
      if ( mBuffer.type == R_SQUARE ) {
        mTokenList.push_back( mBuffer ) ;
        mBuffer = GetToken() ;
      } // if
      else throw UNEXPECTED_TOKEN ;

    } // if
    else throw UNEXPECTED_TOKEN ;

  } // if

  if ( Romce_and_Romloe() ) { } // if                         // romce_and_romloe
  else return false ;

  return true ;

} // Parser::Rest_of_PPMM_Identifier_Started_Basic_Exp()

bool Parser::Sign() {
  if ( mBuffer.type == ADD || mBuffer.type == SUB || mBuffer.type == NOT ) {
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
  } // if
  else return false ;

  return true ;
} // Parser::Sign()

bool Parser::Actual_Parameter_List() {
  if ( Basic_Expression() ) ; // if                          // basic_expression { ',' basic_expression }
  else return false ;

  while ( mBuffer.type == COMMA ) {
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;

    if ( Basic_Expression() ) { } // if
    else throw UNEXPECTED_TOKEN ;

  } // while

  return true ;
} // Parser::Actual_Parameter_List()

bool Parser::Assignment_Operator() {
  if ( mBuffer.type == EQUAL || mBuffer.type == TE || mBuffer.type == DE ||
       mBuffer.type == RE    || mBuffer.type == PE || mBuffer.type == ME ) {
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
  } // if
  else return false ;

  return true ;
} // Parser::Assignment_Operator()

bool Parser::Romce_and_Romloe() {
  if ( Rest_of_Maybe_Logical_OR_Exp() ) {
    if ( mBuffer.type == QUESTION ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      if ( Basic_Expression() ) {
        if ( mBuffer.type == COLON ) {
          mTokenList.push_back( mBuffer ) ;
          mBuffer = GetToken() ;
          if ( Basic_Expression() ) { } // if
          else throw UNEXPECTED_TOKEN ;

        } // if
        else throw UNEXPECTED_TOKEN ;

      } // if
      else throw UNEXPECTED_TOKEN ;

    } // if

  } // if
  else return false ;

  return true ;
} // Parser::Romce_and_Romloe()

bool Parser::Rest_of_Maybe_Logical_OR_Exp() {
  if ( Rest_of_Maybe_Logical_AND_Exp() ) {
    while ( mBuffer.type == OR ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Maybe_Logical_AND_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Rest_of_Maybe_Logical_OR_Exp()

bool Parser::Maybe_Logical_AND_Exp() {
  if ( Maybe_Bit_OR_Exp() ) {
    while ( mBuffer.type == AND ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      
      if ( Maybe_Bit_OR_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Maybe_Logical_AND_Exp()

bool Parser::Rest_of_Maybe_Logical_AND_Exp() {
  if ( Rest_of_Maybe_Bit_OR_Exp() ) {
    while ( mBuffer.type == AND ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Maybe_Bit_OR_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Rest_of_Maybe_Logical_AND_Exp()

bool Parser::Maybe_Bit_OR_Exp() {
  if ( Maybe_Bit_Ex_OR_Exp() ) {
    while ( mBuffer.type == BITOR ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      
      if ( Maybe_Bit_Ex_OR_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Maybe_Bit_OR_Exp()

bool Parser::Rest_of_Maybe_Bit_OR_Exp() {
  if ( Rest_of_Maybe_Bit_Ex_OR_Exp() ) {
    while ( mBuffer.type == BITOR ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Maybe_Bit_Ex_OR_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Rest_of_Maybe_Bit_OR_Exp()

bool Parser::Maybe_Bit_Ex_OR_Exp() {
  if ( Maybe_Bit_AND_Exp() ) {
    while ( mBuffer.type == XOR ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      
      if ( Maybe_Bit_AND_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Maybe_Bit_Ex_OR_Exp()

bool Parser::Rest_of_Maybe_Bit_Ex_OR_Exp() {
  if ( Rest_of_Maybe_Bit_AND_Exp() ) {
    while ( mBuffer.type == XOR ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Maybe_Bit_AND_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Rest_of_Maybe_Bit_Ex_OR_Exp()

bool Parser::Maybe_Bit_AND_Exp() {
  if ( Maybe_Equality_Exp() ) {
    while ( mBuffer.type == BITAND ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      
      if ( Maybe_Equality_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Maybe_Bit_AND_Exp()

bool Parser::Rest_of_Maybe_Bit_AND_Exp() {
  if ( Rest_of_Maybe_Equality_Exp() ) {
    while ( mBuffer.type == BITAND ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Maybe_Equality_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Rest_of_Maybe_Bit_AND_Exp()

bool Parser::Maybe_Equality_Exp() {
  if ( Maybe_Relational_Exp() ) {
    while ( mBuffer.type == EQ || mBuffer.type == NEQ ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      
      if ( Maybe_Relational_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Maybe_Equality_Exp()

bool Parser::Rest_of_Maybe_Equality_Exp() {
  if ( Rest_of_Maybe_Relational_Exp() ) {
    while ( mBuffer.type == EQ || mBuffer.type == NEQ  ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Maybe_Relational_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Rest_of_Maybe_Equality_Exp()

bool Parser::Maybe_Relational_Exp() {
  if ( Maybe_Shift_Exp() ) {
    while ( mBuffer.type == LESS_THAN || mBuffer.type == GREATER_THAN || 
            mBuffer.type == LE        || mBuffer.type == GE ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      
      if ( Maybe_Shift_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Maybe_Relational_Exp()

bool Parser::Rest_of_Maybe_Relational_Exp() {
  if ( Rest_of_Maybe_Shift_Exp() ) {
    while ( mBuffer.type == LESS_THAN || mBuffer.type == GREATER_THAN || 
            mBuffer.type == LE        || mBuffer.type == GE ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Maybe_Shift_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Rest_of_Maybe_Relational_Exp()

bool Parser::Maybe_Shift_Exp() {
  if ( Maybe_Additive_Exp() ) {
    while ( mBuffer.type == LS || mBuffer.type == RS ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      
      if ( Maybe_Additive_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Maybe_Shift_Exp()

bool Parser::Rest_of_Maybe_Shift_Exp() {
  if ( Rest_of_Maybe_Additive_Exp() ) {
    while ( mBuffer.type == LS || mBuffer.type == RS ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Maybe_Additive_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Rest_of_Maybe_Shift_Exp()

bool Parser::Maybe_Additive_Exp() {
  if ( Maybe_Mult_Exp() ) {
    while ( mBuffer.type == ADD || mBuffer.type == SUB ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      
      if ( Maybe_Mult_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Maybe_Additive_Exp()

bool Parser::Rest_of_Maybe_Additive_Exp() {
  if ( Rest_of_Maybe_Mult_Exp() ) {
    while ( mBuffer.type == ADD || mBuffer.type == SUB ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Maybe_Mult_Exp() ) { } // if
      else throw UNEXPECTED_TOKEN ;

    } // while

  } // if
  else return false ;

  return true ;
} // Parser::Rest_of_Maybe_Additive_Exp()

bool Parser::Maybe_Mult_Exp() {
  if ( Unary_Exp() ) {

    if ( Rest_of_Maybe_Mult_Exp() ) { } // if
    else throw UNEXPECTED_TOKEN ;

  } // if
  else return false ;

  return true ;
} // Parser::Maybe_Mult_Exp()

bool Parser::Rest_of_Maybe_Mult_Exp() {
  while ( mBuffer.type == MULT || mBuffer.type == DIVIDE || mBuffer.type == MOD ) {
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;

    if ( Unary_Exp() ) { } // if
    else throw UNEXPECTED_TOKEN ;

  } // while

  return true ;
} // Parser::Rest_of_Maybe_Mult_Exp()

bool Parser::Unary_Exp() {
  if ( Sign() ) {
    while ( Sign() ) { } // while

    if ( Signed_Unary_Exp() ) { } // if
    else throw UNEXPECTED_TOKEN ;

  } // if
  else if ( Unsigned_Unary_Exp() ) ; // else if
  else if ( mBuffer.type == PP || mBuffer.type == MM ) {
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
    if ( mBuffer.type == IDENT ) {
      if ( !FindDefinedToken( mBuffer.name ) )     // not sure!
        throw UNDEFINED_TOKEN ;
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      if ( mBuffer.type == L_SQUARE ) {
        mTokenList.push_back( mBuffer ) ;
        mBuffer = GetToken() ;
        if ( Expression() ) {
          if ( mBuffer.type == R_SQUARE ) {
            mTokenList.push_back( mBuffer ) ;
            mBuffer = GetToken() ;
          } // if
          else throw UNEXPECTED_TOKEN ;

        } // if
        else throw UNEXPECTED_TOKEN ;

      } // if

    } // if
    else throw UNEXPECTED_TOKEN ;

  } // else if
  else return false ;

  return true ;
} // Parser::Unary_Exp()

bool Parser::Signed_Unary_Exp() {
  if ( mBuffer.type == IDENT ) {
    if ( !FindDefinedToken( mBuffer.name ) && !FindDefinedFunction( mBuffer.name ) )
      throw UNDEFINED_TOKEN ;
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
    if ( mBuffer.type == L_PARENTHESES ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      if ( Actual_Parameter_List() ) 
        ;
      if ( mBuffer.type == R_PARENTHESES ) {
        mTokenList.push_back( mBuffer ) ;
        mBuffer = GetToken() ;
      } // if
      else throw UNEXPECTED_TOKEN ;
    } // if
    else if ( mBuffer.type == L_SQUARE ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      if ( Expression() ) {
        if ( mBuffer.type == R_SQUARE ) {
          mTokenList.push_back( mBuffer ) ;
          mBuffer = GetToken() ;
        } // if
        else throw UNEXPECTED_TOKEN ;
      } // if
      else throw UNEXPECTED_TOKEN ;
    } // else if

  } // if
  else if ( mBuffer.type == CONSTANT ) {
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
  } // else if
  else if ( mBuffer.type == L_PARENTHESES ) {
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
    if ( Expression() ) {
      if ( mBuffer.type == R_PARENTHESES ) {
        mTokenList.push_back( mBuffer ) ;
        mBuffer = GetToken() ;
      } // if
      else throw UNEXPECTED_TOKEN ;
    } // if
    else throw UNEXPECTED_TOKEN ;
  } // else if
  else return false ;

  return true ;
} // Parser::Signed_Unary_Exp()

bool Parser::Unsigned_Unary_Exp() {
  if ( mBuffer.type == IDENT ) {
    if ( !FindDefinedToken( mBuffer.name ) && !FindDefinedFunction( mBuffer.name ) )
      throw UNDEFINED_TOKEN ;
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
    if ( mBuffer.type == L_PARENTHESES ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;

      if ( Actual_Parameter_List() ) 
        ;

      if ( mBuffer.type == R_PARENTHESES ) {
        mTokenList.push_back( mBuffer ) ;
        mBuffer = GetToken() ;
      } // if
      else throw UNEXPECTED_TOKEN ;

    } // if
    else if ( mBuffer.type == L_SQUARE ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
      if ( Expression() ) {
        if ( mBuffer.type == R_SQUARE ) {
          mTokenList.push_back( mBuffer ) ;
          mBuffer = GetToken() ;

          if ( mBuffer.type == PP || mBuffer.type == MM ) {
            mTokenList.push_back( mBuffer ) ;
            mBuffer = GetToken() ;
          } // if

        } // if
        else throw UNEXPECTED_TOKEN ;
      } // if
      else throw UNEXPECTED_TOKEN ;

    } // else if
    else if ( mBuffer.type == PP || mBuffer.type == MM ) {
      mTokenList.push_back( mBuffer ) ;
      mBuffer = GetToken() ;
    } // else if

  } // if
  else if ( mBuffer.type == CONSTANT ) {
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
  } // else if
  else if ( mBuffer.type == L_PARENTHESES ) {
    mTokenList.push_back( mBuffer ) ;
    mBuffer = GetToken() ;
    if ( Expression() ) {
      if ( mBuffer.type == R_PARENTHESES ) {
        mTokenList.push_back( mBuffer ) ;
        mBuffer = GetToken() ;
      } // if
      else throw UNEXPECTED_TOKEN ;
    } // if
    else throw UNEXPECTED_TOKEN ;
  } // else if
  else return false ;

  return true ;
} // Parser::Unsigned_Unary_Exp()

// --------------------------------------------------------------------------------------------------

void Project3() {
  Parser * parser = new Parser() ;
  bool endOfProgram = false ;
  map< string, DefinedIDInfo > tempIDTable ;
  DefinedIDInfo tempDefinedID ;
  gStackOfID = new vector< map< string, DefinedIDInfo > >() ;
  gStackOfID->push_back( tempIDTable );
  tempDefinedID.type = COUT ;           
  tempDefinedID.value = "0" ;
  tempDefinedID.stackLayer = 0 ;                 
  gStackOfID->at( 0 )["cout"] = tempDefinedID ;              // 覆蓋變數內容

  cout << "Our-C running ..." << endl ;
  while ( !endOfProgram ) {
    try {

      if ( !parser->User_Input() ) 
        endOfProgram = true ;

    } // try
    catch ( ErrorType message ) {
      if ( parser->mBuffer.line == 0 ) 
        parser->mBuffer.line = 1 ;
        
      if ( message == UNEXPECTED_TOKEN ) {
        if ( parser->mBuffer.type == UNRECOGNIZED ) 
          cout << "Line " << parser->mBuffer.line << " : unrecognized token with first char \'" ;
        else
          cout << "Line " << parser->mBuffer.line << " : unexpected token \'" ;
        cout << parser->mBuffer.name << "\'\n" ;
      } // if
      else if ( message == UNDEFINED_TOKEN ) {
        cout << "Line " << parser->mBuffer.line << " : undefined identifier \'" ;
        cout << parser->mBuffer.name << "\'\n" ;
      } // else if
      else if ( message == END_OF_PROGRAM )
        endOfProgram = true ;

      char ch = cin.peek() ;
      while ( ch != '\n' ) {
        ch = cin.get() ;
        ch = cin.peek() ;
      } // while

      while ( gStackOfID->size() > 1 ) 
        gStackOfID->pop_back() ;
      gKeepBuffer = false ; 
    } // catch
    
    parser->mTokenList.clear() ;
  } // while
  
  cout << "Our-C exited ..." ;
  
} // Project3()

int main() {
  // int uTestNum = 0 ;
  cin >> gTestNum ;
  Project3() ;

  return 0 ;
} // main()