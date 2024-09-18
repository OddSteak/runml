# Define a function to calculate the area of a rectangle
function rectarea length width
	area <- length * width
	return area
#
# Define a function to calculate the area of a circle
function circlearea radius
	pi <- 3.14159
	area <- pi * radius * radius
	return area
#
# Define a function that adds two numbers, subtracts a value, multiplies, divides, and prints the result
function complexops a b c
	result <- ( a + b ) * c - ( a / 2.0 )
	return result
#
# Main program starts here
#
# Calculate the area of a rectangle
rectlength <- 5.0
rectwidth  <- 10.0
rarea   <- rectarea( rectlength , rectwidth )
print rarea   # Expected: 50.0 (5.0 * 10.0)
#
# Calculate the area of a circle
circradius <- 3.0
circarea   <- circlearea( circradius )
print circarea    # Expected: 28.27431 (3.14159 * 3.0 * 3.0)
#
# More complex assignments
valuea  <- 7.0
valueb  <- rectarea( 2.0, 3.0 ) + 4.0 / 2.0
# rectarea(2.0, 3.0) = 6.0 -> 6.0 + 4.0 / 2.0 = 6.0 + 2.0 = 8.0
valuec  <- circarea - 5.0 * 2.0
# circarea = 28.27431 -> 28.27431 - (5.0 * 2.0) = 28.27431 - 10.0 = 18.27431
result  <- complexops( valuea , valueb , valuec )
# (7.0 + 8.0) * 18.27431 - (7.0 / 2.0) = 15.0 * 18.27431 - 3.5 = 274.11465 - 3.5 = 270.61465
print result     # Expected: 270.61465
#
# Final calculation with nested function calls
finalresult <- complexops( rectarea(4.0, 5.0) , circlearea(2.0) , 3.0 )
# rectarea(4.0, 5.0) = 20.0, circlearea(2.0) = 12.56636
# (20.0 + 12.56636) * 3.0 - (20.0 / 2.0) = 32.56636 * 3.0 - 10.0 = 97.69908 - 10.0 = 87.69908
print finalresult   # Expected: 87.69908
