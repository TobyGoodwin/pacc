module Parse where

import Char
import List

import Pos


---------- Data types used for parsing

data ErrorDescriptor =
	  Expected String
   	| Message String

data ParseError = ParseError {
			errorPos	:: Pos,
			errorDescrs	:: [ErrorDescriptor]
		}

data Result d v =
	  Parsed v d ParseError
        | NoParse ParseError

newtype Parser d v = Parser (d -> Result d v)


class Derivs d where
	dvPos	:: d -> Pos
	dvChar	:: d -> Result d Char


---------- Basic parsing combinators

infixl 2 </>		-- ordered choice
infixl 1 <?>		-- error labeling
infixl 1 <?!>		-- unconditional error labeling

-- Standard monadic combinators
instance Derivs d => Monad (Parser d) where 

	-- Sequencing combinator
	(Parser p1) >>= f = Parser parse

		where parse dvs = first (p1 dvs)

		      first (Parsed val rem err) = 
			let Parser p2 = f val
			in second err (p2 rem)
		      first (NoParse err) = NoParse err

		      second err1 (Parsed val rem err) =
			Parsed val rem (joinErrors err1 err)
		      second err1 (NoParse err) =
			NoParse (joinErrors err1 err)

	-- Result-producing combinator
	return x = Parser (\dvs -> Parsed x dvs (nullError dvs))

	-- Failure combinator
	fail [] = Parser (\dvs -> NoParse (nullError dvs))
	fail msg = Parser (\dvs -> NoParse (msgError (dvPos dvs) msg))

-- Ordered choice
(</>) :: Derivs d => Parser d v -> Parser d v -> Parser d v
(Parser p1) </> (Parser p2) = Parser parse

		where parse dvs = first dvs (p1 dvs)

		      first dvs (result @ (Parsed val rem err)) = result
		      first dvs (NoParse err) = second err (p2 dvs)

		      second err1 (Parsed val rem err) =
			Parsed val rem (joinErrors err1 err)
		      second err1 (NoParse err) =
			NoParse (joinErrors err1 err)

-- Semantic predicate: 'satisfy <parser> <pred>' acts like <parser>
-- but only succeeds if the result it generates satisfies <pred>.
satisfy :: Derivs d => Parser d v -> (v -> Bool) -> Parser d v
satisfy (Parser p) test = Parser parse

		where parse dvs = check dvs (p dvs)

		      check dvs (result @ (Parsed val rem err)) =
			if test val then result
				    else NoParse (nullError dvs)
		      check dvs none = none

-- Syntactic predicate: 'followedBy <parser>' acts like <parser>
-- but does not consume any input.
followedBy :: Derivs d => Parser d v -> Parser d v
followedBy (Parser p) = Parser parse

	where parse dvs = case (p dvs) of
		Parsed val rem err -> Parsed val dvs (nullError dvs)
		err -> err

-- Negative syntactic predicate: 'followedBy <parser>' invokes <parser>,
-- then succeeds without consuming any input if <parser> fails,
-- and fails if <parser> succeeds.
notFollowedBy :: Derivs d => Parser d v -> Parser d ()
notFollowedBy (Parser p) = Parser parse

	where parse dvs = case (p dvs) of
		Parsed val rem err -> NoParse (nullError dvs)
		NoParse err -> Parsed () dvs (nullError dvs)

-- Optional combinator: 'optional <parser>' invokes <parser>,
-- then produces the result 'Just <v>' if <parser> produced <v>,
-- or else produces the success result 'Nothing'
-- without consuming any input if <parser> failed.
optional :: Derivs d => Parser d v -> Parser d (Maybe v)
optional p = (do v <- p; return (Just v)) </> return Nothing


---------- Iterative combinators
-- Note: use of these combinators can break
-- a packrat parser's linear-time guarantee.

