# tests to check scoping
#
function sumtest a b
	localvar <- a + b   # local variable
	print localvar      # Expected: a + b
	return localvar
#
# Define a function to test scope
function scopetest x y
	result <- x * y     # Using parameters x and y
	print result        # Expected: x * y
	localvar <- 50.0    # local variable
	print localvar      # Expected: 50.0
	return result
#
# Main program starts here
#
# Test sumtest function
resultsum <- sumtest( 3.0 , 7.0 )
print resultsum
# After the function ends, localvar should be out of scope
# Attempt to print localvar (should not exist anymore)
print localvar          # Expected: 0.0, because localvar is out of scope and defaults to 0.0
#
# Test scopetest function
resultscope <- scopetest( 2.0 , 4.0 )
# Attempt to access localvar again (should be out of scope)
print localvar          # Expected: 0.0, same as above
#
# Test undeclared variable use in an expression
newvar <- newvar + 5.0  # newvar is undeclared, so it should default to 0.0
# test if functions can modify global variables
function modloc
    newvar <- newvar + 5
modloc ()
print newvar            # Expected: 10 (0.0 + 5.0 + 5)
