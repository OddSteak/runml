function sumtest a b
	localvar<-a+b
	return localvar
function testmult a b
	result <-  a  *  b
	return   result
function calcarea length width
	area<-length*width
	return area
rectlength <- 5.0
rectwidth  <-  10.0
rectarea <-   calcarea(   rectlength,rectwidth)
print rectarea
valuea <- testmult(  2.0   ,  sumtest( 4.0 , testmult(2.0,3.0) )  )
# testmult(2.0,3.0) -> 6.0; sumtest(4.0,6.0) -> 10.0; testmult(2.0, 10.0) -> 20.0
print   valuea
valueb <- sumtest(7.0 , testmult(  5.0 , 2.0 ) )  -   testmult( 1.0 , sumtest(3.0,4.0) )
# testmult(5.0,2.0) -> 10.0; sumtest(7.0,10.0) -> 17.0; sumtest(3.0,4.0) -> 7.0; testmult(1.0, 7.0) -> 7.0; 17.0 - 7.0 -> 10.0
print valueb
function scopecheck x
	localvar <- x+10.0
	return   localvar
valuec <-   scopecheck( sumtest(2.0,5.0) ) + sumtest(   valuea , valueb )
# sumtest(2.0,5.0) -> 7.0; scopecheck(7.0) -> 17.0; sumtest(valuea, valueb) -> sumtest(20.0, 10.0) -> 30.0; 17.0 + 30.0 -> 47.0
print  valuec
undefinedvar<-undefinedvar +  5.0
# undefinedvar starts as 0.0; 0.0 + 5.0 -> 5.0
print   undefinedvar
function nestedtest x y
	result<-sumtest(testmult(x,y),sumtest(x,y))
	return result
nestedresult<-nestedtest( sumtest( 4.0 , 5.0 ), testmult(2.0,3.0) )
# sumtest(4.0,5.0) -> 9.0; testmult(2.0,3.0) -> 6.0; nestedtest(9.0, 6.0)
# testmult(9.0,6.0) -> 54.0; sumtest(9.0,6.0) -> 15.0; sumtest(54.0,15.0) -> 69.0
print nestedresult
valuefinal <- scopecheck( sumtest(   3.0, 4.0  ) )  + calcarea( 3.0, 4.0 )
# sumtest(3.0,4.0) -> 7.0; scopecheck(7.0) -> 17.0; calcarea(3.0,4.0) -> 12.0; 17.0 + 12.0 -> 29.0
print   valuefinal
finalsum <-  sumtest( valuefinal ,  nestedresult)
# sumtest(29.0,69.0) -> 98.0
print finalsum
#
print testmult(finalsum,  sumtest(5.0, testmult( 3.0, 2.0 ) ) )
# testmult(3.0,2.0) -> 6.0; sumtest(5.0,6.0) -> 11.0; testmult(98.0,11.0) -> 1078.0
