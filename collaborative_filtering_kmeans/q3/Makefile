JFLAGS = -g
JC = javac
.SUFFIXES: .java .class
.java.class:
	$(JC) $(JFLAGS) $*.java

CLASSES = \
	KMeans.java
default: classes

classes: $(CLASSES:.java=.class)

clean:
	find . -name "*.class" -exec rm {} \;
