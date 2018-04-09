all:	q1 q3
.PHONY:	all q1 q3 clean

q1:
	cd q1; ./configure; make -k; cd ..
q3:
	cd q3; make; cd ..
clean:
	cd q1; make clean; cd ../q3; make clean; cd ..
