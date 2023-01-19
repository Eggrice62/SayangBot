#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

vector<string> opt_ms2mml(string s) {
	int iCurrent = 0;
	if (s == "") { return vector<string> {s, "4"}; }
	string sBefore = s;
	
	iCurrent = 0;
	int sumCheckBefore = 0;
	int lengthForSumCheck = 4;
	int tpqForSumCheck = 480;
	while (iCurrent < s.length()) {
		if (s.substr(iCurrent,1) == "a" || s.substr(iCurrent,1) == "b" || s.substr(iCurrent,1) == "c" || s.substr(iCurrent,1) == "d" || s.substr(iCurrent,1) == "e" || s.substr(iCurrent,1) == "f" || s.substr(iCurrent,1) == "g" || s.substr(iCurrent,1) == "r") {
			sumCheckBefore += 4*tpqForSumCheck/lengthForSumCheck;
			if (iCurrent+1 < s.length()) {
				if (s.substr(iCurrent+1,1) == ".") {
					sumCheckBefore += 4*tpqForSumCheck/lengthForSumCheck/2;
				} else if ((s.substr(iCurrent+1,1) == "+" || s.substr(iCurrent+1,1) == "-") && iCurrent+2 < s.length()) {
					if (s.substr(iCurrent+2,1) == ".") {
						sumCheckBefore += 4*tpqForSumCheck/lengthForSumCheck/2;
					} else if (s.substr(iCurrent+2,1) == "0" || s.substr(iCurrent+2,1) == "1" || s.substr(iCurrent+2,1) == "2" || s.substr(iCurrent+2,1) == "3" || s.substr(iCurrent+2,1) == "4"
							|| s.substr(iCurrent+2,1) == "5" || s.substr(iCurrent+2,1) == "6" || s.substr(iCurrent+2,1) == "7" || s.substr(iCurrent+2,1) == "8" || s.substr(iCurrent+2,1) == "9") {
						sumCheckBefore -= 4*tpqForSumCheck/lengthForSumCheck;
						int iLengthStart = iCurrent+2;
						int iLengthEnd = iCurrent+2;
						while ((iLengthEnd < s.length()) && (s.substr(iLengthEnd,1) == "0" || s.substr(iLengthEnd,1) == "1" || s.substr(iLengthEnd,1) == "2" || s.substr(iLengthEnd,1) == "3" || s.substr(iLengthEnd,1) == "4"
														|| s.substr(iLengthEnd,1) == "5" || s.substr(iLengthEnd,1) == "6" || s.substr(iLengthEnd,1) == "7" || s.substr(iLengthEnd,1) == "8" || s.substr(iLengthEnd,1) == "9")) {
							iLengthEnd++;
						}
						string lengthText = s.substr(iLengthStart,iLengthEnd-iLengthStart);
						stringstream ssInt(lengthText);
						int tempLength = -1;
						ssInt >> tempLength;
						sumCheckBefore += 4*tpqForSumCheck/tempLength;
						if (iLengthEnd < s.length()) {
							if (s.substr(iLengthEnd,1) == ".") {
								sumCheckBefore += 4*tpqForSumCheck/tempLength/2;
							}
						}
					}
				} else if (s.substr(iCurrent+1,1) == "0" || s.substr(iCurrent+1,1) == "1" || s.substr(iCurrent+1,1) == "2" || s.substr(iCurrent+1,1) == "3" || s.substr(iCurrent+1,1) == "4"
						|| s.substr(iCurrent+1,1) == "5" || s.substr(iCurrent+1,1) == "6" || s.substr(iCurrent+1,1) == "7" || s.substr(iCurrent+1,1) == "8" || s.substr(iCurrent+1,1) == "9") {
					sumCheckBefore -= 4*tpqForSumCheck/lengthForSumCheck;
					int iLengthStart = iCurrent+1;
					int iLengthEnd = iCurrent+1;
					while ((iLengthEnd < s.length()) && (s.substr(iLengthEnd,1) == "0" || s.substr(iLengthEnd,1) == "1" || s.substr(iLengthEnd,1) == "2" || s.substr(iLengthEnd,1) == "3" || s.substr(iLengthEnd,1) == "4"
													|| s.substr(iLengthEnd,1) == "5" || s.substr(iLengthEnd,1) == "6" || s.substr(iLengthEnd,1) == "7" || s.substr(iLengthEnd,1) == "8" || s.substr(iLengthEnd,1) == "9")) {
						iLengthEnd++;
					}
					string lengthText = s.substr(iLengthStart,iLengthEnd-iLengthStart);
					stringstream ssInt(lengthText);
					int tempLength = -1;
					ssInt >> tempLength;
					sumCheckBefore += 4*tpqForSumCheck/tempLength;
					if (iLengthEnd < s.length()) {
						if (s.substr(iLengthEnd,1) == ".") {
							sumCheckBefore += 4*tpqForSumCheck/tempLength/2;
						}
					}
				}
			}
		} else if (s.substr(iCurrent,1) == "l") {
			int iLengthStart = iCurrent+1;
			int iLengthEnd = iCurrent+1;
			while ((iLengthEnd < s.length()) && (s.substr(iLengthEnd,1) == "0" || s.substr(iLengthEnd,1) == "1" || s.substr(iLengthEnd,1) == "2" || s.substr(iLengthEnd,1) == "3" || s.substr(iLengthEnd,1) == "4"
											|| s.substr(iLengthEnd,1) == "5" || s.substr(iLengthEnd,1) == "6" || s.substr(iLengthEnd,1) == "7" || s.substr(iLengthEnd,1) == "8" || s.substr(iLengthEnd,1) == "9")) {
				iLengthEnd++;
			}
			string lengthText = s.substr(iLengthStart,iLengthEnd-iLengthStart);
			stringstream ssInt(lengthText);
			ssInt >> lengthForSumCheck;
		}
		iCurrent++;
	}
	
	iCurrent = 0;
	int p1 = 0; int p2 = 0; int p3 = 0; int p4 = 0; int p5 = 0; int p6 = 0; int p7 = 0;
	int intermediateNote = 0;
	int targetOutLoop = 1;
	while (p1+2 < s.length() && MYPE == 1) {
		if (s.substr(p1,3) == "l32") {
			p2 = p1 + 3;
			targetOutLoop = 2;
			while (p2 < s.length()) {
				if (s.substr(p2,1) == "a" || s.substr(p2,1) == "b" || s.substr(p2,1) == "c" || s.substr(p2,1) == "d" || s.substr(p2,1) == "e" || s.substr(p2,1) == "f" || s.substr(p2,1) == "g" || s.substr(p2,1) == "r") {
					p3 = p2 + 1;
					targetOutLoop = 3;
					while (p3 < s.length()) {
						if (s.substr(p3,1) == "+" || s.substr(p3,1) == "-") { p3++; continue; }
						if (s.substr(p3,1) == ".") {
							p4 = p3 + 1;
							targetOutLoop = 4;
							while (p4 < s.length()) {
								if (s.substr(p4,1) == "a" || s.substr(p4,1) == "b" || s.substr(p4,1) == "c" || s.substr(p4,1) == "d" || s.substr(p4,1) == "e" || s.substr(p4,1) == "f" || s.substr(p4,1) == "g" || s.substr(p4,1) == "r") {
									p5 = p4 + 1;
									targetOutLoop = 5;
									while (p5 < s.length()) {
										if (s.substr(p5,1) == "a" || s.substr(p5,1) == "b" || s.substr(p5,1) == "c" || s.substr(p5,1) == "d" || s.substr(p5,1) == "e" || s.substr(p5,1) == "f" || s.substr(p5,1) == "g" || s.substr(p5,1) == "r") {
											p6 = p5 + 1;
											targetOutLoop = 6;
											while (p6 < s.length()) {
												if (s.substr(p6,1) == "+" || s.substr(p6,1) == "-") { p6++; continue; }
												if (s.substr(p6,1) == ".") {
													p7 = p6 + 1;
													targetOutLoop = 7;
													while (p7 < s.length()) {
														if (s.substr(p7,1) == "a" || s.substr(p7,1) == "b" || s.substr(p7,1) == "c" || s.substr(p7,1) == "d" || s.substr(p7,1) == "e" || s.substr(p7,1) == "f" || s.substr(p7,1) == "g" || s.substr(p7,1) == "r") {
															intermediateNote = 0;
															for (int iInter=p1; iInter<p2; iInter++) {
																if (s.substr(iInter,1) == "a" || s.substr(iInter,1) == "b" || s.substr(iInter,1) == "c" || s.substr(iInter,1) == "d" || s.substr(iInter,1) == "e" || s.substr(iInter,1) == "f" || s.substr(iInter,1) == "g" || s.substr(iInter,1) == "r") {
																	intermediateNote++;
																}
															}
															if (intermediateNote == 0) {
																s = s.substr(0,p1) + "l24" + s.substr(p1+3,p2-p1-3) + s.substr(p2,p3-p2) + s.substr(p3+1,p4-p3-1) + s.substr(p4,p5-p4) + s.substr(p5,p6-p5) + s.substr(p6+1,p7-p6-1) + "l32" + s.substr(p7);
															} else {
																s = s.substr(0,p1) + s.substr(p1,p2-p1) + "l24" + s.substr(p2,p3-p2) + s.substr(p3+1,p4-p3-1) + s.substr(p4,p5-p4) + s.substr(p5,p6-p5) + s.substr(p6+1,p7-p6-1) + "l32" + s.substr(p7);
															}
															targetOutLoop = 1;
															break;
														} else if (s.substr(p7,1) == "l") {
															intermediateNote = 0;
															for (int iInter=p1; iInter<p2; iInter++) {
																if (s.substr(iInter,1) == "a" || s.substr(iInter,1) == "b" || s.substr(iInter,1) == "c" || s.substr(iInter,1) == "d" || s.substr(iInter,1) == "e" || s.substr(iInter,1) == "f" || s.substr(iInter,1) == "g" || s.substr(iInter,1) == "r") {
																	intermediateNote++;
																}
															}
															if (intermediateNote == 0) {
																s = s.substr(0,p1) + "l24" + s.substr(p1+3,p2-p1-3) + s.substr(p2,p3-p2) + s.substr(p3+1,p4-p3-1) + s.substr(p4,p5-p4) + s.substr(p5,p6-p5) + s.substr(p6+1,p7-p6-1) + s.substr(p7);
															} else {
																s = s.substr(0,p1) + s.substr(p1,p2-p1) + "l24" + s.substr(p2,p3-p2) + s.substr(p3+1,p4-p3-1) + s.substr(p4,p5-p4) + s.substr(p5,p6-p5) + s.substr(p6+1,p7-p6-1) + s.substr(p7);
															}
															targetOutLoop = 1;
															break;
														}
														p7++;
													}
													if (targetOutLoop < 6) { break; }
													if (p7 == s.length()) {
														intermediateNote = 0;
														for (int iInter=p1; iInter<p2; iInter++) {
															if (s.substr(iInter,1) == "a" || s.substr(iInter,1) == "b" || s.substr(iInter,1) == "c" || s.substr(iInter,1) == "d" || s.substr(iInter,1) == "e" || s.substr(iInter,1) == "f" || s.substr(iInter,1) == "g" || s.substr(iInter,1) == "r") {
																intermediateNote++;
															}
														}
														if (intermediateNote == 0) {
															s = s.substr(0,p1) + "l24" + s.substr(p1+3,p2-p1-3) + s.substr(p2,p3-p2) + s.substr(p3+1,p4-p3-1) + s.substr(p4,p5-p4) + s.substr(p5,p6-p5) + s.substr(p6+1);
														} else {
															s = s.substr(0,p1) + s.substr(p1,p2-p1) + "l24" + s.substr(p2,p3-p2) + s.substr(p3+1,p4-p3-1) + s.substr(p4,p5-p4) + s.substr(p5,p6-p5) + s.substr(p6+1);
														}
														targetOutLoop = 1;
														break;
													}
												} else {
													targetOutLoop = 2;
													break;
												}
											}
											if (targetOutLoop < 6) { break; }
										} else if (s.substr(p5,1) == "." || s.substr(p5,1) == "l" || s.substr(p5,1) == "t" || s.substr(p5,1) == "0" || s.substr(p5,1) == "1" || s.substr(p5,1) == "2" || s.substr(p5,1) == "3" || s.substr(p5,1) == "4" || s.substr(p5,1) == "5" || s.substr(p5,1) == "6" || s.substr(p5,1) == "7" || s.substr(p5,1) == "8" || s.substr(p5,1) == "9") {
											targetOutLoop = 2;
											break;
										} else if (s.substr(p5,1) == "v" || s.substr(p5,1) == "p" || s.substr(p5,1) == "s" || s.substr(p5,1) == "m") {
											while (p5+1 < s.length()) {
												if (!(s.substr(p5+1,1) == "0" || s.substr(p5+1,1) == "1" || s.substr(p5+1,1) == "2" || s.substr(p5+1,1) == "3" || s.substr(p5+1,1) == "4" || s.substr(p5+1,1) == "5" || s.substr(p5+1,1) == "6" || s.substr(p5+1,1) == "7" || s.substr(p5+1,1) == "8" || s.substr(p5+1,1) == "9")) { break; }
												p5++;
											}
										}
										if (targetOutLoop < 5) { break; }
										p5++;
									}
									if (p5 == s.length()) {
										targetOutLoop = 2;
										break;
									}
								} else if (s.substr(p4,1) == "l" || s.substr(p4,1) == "t") {
									targetOutLoop = 2;
									break;
								}
								if (targetOutLoop < 4) { break; }
								p4++;
							}
							if (p4 == s.length()) {
								targetOutLoop = 2;
								break;
							}
						} else {
							targetOutLoop = 2;
							break;
						}
						if (targetOutLoop < 3) { break; }
					} // while p3
				} else if (s.substr(p2,1) == "l") {
					targetOutLoop = 1;
					break;
				}
				if (targetOutLoop < 2) { break; }
				p2++;
			} // while p2
		} // if p2 l32
		p1++;
	} // while p1
	
	iCurrent = 0;
	p1 = 0; p2 = 0; p3 = 0; p4 = 0; p5 = 0; p6 = 0; p7 = 0; int p8 = 0; int p9 = 0; int p10 = 0; int p11 = 0;
	int p4_sub = 0; int p10_sub = 0;
	intermediateNote = 0;
	targetOutLoop = 1;
	int isThereTie1 = -1; // -1 : not found, -2 : not required, etc : position
	int isThereTie2 = -1;
	// cout << s << endl;
	while (p1+2 < s.length() && MYPE == 1) {
		if (s.substr(p1,3) == "l16") {
			// cout << to_string(p1) << " p1 " << s.substr(p1,30) << endl;
			p2 = p1 + 3;
			targetOutLoop = 2;
			while (p2 < s.length()) {
				if (s.substr(p2,1) == "a" || s.substr(p2,1) == "b" || s.substr(p2,1) == "c" || s.substr(p2,1) == "d" || s.substr(p2,1) == "e" || s.substr(p2,1) == "f" || s.substr(p2,1) == "g" || s.substr(p2,1) == "r") {
					// cout << to_string(p2) << " p2 " << s.substr(p2,30) << endl;
					p3 = p2 + 1;
					if (s.substr(p2,1) == "r") {
						isThereTie1 = -2;
					} else {
						isThereTie1 = -1;
					}
					targetOutLoop = 3;
					while (p3+2 < s.length()) {
						if (s.substr(p3,3) == "l64") {
							// cout << to_string(p3) << " p3 " << s.substr(p3,30) << endl;
							p4 = p3 + 3;
							targetOutLoop = 4;
							while (p4 < s.length()) {
								if (s.substr(p4,1) == "&") { isThereTie1 = p4; p4++; continue; }
								if (s.substr(p4,1) == "a" || s.substr(p4,1) == "b" || s.substr(p4,1) == "c" || s.substr(p4,1) == "d" || s.substr(p4,1) == "e" || s.substr(p4,1) == "f" || s.substr(p4,1) == "g" || s.substr(p4,1) == "r") {
									// cout << to_string(p4) << " p4 " << s.substr(p4,30) << endl;
									if (isThereTie1 == -1) { targetOutLoop = 2; break; }
									p4_sub = (p4+1 < s.length() && ((s.substr(p4+1,1) == "+") || (s.substr(p4+1,1) == "-"))) ? 2 : 1;
									p5 = p4 + 1;
									targetOutLoop = 5;
									while (p5+2 < s.length()) {
										if (s.substr(p5,3) == "l16") {
											// cout << to_string(p5) << " p5 " << s.substr(p5,30) << endl;
											p6 = p5 + 3;
											targetOutLoop = 6;
											while (p6 < s.length()) {
												if (s.substr(p6,1) == "a" || s.substr(p6,1) == "b" || s.substr(p6,1) == "c" || s.substr(p6,1) == "d" || s.substr(p6,1) == "e" || s.substr(p6,1) == "f" || s.substr(p6,1) == "g" || s.substr(p6,1) == "r") {
													// cout << to_string(p6) << " p6 " << s.substr(p6,30) << endl;
													p7 = p6 + 1;
													targetOutLoop = 7;
													while (p7 < s.length()) {
														if (s.substr(p7,1) == "+" || s.substr(p7,1) == "-") { p7++; continue; }
														if (s.substr(p7,1) == ".") {
															// cout << to_string(p7) << " p7 " << s.substr(p7,30) << endl;
															p8 = p7 + 1;
															targetOutLoop = 8;
															while (p8 < s.length()) {
																if (s.substr(p8,1) == "a" || s.substr(p8,1) == "b" || s.substr(p8,1) == "c" || s.substr(p8,1) == "d" || s.substr(p8,1) == "e" || s.substr(p8,1) == "f" || s.substr(p8,1) == "g" || s.substr(p8,1) == "r") {
																	// cout << to_string(p8) << " p8 " << s.substr(p8,30) << endl;
																	p9 = p8 + 1;
																	if (s.substr(p8,1) == "r") {
																		isThereTie2 = -2;
																	} else {
																		isThereTie2 = -1;
																	}
																	targetOutLoop = 9;
																	while (p9+2 < s.length()) {
																		if (s.substr(p9,3) == "l64") {
																			// cout << to_string(p9) << " p9 " << s.substr(p9,30) << endl;
																			p10 = p9 + 3;
																			targetOutLoop = 10;
																			while (p10 < s.length()) {
																				if (s.substr(p10,1) == "&") { isThereTie2 = p10; p10++; continue; }
																				if (s.substr(p10,1) == "a" || s.substr(p10,1) == "b" || s.substr(p10,1) == "c" || s.substr(p10,1) == "d" || s.substr(p10,1) == "e" || s.substr(p10,1) == "f" || s.substr(p10,1) == "g" || s.substr(p10,1) == "r") {
																					// cout << to_string(p10) << " p10 " << s.substr(p10,30) << endl;
																					if (isThereTie2 == -1) { targetOutLoop = 2; break; }
																					p10_sub = (p10+1 < s.length() && ((s.substr(p10+1,1) == "+") || (s.substr(p10+1,1) == "-"))) ? 2 : 1;
																					p11 = p10 + 1;
																					targetOutLoop = 11;
																					while (p11 < s.length()) {
																						if (s.substr(p11,1) == "a" || s.substr(p11,1) == "b" || s.substr(p11,1) == "c" || s.substr(p11,1) == "d" || s.substr(p11,1) == "e" || s.substr(p11,1) == "f" || s.substr(p11,1) == "g" || s.substr(p11,1) == "r") {
																							if (isThereTie2 == -2) {
																								isThereTie2 = p10; s = s.substr(0,p10) + " " + s.substr(p10); p10++; p11++;
																							}
																							if (isThereTie1 == -2) {
																								isThereTie1 = p4; s = s.substr(0,p4) + " " + s.substr(p4); p4++; p5++; p6++; p7++; p8++; p9++; p10++; isThereTie2++; p11++;
																							}
																							intermediateNote = 0;
																							for (int iInter=p1; iInter<p2; iInter++) {
																								if (s.substr(iInter,1) == "a" || s.substr(iInter,1) == "b" || s.substr(iInter,1) == "c" || s.substr(iInter,1) == "d" || s.substr(iInter,1) == "e" || s.substr(iInter,1) == "f" || s.substr(iInter,1) == "g" || s.substr(iInter,1) == "r") {
																									intermediateNote++;
																								}
																							}
																							if (intermediateNote == 0) {
																								// cout << "opt1" << endl;
																								// cout << s << endl;
																								s = s.substr(0,p1) + "l12" + s.substr(p1+3,p2-p1-3) + s.substr(p2,p3-p2) + s.substr(p3+3,isThereTie1-p3-3) + s.substr(isThereTie1+1,p4-isThereTie1-1) + s.substr(p4+p4_sub,p5-p4-p4_sub) + s.substr(p5+3,p6-p5-3) + s.substr(p6,p7-p6) + s.substr(p7+1,p8-p7-1) + s.substr(p8,p9-p8) + s.substr(p9+3,isThereTie2-p9-3) + s.substr(isThereTie2+1,p10-isThereTie2-1) + s.substr(p10+p10_sub,p11-p10-p10_sub) + "l64" + s.substr(p11);
																								// cout << s << endl;
																							} else {
																								// cout << "opt2" << endl;
																								// cout << s << endl;
																								s = s.substr(0,p1) + s.substr(p1,p2-p1) + "l12" + s.substr(p2,p3-p2) + s.substr(p3+3,isThereTie1-p3-3) + s.substr(isThereTie1+1,p4-isThereTie1-1) + s.substr(p4+p4_sub,p5-p4-p4_sub) + s.substr(p5+3,p6-p5-3) + s.substr(p6,p7-p6) + s.substr(p7+1,p8-p7-1) + s.substr(p8,p9-p8) + s.substr(p9+3,isThereTie2-p9-3) + s.substr(isThereTie2+1,p10-isThereTie2-1) + s.substr(p10+p10_sub,p11-p10-p10_sub) + "l64" + s.substr(p11);
																								// cout << s << endl;
																							}
																							targetOutLoop = 1;
																							break;
																						} else if (s.substr(p11,1) == "l") {
																							if (isThereTie2 == -2) {
																								isThereTie2 = p10; s = s.substr(0,p10) + " " + s.substr(p10); p10++; p11++;
																							}
																							if (isThereTie1 == -2) {
																								isThereTie1 = p4; s = s.substr(0,p4) + " " + s.substr(p4); p4++; p5++; p6++; p7++; p8++; p9++; p10++; isThereTie2++; p11++;
																							}
																							intermediateNote = 0;
																							for (int iInter=p1; iInter<p2; iInter++) {
																								if (s.substr(iInter,1) == "a" || s.substr(iInter,1) == "b" || s.substr(iInter,1) == "c" || s.substr(iInter,1) == "d" || s.substr(iInter,1) == "e" || s.substr(iInter,1) == "f" || s.substr(iInter,1) == "g" || s.substr(iInter,1) == "r") {
																									intermediateNote++;
																								}
																							}
																							if (intermediateNote == 0) {
																								// cout << "opt3" << endl;
																								// cout << s << endl;
																							// cout << "p1 " << to_string(p1) << " " << s.substr(p1) << endl;
																							// cout << "p2 " << to_string(p2) << " " << s.substr(p2,1) << endl;
																							// cout << "p3 " << to_string(p3) << " " << s.substr(p3,1) << endl;
																							// cout << "p4 " << to_string(p4) << " " << s.substr(p4,1) << endl;
																							// cout << "p5 " << to_string(p5) << " " << s.substr(p5,1) << endl;
																							// cout << "p6 " << to_string(p6) << " " << s.substr(p6,1) << endl;
																							// cout << "p7 " << to_string(p7) << " " << s.substr(p7,1) << endl;
																							// cout << "p8 " << to_string(p8) << " " << s.substr(p8,1) << endl;
																							// cout << "p9 " << to_string(p9) << " " << s.substr(p9,1) << endl;
																							// cout << "p10 " << to_string(p10) << " " << s.substr(p10,1) << endl;
																							// cout << "p11 " << to_string(p11) << " " << s.substr(p11,1) << endl;
																							// cout << "isThereTie1 " << to_string(isThereTie1) << endl;
																							// cout << "isThereTie2 " << to_string(isThereTie2) << endl;
																								s = s.substr(0,p1) + "l12" + s.substr(p1+3,p2-p1-3) + s.substr(p2,p3-p2) + s.substr(p3+3,isThereTie1-p3-3) + s.substr(isThereTie1+1,p4-isThereTie1-1) + s.substr(p4+p4_sub,p5-p4-p4_sub) + s.substr(p5+3,p6-p5-3) + s.substr(p6,p7-p6) + s.substr(p7+1,p8-p7-1) + s.substr(p8,p9-p8) + s.substr(p9+3,isThereTie2-p9-3) + s.substr(isThereTie2+1,p10-isThereTie2-1) + s.substr(p10+p10_sub,p11-p10-p10_sub) + s.substr(p11);
																								// cout << s << endl;
																							} else {
																								// cout << "opt4" << endl;
																								// cout << s << endl;
																								s = s.substr(0,p1) + s.substr(p1,p2-p1) + "l12" + s.substr(p2,p3-p2) + s.substr(p3+3,isThereTie1-p3-3) + s.substr(isThereTie1+1,p4-isThereTie1-1) + s.substr(p4+p4_sub,p5-p4-p4_sub) + s.substr(p5+3,p6-p5-3) + s.substr(p6,p7-p6) + s.substr(p7+1,p8-p7-1) + s.substr(p8,p9-p8) + s.substr(p9+3,isThereTie2-p9-3) + s.substr(isThereTie2+1,p10-isThereTie2-1) + s.substr(p10+p10_sub,p11-p10-p10_sub) + s.substr(p11);
																								// cout << s << endl;
																							}
																							targetOutLoop = 1;
																							break;
																						} else if (s.substr(p11,1) == ".") {
																							targetOutLoop = 2;
																							break;
																						}
																						p11++;
																					}
																					if (targetOutLoop < 10) { break; }
																					if (p11 == s.length()) {
																						if (isThereTie2 == -2) {
																							isThereTie2 = p10; s = s.substr(0,p10) + " " + s.substr(p10); p10++; p11++;
																						}
																						if (isThereTie1 == -2) {
																							isThereTie1 = p4; s = s.substr(0,p4) + " " + s.substr(p4); p4++; p5++; p6++; p7++; p8++; p9++; p10++; isThereTie2++; p11++;
																						}
																						intermediateNote = 0;
																						for (int iInter=p1; iInter<p2; iInter++) {
																							if (s.substr(iInter,1) == "a" || s.substr(iInter,1) == "b" || s.substr(iInter,1) == "c" || s.substr(iInter,1) == "d" || s.substr(iInter,1) == "e" || s.substr(iInter,1) == "f" || s.substr(iInter,1) == "g" || s.substr(iInter,1) == "r") {
																								intermediateNote++;
																							}
																						}
																						if (intermediateNote == 0) {
																							// cout << "opt5" << endl;
																							// cout << s << endl;
																							s = s.substr(0,p1) + "l12" + s.substr(p1+3,p2-p1-3) + s.substr(p2,p3-p2) + s.substr(p3+3,isThereTie1-p3-3) + s.substr(isThereTie1+1,p4-isThereTie1-1) + s.substr(p4+p4_sub,p5-p4-p4_sub) + s.substr(p5+3,p6-p5-3) + s.substr(p6,p7-p6) + s.substr(p7+1,p8-p7-1) + s.substr(p8,p9-p8) + s.substr(p9+3,isThereTie2-p9-3) + s.substr(isThereTie2+1,p10-isThereTie2-1);
																							// cout << s << endl;
																						} else {
																							// cout << "opt6" << endl;
																							// cout << s << endl;
																							s = s.substr(0,p1) + s.substr(p1,p2-p1) + "l12" + s.substr(p2,p3-p2) + s.substr(p3+3,isThereTie1-p3-3) + s.substr(isThereTie1+1,p4-isThereTie1-1) + s.substr(p4+p4_sub,p5-p4-p4_sub) + s.substr(p5+3,p6-p5-3) + s.substr(p6,p7-p6) + s.substr(p7+1,p8-p7-1) + s.substr(p8,p9-p8) + s.substr(p9+3,isThereTie2-p9-3) + s.substr(isThereTie2+1,p10-isThereTie2-1);
																							// cout << s << endl;
																						}
																						targetOutLoop = 1;
																						break;
																					}
																				} else if (s.substr(p10,1) == "t") {
																					targetOutLoop = 2;
																					break;
																				}
																				if (targetOutLoop < 10) { break; }
																				p10++;
																			}
																			if (p10 == s.length()) {
																				targetOutLoop = 2;
																				break;
																			}
																		} else if (s.substr(p9,1) == "." || s.substr(p9,1) == "l" || s.substr(p9,1) == "t" || s.substr(p9,1) == "a" || s.substr(p9,1) == "b" || s.substr(p9,1) == "c" || s.substr(p9,1) == "d" || s.substr(p9,1) == "e" || s.substr(p9,1) == "f" || s.substr(p9,1) == "g" || s.substr(p9,1) == "r") {
																			targetOutLoop = 2;
																			break;
																		}
																		if (targetOutLoop < 9) { break; }
																		p9++;
																	}
																	if (p9 == s.length()) {
																		targetOutLoop = 2;
																		break;
																	}
																} else if (s.substr(p8,1) == "l" || s.substr(p8,1) == "t") {
																	targetOutLoop = 2;
																	break;
																}
																if (targetOutLoop < 8) { break; }
																p8++;
															}
															if (p8 == s.length()) {
																targetOutLoop = 2;
																break;
															}
														} else {
															targetOutLoop = 2;
															break;
														}
														if (targetOutLoop < 7) { break; }
													}
													if (p7 == s.length()) {
														targetOutLoop = 2;
														break;
													}
												} else if (s.substr(p6,1) == "t") {
													targetOutLoop = 2;
													break;
												}
												if (targetOutLoop < 6) { break; }
												p6++;
											}
											if (p6 == s.length()) {
												targetOutLoop = 2;
												break;
											}
										} else if (s.substr(p5,1) == "." || s.substr(p5,1) == "l" || s.substr(p5,1) == "t" || s.substr(p5,1) == "a" || s.substr(p5,1) == "b" || s.substr(p5,1) == "c" || s.substr(p5,1) == "d" || s.substr(p5,1) == "e" || s.substr(p5,1) == "f" || s.substr(p5,1) == "g" || s.substr(p5,1) == "r") {
											targetOutLoop = 2;
											break;
										}
										if (targetOutLoop < 5) { break; }
										p5++;
									}
									if (p5 == s.length()) {
										targetOutLoop = 2;
										break;
									}
								} else if (s.substr(p4,1) == "t") {
									targetOutLoop = 2;
									break;
								}
								if (targetOutLoop < 4) { break; }
								p4++;
							}
							if (p4 == s.length()) {
								targetOutLoop = 2;
								break;
							}
						} else if (s.substr(p3,1) == "." || s.substr(p3,1) == "l" || s.substr(p3,1) == "t" || s.substr(p3,1) == "a" || s.substr(p3,1) == "b" || s.substr(p3,1) == "c" || s.substr(p3,1) == "d" || s.substr(p3,1) == "e" || s.substr(p3,1) == "f" || s.substr(p3,1) == "g" || s.substr(p3,1) == "r") {
							targetOutLoop = 2;
							break;
						}
						if (targetOutLoop < 3) { break; }
						p3++;
					} // while p3
				} else if (s.substr(p2,1) == "l") {
					targetOutLoop = 1;
					break;
				}
				if (targetOutLoop < 2) { break; }
				p2++;
			} // while p2
		} // if p2 l32
		p1++;
	} // while p1
	
	iCurrent = 0;
	int iCurrentNumLength = 0;
	vector<string> listLength;
	vector<int> numLength;
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
	
	iCurrent = 0;
	int sumCheckAfter = 0;
	lengthForSumCheck = 4;
	while (iCurrent < s.length()) {
		if (s.substr(iCurrent,1) == "a" || s.substr(iCurrent,1) == "b" || s.substr(iCurrent,1) == "c" || s.substr(iCurrent,1) == "d" || s.substr(iCurrent,1) == "e" || s.substr(iCurrent,1) == "f" || s.substr(iCurrent,1) == "g" || s.substr(iCurrent,1) == "r") {
			sumCheckAfter += 4*tpqForSumCheck/lengthForSumCheck;
			if (iCurrent+1 < s.length()) {
				if (s.substr(iCurrent+1,1) == ".") {
					sumCheckAfter += 4*tpqForSumCheck/lengthForSumCheck/2;
				} else if ((s.substr(iCurrent+1,1) == "+" || s.substr(iCurrent+1,1) == "-") && iCurrent+2 < s.length()) {
					if (s.substr(iCurrent+2,1) == ".") {
						sumCheckAfter += 4*tpqForSumCheck/lengthForSumCheck/2;
					} else if (s.substr(iCurrent+2,1) == "0" || s.substr(iCurrent+2,1) == "1" || s.substr(iCurrent+2,1) == "2" || s.substr(iCurrent+2,1) == "3" || s.substr(iCurrent+2,1) == "4"
							|| s.substr(iCurrent+2,1) == "5" || s.substr(iCurrent+2,1) == "6" || s.substr(iCurrent+2,1) == "7" || s.substr(iCurrent+2,1) == "8" || s.substr(iCurrent+2,1) == "9") {
						sumCheckAfter -= 4*tpqForSumCheck/lengthForSumCheck;
						int iLengthStart = iCurrent+2;
						int iLengthEnd = iCurrent+2;
						while ((iLengthEnd < s.length()) && (s.substr(iLengthEnd,1) == "0" || s.substr(iLengthEnd,1) == "1" || s.substr(iLengthEnd,1) == "2" || s.substr(iLengthEnd,1) == "3" || s.substr(iLengthEnd,1) == "4"
														|| s.substr(iLengthEnd,1) == "5" || s.substr(iLengthEnd,1) == "6" || s.substr(iLengthEnd,1) == "7" || s.substr(iLengthEnd,1) == "8" || s.substr(iLengthEnd,1) == "9")) {
							iLengthEnd++;
						}
						string lengthText = s.substr(iLengthStart,iLengthEnd-iLengthStart);
						stringstream ssInt(lengthText);
						int tempLength = -1;
						ssInt >> tempLength;
						sumCheckAfter += 4*tpqForSumCheck/tempLength;
						if (iLengthEnd < s.length()) {
							if (s.substr(iLengthEnd,1) == ".") {
								sumCheckAfter += 4*tpqForSumCheck/tempLength/2;
							}
						}
					}
				} else if (s.substr(iCurrent+1,1) == "0" || s.substr(iCurrent+1,1) == "1" || s.substr(iCurrent+1,1) == "2" || s.substr(iCurrent+1,1) == "3" || s.substr(iCurrent+1,1) == "4"
						|| s.substr(iCurrent+1,1) == "5" || s.substr(iCurrent+1,1) == "6" || s.substr(iCurrent+1,1) == "7" || s.substr(iCurrent+1,1) == "8" || s.substr(iCurrent+1,1) == "9") {
					sumCheckAfter -= 4*tpqForSumCheck/lengthForSumCheck;
					int iLengthStart = iCurrent+1;
					int iLengthEnd = iCurrent+1;
					while ((iLengthEnd < s.length()) && (s.substr(iLengthEnd,1) == "0" || s.substr(iLengthEnd,1) == "1" || s.substr(iLengthEnd,1) == "2" || s.substr(iLengthEnd,1) == "3" || s.substr(iLengthEnd,1) == "4"
													|| s.substr(iLengthEnd,1) == "5" || s.substr(iLengthEnd,1) == "6" || s.substr(iLengthEnd,1) == "7" || s.substr(iLengthEnd,1) == "8" || s.substr(iLengthEnd,1) == "9")) {
						iLengthEnd++;
					}
					string lengthText = s.substr(iLengthStart,iLengthEnd-iLengthStart);
					stringstream ssInt(lengthText);
					int tempLength = -1;
					ssInt >> tempLength;
					sumCheckAfter += 4*tpqForSumCheck/tempLength;
					if (iLengthEnd < s.length()) {
						if (s.substr(iLengthEnd,1) == ".") {
							sumCheckAfter += 4*tpqForSumCheck/tempLength/2;
						}
					}
				}
			}
		} else if (s.substr(iCurrent,1) == "l") {
			int iLengthStart = iCurrent+1;
			int iLengthEnd = iCurrent+1;
			while ((iLengthEnd < s.length()) && (s.substr(iLengthEnd,1) == "0" || s.substr(iLengthEnd,1) == "1" || s.substr(iLengthEnd,1) == "2" || s.substr(iLengthEnd,1) == "3" || s.substr(iLengthEnd,1) == "4"
											|| s.substr(iLengthEnd,1) == "5" || s.substr(iLengthEnd,1) == "6" || s.substr(iLengthEnd,1) == "7" || s.substr(iLengthEnd,1) == "8" || s.substr(iLengthEnd,1) == "9")) {
				iLengthEnd++;
			}
			string lengthText = s.substr(iLengthStart,iLengthEnd-iLengthStart);
			stringstream ssInt(lengthText);
			ssInt >> lengthForSumCheck;
		}
		iCurrent++;
	}
	
	if (sumCheckBefore != sumCheckAfter) {
		cout << "optimization error!" << endl;
		cout << to_string(sumCheckBefore) << endl;
		cout << sBefore << endl;
		cout << to_string(sumCheckAfter) << endl;
		cout << s << endl;
		exit(1);
	}
	
	if (sBefore.length() < s.length()) {
		cout << "low efficiency!" << endl;
		cout << to_string(sBefore.length()) << endl;
		cout << sBefore << endl;
		cout << to_string(s.length()) << endl;
		cout << s << endl;
	}
	
	return vector<string> {s, lastUnoptLength};
}