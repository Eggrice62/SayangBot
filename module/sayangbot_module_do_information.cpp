#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

void do_information() {
	if (errorNow) { return; }
	
	bool isFirstMessage = true;
	
	vector<string> tempMessage;
	tempMessage.push_back("");
	tempMessage[tempMessage.size()-1] += "```$newline$BPM";
	if (tempomat_before_transform.size() == 0) {
		tempMessage[tempMessage.size()-1] += "$newline$\t정보 없음, 120으로 임의 고정";
	} else if (tempomat_before_transform.size() == 1 && tempomat_before_transform[0][0] == 0) {
		tempMessage[tempMessage.size()-1] += "$newline$\t" + to_string(tempomat_before_transform[0][1]);
	} else if (tempomat_before_transform.size() == 1 && tempomat_before_transform[0][0] != 0) {
		if (tempomat_before_transform[0][1] == 120) {
			tempMessage[tempMessage.size()-1] += "$newline$\t" + to_string(tempomat_before_transform[0][1]);
		} else {
			tempMessage[tempMessage.size()-1] += "$newline$\t120 -> " + to_string(tempomat_before_transform[0][1]);
		}
	} else {
		vector<int> tempoInfo {120, 99999, -1, 120};
		for (int i=0; i<tempomat_before_transform.size(); i++) {
			if (tempomat_before_transform[i][0] == 0) { tempoInfo[0] = tempomat_before_transform[i][1]; }
			tempoInfo[1] = (tempoInfo[1] < tempomat_before_transform[i][1]) ? tempoInfo[1] : tempomat_before_transform[i][1];
			tempoInfo[2] = (tempoInfo[2] > tempomat_before_transform[i][1]) ? tempoInfo[2] : tempomat_before_transform[i][1];
			tempoInfo[3] = tempomat_before_transform[i][1];
		}
		tempMessage[tempMessage.size()-1] += "$newline$\t시작 : " + to_string(tempoInfo[0]) + "$newline$\t최소 : " + to_string(tempoInfo[1]) + "$newline$\t최대 : " + to_string(tempoInfo[2]) + "$newline$\t종료 : " + to_string(tempoInfo[3]);
	}

	/*tempMessage[tempMessage.size()-1] += "$newline$박자$newline$";
	if (iMaxTimeSignature>0) {
		for (i=0; i<iMaxTimeSignature; i++) {
			if (i!=0) { tempMessage[tempMessage.size()-1] += " -> "; }
			tempMessage[tempMessage.size()-1] += timesigmat[i][1][0].toString() + "/" + Math.pow(2, timesigmat[i][1][1]).toString();
			if (i>5) { tempMessage[tempMessage.size()-1] += " -> ..."; break; }
		}
	} else {
		tempMessage[tempMessage.size()-1] += "정보 없음";
	}*/

	tempMessage[tempMessage.size()-1] += "$newline$재생 시간$newline$\t";
	if (maxCurrentTimeInReal > 3600) {
		tempMessage[tempMessage.size()-1] += to_string((int)(maxCurrentTimeInReal/3600)) + "시간 ";
		maxCurrentTimeInReal = maxCurrentTimeInReal - (int)(maxCurrentTimeInReal/3600) * 3600;
	}
	if (maxCurrentTimeInReal > 60) {
		tempMessage[tempMessage.size()-1] += to_string((int)(maxCurrentTimeInReal/60)) + "분 ";
		maxCurrentTimeInReal = maxCurrentTimeInReal - (int)(maxCurrentTimeInReal/60) * 60;
	}
	tempMessage[tempMessage.size()-1] += to_string((int)(maxCurrentTimeInReal)) + "초";
	
	tempMessage[tempMessage.size()-1] += "$newline$곡 전체 틱 / 4분 음표 틱$newline$\t";
	tempMessage[tempMessage.size()-1] += to_string(maxLengthTrack) + " / " + to_string(tickperquarter);

	tempMessage[tempMessage.size()-1] += "$newline$트랙 정보";
	for (itrack=0; itrack<tracks; itrack++) {
		tempMessage[tempMessage.size()-1] += "$newline$\t" + to_string(itrack+1) + "번 트랙" + trackName[itrack];
		for (ichannel=0; ichannel<iMaxChannel; ichannel++) {
			if (numNoteTrackChannel[itrack][ichannel] > 0) {
				tempMessage[tempMessage.size()-1] += "$newline$\t\t" + to_string(ichannel+1+iPort[itrack]*16) + "번 채널";
				tempMessage[tempMessage.size()-1] += "$newline$\t\t\t악기 : ";
				if (ichannel%16 == 9) {
					tempMessage[tempMessage.size()-1] += "Drumset";
				} else {
					if (instrumentTrackChannel[itrack][ichannel].size() == 0) {
						int countOtherInst = 0;
						for (int itrack2=0; itrack2<tracks; itrack2++) {
							if (instrumentTrackChannel[itrack2][ichannel].size() > 0) {
								countOtherInst++;
							}
						}
						if (countOtherInst == 1) {
							for (int itrack2=0; itrack2<tracks; itrack2++) {
								if (instrumentTrackChannel[itrack2][ichannel].size() > 0) {
									for (iInstr=0; iInstr<instrumentTrackChannel[itrack2][ichannel].size(); iInstr++) {
										if (iInstr!=0) { tempMessage[tempMessage.size()-1] += " -> "; }
										tempMessage[tempMessage.size()-1] += instr2name(instrumentTrackChannel[itrack2][ichannel][iInstr][1]);
										if (iInstr>5) { tempMessage[tempMessage.size()-1] += " -> ..."; break; }
									}
									tempMessage[tempMessage.size()-1] += " (" + to_string(itrack2+1) + "번 트랙에서 추정함)";
								}
							}
						} else {
							tempMessage[tempMessage.size()-1] += "정보 없음";
						}
					} else {
						for (iInstr=0; iInstr<instrumentTrackChannel[itrack][ichannel].size(); iInstr++) {
							if (iInstr!=0) {
								if (instr2name(instrumentTrackChannel[itrack][ichannel][iInstr][1])==instr2name(instrumentTrackChannel[itrack][ichannel][iInstr-1][1])) {
									continue;
								} else {
									tempMessage[tempMessage.size()-1] += " -> ";
									tempMessage[tempMessage.size()-1] += instr2name(instrumentTrackChannel[itrack][ichannel][iInstr][1]);
								}
							} else {
								tempMessage[tempMessage.size()-1] += instr2name(instrumentTrackChannel[itrack][ichannel][iInstr][1]);
							}
							if (iInstr>5) { tempMessage[tempMessage.size()-1] += " -> ..."; break; }
						}
					}
				}
				tempMessage[tempMessage.size()-1] += "$newline$\t\t\t음표 개수 : " + to_string(numNoteTrackChannel[itrack][ichannel]);
				if (sustainTrackChannel[itrack][ichannel].size() > 0) {
					tempMessage[tempMessage.size()-1] += "$newline$\t\t\t서스테인 이벤트 포함";
				}
			}
		}
		if (tempMessage[tempMessage.size()-1].size() > 1500 && itrack != tracks-1) {
			tempMessage[tempMessage.size()-1] += "$newline$```";
			tempMessage.push_back("");
			tempMessage[tempMessage.size()-1] = "```";
		}
	}

	if (iMaxText > 0) {
		tempMessage[tempMessage.size()-1] += "$newline$텍스트 이벤트";
		for (int i=0; i<iMaxText; i++) {
			tempMessage[tempMessage.size()-1] += "$newline$\t";
			double currentTimeInReal = textMat[i].tick;
			if (currentTimeInReal > 3600) {
				tempMessage[tempMessage.size()-1] += to_string((int)(currentTimeInReal/3600)) + "시간 ";
				currentTimeInReal = currentTimeInReal - (int)(currentTimeInReal/3600) * 3600;
			}
			if (currentTimeInReal > 60) {
				tempMessage[tempMessage.size()-1] += to_string((int)(currentTimeInReal/60)) + "분 ";
				currentTimeInReal = currentTimeInReal - (int)(currentTimeInReal/60) * 60;
			}
			tempMessage[tempMessage.size()-1] += to_string((int)(currentTimeInReal)) + "초 : ";
			tempMessage[tempMessage.size()-1] += textMat[i].content;
			if (i > 10) {
				tempMessage[tempMessage.size()-1] += "$newline$이하 생략";
				if (tempMessage.size() > 1500 && i != iMaxText-1) {
					tempMessage[tempMessage.size()-1] += "$newline$```";
					tempMessage.push_back("");
					tempMessage[tempMessage.size()-1] = "```";
				}
				break;
			}
			if (tempMessage.size() > 1500 && i != iMaxText-1) {
				tempMessage[tempMessage.size()-1] += "$newline$```";
				tempMessage.push_back("");
				tempMessage[tempMessage.size()-1] = "```";
			}
		}
	}

	if (iMaxCopyright > 0) {
		tempMessage[tempMessage.size()-1] += "$newline$저작권 정보";
		for (int i=0; i<iMaxCopyright; i++) {
			tempMessage[tempMessage.size()-1] += "$newline$" + copyrightMat[i].content;
		}
	}

	tempMessage[tempMessage.size()-1] += "$newline$```";
	
	for (int iMessage=0; iMessage<tempMessage.size(); iMessage++) {
		append_text_to_vectorstr(&outputSayang, tempMessage[iMessage]);
	}
}