start @sum@ (number left, number right)
return left + right.
end @sum@

start @fly@ ()
flap value = sum<<1, 2 * (3 + 4)>> - 5 / 2 % 2.
honk<<"value: ", value, sum<<value, 1>>>>.
end @fly@
