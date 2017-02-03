module Verbally (Type(..), Currency(..), verbally) where
	data Type = Simple deriving Show
	data Currency = Currency {
		singular 	::	String,
		plural 		::	String
	} deriving Show

	verbally :: Currency -> Integer -> String
	verbally cur n = convert n ++ " " ++ (curr_name cur n)

	curr_name cur n 
		|  (abs n) == 1 = singular cur
		|	otherwise = plural cur	

	convert :: Integer -> String
	convert n
		|n < 0 = "minus " ++ convert ((-1) * n)
		|n == 1 = "one "
		|(abs n) < (10^3003)  = foldr shouldTranslate [] (numberToList (n `div` 1000000) 1 []) ++ convert6  (n `mod` 1000000)
		|otherwise = "lots of"
	-- kod wzorowany na ksiazce
	-- Richard Bird Philip Wadler Introduction to Functional Programming
	-- rozdzial 4.1
	convert2 n = combine2 (digits2 n)

	digits2 n = (n `div` 10, n `mod` 10)

	combine2 (0,0) = ""
	combine2 (0,u) = (units!! (u-1))
	combine2 (1,u) = (teens!! u )
	combine2 (t,0) = (tens!! (t-2) )
	combine2 (t,u) = tens !! (t-2) ++ " " ++ (units!! (u-1) )

	convert3 n = combine3 ( digits3 (fromIntegral n))

	digits3 n = (n `div` 100, n `mod` 100)

	combine3 (0,t) = (convert2 t)
	combine3 (h,0) = (units !! (h-1) ) ++ " hundred"
	combine3 (h,t) = (units !! (h-1) ) ++ " hundred and "++ (convert2 t)

	convert6 n = combine6 ( digits6 (fromIntegral n) )

	digits6 n = ( n `div` 1000 , n `mod` 1000)

	combine6 (0, h) = (convert3 h)
	combine6 (m, 0) = (convert3 m) ++ " thousand"
	combine6 (m, h) = (convert3 m) ++ " thousand" ++ (link h )++ (convert3 h)

	link h 
		| h < 100 	= " and "
		| otherwise = " " 
	
	units = ["one" , "two" , "three" , "four" , "five" ,"six" , "seven" , "eight" , "nine"]
	teens = ["ten" , "eleven" , "twelve" , "thirteen" , "fourteen" , "fifteen" , "sixteen" , "seventeen" , "eighteen" , "nineteen"]
	tens = ["twenty" , "thirty" , "forty" , "fifty" , "sixty", "seventy", "eighty" , "ninety"]
	smallprefix =["","mi","bi","tri","kwadri","kwinti","seksti","septi","okti","noni"]
	bigprefixa =["","un","do","tri","kwatuor","kwin","seks","septen","okto","nowem"]
	bigprefixb =["","deci","wici","tryci","kwadragi","kwintagi","seksginti","septagi","oktagi","nonagi"]
	bigprefixc =["","centi","ducenti","trycenti","kwadrygi","kwinge"," sescenti","septynge","oktynge","nonge"]

	--numberToList :: Integer -> Int -> [(Int,Integer)] -> [(Int,Integer)] 
	numberToList n i xs 
			 | n > 0 = numberToList (n `div` 1000) (i+1) ((i, n `mod` 1000):xs)
			 | otherwise	= xs

	--tupleToNumber :: (Int,Integer) -> String
	tupleToNumber (i,n) 
		|i < 10 = ( convert3 n ) ++ " "++ (smallprefix !! i)
		|otherwise = ( convert3 n )++ " " ++ (bigprefixa !! (i `mod` 10)) ++ (bigprefixb !! ((i `div` 10) `mod` 10)) ++ (bigprefixc !! ((i `div` 100) `mod` 10) )

	shouldTranslate (i,n) xs
		|n == 0 = xs
		|n == 1 = tupleToNumber (i,n) ++ "llion " ++ xs
		|otherwise = tupleToNumber (i,n) ++ "llions " ++ xs



	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
