VERSION = 0.2
CPPFLAGS = -Wall -O2
PDSAMPLE_OBJS = RNG.o PDSample.o PDSampling.o RangeList.o ScallopedSector.o

all: PDSample

PDSample: $(PDSAMPLE_OBJS)
	g++ -o $@ $(PDSAMPLE_OBJS)

clean:
	rm -f PDSample $(PDSAMPLE_OBJS)

dist:
	rm -rf releases/PDSample-$(VERSION).tar.gz
	mkdir PDSample-$(VERSION)
	cp \
		Makefile PDSample.sln PDSample.vcproj \
		README.txt LICENSE.txt \
		PDSample.cpp PDSampling.cpp PDSampling.h \
		RNG.cpp RNG.h \
		RangeList.cpp RangeList.h \
		ScallopedSector.cpp ScallopedSector.h \
		WeightedDiscretePDF.cpp WeightedDiscretePDF.h \
		quasisampler_prototype.h \
		PDSample-$(VERSION)
	tar cvf releases/PDSample-$(VERSION).tar PDSample-$(VERSION)
	gzip releases/PDSample-$(VERSION).tar
	rm -rf PDSample-$(VERSION)