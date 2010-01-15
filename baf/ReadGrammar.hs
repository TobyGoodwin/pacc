-- Packrat parser for Pappy grammar definition files
module ReadGrammar (pappyParse, pappyParseFile) where

import Char

import Pos
import Parse
import Pappy


-------------------- Top-level tie-up --------------------

data PappyDerivs = PappyDerivs {

		-- Grammar definition structure
		pdGrammar	:: Result PappyDerivs Grammar,
		pdNonterminal	:: Result PappyDerivs Nonterminal,

		-- Grammar rule expressions
		pdAltRule	:: Result PappyDerivs Rule,
		pdSeqRule	:: Result PappyDerivs Rule,
		pdSeqMatch	:: Result PappyDerivs Match,
		pdUnaryRule	:: Result PappyDerivs Rule,
		pdPrimRule	:: Result PappyDerivs Rule,

		-- Lexical structure
		pdIdentifier	:: Result PappyDerivs String,
		pdWord		:: Result PappyDerivs String,
		pdSymbol	:: Result PappyDerivs String,
		pdRawCode	:: Result PappyDerivs String,
		pdCodeBlock	:: Result PappyDerivs String,
		pdCodeChars	:: Result PappyDerivs String,
		pdCodeSQChars	:: Result PappyDerivs String,
		pdCodeDQChars	:: Result PappyDerivs String,
		pdCharLit	:: Result PappyDerivs Char,
		pdStringLit	:: Result PappyDerivs String,
		pdWhitespace	:: Result PappyDerivs String,

		-- Input text
		pdChar		:: Result PappyDerivs Char,
		pdPos		:: Pos
	}

instance Derivs PappyDerivs where
	dvPos d = pdPos d
	dvChar d = pdChar d

grammar		= Parser pdGrammar
nonterminal	= Parser pdNonterminal
altRule		= Parser pdAltRule
seqRule		= Parser pdSeqRule
seqMatch	= Parser pdSeqMatch
unaryRule	= Parser pdUnaryRule
primRule	= Parser pdPrimRule
identifier	= Parser pdIdentifier
word		= Parser pdWord
symbol		= Parser pdSymbol
rawCode		= Parser pdRawCode
codeBlock	= Parser pdCodeBlock
codeChars	= Parser pdCodeChars
codeSQChars	= Parser pdCodeSQChars
codeDQChars	= Parser pdCodeDQChars
charLit		= Parser pdCharLit
stringLit	= Parser pdStringLit
whitespace	= Parser pdWhitespace

pappyParse :: String -> String -> Either Grammar String
pappyParse name text =
	let	initpos = Pos name 1 1
		d = pappyDerivs initpos text
	in case pdGrammar d of
		Parsed g _ _ -> Left g
		NoParse e -> Right (show e)

pappyParseFile :: String -> IO Grammar
pappyParseFile filename =
	do	text <- readFile filename
		let	initpos = Pos filename 1 1
			d = pappyDerivs initpos text
		case pdGrammar d of
			Parsed g _ _ -> return g
			NoParse e -> fail (show e)

pappyDerivs :: Pos -> String -> PappyDerivs
pappyDerivs pos text = d
	where	d = PappyDerivs
			(pGrammar d)
			(pNonterminal d)
			(pAltRule d)
			(pSeqRule d)
			(pSeqMatch d)
			(pUnaryRule d)
			(pPrimRule d)
			(pIdentifier d)
			(pWord d)
			(pSymbol d)
			(pRawCode d)
			(pCodeBlock d)
			(pCodeChars d)
			(pCodeSQChars d)
			(pCodeDQChars d)
			(pCharLit d)
			(pStringLit d)
			(pWhitespace d)
			chr pos
		chr = case text of
			[] -> NoParse (eofError d)
			(c:cs) -> Parsed c (pappyDerivs (nextPos pos c) cs)
					 (nullError d)

-------------------- Grammar Definition Structure --------------------

