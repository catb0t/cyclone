/* Trevor Jim, 2001 */

%{
#include "xmlparse.h"
#include "xmlscan.h"
#include "string.h"

using Core;
using List;
using Xml;              // XML abstract syntax
using Lexing;

void yyerror(string s) { return; } 

// global state (we're not re-entrant)
namespace XmlParse {
Core::opt_t<Lexbuf<Function_lexbuf_state<Stdio::FILE@>>> lbuf = null;
static list_t<content_t> parse_result = null;
void error(string msg) {
  throw new Core::Failure(msg);
}

tunion context {
  InTag,
  Normal
};

static tunion context current = Normal;

void setContext(tunion context c) {
  current = c;
}

tunion context getContext() {
  return current;
} 


}

using XmlParse;

%}

// Tokens
%token ATTVALUE1
%token ATTVALUE2
%token CDSECT
%token CHARDATA
%token CLOSE
%token COMMENT
%token ENDOFFILE
%token EQ
%token NAME
%token OPEN
%token OPENSLASH
%token PI
%token REFERENCE
%token SLASHCLOSE

// Union of types of productions
%union{
  String_tok(string);
  Pi_tok($(Xml::name,string)@);
  Element_tok(Xml::element_t);
  Content_tok(List::list_t<Xml::content_t>);
  Attribute_tok(Xml::attribute_t);
  Attributes_tok(List::list_t<Xml::attribute_t>);
  STag_tok($(string,List::list_t<Xml::attribute_t>)@);
  EmptyElemTag_tok($(string,List::list_t<Xml::attribute_t>)@);
}

// Tags for productions
%type <String_tok> ATTVALUE1
%type <String_tok> ATTVALUE2
%type <String_tok> CDSECT
%type <String_tok> CHARDATA
%type <String_tok> COMMENT
%type <String_tok> NAME
%type <String_tok> OPEN
%type <String_tok> opn
%type <String_tok> OPENSLASH
%type <String_tok> opnslash
%type <Pi_tok>     PI
%type <String_tok> REFERENCE
%type <Element_tok> element
%type <Content_tok> content
%type <Content_tok> content0
%type <Attribute_tok> attribute
%type <Attributes_tok> attributes
%type <STag_tok> sTag
%type <String_tok> eTag
%type <EmptyElemTag_tok> emptyElemTag

%start content0

%%

content0: content     { $$ = $!1; XmlParse::parse_result = $1; }

element:
  emptyElemTag      { let $(n,a) = *($1); $$ = ^$(new Empty(n,a)); }
| sTag content eTag { let $(sn,a) = *($1);
                      let en = $3;
                      if (String::strcmp(sn,en) == 0)
                        $$ = ^$(new StartEnd(sn,a,$2));
                      else error("tag mismatch");
                    }

sTag:
  opn attributes cls { $$ = ^$(new $($1,$2)); }

opn:
  OPEN { setContext(InTag); $$ = $!1; }

attributes:
  /* empty */          { $$ = ^$(null); }
| attribute attributes { $$ = ^$(new List($1,$2)); }

attribute:
  NAME EQ ATTVALUE1 { $$ = ^$(new $($1,new Attvalue1($3))); }
| NAME EQ ATTVALUE2 { $$ = ^$(new $($1,new Attvalue2($3))); }

cls:
  CLOSE { setContext(Normal); }

content:
  /* empty */       { $$ = ^$(null); }
| element content   { $$ = ^$(new List(new Element($1),$2)); }
| CHARDATA content  { $$ = ^$(new List(new Chardata($1),$2)); }
| REFERENCE content { $$ = ^$(new List(new Reference($1),$2)); }
| CDSECT content    { $$ = ^$(new List(new Cdsect($1),$2)); }
| PI content        { $$ = ^$(new List(new Pi($1),$2)); }
| COMMENT content   { $$ = ^$(new List(new Comment($1),$2)); }

eTag:
  opnslash cls { $$ = $!1; }

opnslash:
  OPENSLASH { setContext(InTag); $$ = $!1; }

emptyElemTag:
  opn attributes slashcls { $$ = ^$(new $($1,$2)); }

slashcls:
  SLASHCLOSE { setContext(Normal); }

%%

namespace XmlParse{
  list_t<content_t> parse_file(Stdio::FILE @f) {
    parse_result = null;
    lbuf = new Core::Opt(from_file(f));
    XmlScan::init();
    yyparse();
    return parse_result;
  }
}