-- Zero or more repetition combinator:
-- 'many <parser>' invokes <parser> repeatedly until it fails,
-- collecting all success result values into a list.
-- Always succeeds, producing an empty list in the degenerate case.
many :: Derivs d => Parser d v -> Parser d [v]
many p = (do { v <- p; vs <- many p; return (v : vs) } )
	 </> return []

-- One or more repetition combinator:
-- 'many1 <parser>' invokes <parser> repeatedly until it fails,
-- collecting all success result values into a list.
-- Fails if <parser> does not succeed even once.
many1 :: Derivs d => Parser d v -> Parser d [v]
many1 p = do { v <- p; vs <- many p; return (v : vs) }

-- One or more repetitions with a separator:
-- 'sepBy1 <parser> <separator>' scans one or more iterations of <parser>,
-- with a match of <separator> between each instance.
-- Only the results of <parser> are collected into the final result list.
sepBy1 :: Derivs d => Parser d v -> Parser d vsep -> Parser d [v]
sepBy1 p psep = do v <- p
		   vs <- many (do { psep; p })
		   return (v : vs)

-- Zero or more repetitions with a separator:
-- like sepBy1, but succeeds with an empty list if nothing can be parsed.
sepBy :: Derivs d => Parser d v -> Parser d vsep -> Parser d [v]
sepBy p psep = sepBy1 p psep </> return []

-- Zero or more repetitions with a terminator
endBy :: Derivs d => Parser d v -> Parser d vend -> Parser d [v]
endBy p pend = many (do { v <- p; pend; return v })

-- One or more repetitions with a terminator
endBy1 :: Derivs d => Parser d v -> Parser d vend -> Parser d [v]
endBy1 p pend = many1 (do { v <- p; pend; return v })

-- One or more repetitions with a separator or terminator:
-- 'sepEndBy1 <parser> <septerm>' scans for a sequence of <parser> matches
-- in which instances are separated by <septerm>,
-- and if a <septerm> is found following the last <parser> match
-- then it is consumed as well.
sepEndBy1 :: Derivs d => Parser d v -> Parser d vsep -> Parser d [v]
sepEndBy1 p psep = do v <- sepBy1 p psep; optional psep; return v

-- Zero or more repetitions with a separator or terminator.
sepEndBy :: Derivs d => Parser d v -> Parser d vsep -> Parser d [v]
sepEndBy p psep = do v <- sepBy p psep; optional psep; return v

-- One or more repetitions separated by left-associative operators.
-- 'chainl1 <term> <oper>' matches instances of <term> separated by <oper>,
-- but uses the result of <oper> as a left-associative binary combinator:
-- e.g., 't1 op t2 op t3' is interpreted as '(t1 op t2) op t3'
chainl1 :: Derivs d => Parser d v -> Parser d (v->v->v) -> Parser d v
chainl1 p psep =
	let psuffix z = (do f <- psep
			    v <- p
			    psuffix (f z v))
			</> return z
	in do v <- p
	      psuffix v

-- Zero or more repetitions separated by left-associative operators.
chainl :: Derivs d => Parser d v -> Parser d (v->v->v) -> v -> Parser d v
chainl p psep z = chainl1 p psep </> return z

-- One or more repetitions separated by left-associative operators:
-- e.g., 't1 op t2 op t3' is interpreted as 't1 op (t2 op t3)'
chainr1 :: Derivs d => Parser d v -> Parser d (v->v->v) -> Parser d v
chainr1 p psep = (do v <- p
		     f <- psep
		     w <- chainr1 p psep
		     return (f v w))
		 </> p

-- Zero or more repetitions separated by left-associative operators.
chainr :: Derivs d => Parser d v -> Parser d (v->v->v) -> v -> Parser d v
chainr p psep z = chainr1 p psep </> return z

-- N-ary ordered choice:
-- given a list of parsers producing results of the same type,
-- try them all in order and use the first successful result.
choice :: Derivs d => [Parser d v] -> Parser d v
choice [p] = p
choice (p:ps) = p </> choice ps


---------- Error handling
instance Eq ErrorDescriptor where
	Expected e1 == Expected e2	= e1 == e2
	Message m1 == Message m2	= m1 == m2
	_ == _				= False

