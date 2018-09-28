.PHONY: clean

PGM: clean
	g++ -std=c++11 ProcessGroupMonitor.cpp -o PGM 

clean:
	rm PGM
