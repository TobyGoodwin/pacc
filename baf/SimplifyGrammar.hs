module SimplifyGrammar (simplifyGrammar) where

import Pappy


-- Rewrite composite and simple left recursive rules
simplifyGrammar :: Grammar -> Grammar
simplifyGrammar (parsername, topcode, topnts, nonterms) =
	(parsername, topcode, topnts, iteropt topnts nonterms)


-- Iterate the optimizations until no more simplifications are possible,
-- since eliminating one nonterminal may make others reducible.
-- Be careful not to eliminate any nonterminals listed in 'tops'.
iteropt :: [Identifier] -> [Nonterminal] -> [Nonterminal]
iteropt tops nts = nts'''' where
	(nts', peepholed) = peephole nts
	(nts'', collapsed) = collapse tops nts'
	(nts''', inlined) = inline tops nts''
	nts'''' = if peepholed || collapsed || inlined
			then iteropt tops nts''' else nts'''


-- Simple peephole grammar optimizer,
-- implementing various local simplifications.
peephole :: [Nonterminal] -> ([Nonterminal], Bool)
peephole g = nonterms g where

	nonterms ((n, t, r) : g) = ((n, t, r') : g', sr || sg) where
		(r', sr) = rule r
		(g', sg) = nonterms g
	nonterms [] = ([], False)


	---------- Simplify a rule
	rule :: Rule -> (Rule, Bool)
	rule (r @ (RulePrim n)) = (r, False)
	rule (r @ (RuleChar c)) = (r, False)
	rule (r @ (RuleString s)) = (r, False)

	-- Eliminate useless sequencing
	rule (RuleSeq [MatchName r id] (ProdName id')) | id' == id =
		(r', True) where
		(r', _) = rule r

	rule (RuleSeq ms p) = (RuleSeq ms' p, sms) where
		(ms', sms) = seq ms

	-- Eliminate degenerate alternation
	rule (RuleAlt [r]) = (r', True) where (r', _) = rule r

	-- Flatten nested alternation operators
	rule (RuleAlt (RuleAlt rs1 : rs2)) = (r'', True) where
		r' = RuleAlt (rs1 ++ rs2)
		(r'', _) = rule r'

	rule (RuleAlt rs) = (RuleAlt rs', srs) where (rs', srs) = alts rs

	rule (RuleOpt r) = (RuleOpt r', sr) where (r', sr) = rule r
	rule (RuleStar r) = (RuleStar r', sr) where (r', sr) = rule r
	rule (RulePlus r) = (RulePlus r', sr) where (r', sr) = rule r

	rule (RuleSwitchChar crs dfl) = cases False [] crs where

		cases b ncrs ((c, r) : crs) =
			cases (b' || b) ((c, r') : ncrs) crs
			where	(r', b') = rule r
		cases b ncrs [] = dodfl b (reverse ncrs) dfl

		dodfl b ncrs (Just r) =
			(RuleSwitchChar ncrs (Just r'), b || b')
			where	(r', b') = rule r

		dodfl b ncrs (Nothing) =
			(RuleSwitchChar ncrs Nothing, b)


	---------- Simplify a list of alternatives
	alts :: [Rule] -> ([Rule], Bool)

	-- Left-factor consecutive alternatives starting with characters,
	-- producing an efficiently-implementable SwitchChar rule.
	alts (rs @ (RuleSeq (MatchAnon (RuleChar _) : _) _ :
		    RuleSeq (MatchAnon (RuleChar _) : _) _ : _)) =
		mksw [] rs where

		-- Collect alternatives starting with a character as cases.
		mksw cases (RuleSeq (MatchAnon (RuleChar c) : ms) p : rs) =
			mksw (addcase c (RuleSeq ms p) cases) rs

		-- If we hit an empty alternative, it always matches,
		-- so we can use it as the default case and ignore any others.
		mksw cases (RuleSeq [] p : _) = (rs'', True) where
			rs' = [RuleSwitchChar cases (Just (RuleSeq [] p))]
			(rs'', _) = alts rs'

		-- If we hit some other kind of alternative,
		-- we have to stop there and keep the rest separate.
		mksw cases rs = (rs'', True) where
			rs' = RuleSwitchChar cases Nothing : rs
			(rs'', _) = alts rs'

		-- Add an alternative to a list of cases, merging appropriately
		addcase c r [] = [(c, r)]
		addcase c r ((c', RuleAlt rs) : crs) | c == c' =
			(c, RuleAlt (rs ++ [r])) : crs
		addcase c r ((c', r') : crs) | c == c' =
			(c, RuleAlt [r', r]) : crs
		addcase c r ((c', r') : crs) =
			(c', r') : addcase c r crs

	-- Special degenerate case of character left-factoring.
	alts [RuleSeq (MatchAnon (RuleChar c) : ms) p, RuleSeq [] p2] =
		([RuleSwitchChar [(c, r1')] (Just r2')], True)
		where	(r1', _) = rule (RuleSeq ms p)
			(r2', _) = rule (RuleSeq [] p2)

	alts (r : rs) = (r' : rs', sr || srs) where
		(r', sr) = rule r
		(rs', srs) = alts rs
	alts [] = ([], False)


	---------- Simplify a list of matches in a sequence
	seq :: [Match] -> ([Match], Bool)

	-- Flatten nested sequencing operators if possible
	seq (MatchAnon (RuleSeq ms1 _) : ms2) = (ms', True) where
		(ms', _) = seq (ms1 ++ ms2)

	seq ((m @ (MatchName (RuleSeq ms1 (ProdName idi)) ido)) : ms2) =
		flattenseq m ms1 idi (\r -> MatchName r ido) ms2

	seq ((m @ (MatchPat (RuleSeq ms1 (ProdName idi)) p)) : ms2) =
		flattenseq m ms1 idi (\r -> MatchPat r p) ms2

	seq ((m @ (MatchString (RuleSeq ms1 (ProdName idi)) s)) : ms2) =
		flattenseq m ms1 idi (\r -> MatchString r s) ms2

--	-- merge anonymous character and string sequences
--	seq (MatchAnon (RuleChar c1) : MatchAnon (RuleChar c2) : ms) =
--		(ms'', True) where
--		ms' = MatchAnon (RuleString [c1, c2]) : ms
--		(ms'', _) = seq ms'
--
--	seq (MatchAnon (RuleChar c1) : MatchAnon (RuleString s2) : ms) =
--		(ms'', True) where
--		ms' = MatchAnon (RuleString (c1 : s2)) : ms
--		(ms'', _) = seq ms'
--
--	seq (MatchAnon (RuleString s1) : MatchAnon (RuleChar c2) : ms) =
--		(ms'', True) where
--		ms' = MatchAnon (RuleString (s1 ++ [c2])) : ms
--		(ms'', _) = seq ms'
--
--	seq (MatchAnon (RuleString s1) : MatchAnon (RuleString s2) : ms) =
--		(ms'', True) where
--		ms' = MatchAnon (RuleString (s1 ++ s2)) : ms
--		(ms'', _) = seq ms'

	seq (m : ms) = (m' : ms', sm || sms) where
		(m', sm) = match m
		(ms', sms) = seq ms

	seq [] = ([], False)


	-- Helper for sequence flattening code above
	flattenseq m ms1 idi f ms2 =
		case simpidi ms1 of
			True -> (ms', True) where
					(ms', _) = seq (rebind ms1 ++ ms2)
			False -> (m' : ms2', sm || sms2) where
					(m', sm) = match m
					(ms2', sms2) = seq ms2

		where
		-- Make sure idi is bound by a simple MatchName,
		-- rather than within a Haskell pattern.
		simpidi [] = False
		simpidi (MatchName d id' : ms) | id' == idi = True
		simpidi (m : ms) = simpidi ms

		-- Convert the matcher for idi into the appropriate substitute
		rebind [] = error "flattenseq/rebind - oops!"
		rebind (MatchName r id' : ms) | id' == idi = f r : ms
		rebind (m : ms) = m : rebind ms



	---------- Simplify a single match in a sequence
	match :: Match -> (Match, Bool)
	match (MatchAnon r) = (MatchAnon r', sr) where (r', sr) = rule r
	match (MatchName r id) = (MatchName r' id, sr) where (r', sr) = rule r
	match (MatchPat r p) = (MatchPat r' p, sr) where (r', sr) = rule r
	match (MatchString r s) = (MatchString r' s, sr) where (r', sr) = rule r
	match (MatchAnd r) = (MatchAnd r', sr) where (r', sr) = rule r
	match (MatchNot r) = (MatchNot r', sr) where (r', sr) = rule r
	match (r @ (MatchPred p)) = (r, False)



-- Search for and eliminate nonterminals in a grammar
-- that are structurally equivalent to some other nonterminal.
-- This optimization is fairly straightforward and stupid:
-- e.g., it doesn't catch structurally equivalent
-- mutually recursive _groups_ of nonterminals -
-- but I have no reason to believe such groups are likely to be common.
-- Most of the duplicates this pass finds
-- are star- and plus-rules generated above,
-- but we run the optimization over the whole grammar anyway
-- just in case the original grammar has some duplicates we can eliminate.
collapse :: [Identifier] -> [Nonterminal] -> ([Nonterminal], Bool)
collapse tops g = scandups [] (\n -> n) g where

	-- Scan a grammar for duplicates to eliminate.
	-- ns is the list of nonterminals we can eliminate,
	-- and f is a function to rename those nonterminals correctly.
	-- We will actually eliminate those nonterminals all at once later.
	-- Always rename to the _last_ duplicate in the grammar,
	-- to make sure it won't also go away if there are several.
	scandups ns f ((n, t, r) : g) =
		if n `elem` tops then scandups ns f g
		else case scandupof n t r f (reverse g) of
			Just n' -> scandups (n:ns) f' g where
					f' nx = if nx == n then n' else f nx
			Nothing -> scandups ns f g

	scandups ns f [] =
		case ns of
			[] -> (g, False)
			_ -> (rebuild ns f g, True)


	-- Scan for a duplicate of a particular nonterminal.
	scandupof n t r f ((n', t', r') : g) =
		if n' == n then error "shouldn't have found myself!"
		else if t' == t && equivRule f' r r' then Just n'
		else scandupof n t r f g

		where	f' nx = if nx == n then n' else f nx

	scandupof n t r f [] = Nothing


	-- Check two rules for structural equivalence
	-- under the nonterminal renaming function f.
	equivRule f (RulePrim n1) (RulePrim n2) = f n1 == f n2
	equivRule f (RuleChar c1) (RuleChar c2) = c1 == c2
	equivRule f (RuleString s1) (RuleString s2) = s1 == s2
	equivRule f (RuleSeq ms1 p1) (RuleSeq ms2 p2) = p1 == p2 && seq ms1 ms2
		where	seq (m1 : ms1) (m2 : ms2) =
				equivMatch f m1 m2 && seq ms1 ms2
			seq [] [] = True
			seq _ _ = False
	equivRule f (RuleAlt rs1) (RuleAlt rs2) = alts rs1 rs2
		where	alts (r1 : rs1) (r2 : rs2) =
				equivRule f r1 r2 && alts rs1 rs2
			alts [] [] = True
			alts _ _ = False
	equivRule f (RuleOpt r1) (RuleOpt r2) = equivRule f r1 r2
	equivRule f (RuleStar r1) (RuleStar r2) = equivRule f r1 r2
	equivRule f (RulePlus r1) (RulePlus r2) = equivRule f r1 r2
	equivRule f (RuleSwitchChar crs1 dfl1) (RuleSwitchChar crs2 dfl2) =
		cases crs1 crs2 && eqdefl dfl1 dfl2
		where	cases ((c1, r1) : crs1) ((c2, r2) : crs2) =
				c1 == c2 && equivRule f r1 r2 && cases crs1 crs2
			cases [] [] = True
			cases _ _ = False
			eqdefl (Just r1) (Just r2) = equivRule f r1 r2
			eqdefl (Nothing) (Nothing) = True
			eqdefl _ _ = False
	equivRule f _ _ = False

	equivMatch f (MatchAnon r1) (MatchAnon r2) = equivRule f r1 r2
	equivMatch f (MatchName r1 id1) (MatchName r2 id2) =
		id1 == id2 && equivRule f r1 r2
	equivMatch f (MatchPat r1 p1) (MatchPat r2 p2) =
		p1 == p2 && equivRule f r1 r2
	equivMatch f (MatchString r1 s1) (MatchString r2 s2) =
		s1 == s2 && equivRule f r1 r2
	equivMatch f (MatchAnd r1) (MatchAnd r2) = equivRule f r1 r2
	equivMatch f (MatchNot r1) (MatchNot r2) = equivRule f r1 r2
	equivMatch f (MatchPred p1) (MatchPred p2) = p1 == p2
	equivMatch f _ _ = False


	-- Rebuild a grammar, eliminating definitions for nonterminals in 'ns'
	-- and applying renaming function 'f' to all nonterminal references.
	rebuild ns f [] = []
	rebuild ns f ((n, t, r) : g) =
		if n `elem` ns then rebuild ns f g
		else (n, t, r') : rebuild ns f g

		where r' = rebuildRule f r

	rebuildRule f (RulePrim n) = RulePrim (f n)
	rebuildRule f (RuleChar c) = RuleChar c
	rebuildRule f (RuleString s) = RuleString s
	rebuildRule f (RuleSeq ms p) = RuleSeq (reseq ms) p
		where	reseq (m : ms) = rebuildMatch f m : reseq ms
			reseq [] = []
	rebuildRule f (RuleAlt rs) = RuleAlt (alts rs)
		where	alts (r : rs) = rebuildRule f r : alts rs
			alts [] = []
	rebuildRule f (RuleOpt r) = RuleOpt (rebuildRule f r)
	rebuildRule f (RuleStar r) = RuleStar (rebuildRule f r)
	rebuildRule f (RulePlus r) = RulePlus (rebuildRule f r)
	rebuildRule f (RuleSwitchChar crs dfl) =
		RuleSwitchChar (cases crs) (defl dfl)
		where	cases ((c, r) : crs) = (c, rebuildRule f r) : cases crs
			cases [] = []
			defl (Just r) = Just (rebuildRule f r)
			defl (Nothing) = Nothing

	rebuildMatch f (MatchAnon r) = MatchAnon (rebuildRule f r)
	rebuildMatch f (MatchName r id) = MatchName (rebuildRule f r) id
	rebuildMatch f (MatchPat r p) = MatchPat (rebuildRule f r) p
	rebuildMatch f (MatchString r s) = MatchString (rebuildRule f r) s
	rebuildMatch f (MatchAnd r) = MatchAnd (rebuildRule f r)
	rebuildMatch f (MatchNot r) = MatchNot (rebuildRule f r)
	rebuildMatch f (MatchPred p) = MatchPred p



-- Inline all nonterminals that are only referenced once in a grammar.
-- Also eliminates any nonterminals not referenced anywhere
-- except perhaps in their own rule.
inline :: [Identifier] -> [Nonterminal] -> ([Nonterminal], Bool)
inline tops g = scan g where

	-- Scan for a nonterminal we can eliminate.
	scan ((n, t, r) : nts) | n `elem` tops = scan nts
	scan ((n, t, r) : nts) =
		if elim then (rebuild n g, True) else scan nts where

		elim = case refs n g of
			0 -> True
			1 -> refsRule n r == 0
			_ -> refsRule n r == 0 && sizeofRule r <= 2

	scan [] = (g, False)	-- nothing eliminated


	-- Count the number of references to a particular nonterminal
	-- *outside of* that nonterminal's own definition.
	refs n [] = 0
	refs n ((n', t', r') : nts) =
		if (n == n') then refs n nts
		else refsRule n r' + refs n nts

	-- Count the number of references to nonterminal 'n' in rule 'r'
	refsRule :: Identifier -> Rule -> Int
	refsRule n (RulePrim n') = if n' == n then 1 else 0
	refsRule n (RuleChar c) = 0
	refsRule n (RuleString s) = 0
	refsRule n (RuleSeq ms p) = seq ms
		where	seq (m : ms) = refsMatch n m + seq ms
			seq [] = 0
	refsRule n (RuleAlt rs) = alts rs
		where	alts (r : rs) = refsRule n r + alts rs
			alts [] = 0
	refsRule n (RuleOpt r) = refsRule n r
	refsRule n (RuleStar r) = refsRule n r
	refsRule n (RulePlus r) = refsRule n r
	refsRule n (RuleSwitchChar crs dfl) = cases crs + deflt dfl
		where	cases ((c, r) : crs) = refsRule n r + cases crs
			cases [] = 0
			deflt (Just r) = refsRule n r
			deflt (Nothing) = 0

	refsMatch n (MatchAnon r) = refsRule n r
	refsMatch n (MatchName r id) = refsRule n r
	refsMatch n (MatchPat r p) = refsRule n r
	refsMatch n (MatchString r s) = refsRule n r
	refsMatch n (MatchAnd r) = refsRule n r
	refsMatch n (MatchNot r) = refsRule n r
	refsMatch n (MatchPred p) = 0


	-- Rebuild the grammar, eliminating nonterminals in 'ns'
	-- and applying the inlining function 'f'.
	rebuild ns [] = []
	rebuild ns ((n, t, r) : nts) =
		if n == ns then rebuild ns nts
		else (n, t, rebuildRule ns r) : rebuild ns nts

	rebuildRule ns (RulePrim n) =
		if n == ns
		then rebuildRule ns (find n g)
		else RulePrim n
	rebuildRule ns (RuleChar c) = RuleChar c
	rebuildRule ns (RuleString s) = RuleString s
	rebuildRule ns (RuleSeq ms p) = RuleSeq (reseq ms) p
		where	reseq (m : ms) = rebuildMatch ns m : reseq ms
			reseq [] = []
	rebuildRule ns (RuleAlt rs) = RuleAlt (alts rs)
		where	alts (r : rs) = rebuildRule ns r : alts rs
			alts [] = []
	rebuildRule ns (RuleOpt r) = RuleOpt (rebuildRule ns r)
	rebuildRule ns (RuleStar r) = RuleStar (rebuildRule ns r)
	rebuildRule ns (RulePlus r) = RulePlus (rebuildRule ns r)
	rebuildRule ns (RuleSwitchChar crs dfl) =
		RuleSwitchChar (cases crs) (deflt dfl)
		where	cases ((c, r) : crs) =
				(c, rebuildRule ns r) : cases crs
			cases [] = []
			deflt (Just r) = Just (rebuildRule ns r)
			deflt (Nothing) = Nothing

	rebuildMatch ns (MatchAnon r) = MatchAnon (rebuildRule ns r)
	rebuildMatch ns (MatchName r id) = MatchName (rebuildRule ns r) id
	rebuildMatch ns (MatchPat r p) = MatchPat (rebuildRule ns r) p
	rebuildMatch ns (MatchString r s) = MatchString (rebuildRule ns r) s
	rebuildMatch ns (MatchAnd r) = MatchAnd (rebuildRule ns r)
	rebuildMatch ns (MatchNot r) = MatchNot (rebuildRule ns r)
	rebuildMatch ns (MatchPred p) = MatchPred p


	-- Find the definition rule for a particular nonterminal
	find n ((n', t', r') : nts) =
		if n' == n then r' else find n nts
	find n [] = error ("inline: can't find nonterminal " ++ n)

