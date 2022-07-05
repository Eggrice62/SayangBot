#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int transform_tempo(int t, vector<vector<int>> tempomat_local) {
	int newt = 0;
	int curTempoIndex = 0;
	
	vector<double> transformMat;
	for (int i=0; i<tempomat_local.size(); i++) {
		double curCoef = 1.;
		if (tempomat_local[i][1] < 2) {
			curCoef *= 32.;
		} else if (tempomat_local[i][1] < 4) {
			curCoef *= 16.;
		} else if (tempomat_local[i][1] < 8) {
			curCoef *= 8.;
		} else if (tempomat_local[i][1] < 16) {
			curCoef *= 4.;
		} else if (tempomat_local[i][1] < 32) {
			curCoef *= 2.;
		}
		if (tempomat_local[i][1] > 4080) {
			curCoef /= 32.;
		} else if (tempomat_local[i][1] > 2040) {
			curCoef /= 16.;
		} else if (tempomat_local[i][1] > 1020) {
			curCoef /= 8.;
		} else if (tempomat_local[i][1] > 510) {
			curCoef /= 4.;
		} else if (tempomat_local[i][1] > 255) {
			curCoef /= 2.;
		}
		transformMat.push_back(curCoef);
	}
	
	while (1) {
		if (curTempoIndex+1 < tempomat_local.size()) {
			if (t > tempomat_local[curTempoIndex+1][0]) {
				newt += (tempomat_local[curTempoIndex+1][0] - tempomat_local[curTempoIndex][0]) * transformMat[curTempoIndex];
			} else {
				newt += (t - tempomat_local[curTempoIndex][0]) * transformMat[curTempoIndex];
				break;
			}
		} else if (tempomat_local.size() > 0) {
			newt += (t - tempomat_local[curTempoIndex][0]) * transformMat[curTempoIndex];
			break;
		} else {
			newt = t;
			break;
		}
		curTempoIndex++;
	}
	
	return newt;
}