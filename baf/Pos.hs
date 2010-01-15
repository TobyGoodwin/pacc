-- Simple data type to keep track of character positions
-- within a text file or other text stream.
module Pos where


-- Basic position indicator data type: filename, line number, column number.
data Pos = Pos {
		posFile	:: !String,
		posLine	:: !Int,
		posCol	:: !Int
	}

-- Incrementally compute the next position in a text file
-- if 'c' is the character at the current position.
-- Follows the standard convention of 8-character tab stops.
nextPos (Pos file line col) c =
	if c == '\n' then Pos file (line + 1) 1
	else if c == '\t' then Pos file line ((div (col + 8 - 1) 8) * 8 + 1)
	else Pos file line (col + 1)

-- Two positions are equal if each of their elements are equal.
instance Eq Pos where
	Pos f1 l1 c1 == Pos f2 l2 c2 =
		f1 == f2 && l1 == l2 && c1 == c2

-- Two positions are ordered by line number, then column number.
instance Ord Pos where
	Pos f1 l1 c1 <= Pos f2 l2 c2 =
		(l1 < l2) || (l1 == l2 && c1 <= c2)

-- Standard way to display positions - "file:line:col"
instance Show Pos where
	show (Pos file line col) = file ++ ":" ++ show line ++ ":" ++ show col

-- Show a position relative to a base position.
-- If the new position is in the same line, just show its column number;
-- otherwise if the new position is in the same file,
-- just show its line and column numbers;
-- otherwise show the complete new position.
showPosRel (Pos file line col) (Pos file' line' col') =
	if (file == file')
	then	if (line == line')
		then "column " ++ show col'
		else "line " ++ show line' ++ ", column " ++ show col'
	else show (Pos file' line' col')