pGrammar :: PappyDerivs -> Result PappyDerivs Grammar
Parser pGrammar =
	do	whitespace
		keyword "parser"; name <- identifier; sym ":"
		c1 <- rawCode </> return []
		keyword "top"; tops <- sepBy1 identifier (sym ",")
		ns <- many1 nonterminal
		c2 <- rawCode </> return []
		let code = c1 ++ "\n" ++ c2 ++ "\n"
		notFollowedBy anyChar
		return (name, code, tops, ns)

pNonterminal :: PappyDerivs -> Result PappyDerivs Nonterminal
Parser pNonterminal =
	do	n <- identifier
		sym "::"
		t <- identifier </> rawCode
		sym "="
		r <- altRule
		return (n, t, r)


-------------------- Grammar Rule Expressions --------------------

pAltRule :: PappyDerivs -> Result PappyDerivs Rule
Parser pAltRule = 
	do alts <- sepBy1 seqRule (sym "/")
	   case alts of
		[a] -> return a
		_ -> return (RuleAlt alts)

pSeqRule :: PappyDerivs -> Result PappyDerivs Rule
Parser pSeqRule =
	    (do ms <- many seqMatch
		sym "->"
		id <- identifier
		return (RuleSeq ms (ProdName id)))
	</> (do ms <- many seqMatch
		sym "->"
		c <- rawCode
		return (RuleSeq ms (ProdCode c)))
	</> unaryRule

pSeqMatch :: PappyDerivs -> Result PappyDerivs Match
Parser pSeqMatch =
	    (do i <- identifier
		sym ":"
		r <- unaryRule
		return (MatchName r i))
	</> (do p <- rawCode
		sym ":"
		r <- unaryRule
		return (MatchPat r p))
	</> (do c <- charLit
		sym ":"
		r <- unaryRule
		return (MatchString r [c]))
	</> (do s <- stringLit
		sym ":"
		r <- unaryRule
		return (MatchString r s))
	</> (do sym "&"
		r <- unaryRule
		return (MatchAnd r))
	</> (do sym "!"
		r <- unaryRule
		return (MatchNot r))
	</> (do sym "&"
		p <- rawCode
		return (MatchPred p))
	</> (do r <- unaryRule
		return (MatchAnon r))

pUnaryRule :: PappyDerivs -> Result PappyDerivs Rule
Parser pUnaryRule =
	    (do	r <- primRule
		sym "?"
		return (RuleOpt r))
	</> (do	r <- primRule
		sym "+"
		return (RulePlus r))
	</> (do	r <- primRule
		sym "*"
		return (RuleStar r))
	</> primRule

pPrimRule :: PappyDerivs -> Result PappyDerivs Rule
Parser pPrimRule =
	    (do n <- identifier
		return (RulePrim n))
	</> (do c <- charLit
		return (RuleChar c))
	</> (do s <- stringLit
		return (RuleString s))
	</> (do sym "("
		r <- altRule
		sym ")"
		return r)

-------------------- Lexical Structure --------------------


-- Keywords and identifiers

keyword :: String -> Parser PappyDerivs String
keyword w =
	(do	s <- word
		if s == w then return w else fail "")
	<?!> show w

pIdentifier :: PappyDerivs -> Result PappyDerivs String
Parser pIdentifier =
	(do	s <- word
		if s `elem` keywords then fail "" else return s)
	<?!> "identifier"

pWord :: PappyDerivs -> Result PappyDerivs String
Parser pWord =
	do	c <- satisfy anyChar isIdentStart
		cs <- many (satisfy anyChar isIdentCont)
		whitespace
		return (c : cs)

isIdentStart c = isAlpha c || c == '_'
isIdentCont c = isIdentStart c || isDigit c || c == '\''

keywords = [
	"parser", "top"]


-- Symbols: operators, parentheses, etc.

