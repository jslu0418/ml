# README for Homework3 of CS6375.502
# In root directory of Homework3, use "make" to compile
# This will compile both the question 1 and question 3 codes
make
# Then, run the program 1 by using following instructions
# The first argument is Training data path and the second
#  argument is Testing data path
# Note: This program uses 15 threads parallel, in csgrads1
#  you can expect it finish in less than 3 minutes.
q1/src/main q1/netflix/TrainingRatings.txt q1/netflix/TestingRatings.txt
# Then, run the program 3 by using following instructions
# The first argument is origin image path and the second
# argument is K value, the third argument is output path
cd q3; java KMeans Koala.jpg 2 Koala_new.jpg; cd ..
cd q3; java KMeans Penguins.jpg 20 Penguins_new.jpg; cd ..

