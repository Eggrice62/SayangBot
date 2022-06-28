#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void send_ms2mml_files() {
	vector<string> writtenFileList_final;
	vector<int> writtenCharacterList_final;
	vector<int> writtenChordList_final;
	vector<int> requiredScoreList (3, 0);
	int requiredMeso = 0;
	int sumNotPercurssion = 0;
	
	for (int i=0; i<writtenFileList[0].size(); i++) {
		if (writtenFileList[0][i].size() > writtenFileList[1][i].size()) {
			for (int j=0; j<writtenFileList[1][i].size(); j++) {
				writtenFileList_final.push_back(writtenFileList[1][i][j]);
				writtenCharacterList_final.push_back(writtenCharacterList[1][i][j]);
				writtenChordList_final.push_back(writtenChordList[1][i][j]);
				if (writtenScoreList[1][i][j] == 3000) {
					requiredScoreList[2]++;
					requiredMeso += 20000;
				} else if (writtenScoreList[1][i][j] == 5000) {
					requiredScoreList[1]++;
					requiredMeso += 50000;
				} else {
					requiredScoreList[0]++;
					requiredMeso += 125000;
				}
				if (isNotPercurssion[i]) { sumNotPercurssion++; }
			}
		} else if (writtenFileList[0][i].size() < writtenFileList[1][i].size()) {
			for (int j=0; j<writtenFileList[0][i].size(); j++) {
				writtenFileList_final.push_back(writtenFileList[0][i][j]);
				writtenCharacterList_final.push_back(writtenCharacterList[0][i][j]);
				writtenChordList_final.push_back(writtenChordList[0][i][j]);
				if (writtenScoreList[0][i][j] == 3000) {
					requiredScoreList[2]++;
					requiredMeso += 20000;
				} else if (writtenScoreList[0][i][j] == 5000) {
					requiredScoreList[1]++;
					requiredMeso += 50000;
				} else {
					requiredScoreList[0]++;
					requiredMeso += 125000;
				}
				if (isNotPercurssion[i]) { sumNotPercurssion++; }
			}
		} else {
			if (writtenFileList[0][i].size() == 0) { continue; }
			if (writtenCharacterList[0][i][writtenCharacterList[0][i].size()-1] > writtenCharacterList[1][i][writtenCharacterList[1][i].size()-1]) {
				for (int j=0; j<writtenFileList[1][i].size(); j++) {
					writtenFileList_final.push_back(writtenFileList[1][i][j]);
					writtenCharacterList_final.push_back(writtenCharacterList[1][i][j]);
					writtenChordList_final.push_back(writtenChordList[1][i][j]);
					if (writtenScoreList[1][i][j] == 3000) {
						requiredScoreList[2]++;
						requiredMeso += 20000;
					} else if (writtenScoreList[1][i][j] == 5000) {
						requiredScoreList[1]++;
						requiredMeso += 50000;
					} else {
						requiredScoreList[0]++;
						requiredMeso += 125000;
					}
					if (isNotPercurssion[i]) { sumNotPercurssion++; }
				}
			} else {
				for (int j=0; j<writtenFileList[0][i].size(); j++) {
					writtenFileList_final.push_back(writtenFileList[0][i][j]);
					writtenCharacterList_final.push_back(writtenCharacterList[0][i][j]);
					writtenChordList_final.push_back(writtenChordList[0][i][j]);
					if (writtenScoreList[0][i][j] == 3000) {
						requiredScoreList[2]++;
						requiredMeso += 20000;
					} else if (writtenScoreList[0][i][j] == 5000) {
						requiredScoreList[1]++;
						requiredMeso += 50000;
					} else {
						requiredScoreList[0]++;
						requiredMeso += 125000;
					}
					if (isNotPercurssion[i]) { sumNotPercurssion++; }
				}
			}
		}
	}
	
	if (writtenFileList_final.size() > 10 && sumNotPercurssion <= 10) {
		if (!isEnglish) {
			append_warning_to_vectorstr(&outputSayang, "생성된 합주 악보의 연주자 수가 10명을 초과합니다. 메이플스토리2에서 10명을 초과하는 합주는 2개 파티에 나뉘어 집결한 후 동시에 시작하여 연주한 사례가 있으나, 컴퓨터 2대를 동시에 사용하는 플레이어가 있어야 합니다. 악보 글자수를 줄여 인원수를 줄이려면 $prefix$합주악보 해상도 5 서스테인뭉개기 2 등과 같이 사용해 보세요.");
		} else {
			append_warning_to_vectorstr(&outputSayang, "The number of players in the created ensemble score exceeds 10. In MapleStory 2, there is a case where an ensemble of more than 10 people was divided into two parties and assembled and then started and played at the same time, but there must be a player using two computers. If you want to reduce the number of characters by reducing the number of characters in the score, try using it with $prefix$ensemble resolution 5 mergesustain 2, etc.");
		}
	}
	if (sumNotPercurssion > 10) {
		if (!isEnglish) {
			append_warning_to_vectorstr(&outputSayang, "이 합주 악보는 메이플스토리2에서 정상적으로 연주되지 않을 가능성이 큽니다. 큰북/작은북/심벌즈를 제외한 악기가 10개 이하여야 합니다.");
		} else {
			append_warning_to_vectorstr(&outputSayang, "It is highly likely that this ensemble score will not be played normally in Maple Story 2. There must be no more than 10 instruments except for the bass drum/snare drum/cymbals.");
		}
	}
	
	string outputFileSendName = "";
	if (writtenFileList_final.size() == 0) {
		if (!isEnglish) {
			append_error_to_vectorstr(&outputSayang, "미디 파일에 포함된 음표가 없거나 SayangBot이 읽지 못했습니다...");
		} else {
			append_error_to_vectorstr(&outputSayang, "Midi file contains no notes or SayangBot couldn't read it...");
		}
		return;
	} else if (writtenFileList_final.size() == 1) {
		outputFileSendName = writtenFileList_final[0];
	} else {
		string commandZip = "rm out100.zip 2> /dev/null";
		int resultZip = system(commandZip.c_str());
		for (int i=0; i<writtenFileList_final.size(); i++) {
			commandZip = "zip out100.zip \"" + writtenFileList_final[i] + "\"";
			int resultZip = system(commandZip.c_str());
		}
		outputFileSendName = "out100.zip";
	}
	
	string tempstring = "";
	if (!isMML) {
		tempstring += "mid->ms2mml 변환 완료";
	} else {
		tempstring += "mid->mml 변환 완료";
	}
	if (writtenFileList_final.size() == 1) {
		tempstring += "$newline$독주 악보";
	} else if (writtenFileList_final.size() > 1 && (lOnoffVerticalDivision || command=="합주악보")) {
		tempstring += "$newline$" + to_string(writtenFileList_final.size()) + "인 합주 악보";
		if (requiredScoreList[0] > 0) {
			tempstring += "$newline$\t고급 악보 : " + to_string(requiredScoreList[0]) + "장";
		}
		if (requiredScoreList[1] > 0) {
			tempstring += "$newline$\t중급 악보 : " + to_string(requiredScoreList[1]) + "장";
		}
		if (requiredScoreList[2] > 0) {
			tempstring += "$newline$\t초급 악보 : " + to_string(requiredScoreList[2]) + "장";
		}
		tempstring += "$newline$\t필요한 메소 : " + to_string(requiredMeso);
	} else {
		tempstring += "$newline$독주 악보 - " + to_string(writtenFileList_final.size()) + "장 연속 재생$newline$만약 연속 독주 대신 합주를 원하신다면$newline$ - $prefix$합주악보$newline$ - $prefix$악보 화음분할$newline$등과 같이 사용해 보세요.";
	}
	append_information_to_vectorstr(&outputSayang, tempstring);
	for (int iWritten=0; iWritten<writtenFileList_final.size(); iWritten++) {
		if (iWritten == 0) { tempstring = "$newline$```ini"; }
		if (iWritten != 0) { tempstring += "$newline$"; }
		tempstring += "$newline$[" + writtenFileList_final[iWritten] + "]$newline$\t[글자 수] : " + to_string(writtenCharacterList_final[iWritten]) + "$newline$\t[화음 수] : " + to_string(writtenChordList_final[iWritten]);
		if (iWritten == writtenFileList_final.size() - 1) { tempstring += "$newline$```"; }
	}
	append_text_to_vectorstr(&outputSayang, tempstring);
	append_file_to_vectorstr(&outputSayang, outputFileSendName);
}