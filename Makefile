all:
	g++ -O3 -Wall -g sim_git.cpp -o sim

clean:
	rm -f *.o sim
