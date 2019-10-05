###########################################################
##		CS 344 - OS1
##			Amar Raad / raadv
##		Program Py - Python Exploration
##			Feb 27, 2019
## ------------------------------------------------------##
##  Your script must satisfy the following requirements:
##
##  1.) Contained in one single file, called "mypython.py".
##  2.) When executed, creates 3 files in the same directory as your script, each named 
##         differently & remain there after your script finishes executing. 
##      Each of these 3 files must contain exactly 10 random lowercase characters.
##          With no spaces, & the final (11th) character pf each file MUST be a [\n].
##  3.) When executed, output sent to stdout should first print out the contents 
##        of the 3 files it is creating in exactly the format given below.
##  4.) After the file contents of all three files have been printed, print out 
##        two random integers each on a separate line 
##          (whose range is from 1 to 42, inclusive).
##  5.) Finally, on the last (sixth) line, print out the product of the two numbers.
## ------------------------------------------------------------------------------------ ##

### importing these... ###
import random, string
from random import randint
### ------------------ ###

#### FILE / STRINGS PART !! ####
## setting names & open the 3 files to write to... ##
x = 1
while x < 4:
	# on the 1st loop, file#1
	if x == 1:
		fileName = open("File_1", 'w')
	# on the 2nd loop, file#2	
	if x == 2:
		fileName = open("File_2", 'w')
	# on the 3rd loop, file#3	
	if x == 3:
		fileName = open("File_3", 'w')
	##------------------------------##
	# to keysmash, generate a string of random letters....
	keysmash  = ''.join(random.choice(string.ascii_lowercase) for letts in range(10))
	fileName.write(keysmash + "\n")		# pass/write that string to current file
	fileName.close()						# close current file
	x+=1			# increment x to loop to next file.....
## ----------------------------------------------- ##   
#print "\nGenerating Keysmashes to Files..."
## open these 3 files for reading... ##
#
y = 1
while y < 4:
	# on the 1st loop, file#1
	if y == 1:
		fileName = open("File_1", 'r')
	# on the 2nd loop, file#2
	if y == 2:
		fileName = open("File_2", 'r')
	# on the 3rd loop, file#3
	if y == 3:
		fileName = open("File_3", 'r')
	##------------------------------##
	contents = fileName.read(10)		# read the string from the line in the current file
#	print "file ", y, ": ", contents	# print the contents
	print contents						# print the contents (required format)
	fileName.close()				# close current file
	y+=1			# increment y to loop to next file.....
## --------------------------------- ##

#### NUMBERS / MATH PART !! ####
##  using randint from imported random...  ##
##  generate random numbers between 1 & 42 ##
firstNum = randint(1, 42)	# for 1st num
secondNum = randint(1, 42)	# for 2nd num

# print the random numbers assigned to firstNum & secondNum....
#print "\nGenerating Random Numbers..."
#print "1st number: " , firstNum		# print 1st num
#print "2nd number: " , secondNum		# print 2nd num
print firstNum							# print 1st num (required format)
print  secondNum						# print 2nd num (required format)

# calculate product of previously generated numbers
product = firstNum * secondNum						# product calculated
#print "\nSolving for Product..."
#print firstNum, " * ", secondNum, " = ", product	# show the math	
#print "Product is: ", product, " !!\n"				# print product
print product 										# print product (required format)