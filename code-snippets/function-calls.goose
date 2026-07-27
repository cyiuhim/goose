
## to define a new function, pass in the parameters in the brackets after the function name
start @honk_something@ (word str, number n)
honk<<"a goose once honked: ", str, " ", n, " times.">>.
end @honk_something@

## you can do calculations and make a return value too! 
start @addition@ (number a, number b)
return a+b.
end @addition@ 

## now the main function! 
start @fly@ ()
honk_something<<"long long man", 8>>.
flap a = 3.
flap b = 5.
honk<<a, " plus ", b, " is ", addition(a,b)>>.
end @fly@
