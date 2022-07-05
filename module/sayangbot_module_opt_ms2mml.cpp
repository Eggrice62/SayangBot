#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

vector<string> opt_ms2mml(string s) {
	int iCurrent = 0;
	int iCurrentNumLength = 0;
	vector<string> listLength;
	vector<int> numLength;
	if (s == "") { return vector<string> {s, "4"}; }
	
	// cout << "optimizing " << s << endl;
	// cout << "length " << s.length() << endl;
	
	while (iCurrent < s.length()) {
		if (s.substr(iCurrent,1) == "l") {
			int iLengthStart = iCurrent+1;
			int iLengthEnd = iCurrent+1;
			while ((iLengthEnd < s.length()) && (s.substr(iLengthEnd,1) == "0" || s.substr(iLengthEnd,1) == "1" || s.substr(iLengthEnd,1) == "2" || s.substr(iLengthEnd,1) == "3" || s.substr(iLengthEnd,1) == "4"
											|| s.substr(iLengthEnd,1) == "5" || s.substr(iLengthEnd,1) == "6" || s.substr(iLengthEnd,1) == "7" || s.substr(iLengthEnd,1) == "8" || s.substr(iLengthEnd,1) == "9")) {
				iLengthEnd++;
			}
			listLength.push_back(s.substr(iLengthStart,iLengthEnd-iLengthStart));
			if (iCurrentNumLength > 0 && listLength.size() > 1) {
				numLength.push_back(iCurrentNumLength);
				iCurrentNumLength = 0;
			}
		} else if (s.substr(iCurrent,1) == "a" || s.substr(iCurrent,1) == "b" || s.substr(iCurrent,1) == "c" || s.substr(iCurrent,1) == "d" || s.substr(iCurrent,1) == "e" || s.substr(iCurrent,1) == "f" || s.substr(iCurrent,1) == "g" || s.substr(iCurrent,1) == "r") {
			iCurrentNumLength++;
		}
		iCurrent++;
	}
	numLength.push_back(iCurrentNumLength);
	iCurrentNumLength = 0;
	
	// cout << endl << "length list" << endl;
	// for (iCurrent=0; iCurrent<listLength.size(); iCurrent++) {
		// cout << listLength[iCurrent] << endl;
	// }
	
	// cout << endl << "num list" << endl;
	// for (iCurrent=0; iCurrent<numLength.size(); iCurrent++) {
		// cout << numLength[iCurrent] << endl;
	// }
	
	if (listLength.size() == 0) { listLength.push_back("4"); }
	
	int iLengthNow = 0;
	iCurrent = 0;
	while (iCurrent < s.length()) {
		if (s.substr(iCurrent,1) == "l") {
			if (iLengthNow == 0) {
				// pass;
			} else {
				if (numLength[iLengthNow] == 1) {
					string lengthCurrentNote = listLength[iLengthNow];
					int iLengthCurrentNote = lengthCurrentNote.length();
					int iLengthCurrentName = 1;
					if (s.substr(iCurrent+1+iLengthCurrentNote,1) == "&") {
						iLengthCurrentName++;
					}
					if (s.substr(iCurrent+1+iLengthCurrentNote+iLengthCurrentName,1) == "+" || s.substr(iCurrent+1+iLengthCurrentNote+iLengthCurrentName,1) == "-") {
						iLengthCurrentName++;
					}
					s = s.substr(0,iCurrent) + s.substr(iCurrent+1+iLengthCurrentNote,iLengthCurrentName) + s.substr(iCurrent+1,iLengthCurrentNote) + s.substr(iCurrent+1+iLengthCurrentNote+iLengthCurrentName);
				} else {
					int invLength = iLengthNow - 1;
					while (numLength[invLength] == 1 && invLength > 0) {
						invLength--;
					}
					if (listLength[invLength] == listLength[iLengthNow]) {
						string lengthCurrentNote = listLength[iLengthNow];
						int iLengthCurrentNote = lengthCurrentNote.length();
						s = s.substr(0,iCurrent) + s.substr(iCurrent+1+iLengthCurrentNote);
					}
				}
			}
			iLengthNow++;
		}
		iCurrent++;
	}
	
	// cout << endl << "optimized : " << s << endl;
	// cout << "length " << s.length() << endl;
	
	string lastUnoptLength = "";
	if (listLength.size() > 0) {
		lastUnoptLength = listLength[0];
	} else {
		lastUnoptLength = "4";
	}
	for (iCurrent=0; iCurrent<numLength.size(); iCurrent++) {
		if (numLength[iCurrent] > 1) {
			lastUnoptLength = listLength[iCurrent];
		}
	}
	
	// cout << "lastUnoptLength : " << lastUnoptLength << endl;
	
	iCurrent = 0;
	while (iCurrent < s.length()-2) {
		if (s.substr(iCurrent,3) == "<b>") {
			string additiveStr = "";
			if (iCurrent < s.length()-3) {
				additiveStr = s.substr(iCurrent+3);
			}
			s = s.substr(0,iCurrent) + "c-" + additiveStr;
		} else if (s.substr(iCurrent,3) == ">c<") {
			string additiveStr = "";
			if (iCurrent < s.length()-3) {
				additiveStr = s.substr(iCurrent+3);
			}
			s = s.substr(0,iCurrent) + "b+" + additiveStr;
		}
		iCurrent++;
	}
	
	iCurrent = 0;
	while (iCurrent < s.length()-3) {
		if (s.substr(iCurrent,4) == "<br>") {
			string additiveStr = "";
			if (iCurrent < s.length()-4) {
				additiveStr = s.substr(iCurrent+4);
			}
			s = s.substr(0,iCurrent) + "c-r" + additiveStr;
		} else if (s.substr(iCurrent,4) == ">cr<") {
			string additiveStr = "";
			if (iCurrent < s.length()-4) {
				additiveStr = s.substr(iCurrent+4);
			}
			s = s.substr(0,iCurrent) + "b+r" + additiveStr;
		}
		iCurrent++;
	}
	
	return vector<string> {s, lastUnoptLength};
}