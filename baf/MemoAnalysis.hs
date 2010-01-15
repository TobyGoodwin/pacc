module MemoAnalysis (memoAnalysis) where

import List
import Pappy


-- Cost metric for a rule as used below,
-- Infinite if a cycle is detected.
data Cost = Cost Int | Infinite

instance Eq Cost where
	Cost c1 == Cost c2 = c1 == c2
	Infinite == Infinite = True
	_ == _ = False

instance Ord Cost where
	Cost c1 <= Cost c2 = c1 <= c2
	Infinite <= Infinite = True
	Cost c1 <= Infinite = True
	Infinite <= Cost c2 = False

instance Show Cost where
	show (Cost c) = show c
	show Infinite = "<infinite>"

instance Num Cost where
	Cost c1 + Cost c2 = Cost (c1 + c2)
	Infinite + _ = Infinite
	_ + Infinite = Infinite

	c1 * c2 = error "no multiplication for Cost"
	abs c = error "no 'abs' for Cost"
	signum c = error "no 'signum' for Cost"

	fromInteger i = Cost (fromInteger i)


-- Produce a list of the nonterminals in a grammar to be memoized,
-- by analyzing the size and recursion structure of the grammar.
-- What we are doing is essentially "virtual inlining":
-- finding small- to medium-size definitions that we _could_ inline directly,
-- but do not want to because of the code expansion they would cause,
-- and instead turn those definitions into simple Haskell functions.
-- Since every circularity in the grammar is still broken by memoization,
-- this transformation does not break the linear parse time guarantee.
memoAnalysis :: [Nonterminal] -> [Identifier]
memoAnalysis nts = iter [] where

	-- Iterate over the grammar,
	-- at each step finding the smallest nonterminal that can be inlined,
	-- and adding that to our list of "virtual inline" nonterminals.
	-- We have to take the list into account in subsequent iterations
	-- in order to ensure that cylces stay broken,
	-- and to take virtual inlining into account in our size metrics:
	-- the size of the definition for a virtual-inlined nonterminal
	-- counts towards the sizes of rules that reference it.
	iter :: [Nonterminal] -> [Identifier]
	iter vnts =
		case sel vnts Nothing nts of
			Just vnt -> iter (vnt : vnts)
			Nothing -> memos nts where

				memos ((n, t, r) : nts) =
					case findnt n vnts of
						Just _ -> memos nts
						Nothing -> n : memos nts
				memos [] = []


	-- Find the smallest nonterminal that can be inlined.
	sel :: [Nonterminal] -> Maybe (Nonterminal, Cost) -> [Nonterminal] ->
		Maybe Nonterminal
	sel vnts (Just (nt, c)) ((nt' @ (n', t', r')) : nts) =
		if use vnts c c' n' r'
		then sel vnts (Just (nt', c')) nts
		else sel vnts (Just (nt, c)) nts
		where	c' = costrule vnts [n'] r'

	sel vnts (Nothing) ((nt' @ (n', t', r')) : nts) =
		if use vnts Infinite c' n' r'
		then sel vnts (Just (nt', c')) nts
		else sel vnts (Nothing) nts
		where	c' = costrule vnts [n'] r'

	sel vnts (Just (nt, c)) [] = Just nt
	sel vnts (Nothing) [] = Nothing


	-- Taking a particular nonterminal as a candidate,
	-- determine if we should use it (possibly over an existing candidate).
	-- Only select nonterminals that aren't already in vnts.
	use vnts curcost newcost n r =
		newcost < curcost && newcost < 25 && notfound where
		notfound = case findnt n vnts of
				Just _ -> False
				Nothing -> True


	-- Scan a rule and all virtually-inlined rules it refers to,
	-- calculating the total cost of the rule, and making sure that
	-- we do not hit a nonterminal we've already visited.
	-- ns: nonterminals we have already visited in our search.
	costrule :: [Nonterminal] -> [Identifier] -> Rule -> Cost
	costrule vnts ns (RulePrim n) =
		if n `elem` ns then Infinite
		else case findnt n vnts of
			Just (_, _, r) -> 1 + costrule vnts (n:ns) r
			Nothing -> Cost 1	-- not virtually-inlined (yet)

	costrule vnts ns (RuleChar c) = 1

	costrule vnts ns (RuleSeq ms p) = 1 + seq ms where
		seq (MatchAnon r : ms) = costrule vnts ns r + seq ms
		seq (MatchName r id : ms) = costrule vnts ns r + seq ms
		seq (MatchPat r p : ms) = costrule vnts ns r + seq ms
		seq (MatchString r s : ms) = costrule vnts ns r + seq ms
		seq (MatchAnd r : ms) = costrule vnts ns r + seq ms
		seq (MatchNot r : ms) = costrule vnts ns r + seq ms
		seq (MatchPred c : ms) = 1
		seq [] = 0

	costrule vnts ns (RuleAlt rs) = 1 + alts rs where
		alts (r : rs) = costrule vnts ns r + alts rs
		alts [] = 0

	costrule vnts ns (RuleOpt r) = costrule vnts ns r

	costrule vnts ns (RuleString s) = 1
	costrule vnts ns (RuleStar r) = costrule vnts ns r
	costrule vnts ns (RulePlus r) = costrule vnts ns r

	costrule vnts ns (RuleExpect r ss) = costrule vnts ns r

	costrule vnts ns (RuleSwitchChar crs dfl) = 1 + cases crs + defl dfl
		where	cases ((c, r) : crs) = costrule vnts ns r + cases crs
			cases [] = 0
			defl (Just r) = costrule vnts ns r
			defl (Nothing) = 0


	-- Find a named nonterminal in a list.
	findnt :: Identifier -> [Nonterminal] -> Maybe Nonterminal
	findnt n ((nt @ (n', t', r')) : nts) | n' == n = Just nt
	findnt n (nt : nts) = findnt n nts
	findnt n [] = Nothing


