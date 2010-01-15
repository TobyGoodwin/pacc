module Main where

import IO
import System

import Pappy
import ReadGrammar
import ReduceGrammar
import SimplifyGrammar
import MemoAnalysis
import WriteParser


processArgs =
	do	args <- getArgs
		case args of
			[inname] -> return (inname, mkoutname inname)
			[inname, outname] -> return (inname, outname)
			_ -> usage

	where
	mkoutname inname =
		case reverse inname of
			'y':'p':'p':'a':'p':'.':inr ->
				reverse inr ++ ".hs"
			_ -> inname ++ ".hs"

	usage = fail "Usage: pappy <infile> [<outfile>]"


processFile inname outname =
	do	(n, c, t, g) <- pappyParseFile inname
		putStrLn ("Original grammar: " ++ show (length g) ++
			  " size " ++ show (sizeofNonterminals g))
		--putStrLn (showNonterminals g)

		let	(n', c', t', g') = reduceGrammar (n, c, t, g)
		putStrLn ("Reduced grammar: " ++ show (length g') ++
			  " size " ++ show (sizeofNonterminals g'))
		--putStrLn (showNonterminals g')

		let	(n'', c'', t'', g'') = simplifyGrammar (n', c', t', g')
		putStrLn ("Simplified grammar: " ++ show (length g'') ++
			  " size " ++ show (sizeofNonterminals g''))
		--putStrLn (showNonterminals g'')

		let	m'' = memoAnalysis g''
		putStrLn ("Memoized: " ++ show (length m''))

		let	parser = writeParser m'' (n'', c'', t'', g'')
		writeFile outname parser


main = do	(inname, outname) <- processArgs
		processFile inname outname

