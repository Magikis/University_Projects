import Verbally
import System.Environment

main = do 
	args <- getArgs
	putStrLn ( verbally (chooseCur (args !! 1) ) (read (args !! 0) :: Integer))

chooseCur:: String -> Currency	
chooseCur x
	|x == "AUD" = Currency "Australian Dollar" "Australian Dollars"
	|x == "BGN" = Currency "Lev" "Levs" 
	|x == "BRL" = Currency "Real" "Reals" 
	|x == "BYR" = Currency "Belarusian Ruble" "Belarusian Rubles" 
	|x == "CAD" = Currency "Canadian Dollar" "Canadian Dollars"
	|x == "CHF" = Currency "Swiss Franc" "Swiss Francs" 
	|x == "CNY" = Currency "Yuan Renminbi" "Yuans Renminbi" 
	|x == "CZK" = Currency "Czech Koruna" "Czech Korunas" 
	|x == "DKK" = Currency "Danish Krone" "Danish Krones" 
	|x == "EUR" = Currency "Euro" "Euros" 
	|x == "GBP" = Currency "British Pound" "British Pounds"
	|x == "HKD" = Currency "Hongkong Dollar" "Hongkong Dollars"
	|x == "HRK" = Currency "Kuna" "Kunas" 
	|x == "HUF" = Currency "Forint" "Forints" 
	|x == "IDR" = Currency "Indonesian Rupiah" "Indonesian Rupees"
	|x == "ISK" = Currency "Icelandic Krona" "Icelandic Kronas" 
	|x == "JPY" = Currency "Yen" "Yens" 
	|x == "KRW" = Currency "South Korean Won" "South Korean Wons" 
	|x == "MXN" = Currency "Mexican Peso" "Mexican Pesos"
	|x == "MYR" = Currency "Ringgit" "Ringgits" 
	|x == "NOK" = Currency "Norwegian Krone" "Norwegian Krones"
	|x == "NZD" = Currency "New Zealand Dollar" "New Zealand Dollars"
	|x == "PHP" = Currency "Philippine Peso" "Philippine Pesos" 
	|x == "PLN" = Currency "Zloty" "Zlotys" 
	|x == "RON" = Currency "Romanian Leu" "Romanian Leus" 
	|x == "RUB" = Currency "Russian Ruble" "Russian Rubles"
	|x == "SDR" = Currency "Special Drawing Rights" "Special Drawing Rights" 
	|x == "SEK" = Currency "Swedish Krona" "Swedish Kronas" 
	|x == "SGD" = Currency "Singapore Dollar" "Singapore Dollars" 
	|x == "THB" = Currency "Baht" "Bahts" 
	|x == "TRY" = Currency "Turkish Lira" "Turkish Liras" 
	|x == "UAH" = Currency "Hryvnia" "Hryvnias" 
	|x == "USD" = Currency "US Dollar" "US Dollars" 
	|x == "ZAR" = Currency "Rand" "Rands" 

