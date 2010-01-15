module Pappy where

type Identifier = String
type RawCode = String

data Match =
	  MatchAnon Rule			-- anonymous match
	| MatchName Rule Identifier		-- unrestricted named match
	| MatchPat Rule RawCode			-- match Haskell pattern
	| MatchString Rule String		-- match string constant
	| MatchAnd Rule				-- and-followed-by predicate
	| MatchNot Rule				-- not-followed-by predicate
	| MatchPred RawCode			-- semantic predicate

data Producer =
	  ProdName Identifier			-- select a single identifier
	| ProdCode RawCode			-- arbitrary Haskell expression

data Rule =
	-- Standard rules
	  RulePrim Identifier			-- nonterminal
	| RuleChar Char				-- character literal
	| RuleSeq [Match] Producer		-- sequence
	| RuleAlt [Rule]			-- alternation
	| RuleOpt Rule				-- optional

	-- Composite rules
	| RuleString String			-- string literal
	| RuleStar Rule				-- zero or more
	| RulePlus Rule				-- one or more

	-- Error handling
	| RuleExpect Rule [String]		-- construct(s) expected

	-- Low-level rules for efficient single-choice alternation
	| RuleSwitchChar [(Char, Rule)] (Maybe Rule)
	| RuleSwitchString Identifier [(String, Rule)] (Maybe Rule)


-- Nonterminal definition: (nonterminal name, type, definition)
type Nonterminal = (Identifier, RawCode, Rule)

-- Grammar definition: (grammar name, top-level code, top-level nonterminals,
--			nonterminal definitions)
type Grammar = (Identifier, RawCode, [Identifier], [Nonterminal])



-- Structural equality testing for rules
instance Eq Match where
	MatchAnon r1 == MatchAnon r2		= r1 == r2
	MatchName r1 id1 == MatchName r2 id2	= r1 == r2 && id1 == id2
	MatchPat r1 p1 == MatchPat r2 p2	= r1 == r2 && p1 == p2
	MatchString r1 s1 == MatchString r2 s2	= r1 == r2 && s1 == s2
	MatchAnd r1 == MatchAnd r2		= r1 == r2
	MatchNot r1 == MatchNot r2		= r1 == r2
	MatchPred p1 == MatchPred p2		= p1 == p2
	_ == _					= False

instance Eq Producer where
	ProdName id1 == ProdName id2		= id1 == id2
	ProdCode c1 == ProdCode c2		= c1 == c2
	_ == _					= False

instance Eq Rule where
	RulePrim n1 == RulePrim n2		= n1 == n2
	RuleChar c1 == RuleChar c2		= c1 == c2
	RuleString s1 == RuleString s2		= s1 == s2
	RuleSeq ms1 p1 == RuleSeq ms2 p2	= ms1 == ms2 && p1 == p2
	RuleAlt rs1 == RuleAlt rs2		= rs1 == rs2
	RuleOpt r1 == RuleOpt r2		= r1 == r2
	RuleStar r1 == RuleStar r2		= r1 == r2
	RulePlus r1 == RulePlus r2		= r1 == r2
	RuleExpect r1 ss1 == RuleExpect r2 ss2	= r1 == r2 && ss1 == ss2
	RuleSwitchChar crs1 dfl1 ==
		RuleSwitchChar crs2 dfl2	= crs1 == crs2 && dfl1 == dfl2
	_ == _					= False


-- Show functions
instance Show Match where
	show (MatchAnon r) = showRuleUnary r
	show (MatchName r id) = showRuleUnary r ++ ":" ++ id
	show (MatchPat r p) = showRuleUnary r ++ ":{" ++ p ++ "}"
	show (MatchString r s) = showRuleUnary r ++ ":" ++ show s
	show (MatchAnd r) = "&" ++ showRuleUnary r
	show (MatchNot r) = "!" ++ showRuleUnary r
	show (MatchPred c) = "&{" ++ c ++ "}"

instance Show Producer where
	show (ProdName id) = id
	show (ProdCode c) = "{" ++ c ++ "}"

