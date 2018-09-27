.PHONY: clean

PGM:
	g++ ProcessGroupMonitor.cpp -o PGM 

clean:
	rm PGM
