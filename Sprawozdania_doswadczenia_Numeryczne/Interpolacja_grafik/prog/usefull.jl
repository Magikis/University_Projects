#Kacper Kulczak 279079
#Program do zadania 2.12 na pracownie z przedmiotu Analiza Numeryczna M


#Odwracanie listy
function myreverse(xs) 
    res = copy(xs)
    for i = length(xs): -1: 1
        res[i] = xs[length(xs)+1-i]
    end
    return res 
end
#Znajdowanie elemnentu maksymalnego w tablicy
function max(xs)
    res = xs[1]
    for i = xs
        if i> res 
            res = i
        end
    end
    return res 
end