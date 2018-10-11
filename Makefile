.PHONY: clean

PGM: clean
	g++ -std=c++11 ProcessGroupMonitor.cpp -g -o PGM 

clean:
	rm -f PGM