failAt :: Derivs d => Pos -> String -> Parser d v
failAt pos msg = Parser (\dvs -> NoParse (msgError pos msg))

-- Annotate a parser with a description of the construct to be parsed.
-- The resulting parser yields an "expected" error message
-- if the construct cannot be parsed
-- and if no error information is already available
-- indicating a position farther right in the source code
-- (which would normally be more localized/detailed information).
(<?>) :: Derivs d => Parser d v -> String -> Parser d v
(Parser p) <?> desc = Parser (\dvs -> munge dvs (p dvs))

			where munge dvs (Parsed v rem err) =
				Parsed v rem (fix dvs err)
			      munge dvs (NoParse err) =
				NoParse (fix dvs err)

			      fix dvs (err @ (ParseError p ms)) =
				if p > dvPos dvs then err
				else expError (dvPos dvs) desc

-- Stronger version of the <?> error annotation operator above,
-- which unconditionally overrides any existing error information.
(<?!>) :: Derivs d => Parser d v -> String -> Parser d v
(Parser p) <?!> desc = Parser (\dvs -> munge dvs (p dvs))

			where munge dvs (Parsed v rem err) =
				Parsed v rem (fix dvs err)
			      munge dvs (NoParse err) =
				NoParse (fix dvs err)

			      fix dvs (err @ (ParseError p ms)) =
				expError (dvPos dvs) desc