sym :: String -> Parser PappyDerivs String
sym s = (do	s' <- symbol
		if s' == s then return s else fail "")
	    <?!> show s

pSymbol :: PappyDerivs -> Result PappyDerivs String
Parser pSymbol =
	do	s <- stringFrom symbols
		whitespace
		return s

symbols = [
	"->",
	"::",
	":",
	"=",
	"(",
	")",
	"/",
	"+",
	"*",
	"?",
	"&",
	"!"]


-- Raw code block, ignoring following whitespace
pRawCode :: PappyDerivs -> Result PappyDerivs String
Parser pRawCode =
	do	c <- codeBlock
		whitespace
		return c

-- Raw code block, consuming only the braces and contents
pCodeBlock :: PappyDerivs -> Result PappyDerivs String
Parser pCodeBlock =
	do	char '{'
		c <- codeChars
		char '}'
		return c

-- Characters in a code block
pCodeChars :: PappyDerivs -> Result PappyDerivs String
Parser pCodeChars =
	    (do	b <- codeBlock		-- nested code blocks
		cs <- codeChars
		return ("{" ++ b ++ "}" ++ cs))
	</> (do char '\''		-- character literals
		lit <- codeSQChars
		char '\''
		cs <- codeChars
		return ("\'" ++ lit ++ "\'" ++ cs))
	</> (do char '\"'		-- string literals
		lit <- codeDQChars
		char '\"'
		cs <- codeChars
		return ("\"" ++ lit ++ "\"" ++ cs))
	</> (do ic <- satisfy anyChar isIdentStart	-- identifiers
		ics <- many (satisfy anyChar isIdentCont)
		cs <- codeChars
		return (ic : ics ++ cs))
	</> (do s <- lineComment	-- comments
		cs <- codeChars
		return (s ++ cs))
	</> (do c <- noneOf "{}\"\'"	-- other characters
		cs <- codeChars
		return (c : cs))
	</> return []

-- Characters in a single-quoted character literal in a code block
pCodeSQChars :: PappyDerivs -> Result PappyDerivs String
Parser pCodeSQChars =
	    (do char '\\'
		c <- anyChar
		cs <- codeSQChars
		return ('\\' : c : cs))
	</> (do c <- noneOf "\'\\\r\n"
		cs <- codeSQChars
		return (c : cs))
	</> return []

-- Characters in a double-quoted string literal in a code block
pCodeDQChars :: PappyDerivs -> Result PappyDerivs String
Parser pCodeDQChars =
	    (do char '\\'
		c <- anyChar
		cs <- codeDQChars
		return ('\\' : c : cs))
	</> (do c <- noneOf "\"\\\r\n"
		cs <- codeDQChars
		return (c : cs))
	</> return []

-- Character and string literals

quotedChar quote =
	(do char '\\'
	    c <- anyChar
	    case c of
		'n' -> return '\n'
		'r' -> return '\r'
		't' -> return '\t'
		'v' -> return '\v'
		'f' -> return '\f'
		'\\' -> return '\\'
		'\'' -> return '\''
		'\"' -> return '\"'
		-- XXX octal characters, other escapes
		_ -> fail "invalid escape sequence")
	</> satisfy anyChar (\c -> c /= quote)

pCharLit :: PappyDerivs -> Result PappyDerivs Char
Parser pCharLit = (do char '\''
		      c <- quotedChar '\''
		      char '\''
		      whitespace
		      return c)

pStringLit :: PappyDerivs -> Result PappyDerivs String
Parser pStringLit = (do char '"'
		        s <- many (quotedChar '"')
		        char '"'
			whitespace
		        return s)


-- Whitespace

pWhitespace :: PappyDerivs -> Result PappyDerivs String
Parser pWhitespace =
	do	ss <- many (many1 spaceChar </> lineComment)
		return (concat ss)

spaceChar :: Parser PappyDerivs Char
spaceChar = satisfy anyChar isSpace

--flowComment :: Parser PappyDerivs ()
--flowComment =
--	do	string "-*"
--		many (do notFollowedBy (string "*/"); anyChar)
--		string "*-"
--		return ()

lineComment :: Parser PappyDerivs String
lineComment =
	do	s1 <- string "--"
		s2 <- many (do notFollowedBy lineTerminator; anyChar)
		s3 <- lineTerminator
		return (s1 ++ s2 ++ s3)

lineTerminator :: Parser PappyDerivs String
lineTerminator =
	    (do string "\r\n")
	</> string "\r"
	</> string "\n"

