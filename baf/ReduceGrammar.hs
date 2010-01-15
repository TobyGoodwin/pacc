-- Pappy grammar analysis/reduction module:
--
--	- checks that all referenced nonterminals have definitions
--	- checks that nonterminals are not defined multiple times
--	- checks for illegal (e.g., indirect) left recursion
--	- rewrites simple left-recursive rules using right recursion
--	- rewrites '*' (zero-or-more) and '+' (one-or-more) iteration rules
--
module ReduceGrammar (reduceGrammar) where

import Pappy

-- Rewrite composite and simple left recursive rules
reduceGrammar :: Grammar -> Grammar
reduceGrammar (parsername, topcode, topnts, nonterms) = g'' where

	-- First reduce the grammar
	g' = (parsername, topcode, topnts, reverse (reducents [] nonterms))

	-- Then check it for remaining illegal left recursion
	g'' = case checkLeftRecursion g' of
		Just e -> error e
		Nothing -> g'


	-- Reduce the rules in a grammar and add them to 'ng'
	reducents ng [] = ng
	reducents ng ((n, t, r) : g) =
		if existstnt n ng
		then error ("Duplicate nonterminal " ++ show n)
		else reducents ng''' g where

		-- First rewrite composite constructs in the rule
		(ng', r') = rerule ng r

		-- Then eliminate simple left recursion, if any
		(ng'', r'') = elimleft ng' n t r'

		-- Add the final reduced nonterminal to the grammar
		ng''' = (n, t, r'') : ng''


	-- Reduce iterative operators in a grammar rule.
	rerule ng (r @ (RulePrim n)) =
		if existstnt n nonterms
		then (ng, r)
		else error ("Reference to undefined nonterminal " ++ show n)

	rerule ng (r @ (RuleChar c)) = (ng, r)

	rerule ng (RuleSeq ms p) = (ng', RuleSeq ms' p) where

		(ng', ms') = reseq ng ms

		reseq ng [] = (ng, [])
		reseq ng (MatchAnon r : ms) = (ng'', MatchAnon r' : ms')
			where	(ng', r') = rerule ng r
				(ng'', ms') = reseq ng' ms
		reseq ng (MatchName r id : ms) = (ng'', MatchName r' id : ms')
			where	(ng', r') = rerule ng r
				(ng'', ms') = reseq ng' ms
		reseq ng (MatchPat r p : ms) = (ng'', MatchPat r' p : ms')
			where	(ng', r') = rerule ng r
				(ng'', ms') = reseq ng' ms
		reseq ng (MatchString r s : ms) = (ng'', MatchString r' s : ms')
			where	(ng', r') = rerule ng r
				(ng'', ms') = reseq ng' ms
		reseq ng (MatchAnd r : ms) = (ng'', MatchAnd r' : ms')
			where	(ng', r') = rerule ng r
				(ng'', ms') = reseq ng' ms
		reseq ng (MatchNot r : ms) = (ng'', MatchNot r' : ms')
			where	(ng', r') = rerule ng r
				(ng'', ms') = reseq ng' ms
		reseq ng (MatchPred c : ms) = (ng', MatchPred c : ms')
			where	(ng', ms') = reseq ng ms

	rerule ng (RuleAlt [alt]) = rerule ng alt
	rerule ng (RuleAlt alts) = (ng', RuleAlt alts') where

		(ng', alts') = rerules ng alts

		rerules ng [] = (ng, [])
		rerules ng (r : rs) = (ng'', r' : rs') where
			(ng', r') = rerule ng r
			(ng'', rs') = rerules ng' rs

	rerule ng (RuleOpt r) = (ng', RuleOpt r') where
		(ng', r') = rerule ng r

	-- Reduce string literals to character sequences
	rerule ng (RuleString s) =
		(ng, RuleSeq (matches s) (ProdCode (show s)))
		where
		matches (c : cs) = MatchAnon (RuleChar c) : matches cs
		matches [] = []

	-- Reduce zero-or-more (star operator) rules
	rerule ng (RuleStar r) = (ng'', RulePrim n'') where
		(ng', r') = rerule ng r
		n'' = newnt "StarRule" (nonterms ++ ng)
		t'' = "[" ++ infertype r' ++ "]"
		r'' = RuleAlt [
			RuleSeq [MatchName r' "v",
				 MatchName (RulePrim n'') "vs"]
				(ProdCode "v : vs"),
			RuleSeq [] (ProdCode "[]")]
		ng'' = (n'', t'', r'') : ng'
		-- reuse existing equivalent iteration nonterminals

	-- Reduce one-or-more (plus operator) rules
	rerule ng (RulePlus r) = (ng'', RulePrim n'') where
		(ng', r') = rerule ng r
		n'' = newnt "PlusRule" (nonterms ++ ng)
		t'' = "[" ++ infertype r' ++ "]"
		r'' = RuleAlt [
			RuleSeq [MatchName r' "v",
				 MatchName (RulePrim n'') "vs"]
				(ProdCode "v : vs"),
			RuleSeq [MatchName r' "v"]
				(ProdCode "[v]")]
		ng'' = (n'', t'', r'') : ng'
		-- reuse existing equivalent iteration nonterminals


	-- Eliminate simple left recursion in a grammar rule
	elimleft ng n t (r @ (RuleAlt alts)) = fix (scan alts) where

		-- Separate left-recursive alternatives (las)
		-- from terminating alternatives (tas).
		scan [] = ([], [])

		scan ((ra @ (RuleSeq (MatchName (RulePrim n') id'
				     : ms') p)) : ras) =
			if n' == n
			then (ra : las, tas)
			else (las, ra : tas)
			where (las, tas) = scan ras

		scan (ra : ras) = (las, ra : tas)
			where (las, tas) = scan ras

		-- Trivial case: no left-recursive alternatives
		fix ([], _) = (ng, r)

		-- Illegal case: no terminating alternatives
		fix (_, []) =
			error ("No termination for left recursive rule " ++
				 show n)

		-- Left recursive case
		fix (las, tas) = (ng', r') where

			ntail = n ++ "Tail"
			ttail = "(" ++ t ++ " -> " ++ t ++ ")"
			rtail = RuleAlt (map tailalt las ++ [rnull])

			rnull = RuleSeq [] (ProdCode "\\v -> v")

			ng' = (ntail, ttail, rtail) : ng
			r' = RuleAlt (map headalt tas)

			headalt r =
				RuleSeq [MatchName r "l",
					 MatchName (RulePrim ntail) "t"]
					(ProdCode "t l")

			tailalt (RuleSeq (MatchName _ id : ms) p) =
				RuleSeq (ms ++ [m]) (ProdCode code) where

				m = MatchName (RulePrim ntail) "pappyTail"
				code = "\\" ++ id ++ " -> pappyTail (" ++
					oldcode ++ ")"
				oldcode = case p of
						ProdName id' -> id'
						ProdCode c -> c

	-- Default case when nonterminal isn't an alternation
	elimleft ng n t r = (ng, r)


	-- Infer the type of a rule expression, for use by */+ reducers.
	-- Doesn't work on sequences with results produced by raw Haskell code.
	infertype (RulePrim "Char") = "Char"
	infertype (RulePrim n) = t where
		(t, r) = findnt n nonterms
	infertype (RuleChar c) = "Char"
	infertype (RuleString s) = "String"
	infertype (RuleSeq ms (ProdName id)) = findmatch ms where
		findmatch [] = error ("Match variable " ++ id ++ " not found")
		findmatch (MatchName r id' : ms) =
			if id' == id then infertype r else findmatch ms
		findmatch (m : ms) = findmatch ms
	infertype (RuleAlt (r : rs)) = infertype r	-- ignore others
	infertype (RuleOpt r) = "Maybe (" ++ infertype r ++ ")"
	infertype (RuleStar r) = "[" ++ infertype r ++ "]"
	infertype (RulePlus r) = "[" ++ infertype r ++ "]"
	infertype r = error ("Unable to infer type of: " ++ show r)



-- After simple left recursion has been eliminated,
-- check for any remaining (illegal) left recursion.
-- Takes a grammar and returns an error message if any is found.
checkLeftRecursion :: Grammar -> Maybe String
checkLeftRecursion (parsername, topcode, topnts, nonterms) =
	checkgram nonterms nonterms where

	-- Iterate through the grammar checking each nonterminal.
	checkgram :: [Nonterminal] -> [Nonterminal] -> Maybe String
	checkgram g ((n, t, r) : nts) =
		case (checknt g [n] r, checkgram g nts) of
			((_, Just e), _) -> Just e
			(_, Just e) -> Just e
			_ -> Nothing
	checkgram g [] = Nothing

	-- checknt takes a list of nonterminals that have been visited
	-- and the rule to check,
	-- and descends into the rule detecting circular left-references
	-- to those nonterminals.
	-- It returns True if the rule can match the empty string,
	-- False otherwise.
	checknt :: [Nonterminal] -> [Identifier] -> Rule -> (Bool, Maybe String)
	checknt g ns (RulePrim n) =
		if n == "Char" then (False, Nothing)
		else if n `elem` ns
		then (True,
			Just ("Illegal left recursion: " ++
				concat (map (\n -> n ++ " -> ") (reverse ns))
				++ n))
		else let (t, r) = findnt n g in checknt g (n:ns) r

	checknt g ns (RuleChar c) = (False, Nothing)
	checknt g ns (RuleString s) = (s == [], Nothing)

	checknt g ns (RuleSeq ms p) = cseq ms where
		cseq (MatchAnon r : ms) = seq (checknt g ns r) (cseq ms)
		cseq (MatchName r id : ms) = seq (checknt g ns r) (cseq ms)
		cseq (MatchPat r p : ms) = seq (checknt g ns r) (cseq ms)
		cseq (MatchString r s : ms) = seq (checknt g ns r) (cseq ms)
		cseq (MatchAnd r : ms) = pred (checknt g ns r) (cseq ms)
		cseq (MatchNot r : ms) = pred (checknt g ns r) (cseq ms)
		cseq (MatchPred c : ms) = pred (True, Nothing) (cseq ms)
		cseq [] = (True, Nothing)

		-- Used for normal sequence matchers
		seq (_, Just e) _ = (True, Just e)
		seq (True, Nothing) r2 = r2
		seq r1 _ = r1

		-- Used for predicate matchers, which always match empty
		pred (_, Just e) _ = (True, Just e)
		pred (_, Nothing) r2 = r2

	checknt g ns (RuleAlt rs) = calts rs where
		calts [r] = checknt g ns r
		calts (r : rs) =
			case (checknt g ns r, calts rs) of
				((_, Just e), _) -> (True, Just e)
				(_, (_, Just e)) -> (True, Just e)
				((b1, _), (b2, _)) -> (b1 || b2, Nothing)

	checknt g ns (RuleOpt r) =
		case checknt g ns r of
			(_, Just e) -> (True, Just e)
			_ -> (True, Nothing)

	checknt g ns (RuleStar r) =
		case checknt g ns r of
			(_, Just e) -> (True, Just e)
			_ -> (True, Nothing)

	checknt g ns (RulePlus r) = checknt g ns r



-- Check if a terminal or nonterminal of a given name exists
existstnt n nts = n == "Char" || existsnt n nts

-- Check if a nonterminal of a given name exists in a grammar
existsnt n [] = False
existsnt n ((n', _, _) : nts) = (n' == n) || existsnt n nts

-- Find the type and rule for a given nonterminal in the grammar
findnt n [] = error ("Nonterminal " ++ show n ++ " not found")
findnt n ((n', t, r) : nts) = if n' == n then (t, r) else findnt n nts

-- Construct a name for a new nonterminal out of a given basename,
-- using ascending numeric indices to prevent conflicts
newnt base nts = scan 0 where
	scan i = let n = base ++ show i
		 in if existstnt n nts then scan (i+1) else n