-- Join two ParseErrors, giving preference to the one farthest right,
-- or merging their descriptor sets if they are at the same position.
joinErrors :: ParseError -> ParseError -> ParseError
joinErrors (e @ (ParseError p m)) (e' @ (ParseError p' m')) =
	if p' > p || null m then e'
	else if p > p' || null m' then e
	else ParseError p (m `union` m')

nullError dvs = ParseError (dvPos dvs) []

expError pos desc = ParseError pos [Expected desc]

msgError pos msg = ParseError pos [Message msg]

eofError dvs = msgError (dvPos dvs) "end of input"

expected :: Derivs d => String -> Parser d v
expected desc = Parser (\dvs -> NoParse (expError (dvPos dvs) desc))

unexpected :: Derivs d => String -> Parser d v
unexpected str = fail ("unexpected " ++ str)

-- Comparison operators for ParseError just compare relative positions.
instance Eq ParseError where
	ParseError p1 m1 == ParseError p2 m2	= p1 == p2
	ParseError p1 m1 /= ParseError p2 m2	= p1 /= p2

instance Ord ParseError where
	ParseError p1 m1 < ParseError p2 m2	= p1 < p2
	ParseError p1 m1 > ParseError p2 m2	= p1 > p2
	ParseError p1 m1 <= ParseError p2 m2	= p1 <= p2
	ParseError p1 m1 >= ParseError p2 m2	= p1 >= p2

	-- Special behavior: "max" joins two errors
	max p1 p2 = joinErrors p1 p2
	min p1 p2 = undefined

-- Show function for error messages
instance Show ParseError where
	show (ParseError pos []) = 
		show pos ++ ": parse error"
	show (ParseError pos msgs) = expectmsg expects ++ messages msgs
	   where
		expects = getExpects msgs
		getExpects [] = []
		getExpects (Expected exp : rest) = exp : getExpects rest
		getExpects (Message msg : rest) = getExpects rest

		expectmsg [] = ""
		expectmsg [exp] = show pos ++ ": expecting " ++ exp ++ "\n"
		expectmsg [e1, e2] = show pos ++ ": expecting either "
					++ e1 ++ " or " ++ e2 ++ "\n"
		expectmsg (first : rest) = show pos ++ ": expecting one of: "
						++ first ++ expectlist rest
						++ "\n"
		expectlist [last] = ", or " ++ last
		expectlist (mid : rest) = ", " ++ mid ++ expectlist rest

		messages [] = []
		messages (Expected exp : rest) = messages rest
		messages (Message msg : rest) =
			show pos ++ ": " ++ msg ++ "\n" ++ messages rest


---------- Character-oriented parsers

-- 'anyChar' matches any single character.
anyChar :: Derivs d => Parser d Char
anyChar = Parser dvChar

-- 'char <c>' matches the specific character <c>.
char :: Derivs d => Char -> Parser d Char
char ch = satisfy anyChar (\c -> c == ch) <?> show ch

-- 'oneOf <s>' matches any character in string <s>.
oneOf :: Derivs d => [Char] -> Parser d Char
oneOf chs = satisfy anyChar (\c -> c `elem` chs)
	    <?> ("one of the characters " ++ show chs)

-- 'noneOf <s>' matches any character not in string <s>.
noneOf :: Derivs d => [Char] -> Parser d Char
noneOf chs = satisfy anyChar (\c -> not (c `elem` chs))
	     <?> ("any character not in " ++ show chs)

-- 'string <s>' matches all the characters in <s> in sequence.
string :: Derivs d => String -> Parser d String
string str = p str <?> show str

	where p [] = return str
	      p (ch:chs) = do { char ch; p chs }

-- 'stringFrom <ss>' matches any string in the list of strings <ss>.
-- If any strings in <ss> are prefixes of other strings in <ss>,
-- then the prefixes must appear later in the list
-- in order for the longer strings to be recognized at all.
stringFrom :: Derivs d => [String] -> Parser d String
stringFrom [str] = string str
stringFrom (str : strs) = string str </> stringFrom strs

-- Match an uppercase letter.
upper :: Derivs d => Parser d Char
upper = satisfy anyChar isUpper <?> "uppercase letter"

-- Match a lowercase letter.
lower :: Derivs d => Parser d Char
lower = satisfy anyChar isLower <?> "lowercase letter"

-- Match any letter.
letter :: Derivs d => Parser d Char
letter = satisfy anyChar isAlpha <?> "letter"

-- Match any letter or digit.
alphaNum :: Derivs d => Parser d Char
alphaNum = satisfy anyChar isAlphaNum <?> "letter or digit"

-- Match any digit.
digit :: Derivs d => Parser d Char
digit = satisfy anyChar isDigit <?> "digit"

-- Match any hexadecimal digit.
hexDigit :: Derivs d => Parser d Char
hexDigit = satisfy anyChar isHexDigit <?> "hexadecimal digit (0-9, a-f)"

-- Match any octal digit.
octDigit :: Derivs d => Parser d Char
octDigit = satisfy anyChar isOctDigit <?> "octal digit (0-7)"

-- Match a newline.
newline :: Derivs d => Parser d Char
newline = char '\n'

-- Match a tab character.
tab :: Derivs d => Parser d Char
tab = char '\t'

-- Match any whitespace character (space, tab, newline, etc.).
space :: Derivs d => Parser d Char
space = satisfy anyChar isSpace <?> "whitespace character"

-- Match a sequence of zero or more whitespace characters.
spaces :: Derivs d => Parser d [Char]
spaces = many space

-- Match the end of file (i.e., "the absence of a character").
eof :: Derivs d => Parser d ()
eof = notFollowedBy anyChar <?> "end of input"


---------- Parser state manipulation combinators

-- Combinator to get the Derivs object for the current position:
-- e.g., 'dvs <- getDerivs' as part of a 'do' sequence.
getDerivs :: Derivs d => Parser d d
getDerivs = Parser (\dvs -> Parsed dvs dvs (nullError dvs))

-- Combinator to set the Derivs object used for subsequent parsing;
-- typically used to change parsing state elements in the Derivs tuple.
setDerivs :: Derivs d => d -> Parser d ()
setDerivs newdvs = Parser (\dvs -> Parsed () newdvs (nullError dvs))

-- Get the current position in the input text.
getPos :: Derivs d => Parser d Pos
getPos = Parser (\dvs -> Parsed (dvPos dvs) dvs (nullError dvs))

