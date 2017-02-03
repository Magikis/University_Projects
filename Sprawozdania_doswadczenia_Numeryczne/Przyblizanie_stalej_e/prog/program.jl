#Prezentacja wyników do sprawozdania: Zadanie P1.2
#Kacper Kulczak 279079


#Pomocnicza funkcja pomgająca. Potegowanie
function pow(n,w)
    result = one(n)
    for i = one(w):w
        result *= n
    end
    return result
end

# Funkcja obliczajaca stala e za pomoca przyblizenia granicy (1+1/n)^n
function definition_e(n)
    result = (one(n) + one(n) / n)
    for i = one(n):log2(n)
        result *= result
    end
    return result
end
# Funkcja obliczajaca stala e za pomoca szeregu taylora sum(1/ n!)
function taylor_series(n)
    result = zero(n)
    for i = zero(n):n
        result += one(n) / factorial(n-i)
    end
    return result
end
# Funkcja obliczajaca stala e za pomoca ulamka lancuchowego
function continued_fraction(n)
    i = n + n
    result = i
    result = one(n) + one(n) / result
    result = one(n) + one(n) / result
    i -= one(n) + one(n)
    while i > one(n) + one(n)
        result = i + one(n) / result
        result = one(n) + one(n) / result
        result = one(n) + one(n) / result
        i -= one(n) + one(n)
    end
    result = one(n) + one(n) + one(n) / result
    result = one(n) + one(n) / result
    result = one(n) + one(n) + one(n) / result
    return result
    
end
#Drugi szereg SUM (3k+1)/(3k)!
function second_series(n)
    result = zero(n)
    for i = zero(n):n
        result += (one(n) + (convert(typeof(n),3) * (n-i) ) * ( convert(typeof(n),3) * (n-i) ) )/ factorial(convert(typeof(n),3) *(n-i))
    end
    return result
end

       
# Trzeci szreg SUM (4k + 3) / 2^(2k+1) * (2k+1)!
function third_series(n)
    result = zero(n)
    for i = zero(n):n
        temp = convert(typeof(n),4) * (n-i) + one(n) + one(n) + one(n)
        temp /= factorial(convert(typeof(n),2) *(n-i) + one(n)) * pow( convert(typeof(n),2), convert(typeof(n),2)*(n-i) + one(n)) 
        result += temp
    end
    return result * result
end
#Test zapisujace wyniki na zadanym zasiegu 1->range
function test(fun,range)
    container= [[],[],[],[]]
    for i = one(range) : range
        temp = fun(i)
        err = convert(typeof(range),eu)-temp
        push!(container[1], i)
        push!(container[2], temp)
        push!(container[3],err)
        push!(container[4], -log10(abs( err )) )
    end
    return container
end

#Test dla definicji, przedstawiajacy tylko niektore iteracje
function test_def(fun,range)
    container= [[],[],[],[]]
    x = one(range)
    for i = one(range) : range
        x *= convert(typeof(range),8.)
        temp = fun(x)
        err = convert(typeof(range),eu)-temp
        push!(container[1], x)
        push!(container[2], temp)
        push!(container[3],err)
        push!(container[4], -log10(abs( err )) )
    end
    return container
end

#Funkcja formatujaca wyjscie
function myshow(list)
    for i = 1:length(list[1])
        @printf "n=%d val=%.f error=%.4e #cyfr=%.0f\n" list[1][i] list[2][i] list[3][i] list[4][i]
    end
end

setprecision(256)
@show precision(BigFloat)
range = BigFloat(60.)
data1 = test_def(definition_e, BigFloat(30.) )
data2 = test(taylor_series, range)
data3 = test(continued_fraction, range )
data4 = test(second_series, range)
data5 = test(third_series, range)
@printf "\n\n\n"
println("Definition of e")
myshow(data1)
@printf "\n\n\n"
println("Taylor series")
myshow(data2)
@printf "\n\n\n"
println("Contiuned fraction")
myshow(data3)
@printf "\n\n\n"
println("Second Series")
myshow(data4)
@printf "\n\n\n"
println("Third Series")
myshow(data5)

#Funckje do obliczenia ilości mnożeń i dzieleń w zależnosci od iteracji w wykorzytanych
# przeze mnie metodach
function Tdef(n)
    return one(n)+ one(n)+log2(n)
end
function TCF(n)
    return n * convert(typeof(n),3)
end
function Tser1(n)
    return n * convert(typeof(n),2)
end
function Tser2(n)
    return n * convert(typeof(n),4)
end
function Tser3(n)
    return n * convert(typeof(n),5) + one(n)
end