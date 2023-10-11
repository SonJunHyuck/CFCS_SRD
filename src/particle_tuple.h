#pragma once
#include <vector>
#include <stdio.h>

class particle_tuple
{
public:
	int i;
	int j;

	particle_tuple(int i, int j) { this->i = i; this->j = j; }
};


static std::vector<particle_tuple*> get_tuples(int n)
{
	std::vector<particle_tuple*> tuples;
	if (n >= 2) {
		for (int i = 0; i < n; i++) {
			for (int j = i + 1; j < n; j++) {
				tuples.push_back(new particle_tuple(i, j));
			}
		}
	}
	else {
		printf("Error: only one particle\n");
	}
	printf("\n");
	return tuples;
}