instance Show Rule where
	show r = showRuleExpect r


showRuleExpect (RuleExpect r (s:ss)) =
	showRuleAlt r ++ " # " ++ s ++ concat (map (\s -> ", " ++ s) ss)
showRuleExpect (RuleExpect r _) = error "empty RuleExpect!"
showRuleExpect r = showRuleAlt r

showRuleAlt (RuleAlt rs) = alts rs where
	alts [] = "<error: no alternatives in RuleAlt>"
	alts [r] = showRuleSeq r
	alts (r : rs) = showRuleSeq r ++ " | " ++ alts rs
showRuleAlt r = showRuleSeq r

showRuleSeq (RuleSeq ms p) = seq ms where
	seq [] = "-> " ++ show p
	seq (m : ms) = show m ++ " " ++ seq ms
showRuleSeq r = showRuleUnary r

showRuleUnary (RuleOpt r) = showRulePrimary r ++ "?"
showRuleUnary (RuleStar r) = showRulePrimary r ++ "*"
showRuleUnary (RulePlus r) = showRulePrimary r ++ "+"
showRuleUnary r = showRulePrimary r

showRulePrimary (RulePrim n) = n
showRulePrimary (RuleChar c) = show c
showRulePrimary (RuleString s) = show s
showRulePrimary (RuleSwitchChar crs dfl) = "<<SwitchChar>>"	-- XXX
showRulePrimary r = "(" ++ showRuleExpect r ++ ")"

showGrammar (parsername, topcode, nonterms) =
	"parser " ++ parsername ++":\n\n{" ++ topcode ++ "}\n\n" ++
	showNonterminals nonterms

showNonterminals [] = ""
showNonterminals ((n, t, r) : nts) =
	n ++ " :: " ++ t ++ " = \n" ++ rule r ++ "\n\n" ++
	showNonterminals nts

	where
	rule (RuleAlt (r : rs)) = "\t  " ++ showRuleSeq r ++ rules rs
	rule r = "\t  " ++ showRuleAlt r

	rules (r : rs) = "\n\t| " ++ showRuleSeq r ++ rules rs
	rules [] = ""


-- Calculate a simple size metric over a grammar
sizeofNonterminals :: [Nonterminal] -> Int
sizeofNonterminals ((n, t, r) : nts) =
	1 + sizeofRule r + sizeofNonterminals nts
sizeofNonterminals [] = 0

sizeofRules :: [Rule] -> Int
sizeofRules (r : rs) = sizeofRule r + sizeofRules rs
sizeofRules [] = 0

sizeofRule :: Rule -> Int
sizeofRule (RulePrim n) = 1
sizeofRule (RuleChar c) = 1
sizeofRule (RuleString s) = 1
sizeofRule (RuleSeq ms p) = 1 + seq ms where
	seq (m : ms) = sizeofMatch m + seq ms
	seq [] = 0
sizeofRule (RuleAlt rs) = 1 + sizeofRules rs
sizeofRule (RuleOpt r) = 1 + sizeofRule r
sizeofRule (RuleStar r) = 1 + sizeofRule r
sizeofRule (RulePlus r) = 1 + sizeofRule r
sizeofRule (RuleExpect r exs) = 1 + sizeofRule r
sizeofRule (RuleSwitchChar crs dfl) = 1 + cases crs + deflt dfl where
	cases ((c, r) : crs) = sizeofRule r + cases crs
	cases [] = 0
	deflt (Just r) = sizeofRule r
	deflt (Nothing) = 0

sizeofMatch :: Match -> Int
sizeofMatch (MatchAnon r) = sizeofRule r
sizeofMatch (MatchName r id) = sizeofRule r
sizeofMatch (MatchPat r p) = sizeofRule r
sizeofMatch (MatchString r s) = sizeofRule r
sizeofMatch (MatchAnd r) = sizeofRule r
sizeofMatch (MatchNot r) = sizeofRule r
sizeofMatch (MatchPred r) = 1

