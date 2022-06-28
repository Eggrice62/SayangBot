#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "sayangbot_module_opt_ms2mml.cpp"

using namespace std;

void do_ms2mml() {
	if (errorNow) { return; }
	
	if (isAutoEnhance && command == "악보" && iFixedInstrument[0] == -1) {
		double curAvgNote = 0.;
		for (int inote=0; inote<Notes.size(); inote++) {
			if (Notes[inote][1]%16 != 9) {
				curAvgNote += Notes[inote][3];
			}
		}
		curAvgNote /= Notes.size();
		if (curAvgNote < 64) {
			if (!lOnOff_candidate) {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "전반적인 음량이 너무 작아서 자동으로 키워드렸습니다. 이 기능을 원하지 않으시면 $prefix$악보 자동볼륨증가 off 와 같이 사용해 보세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The overall volume was too low, so it was automatically increased. If you do not want this function, try using it with $prefix$solo autovolumeincrease off.");
				}
			}
			for (int inote=0; inote<Notes.size(); inote++) {
				if (Notes[inote][1]%16 != 9) {
					Notes[inote][3] = 127 - (int) ((127-Notes[inote][3])*0.5);
				}
			}
		}
	}
	
	int noteResolution_orig = noteResolution;
	while (tickperquarter % (noteResolution/4) != 0 && noteResolution > 4) {
		noteResolution /= 2;
	}
	if (noteResolution_orig != noteResolution && !lOnOff_candidate) {
		if (!isEnglish) {
			append_warning_to_vectorstr(&outputSayang, "이 미디 파일에서는 " + to_string(noteResolution_orig) + "분음표의 정확한 표현이 불가능합니다. 출력 해상도를 " + to_string(noteResolution) + "분음표로 자동 하향합니다.");
		} else {
			append_warning_to_vectorstr(&outputSayang, "This midi file does not allow accurate representation of " + to_string(noteResolution_orig) +"th notes. Automatically lowers the output resolution to " + to_string(noteResolution) + "th note.");
		}
	}
	
	vector<vector<int>> Notes_before_command = Notes;
	vector<vector<int>> tempomat_before_command = tempomat;
	vector<vector<vector<vector<int>>>> sustainTrackChannel_before_command = sustainTrackChannel;
	vector<string> warningInstrList {};
	
	writtenFileList.push_back(vector<vector<string>> {});
	writtenCharacterList.push_back(vector<vector<int>> {});
	writtenScoreList.push_back(vector<vector<int>> {});
	writtenChordList.push_back(vector<vector<int>> {});
	vector<vector<int>> listInstrItems {};
	vector<int> supportedInstrList;
	vector<vector<int>> autoDaecheInformList;
	
	if (command == "악보") {
		if (iFixedInstrument[0] == -1) {
			for (int inote=0; inote<Notes_before_command.size(); inote++) {
				Notes_before_command[inote][6] = 0;
			}
			for (int iii=0; iii<sustainTrackChannel_before_command.size(); iii++) {
				for (int jjj=0; jjj<sustainTrackChannel_before_command[iii].size(); jjj++) {
					for (int kkk=0; kkk<sustainTrackChannel_before_command[iii][jjj].size(); kkk++) {
						sustainTrackChannel_before_command[iii][jjj][kkk][2] = 0;
					}
				}
			}
			listInstrItems.push_back(vector<int> {0});
		} else {
			if (isAutoDaecheInstrument) {
				supportedInstrList = iFixedInstrument;
				listInstrItems.push_back(supportedInstrList);
				bool isDaeched = false;
				for (int inote=0; inote<Notes_before_command.size(); inote++) {
					if (count(supportedInstrList.begin(), supportedInstrList.end(), Notes[inote][6]) > 0 && supportedInstrList[0] != Notes[inote][6]) {
						isDaeched = true;
						break;
					}
				}
				if (isDaeched && !lOnOff_candidate) {
					if (!isEnglish) {
						tempMessage = "사용자 편의를 위해 악기 ";
					} else {
						tempMessage = "For your convenience, ";
					}
					for (iInstr=1; iInstr<supportedInstrList.size(); iInstr++) {
						if (iInstr != 1) { tempMessage += ", "; }
						tempMessage += instr2name(supportedInstrList[iInstr]);
					}
					if (!isEnglish) {
						tempMessage += "가 메이플스토리2 악기 " + instr2name(supportedInstrList[0]) + " 악보로 통합되어 출력됩니다. 이 기능을 원하지 않으시면 '대체악기사용안함' 옵션을 사용해 주세요.";
					} else {
						tempMessage += " will be automatically integrated into " + instr2name(supportedInstrList[0]) + " If you do not want this feature, please use the 'disablesubstitution' option.";
					}
					append_warning_to_vectorstr(&outputSayang, tempMessage);
				}
			} else {
				supportedInstrList = iFixedInstrument;
				listInstrItems.push_back(vector<int> {supportedInstrList[0]});
			}
		}
	} else if (command == "합주악보") {
		vector<int> tempListInstr;
		for (int inote=0; inote<Notes_before_command.size(); inote++) {
			int preInvestInstr = Notes_before_command[inote][6];
			if (Notes_before_command[inote][1]%16 == 9) { preInvestInstr = 128; }
			if (count(tempListInstr.begin(), tempListInstr.end(), preInvestInstr) == 0) {
				tempListInstr.push_back(preInvestInstr);
			}
		}
		stable_sort(tempListInstr.begin(), tempListInstr.end());
		for (int iInstr=0; iInstr<tempListInstr.size(); iInstr++) {
			int tempCurrentInstr = tempListInstr[iInstr];
			bool isMekong = false;
			vector<string> listInstrKeys = instrumentNameList;
			if (isAutoDaecheInstrument) {
				for (int iInstrInv=0; iInstrInv<listInstrKeys.size(); iInstrInv++) {
					supportedInstrList = instrumentName2num(listInstrKeys[iInstrInv]);
					if (count(supportedInstrList.begin(), supportedInstrList.end(), tempCurrentInstr)) {
						isMekong = true;
						if (count(listInstrItems.begin(), listInstrItems.end(), supportedInstrList) == 0) {
							listInstrItems.push_back(supportedInstrList);
						}
						if (supportedInstrList[0] != tempCurrentInstr) {
							if (count(autoDaecheInformList.begin(), autoDaecheInformList.end(), supportedInstrList) == 0) {
								autoDaecheInformList.push_back(supportedInstrList);
							}
						}
						break;
					}
				}
			} else {
				for (int iInstrInv=0; iInstrInv<listInstrKeys.size(); iInstrInv++) {
					supportedInstrList = instrumentName2num(listInstrKeys[iInstrInv]);
					if (supportedInstrList[0] == tempCurrentInstr) {
						isMekong = true;
						if (count(listInstrItems.begin(), listInstrItems.end(), vector<int> {supportedInstrList[0]}) == 0) {
							listInstrItems.push_back(vector<int> {supportedInstrList[0]});
						}
						break;
					}
				}
			}
			if (!isMekong) {
				if (tempCurrentInstr == 128) {
					bool isBasic = true;
					for (int inote=0; inote<Notes_before_command.size(); inote++) {
						if (Notes_before_command[inote][6] == 128) {
							if (Notes_before_command[inote][2] == 35) { continue; }
							if (Notes_before_command[inote][2] == 36) { continue; }
							if (Notes_before_command[inote][2] == 38) { continue; }
							if (Notes_before_command[inote][2] == 40) { continue; }
							if (Notes_before_command[inote][2] == 49) { continue; }
							if (Notes_before_command[inote][2] == 57) { continue; }
							isBasic = false;
						}
					}
					if (!isBasic && !lOnOff_candidate) {
						if (!isEnglish) {
							append_warning_to_vectorstr(&outputSayang, "이 미디 파일에는 큰북/작은북/심벌즈와 더불어 메이플스토리2에 없는 타악기 소리도 포함되어 있습니다. 합주가 조금 허전해질 수 있습니다. 메이플스토리2에서 타악기 전체를 사용하는 방법에 대해서는 '사양'님에게 문의하십시오.");
						} else {
							append_warning_to_vectorstr(&outputSayang, "This MIDI file contains kick drum / snare drum / cymbal as well as percussion sounds not found in MapleStory 2. The ensemble can be a little empty. For information on how to use the entire percussion instrument in MapleStory 2, please contact '사양'.");
						}
					}
					listInstrItems.push_back(vector<int> {129});
					listInstrItems.push_back(vector<int> {130});
					listInstrItems.push_back(vector<int> {131});
				} else {
					listInstrItems.push_back(vector<int> {tempCurrentInstr});
					warningInstrList.push_back(instr2name(tempCurrentInstr));
				}
			}
		}
		if (autoDaecheInformList.size() > 0 && !lOnOff_candidate) {
			if (!isEnglish) {
				tempMessage = "사용자 편의를 위해 다음과 같이 통합된 악보가 출력됩니다. ";
			} else {
				tempMessage = "For user convenience, the integrated sheet music is output as follows. ";
			}
			for (int iDaeche=0; iDaeche<autoDaecheInformList.size(); iDaeche++) {
				tempMessage += " " + instr2name(autoDaecheInformList[iDaeche][0]) + " (";
				for (iInstr=1; iInstr<autoDaecheInformList[iDaeche].size(); iInstr++) {
					tempMessage += "+"+instr2name(autoDaecheInformList[iDaeche][iInstr]);
					if (iInstr == autoDaecheInformList[iDaeche].size()-1) { tempMessage += ")"; }
				}
			}
			if (!isEnglish) {
				tempMessage += " 이 기능을 원하지 않으시면 '대체악기사용안함' 옵션을 사용해 주세요.";
			} else {
				tempMessage += " If you do not want this feature, please use the 'disablesubstitution' option.";
			}
			append_warning_to_vectorstr(&outputSayang, tempMessage);
		}
	}
	if (warningInstrList.size() > 0 && !lOnOff_candidate) {
		if (!isEnglish) {
			tempMessage = "미디 파일에서 사용하는 악기 중 ";
		} else {
			tempMessage = "Among the instruments used in MIDI file, ";
		}
		for (int iWarningInstr=0; iWarningInstr<warningInstrList.size(); iWarningInstr++) {
			if (iWarningInstr!=0) { tempMessage += ", "; }
			tempMessage += warningInstrList[iWarningInstr];
		}
		if (!isEnglish) {
			tempMessage += "는 메이플스토리2에 없습니다. 대체할만한 악기를 잘 찾아보세요!";
		} else {
			tempMessage += " are not in MapleStory 2. Try to find a good replacement!";
		}
		append_warning_to_vectorstr(&outputSayang, tempMessage);
	}
	
	ofstream outputMidiListInstrItems("11listInstrItems.sayang");
	for (int ittemp=0; ittemp<listInstrItems.size(); ittemp++) {
		outputMidiListInstrItems << ittemp << " - ";
		for (int jttemp=0; jttemp<listInstrItems[ittemp].size(); jttemp++) {
			if (jttemp != 0) { outputMidiListInstrItems << "/"; }
			outputMidiListInstrItems << listInstrItems[ittemp][jttemp];
		}
		outputMidiListInstrItems << '\n';
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	ofstream outputMidiStrings("20strings.sayang");
	
	for (int iInstrMain=0; iInstrMain<listInstrItems.size(); iInstrMain++) {
		writtenFileList[writtenFileList.size()-1].push_back(vector<string> {});
		writtenCharacterList[writtenCharacterList.size()-1].push_back(vector<int> {});
		writtenScoreList[writtenScoreList.size()-1].push_back(vector<int> {});
		writtenChordList[writtenChordList.size()-1].push_back(vector<int> {});
		isNotPercurssion.push_back(listInstrItems[iInstrMain][0] != 129 && listInstrItems[iInstrMain][0] != 130 && listInstrItems[iInstrMain][0] != 131);
		
		supportedInstrList = listInstrItems[iInstrMain];
		tempomat = tempomat_before_command;
		sustainTrackChannel = sustainTrackChannel_before_command;
		Notes = vector<vector<int>> {};
		
		for (int inote=0; inote<Notes_before_command.size(); inote++) {
			bool percCheck = false;
			if (count(supportedInstrList.begin(), supportedInstrList.end(), Notes_before_command[inote][6]) > 0) {
				percCheck = true;
			} else if (supportedInstrList[0]>128) {
				if (Notes_before_command[inote][6] == 128 && supportedInstrList[0] == 129 && Notes_before_command[inote][2] == 57) { percCheck = true; }
				if (Notes_before_command[inote][6] == 128 && supportedInstrList[0] == 129 && Notes_before_command[inote][2] == 49) { percCheck = true; }
				if (Notes_before_command[inote][6] == 128 && supportedInstrList[0] == 130 && Notes_before_command[inote][2] == 35) { percCheck = true; }
				if (Notes_before_command[inote][6] == 128 && supportedInstrList[0] == 130 && Notes_before_command[inote][2] == 36) { percCheck = true; }
				if (Notes_before_command[inote][6] == 128 && supportedInstrList[0] == 131 && Notes_before_command[inote][2] == 38) { percCheck = true; }
				if (Notes_before_command[inote][6] == 128 && supportedInstrList[0] == 131 && Notes_before_command[inote][2] == 40) { percCheck = true; }
			}
			if (percCheck) { Notes.push_back(Notes_before_command[inote]); }
		}
		
		if (Notes.size() == 0) {
			if (command == "악보") {
				if (iFixedInstrument[0] == -1) {
					if (!isEnglish) {
						append_error_to_vectorstr(&outputSayang, "미디 파일에 포함된 음표가 없거나 SayangBot이 읽지 못했습니다...");
					} else {
						append_error_to_vectorstr(&outputSayang, "Either the MIDI file contains no notes or SayangBot couldn't read it...");
					}
					errorNow = true;
					return;
				} else {
					vector<string> instrumentKeyList = instrumentNameList;
					for (int iInstrInv=0; iInstrInv<instrumentKeyList.size(); iInstrInv++) {
						if (instrumentName2num(instrumentKeyList[iInstrInv]) == supportedInstrList) {
							if (!isEnglish) {
								append_warning_to_vectorstr(&outputSayang, "이 미디 파일에는 입력한 악기 (" + instrumentKeyList[iInstrInv] + ") 가 안 나오는 것 같습니다. $prefix$정보 명령어로 악기 정보를 확인해 보세요. 저는 퇴근합니다.");
							} else {
								append_warning_to_vectorstr(&outputSayang, "The instrument (" + instrumentKeyList[iInstrInv] + ") doesn't seem to appear in this MIDI file. Check the instrument information with the $prefix$info command.");
							}
							errorNow = true;
							return;
						}
					}
					if (!isEnglish) {
						append_warning_to_vectorstr(&outputSayang, "이 미디 파일에는 입력한 악기 (" + instr2name(supportedInstrList[0]) + ") 가 안 나오는 것 같습니다. $prefix$정보 명령어로 악기 정보를 확인해 보세요. 저는 퇴근합니다.");
					} else {
						append_warning_to_vectorstr(&outputSayang, "The instrument (" + instr2name(supportedInstrList[0]) + ") doesn't seem to appear in this MIDI file. Check the instrument information with the $prefix$info command.");
					}
					errorNow = true;
					return;
				}
			} else if (command == "합주악보") {
				continue;
			}
		}
		
		int iCurrentWriteHorizontal = 0;
		int iCurrentWriteVertical = 0;
		bool isFirstChord = true;
		if (lOnoffHorizontalDivision) {
			icuttable = vector<int> (3, -1);
			int tempCutIndex = 0;
			while (cuttableMat[tempCutIndex] < timeStartNote) {
				tempCutIndex++;
			}
			icuttable[0] = tempCutIndex;
			icuttable[1] = -1;
			tempCutIndex = cuttableMat.size()-1;
			while (cuttableMat[tempCutIndex] > timeEndNote) {
				tempCutIndex--;
			}
			icuttable[2] = tempCutIndex;
			Notes_orig = Notes;
			tempomat_orig_orig = tempomat;
			sustainTrackChannel_orig = sustainTrackChannel;
		}
		bool lHorizontalDivisionFinished = false;
		int iHorizontalTryCount = 0;
		vector<int> cuttableMat_orig = cuttableMat;
		int tickperquarter_orig = tickperquarter;
		int tempTimeStartNote = 0;
		int tempTimeEndNote = 2147483647;
		int tempCuttableIndex = -1;
		while (!lHorizontalDivisionFinished) {
			tickperquarter = tickperquarter_orig;
			cuttableMat = cuttableMat_orig;
			if (!lOnoffHorizontalDivision) {
				tempTimeStartNote = timeStartNote;
				tempTimeEndNote = timeEndNote;
			} else {
				Notes = Notes_orig;
				tempomat = tempomat_orig_orig;
				sustainTrackChannel = sustainTrackChannel_orig;
				if (icuttable[1] < 0 && icuttable[2] == cuttableMat.size()-1) {
					tempTimeStartNote = cuttableMat[icuttable[0]];
					tempTimeEndNote = cuttableMat[icuttable[2]];
				} else {
					tempCuttableIndex = (int)((icuttable[1]+icuttable[2])/2);
					tempTimeStartNote = cuttableMat[icuttable[0]];
					tempTimeEndNote = cuttableMat[tempCuttableIndex];
				}
			}
			
			if (!isTriplet) {
				for (int iCutCut=0; iCutCut<cuttableMat.size(); iCutCut++) {
					cuttableMat[iCutCut] = (int)(0.5+(cuttableMat[iCutCut]-tempTimeStartNote)/(4.*tickperquarter/noteResolution)) * (4.*tickperquarter/noteResolution) + tempTimeStartNote;
				}
			} else if (isTriplet) {
				for (int iCutCut=0; iCutCut<cuttableMat.size(); iCutCut++) {
					cuttableMat[iCutCut] = (int)(0.5+(cuttableMat[iCutCut]-tempTimeStartNote)/(4.*tickperquarter*2/3/noteResolution)) * (4.*tickperquarter*2/3/noteResolution) + tempTimeStartNote;
				}
			}
			
			if (!isTriplet) {
				for (int itempo=0; itempo<tempomat.size(); itempo++) {
					tempomat[itempo][0] = (tempoResolution*4.*tickperquarter/noteResolution)*(int)(0.5+(tempomat[itempo][0]-tempTimeStartNote)/(tempoResolution*4.*tickperquarter/noteResolution)) + tempTimeStartNote;
					if (itempo!=0) {
						if (tempomat[itempo-1][1] > 255) {
							tempomat[itempo][0] = (2*tempoResolution*4.*tickperquarter/noteResolution)*(int)(0.5+(tempomat[itempo][0]-tempTimeStartNote)/(2.*tempoResolution*4*tickperquarter/noteResolution)) + tempTimeStartNote;
						}
					}
				}
			} else if (isTriplet) {
				for (int itempo=0; itempo<tempomat.size(); itempo++) {
					tempomat[itempo][0] = (tempoResolution*4.*tickperquarter*2/3/noteResolution)*(int)(0.5+(tempomat[itempo][0]-tempTimeStartNote)/(tempoResolution*4.*tickperquarter*2/3/noteResolution)) + tempTimeStartNote;
					if (itempo!=0) {
						if (tempomat[itempo-1][1] > 255) {
							tempomat[itempo][0] = (2*tempoResolution*4.*tickperquarter*2/3/noteResolution)*(int)(0.5+(tempomat[itempo][0]-tempTimeStartNote)/(2.*tempoResolution*4*tickperquarter*2/3/noteResolution)) + tempTimeStartNote;
						}
					}
				}
			}
			stable_sort(tempomat.begin(), tempomat.end(), sortby1stcol);
			bool isRemoved = true;
			while (isRemoved) {
				isRemoved = false;
				for (int i=0; i<tempomat.size(); i++) {
					if (tempomat[i][0] < tempTimeStartNote) {
						isRemoved = true;
						tempomat[i][0] = tempTimeStartNote;
						break;
					}
					if (i != tempomat.size()-1) {
						if (tempomat[i][0] == tempomat[i+1][0]) {
							isRemoved = true;
						}
					}
					if (tempomat[i][0] < tempTimeStartNote) { isRemoved = true; }
					if (tempomat[i][0] > tempTimeEndNote) { isRemoved = true; }
					if (tempomat[i][0] > maxLengthTrack) { isRemoved = true; }
					if (isRemoved) {
						vector<vector<int>> newtempomat {};
						for (int i2=0; i2<tempomat.size(); i2++) {
							if (i2!=i) { newtempomat.push_back(tempomat[i2]); }
						}
						tempomat = newtempomat;
						break;
					}
				}
			}
			tempomat.push_back(vector<int> {2147483647, 255});
			tempomat_orig = tempomat;
			// console.log(tempomat);
			ofstream outputMidiTempo("15tempomat.sayang");
			for (int iTempo=0; iTempo<tempomat.size(); iTempo++) {
				outputMidiTempo << tempomat[iTempo][0] << " " << tempomat[iTempo][1] << '\n';
			}
			
			susmat = vector<vector<int>> {};
			if (iOnoffSustain) {
				for (itrack=0; itrack<tracks; itrack++) {
					for (ichannel=0; ichannel<iMaxChannel; ichannel++) {
						if (!listTrackChannel[itrack][ichannel]) { continue; }
						for (int isus=0; isus<sustainTrackChannel[itrack][ichannel].size(); isus++) {
							if (!isTriplet) {
								sustainTrackChannel[itrack][ichannel][isus][0] = (susResolution*4.*tickperquarter/noteResolution)*(int)(0.5+(sustainTrackChannel[itrack][ichannel][isus][0]-tempTimeStartNote)/(susResolution*4.*tickperquarter/noteResolution)) + tempTimeStartNote;
							} else {
								sustainTrackChannel[itrack][ichannel][isus][0] = (susResolution*4.*tickperquarter*2/3/noteResolution)*(int)(0.5+(sustainTrackChannel[itrack][ichannel][isus][0]-tempTimeStartNote)/(susResolution*4.*tickperquarter*2/3/noteResolution)) + tempTimeStartNote;
							}
							if (sustainTrackChannel[itrack][ichannel][isus][0] < tempTimeStartNote) { continue; }
							if (sustainTrackChannel[itrack][ichannel][isus][0] > tempTimeEndNote) { continue; }
							if (sustainTrackChannel[itrack][ichannel][isus][0] > maxLengthTrack) { continue; }
							if (sustainTrackChannel[itrack][ichannel][isus][0] < timeStartNote) { continue; }
							if (sustainTrackChannel[itrack][ichannel][isus][0] > timeEndNote) { continue; }
							if (count(supportedInstrList.begin(), supportedInstrList.end(), sustainTrackChannel[itrack][ichannel][isus][2]) == 0) { continue; }
							susmat.push_back(sustainTrackChannel[itrack][ichannel][isus]);
						}
					}
				}
			}
			ofstream outputMidiSustain1("16_1susmat.sayang");
			for (int iTempo=0; iTempo<susmat.size(); iTempo++) {
				outputMidiSustain1 << susmat[iTempo][0] << " " << susmat[iTempo][1] << '\n';
			}
			stable_sort(susmat.begin(), susmat.end(), sortby1stcol);
			isRemoved = true;
			while (isRemoved) {
				isRemoved = false;
				for (int i=0; i+1<susmat.size(); i++) {
					if (susmat[i][0] == susmat[i+1][0] && (susmat[i][1]-63.5)*(susmat[i+1][1]-63.5)>0 && susmat[i][2] == susmat[i+1][2]) {
						isRemoved = true;
						vector<vector<int>> newsusmat {};
						for (int i2=0; i2<susmat.size(); i2++) {
							if (i2!=i) { newsusmat.push_back(susmat[i2]); }
						}
						susmat = newsusmat;
						break;
					}
				}
			}
			// susmat.push_back(vector<int> {2147483647, 0, 0});
			susmat_orig = susmat;
			// console.log(susmat);
			ofstream outputMidiSustain("16susmat.sayang");
			for (int iTempo=0; iTempo<susmat.size(); iTempo++) {
				outputMidiSustain << susmat[iTempo][0] << " " << susmat[iTempo][1] << '\n';
			}
			outputMidiSustain.close();
			
			// console.log(Notes.size());
			// console.log(Notes);
			if (maxCutLength>0) {
				vector<int> cutOverlap (Notes.size(), 0);
				for (int inote=0; inote<Notes.size(); inote++) {
					for (int jnote=0; jnote<Notes.size(); jnote++) {
						if ((Notes[jnote][4] >= Notes[inote][5]-(Notes[inote][5]-Notes[inote][4])*maxCutLength) && (Notes[jnote][4] < Notes[inote][5])) {
							double tempValue = (Notes[inote][5]-Notes[jnote][4])/(Notes[inote][5]-Notes[inote][4]);
							if (Notes[inote][5]-Notes[inote][4] == 0) { tempValue = 0; }
							cutOverlap[inote] = (cutOverlap[inote]>tempValue)?cutOverlap[inote]:tempValue;
						}
					}
					cutOverlap[inote] = (cutOverlap[inote]<maxCutLength)?cutOverlap[inote]:maxCutLength;
				}
				for (int inote=0; inote<Notes.size(); inote++) {
					Notes[inote][5] = Notes[inote][4] + (Notes[inote][5]-Notes[inote][4])*(1-cutOverlap[inote]);
				}
			}
			vector<vector<int>> Notes3 {};
			bool isThereDrum = false;
			bool isThereOther = false;
			for (int inote=0; inote<Notes.size(); inote++) {
				if (!listTrackChannel[Notes[inote][0]][Notes[inote][1]]) { continue; }
				if (Notes[inote][2] < pitchLowerBound) { continue; }
				if (Notes[inote][2] > pitchUpperBound) { continue; }
				if (Notes[inote][4] < tempTimeStartNote) { continue; }
				if (Notes[inote][5] > tempTimeEndNote) { continue; }
				if (Notes[inote][4] < timeStartNote) { continue; }
				if (Notes[inote][5] > timeEndNote) { continue; }
				Notes3.push_back(Notes[inote]);
				if (Notes[inote][1]%16 == 9) { isThereDrum = true; }
				if (Notes[inote][1]%16 != 9) { isThereOther = true; }
			}
			Notes = Notes3;
			if (iOnoffDrum == 0 && isThereDrum && isThereOther && !isDrumWarned) {
				append_warning_to_vectorstr(&outputSayang, "드럼 코드와 일반 악기 코드를 같은 악보에 작성하게 됩니다. 의도하지 않은 소리가 날 수 있습니다. 드럼 코드를 제외하고 일반 악기 코드만 작성하려면 드럼제외 옵션을 사용하세요.");
				isDrumWarned = true;
			}
			if (!isTriplet) {
				for (int inote=0; inote<Notes.size(); inote++) {
					Notes[inote][4] = (int)(0.5+(Notes[inote][4]-tempTimeStartNote)/(4.*tickperquarter/noteResolution)) * (4*tickperquarter/noteResolution) + tempTimeStartNote;
					Notes[inote][5] = (int)(0.5+(Notes[inote][5]-tempTimeStartNote)/(4.*tickperquarter/noteResolution)) * (4*tickperquarter/noteResolution) + tempTimeStartNote;
				}
			} else if (isTriplet) {
				for (int inote=0; inote<Notes.size(); inote++) {
					Notes[inote][4] = (int)(0.5+(Notes[inote][4]-tempTimeStartNote)/(4.*tickperquarter*2/3/noteResolution)) * (4*tickperquarter*2/3/noteResolution) + tempTimeStartNote;
					Notes[inote][5] = (int)(0.5+(Notes[inote][5]-tempTimeStartNote)/(4.*tickperquarter*2/3/noteResolution)) * (4*tickperquarter*2/3/noteResolution) + tempTimeStartNote;
				}
			}
			if (iMethodDup == 1) {
				stable_sort(Notes.begin(), Notes.end(), sortFunctionByStarttime);
				vector<vector<int>> Notes3 {};
				vector<int> deathnote (Notes.size(), 0);
				for (int inote=0; inote<Notes.size(); inote++) {
					for (int jnote=inote+1; jnote<Notes.size(); jnote++) {
						if (Notes[jnote][2] == Notes[inote][2] && Notes[inote][4] == Notes[jnote][4]) {
							deathnote[jnote] = 1;
						}
					}
				}
				for (int inote=0; inote<Notes.size(); inote++) {
					if (deathnote[inote] == 0) {
						Notes3.push_back(Notes[inote]);
					}
				}
				Notes = Notes3;
			} else if (iMethodDup == 2) {
				if (!isEnglish) {
					// append_warning_to_vectorstr(&outputSayang, "중복제거 2 옵션을 사용하면 SayangBot이 몇 분간 응답하지 않을 수 있습니다. 문제가 발생한 경우 개발자에게 문의해 주세요.");
				} else {
					// append_warning_to_vectorstr(&outputSayang, "The option may cause SayangBot to become unresponsive for several minutes. If you run into any problems, please contact the developer.");
				}
				int initialLength = Notes.size();
				isRemoved = true;
				while (isRemoved) {
					isRemoved = false;
					cout << "중복 음표 제거 중 : " + to_string((initialLength - Notes.size())) + "/" + to_string(initialLength) << '\n';
					for (int inote=0; inote<Notes.size(); inote++) {
						for (int jnote=0; jnote<Notes.size(); jnote++) {
							if (inote==jnote) { continue; }
							if (Notes[jnote][4] >= Notes[inote][5]) { continue; }
							if (Notes[inote][4] >= Notes[jnote][5]) { continue; }
							if (Notes[jnote][2] != Notes[inote][2]) { continue; }
							if (Notes[inote][4] < Notes[jnote][4]) {
								if (Notes[inote][5] < Notes[jnote][5]) {
									if (Notes[inote][3] <= Notes[jnote][3]) {
										Notes[inote][5] = Notes[jnote][4];
									} else if (Notes[inote][3] > Notes[jnote][3]) {
										Notes[jnote][4] = Notes[inote][5];
									}
								} else if (Notes[inote][5] == Notes[jnote][5]) {
									if (Notes[inote][3] <= Notes[jnote][3]) {
										Notes[inote][5] = Notes[jnote][4];
									} else if (Notes[inote][3] > Notes[jnote][3]) {
										Notes.erase(Notes.begin()+jnote);
									}
								} else if (Notes[inote][5] > Notes[jnote][5]) {
									if (Notes[inote][3] <= Notes[jnote][3]) {
										Notes[inote][5] = Notes[jnote][4];
									} else if (Notes[inote][3] > Notes[jnote][3]) {
										Notes.erase(Notes.begin()+jnote);
									}
								}
							} else if (Notes[inote][4] == Notes[jnote][4]) {
								if (Notes[inote][5] < Notes[jnote][5]) {
									if (Notes[inote][3] <= Notes[jnote][3]) {
										Notes.erase(Notes.begin()+inote);
									} else if (Notes[inote][3] > Notes[jnote][3]) {
										Notes[jnote][4] = Notes[inote][5];
									}
								} else if (Notes[inote][5] == Notes[jnote][5]) {
									if (Notes[inote][3] <= Notes[jnote][3]) {
										Notes.erase(Notes.begin()+inote);
									} else if (Notes[inote][3] > Notes[jnote][3]) {
										Notes.erase(Notes.begin()+jnote);
									}
								} else if (Notes[inote][5] > Notes[jnote][5]) {
									if (Notes[inote][3] <= Notes[jnote][3]) {
										Notes[inote][4] = Notes[jnote][5];
									} else if (Notes[inote][3] > Notes[jnote][3]) {
										Notes.erase(Notes.begin()+jnote);
									}
								}
							} else if (Notes[inote][4] > Notes[jnote][4]) {
								if (Notes[inote][5] < Notes[jnote][5]) {
									if (Notes[inote][3] <= Notes[jnote][3]) {
										Notes.erase(Notes.begin()+inote);
									} else if (Notes[inote][3] > Notes[jnote][3]) {
										Notes[jnote][5] = Notes[inote][4];
									}
								} else if (Notes[inote][5] == Notes[jnote][5]) {
									if (Notes[inote][3] <= Notes[jnote][3]) {
										Notes.erase(Notes.begin()+inote);
									} else if (Notes[inote][3] > Notes[jnote][3]) {
										Notes[jnote][5] = Notes[inote][4];
									}
								} else if (Notes[inote][5] > Notes[jnote][5]) {
									if (Notes[inote][3] <= Notes[jnote][3]) {
										Notes[inote][4] = Notes[jnote][5];
									} else if (Notes[inote][3] > Notes[jnote][3]) {
										Notes[jnote][5] = Notes[inote][4];
									}
								}
							}
							isRemoved = true;
							break;
						}
						if (isRemoved) { break; }
					}
				}
			}
			stable_sort(Notes.begin(), Notes.end(), sortFunctionByStarttime);
			// console.log(Notes.size());
			// console.log(Notes);
			
			int currentTime = tempTimeStartNote;
			
			int curoct = 4;
			int curlen = 4;
			int curvol = 8;
			bool currest = true;
			int curchord = -1;
			
			outputtxt = "";
			outputtxtRollback = outputtxt;
			int measureLength = outputtxt.size();
	
			bool lfinish = false;
			bool isFirstChannel = true;
			bool isplaying = false;
			vector<int> iCountCharacter {};
			vector<string> cCharacter {};
			int iCountCharacterPseudoSustain {};
			string cCharacterPseudoSustain {};
			bool istempsusinter = false;
			bool iscontinuednote = false;
			
			susmat = susmat_orig;
			if (susmat.size() >= 2) {
				susmat[susmat.size()-1][0] = susmat[susmat.size()-2][0];
			} else if (susmat.size() >= 1) {
				susmat[susmat.size()-1][0] = maxLengthTrack;
			} else {
				// pass;
			}
			
			while (!lfinish) {
				vector<vector<int>> Notes2 {};
				vector<vector<int>> susmat2 {};
				tempomat = tempomat_orig;
				
				while (Notes.size() + susmat.size() > 0) {
					bool isSustainComing = false;
					
					if (lOnOff_candidate && curchord >= 0) {
						if (Notes.size() > 0) {
							int numCandidate = (Notes.size()>=20)?20:Notes.size();
							vector<vector<int>> listCandidateNote;
							int minCandiStart = 2147483647;
							int minCandiStartEnd = 2147483647;
							for (int i=0; i<numCandidate; i++) {
								vector<int> curnote = Notes[i];
								if (curnote[4] < minCandiStart) {
									minCandiStart = curnote[4];
								}
							}
							for (int i=0; i<numCandidate; i++) {
								vector<int> curnote = Notes[i];
								if (curnote[4] != minCandiStart) { continue; }
								if (curnote[5] < minCandiStartEnd) {
									minCandiStartEnd = curnote[5];
								}
							}
							for (int i=0; i<numCandidate; i++) {
								vector<int> curnote = Notes[i];
								if (curnote[5] > minCandiStartEnd) { continue; }
								listCandidateNote.push_back(curnote);
							}
							vector<int> scoremat;
							for (int i=0; i<listCandidateNote.size(); i++) {
								vector<int> curnote = listCandidateNote[i];
								int notenumber = curnote[2];
								int volnumber = curnote[3];
								int thisoct = ((notenumber-12)/12);
								int thisvol = (int)(((volnumber+volumeAdd1*8.)*volumeMul2+volumeAdd3*8.)/8.);
								vector<int> tempnotelist = get_length_list2_nonexpand(curnote[5]-curnote[4],tickperquarter,noteResolution);
								int thislen = -1;
								if (tempnotelist.size() > 0) {
									thislen = tempnotelist[0];
								}
								int tempscore = 0;
								if (thisoct == curoct) { tempscore++; }
								if (thisvol == curvol) { tempscore++; }
								if (thislen == curlen) { tempscore++; }
								scoremat.push_back(tempscore);
							}
							int bestscore = -1;
							int bestloc = 999;
							for (int i=listCandidateNote.size()-1; i>=0; i--) {
								if (scoremat[i] >= bestscore) {
									bestscore = scoremat[i];
									bestloc = i;
								}
							}
							if (bestloc != 0) {
								iter_swap(Notes.begin(), Notes.begin() + bestloc);
							}
						}
					}
					
					int starttime = 2147483647;
					int endtime = 2147483647;
					
					vector<int> curnote (7, -1);
					if (Notes.size() > 0) {
						curnote = Notes[0];
						starttime = curnote[4];
						endtime = curnote[5];
					}
					
					if (curchord == -1) {
						if (susmat.size() > 0) {
						// if (susmat[0][0] <= starttime) {
							isSustainComing = true;
							starttime = susmat[0][0];
							endtime = susmat[0][0];
						} else {
							while (Notes.size()>0) {
								Notes2.push_back(Notes[0]);
								Notes.erase(Notes.begin());
							}
							break;
						}
					}
					
					if (outputtxt.length()>lengthHorizontalDivision) {
						vector<string> tempOptVec = opt_ms2mml(outputtxt);
						outputtxt = tempOptVec[0];
						stringstream ssInt(tempOptVec[1]);
						ssInt >> curlen;
						if (outputtxt.length()>lengthHorizontalDivision) {
							if (Notes2.size() == 0) {
								if (!isEnglish) {
									append_error_to_vectorstr(&outputSayang, "이 미디 파일에는 템포 조정 이벤트가 너무 많아서 1만자 악보에 맞게 출력할 수 없습니다. $prefix$악보 템포뭉개기 2 서스테인뭉개기 2 등의 옵션으로 개수를 줄여 보세요. 실행을 종료합니다.");
								} else {
									append_error_to_vectorstr(&outputSayang, "There are too many tempo events in this MIDI file, so it can't be output to fit 10,000-character sheet music. Try reducing the number with options such as $prefix$solo mergetempo 2 mergesustain 2, etc. Terminate execution.");
								}
								errorNow = true;
								return;
							}
							if (curchord == -1 && susmat.size() > 0) {
								if (!isEnglish) {
									append_error_to_vectorstr(&outputSayang, "이 미디 파일에는 서스테인 이벤트가 너무 많아서 1만자 악보에 맞게 출력할 수 없습니다. $prefix$악보 템포뭉개기 2 서스테인뭉개기 2 등의 옵션으로 개수를 줄여 보세요. 실행을 종료합니다.");
								} else {
									append_error_to_vectorstr(&outputSayang, "There are too many sustain events in this MIDI file, so it can't be output to fit 10,000-character sheet music. Try reducing the number with options such as $prefix$solo mergetempo 2 mergesustain 2, etc. Terminate execution.");
								}
								errorNow = true;
								return;
							}
							outputtxt = outputtxtRollback;
							while (Notes.size()>0) {
								Notes2.push_back(Notes[0]);
								Notes.erase(Notes.begin());
							}
							break;
						}
					}
					outputtxtRollback = outputtxt;
					
					if (currentTime - starttime > 0) {
						if (!isSustainComing) {
							Notes2.push_back(curnote);
							Notes.erase(Notes.begin());
						} else {
							susmat2.push_back(susmat[0]);
							susmat.erase(susmat.begin());
						}
						continue;
					}
					
					bool isstart = false;
					istempsusinter = false;
			
					if (starttime >= tempomat[0][0]) {
						starttime = tempomat[0][0];
						isstart = true;
					}
					if (endtime > tempomat[0][0]) {
						endtime = tempomat[0][0];
						istempsusinter = true;
					}
					
					if (starttime - currentTime > 0) {
						vector<int> restlist = get_length_list2_nonexpand(starttime-currentTime,tickperquarter,noteResolution);
						bool lintdotted = false;
						for (int iListInd=0; iListInd<restlist.size(); iListInd++) {
							if (lintdotted) {
								lintdotted = false;
								continue;
							}
							int i = restlist[iListInd];
							if (i != curlen) {
								outputtxt += "l" + to_string(i);
								curlen = i;
							}
							if (iListInd == restlist.size()-2) {
								if (restlist[iListInd+1]==i*2) {
									lintdotted = true;
								}
							}
							if (iOnoffMotion) {
								if (!currest && isFirstChannel && (starttime-currentTime)>tickperquarter*cooldownMotionInQuarter && isplaying) {
									outputtxt += "m0";
									isplaying = false;
								}
							}
							currest = true;
							outputtxt += "r";
							currentTime = currentTime + notetotime2(i,tickperquarter);
							if (lintdotted) {
								outputtxt += ".";
								currentTime = currentTime + notetotime2(i,tickperquarter)/2;
							}
						}
					}
					if (outputtxt.length()>lengthHorizontalDivision) {
						vector<string> tempOptVec = opt_ms2mml(outputtxt);
						outputtxt = tempOptVec[0];
						stringstream ssInt(tempOptVec[1]);
						ssInt >> curlen;
						if (outputtxt.length()>lengthHorizontalDivision) {
							if (Notes2.size() == 0) {
								if (!isEnglish) {
									append_error_to_vectorstr(&outputSayang, "이 미디 파일에는 템포 조정 이벤트가 너무 많아서 1만자 악보에 맞게 출력할 수 없습니다. $prefix$악보 템포뭉개기 2 서스테인뭉개기 2 등의 옵션으로 개수를 줄여 보세요. 실행을 종료합니다.");
								} else {
									append_error_to_vectorstr(&outputSayang, "There are too many tempo events in this MIDI file, so it can't be output to fit 10,000-character sheet music. Try reducing the number with options such as $prefix$solo mergetempo 2 mergesustain 2, etc. Terminate execution.");
								}
								errorNow = true;
								return;
							}
							if (curchord == -1 && susmat.size() > 0) {
								if (!isEnglish) {
									append_error_to_vectorstr(&outputSayang, "이 미디 파일에는 서스테인 이벤트가 너무 많아서 1만자 악보에 맞게 출력할 수 없습니다. $prefix$악보 템포뭉개기 2 서스테인뭉개기 2 등의 옵션으로 개수를 줄여 보세요. 실행을 종료합니다.");
								} else {
									append_error_to_vectorstr(&outputSayang, "There are too many sustain events in this MIDI file, so it can't be output to fit 10,000-character sheet music. Try reducing the number with options such as $prefix$solo mergetempo 2 mergesustain 2, etc. Terminate execution.");
								}
								errorNow = true;
								return;
							}
							outputtxt = outputtxtRollback;
							while (Notes.size()>0) {
								Notes2.push_back(Notes[0]);
								Notes.erase(Notes.begin());
							}
							break;
						}
					}
					outputtxtRollback = outputtxt;
					if (isstart) {
						if (tempomat[0][0] != currentTime) {
							if (!isEnglish) {
								append_error_to_vectorstr(&outputSayang, "템포 변환 중 에러가 발생했습니다. 개발자에게 문의해 주세요...");
							} else {
								append_error_to_vectorstr(&outputSayang, "An error occurred during tempo conversion. Please contact the developer...");
							}
							cout << tempomat[0][0] << '\n';
							cout << currentTime << '\n';
							errorNow = true;
							return;
						}
						if (!isTriplet) {
							outputtxt += "t" + to_string((int)(tempomat[0][1]));
							if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += "p" + to_string((int)(tempomat[0][1])); }
							tickperquarter = tickperquarter_orig;
						} else if (isTriplet) {
							outputtxt += "t" + to_string((int)(tempomat[0][1]*3/2));
							if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += "p" + to_string((int)(tempomat[0][1]*2/3)); }
							tickperquarter = tickperquarter_orig*2/3;
						}
						tempomat.erase(tempomat.begin());
						continue;
					}
					if (outputtxt.length()>lengthHorizontalDivision) {
						vector<string> tempOptVec = opt_ms2mml(outputtxt);
						outputtxt = tempOptVec[0];
						stringstream ssInt(tempOptVec[1]);
						ssInt >> curlen;
						if (outputtxt.length()>lengthHorizontalDivision) {
							if (Notes2.size() == 0) {
								if (!isEnglish) {
									append_error_to_vectorstr(&outputSayang, "이 미디 파일에는 템포 조정 이벤트가 너무 많아서 1만자 악보에 맞게 출력할 수 없습니다. $prefix$악보 템포뭉개기 2 서스테인뭉개기 2 등의 옵션으로 개수를 줄여 보세요. 실행을 종료합니다.");
								} else {
									append_error_to_vectorstr(&outputSayang, "There are too many tempo events in this MIDI file, so it can't be output to fit 10,000-character sheet music. Try reducing the number with options such as $prefix$solo mergetempo 2 mergesustain 2, etc. Terminate execution.");
								}
								errorNow = true;
								return;
							}
							if (curchord == -1 && susmat.size() > 0) {
								if (!isEnglish) {
									append_error_to_vectorstr(&outputSayang, "이 미디 파일에는 서스테인 이벤트가 너무 많아서 1만자 악보에 맞게 출력할 수 없습니다. $prefix$악보 템포뭉개기 2 서스테인뭉개기 2 등의 옵션으로 개수를 줄여 보세요. 실행을 종료합니다.");
								} else {
									append_error_to_vectorstr(&outputSayang, "There are too many sustain events in this MIDI file, so it can't be output to fit 10,000-character sheet music. Try reducing the number with options such as $prefix$solo mergetempo 2 mergesustain 2, etc. Terminate execution.");
								}
								errorNow = true;
								return;
							}
							outputtxt = outputtxtRollback;
							while (Notes.size()>0) {
								Notes2.push_back(Notes[0]);
								Notes.erase(Notes.begin());
							}
							break;
						}
					}
					outputtxtRollback = outputtxt;
					if (isSustainComing) {
						if (susmat[0][1] >= 64) {
							outputtxt += "s1";
						} else {
							outputtxt += "s0";
						}
						susmat.erase(susmat.begin());
						continue;
					}
					if (outputtxt.length()>lengthHorizontalDivision) {
						vector<string> tempOptVec = opt_ms2mml(outputtxt);
						outputtxt = tempOptVec[0];
						stringstream ssInt(tempOptVec[1]);
						ssInt >> curlen;
						if (outputtxt.length()>lengthHorizontalDivision) {
							if (Notes2.size() == 0) {
								if (!isEnglish) {
									append_error_to_vectorstr(&outputSayang, "이 미디 파일에는 템포 조정 이벤트가 너무 많아서 1만자 악보에 맞게 출력할 수 없습니다. $prefix$악보 템포뭉개기 2 서스테인뭉개기 2 등의 옵션으로 개수를 줄여 보세요. 실행을 종료합니다.");
								} else {
									append_error_to_vectorstr(&outputSayang, "There are too many tempo events in this MIDI file, so it can't be output to fit 10,000-character sheet music. Try reducing the number with options such as $prefix$solo mergetempo 2 mergesustain 2, etc. Terminate execution.");
								}
								errorNow = true;
								return;
							}
							if (curchord == -1 && susmat.size() > 0) {
								if (!isEnglish) {
									append_error_to_vectorstr(&outputSayang, "이 미디 파일에는 서스테인 이벤트가 너무 많아서 1만자 악보에 맞게 출력할 수 없습니다. $prefix$악보 템포뭉개기 2 서스테인뭉개기 2 등의 옵션으로 개수를 줄여 보세요. 실행을 종료합니다.");
								} else {
									append_error_to_vectorstr(&outputSayang, "There are too many sustain events in this MIDI file, so it can't be output to fit 10,000-character sheet music. Try reducing the number with options such as $prefix$solo mergetempo 2 mergesustain 2, etc. Terminate execution.");
								}
								errorNow = true;
								return;
							}
							outputtxt = outputtxtRollback;
							while (Notes.size()>0) {
								Notes2.push_back(Notes[0]);
								Notes.erase(Notes.begin());
							}
							break;
						}
					}
					outputtxtRollback = outputtxt;
					
					int thisoct = -1;
					int thisvol = -1;
					int scalenumber = 0;
					if (curchord >= 0) {
						int notenumber = curnote[2];
						int volnumber = curnote[3];
						thisoct = ((notenumber-12)/12);
						thisvol = (int)(((volnumber+volumeAdd1*8.)*volumeMul2+volumeAdd3*8.)/8.);
						thisvol = (thisvol>15)?15:thisvol;
						thisvol = (thisvol <0)?0 :thisvol;
						scalenumber = notenumber%12;
						
						/*if (thisoct == curoct + 1 && scalenumber == 0) {
							thisoct--;
							scalenumber = 12;
						} else if (thisoct == curoct - 1 && scalenumber == 11) {
							thisoct++;
							scalenumber = -1;
						}*/
					} else {
						thisoct = curoct;
						thisvol = curvol;
					}
					
					// testlist = [testlist endtime-currentTime];
					vector<int> notelist = get_length_list2_expand(endtime-currentTime,tickperquarter,noteResolution);
					int tempCurrentTime = currentTime;
					for (int ii=0; ii<notelist.size(); ii++) {
						int i = notelist[ii];
						tempCurrentTime += notetotime2(i,tickperquarter);
					}
					if ( tempCurrentTime > tempomat[0][0]) {
						notelist = get_length_list2_nonexpand(endtime-currentTime,tickperquarter,noteResolution);
					}
					
					if (thisoct != curoct) {
						if (thisoct == curoct - 1) {
							outputtxt += "<";
							curoct = thisoct;
						} else if (thisoct == curoct + 1) {
							outputtxt += ">";
							curoct = thisoct;
						} else {
							outputtxt += "o" + to_string(thisoct);
							curoct = thisoct;
						}
					}
					if (thisvol != curvol) {
						outputtxt += "v" + to_string(thisvol);
						curvol = thisvol;
					}
					bool lfirst = true;
					bool lintdotted = false;
					for (int ii=0; ii<notelist.size(); ii++) {
						if (lintdotted) {
							lintdotted = false;
							continue;
						}
						int i = notelist[ii];
						if (i != curlen) {
							outputtxt += "l" + to_string(i);
							curlen = i;
						}
						if (ii == notelist.size()-2) {
							if (notelist[ii+1]==i*2) {
								lintdotted = true;
							}
						}
						if (!lfirst) {
							outputtxt += "&";
						} else if (iscontinuednote && lfirst) {
							outputtxt += "&";
							iscontinuednote = false;
							lfirst = false;
						} else {
							lfirst = false;
						}
						if (iOnoffMotion) {
							if (currest && isFirstChannel && !isplaying) {
								float randomTempNumber = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
								int tempMotionNumber = (int)(randomTempNumber*5+1);
								outputtxt += "m" + to_string(tempMotionNumber);
								isplaying = true;
							}
						}
						currest = false;
						outputtxt += scaletoname(scalenumber);
						currentTime = currentTime + notetotime2(i,tickperquarter);
						if (lintdotted) {
							outputtxt += ".";
							currentTime = currentTime + notetotime2(i,tickperquarter)/2;
						}
					}
					if (outputtxt.length()>lengthHorizontalDivision) {
						vector<string> tempOptVec = opt_ms2mml(outputtxt);
						outputtxt = tempOptVec[0];
						stringstream ssInt(tempOptVec[1]);
						ssInt >> curlen;
						if (outputtxt.length()>lengthHorizontalDivision) {
							if (Notes2.size() == 0) {
								if (!isEnglish) {
									append_error_to_vectorstr(&outputSayang, "이 미디 파일에는 템포 조정 이벤트가 너무 많아서 1만자 악보에 맞게 출력할 수 없습니다. $prefix$악보 템포뭉개기 2 서스테인뭉개기 2 등의 옵션으로 개수를 줄여 보세요. 실행을 종료합니다.");
								} else {
									append_error_to_vectorstr(&outputSayang, "There are too many tempo events in this MIDI file, so it can't be output to fit 10,000-character sheet music. Try reducing the number with options such as $prefix$solo mergetempo 2 mergesustain 2, etc. Terminate execution.");
								}
								errorNow = true;
								return;
							}
							if (curchord == -1 && susmat.size() > 0) {
								if (!isEnglish) {
									append_error_to_vectorstr(&outputSayang, "이 미디 파일에는 서스테인 이벤트가 너무 많아서 1만자 악보에 맞게 출력할 수 없습니다. $prefix$악보 템포뭉개기 2 서스테인뭉개기 2 등의 옵션으로 개수를 줄여 보세요. 실행을 종료합니다.");
								} else {
									append_error_to_vectorstr(&outputSayang, "There are too many sustain events in this MIDI file, so it can't be output to fit 10,000-character sheet music. Try reducing the number with options such as $prefix$solo mergetempo 2 mergesustain 2, etc. Terminate execution.");
								}
								errorNow = true;
								return;
							}
							outputtxt = outputtxtRollback;
							while (Notes.size()>0) {
								Notes2.push_back(Notes[0]);
								Notes.erase(Notes.begin());
							}
							break;
						}
					}
					outputtxtRollback = outputtxt;
					
					if (notelist.size() == 0) {
						Notes2.push_back(curnote);
					}
					if (!istempsusinter) {
						Notes.erase(Notes.begin());
					} else {
						Notes[0][4] = endtime;
						iscontinuednote = true;
					}
				}
				
				if (Notes2.size() > 0) {
					stable_sort(Notes2.begin(), Notes2.end(), sortFunctionByStarttime);
					Notes = Notes2;
					Notes2 = vector<vector<int>> {};
					stable_sort(susmat2.begin(), susmat2.end(), sortby1stcol);
					susmat = susmat2;
					susmat2 = vector<vector<int>> {};
					currentTime = tempTimeStartNote;
					if (!isTriplet) { tickperquarter = tickperquarter_orig; }
					else { tickperquarter = tickperquarter_orig*2/3; }
					vector<string> tempOptVec = opt_ms2mml(outputtxt);
					outputtxt = tempOptVec[0];
					curoct = 4;
					curlen = 4;
					curvol = 8;
					currest = true;
					if (curchord >= 0) {
						iCountCharacter.push_back(outputtxt.size());
						cCharacter.push_back(outputtxt);
						isFirstChannel = false;
					} else if (curchord == -1) {
						iCountCharacterPseudoSustain = outputtxt.length();
						cCharacterPseudoSustain = outputtxt;
					}
					curchord = curchord + 1;
					if (iOnoffMotion && curchord==0) {
						outputtxt = "m0";
					} else {
						outputtxt = "";
					}
					measureLength = 0;
				} else {
					lfinish = true;
					curchord = curchord + 1;
					vector<string> tempOptVec = opt_ms2mml(outputtxt);
					outputtxt = tempOptVec[0];
					iCountCharacter.push_back(outputtxt.size());
					cCharacter.push_back(outputtxt);
				}
				
			}
			outputMidiStrings << "cCharacterPseudoSustain of instr " << iInstrMain << '\n';
			outputMidiStrings << cCharacterPseudoSustain << '\n';
			for (int i=0; i<cCharacter.size(); i++) {
				outputMidiStrings << "cCharacter of instr " << iInstrMain << " chord " << i << '\n';
				outputMidiStrings << cCharacter[i] << '\n';
			}
			
			int iSumCountCharacter = 0;
			for (int i=0; i<curchord; i++) { iSumCountCharacter += iCountCharacter[i]; }
			iSumCountCharacter += iCountCharacterPseudoSustain;
			
			bool lWrite = false;
			if (!lOnoffHorizontalDivision) {
				lHorizontalDivisionFinished = true;
				lWrite = true;
			} else {
				int tempCutIndex = cuttableMat.size()-1;
				while (cuttableMat[tempCutIndex] > timeEndNote) {
					tempCutIndex--;
				}
				if (icuttable[1] < 0 && icuttable[2] == tempCutIndex) {
					if (iSumCountCharacter <= lengthHorizontalDivision) {
						lHorizontalDivisionFinished = true;
						lWrite = true;
					} else {
						icuttable[1] = icuttable[0];
					}
				} else {
					if (icuttable[2]-icuttable[1] > 1) {
						if (iSumCountCharacter <= lengthHorizontalDivision) {
							icuttable[1] = tempCuttableIndex;
						} else {
							icuttable[2] = tempCuttableIndex;
						}
					} else if (icuttable[2]-icuttable[1] == 1) {
						tempCutIndex = cuttableMat.size()-1;
						while (cuttableMat[tempCutIndex] > timeEndNote) {
							tempCutIndex--;
						}
						if ((icuttable[2] == tempCutIndex && iSumCountCharacter > lengthHorizontalDivision) || (icuttable[0] == icuttable[1])) {
							if (maxLengthTrack/tickperquarter_orig/tempomat.size() < 20) {
								if (!isEnglish) {
									append_error_to_vectorstr(&outputSayang, "이 파일은 1만자 악보에 맞게 자동 분할할 수 없습니다. 미디 파일에 템포 설정 이벤트가 상당히 많은 것으로 생각됩니다. 템포뭉개기 2 등의 옵션을 시도해 보세요.");
								} else {
									append_error_to_vectorstr(&outputSayang, "This file cannot be automatically split to fit 10,000 character scores. I think there are quite a few tempo setting events in the MIDI file. Try options like mergetempo 2.");
								}
								errorNow = true;
								return;
							} else {
								if (!isEnglish) {
									append_error_to_vectorstr(&outputSayang, "이 파일은 1만자 악보에 맞게 자동 분할할 수 없습니다. 실행을 종료합니다. 1만자무시 혹은 화음분할 옵션을 사용해 보세요.");
								} else {
									append_error_to_vectorstr(&outputSayang, "This file cannot be automatically split to fit 10,000 character scores. Terminate execution. Try the ignore10000 or chorddivision option.");
								}
								errorNow = true;
								return;
							}
						}
						lWrite = true;
						icuttable[0] = icuttable[1];
						icuttable[1] = -1;
						tempCutIndex = cuttableMat.size()-1;
						while (cuttableMat[tempCutIndex] > timeEndNote) {
							tempCutIndex--;
						}
						icuttable[2] = tempCutIndex;
					}
				}
			}
			
			if (lWrite) {
				for (int iMaxCheck=0; iMaxCheck<iCountCharacter.size(); iMaxCheck++) {
					if (iCountCharacter[iMaxCheck] > 10000) {
						if (!isEnglish) {
							append_error_to_vectorstr(&outputSayang, "전체 화음 중 1개 이상의 화음 글자수가 1만자를 초과하여 변환할 수 없습니다. 글자수를 줄이는 옵션 (해상도, 템포뭉개기, 서스테인뭉개기 등) 을 시도해 보세요. 오류라고 생각되신다면 개발자에게 제보해 주세요.");
						} else {
							append_error_to_vectorstr(&outputSayang, "One or more of the total chords cannot be converted because the number of chord characters exceeds 10,000. Try reducing the number of characters (resolution, mergetempo, mergesustain, etc.). If you think this is an error, please report it to the developer.");
						}
						errorNow = true;
						return;
					}
					// if (iCountCharacter[iMaxCheck] < 10000 && iCountCharacter[iMaxCheck]+iCountCharacterPseudoSustain > 10000) {
						// if (!isEnglish) {
							// append_error_to_vectorstr(&outputSayang, "서스테인 이벤트가 너무 많아 변환할 수 없습니다. 서스테인뭉개기 옵션으로 서스테인 이벤트의 개수를 줄여 보세요. 오류라고 생각되신다면 개발자에게 제보해 주세요.");
						// } else {
							// append_error_to_vectorstr(&outputSayang, "There are too many sustain events to convert. Reduce the number of sustain events with the mergesustain option. If you think this is an error, please report it to the developer.");
						// }
						// return;
					// }
				}
						
				if (lOnoffHorizontalDivision) {
					iCurrentWriteHorizontal++;
					if (iCurrentWriteHorizontal > 50) {
						if (!isEnglish) {
							append_error_to_vectorstr(&outputSayang, "작성된 악보가 50장을 초과하여 중지합니다. 미디 파일의 길이 및 변환 설정을 확인해 주세요. 오류라고 생각되신다면 개발자에게 문의해 주세요.");
						} else {
							append_error_to_vectorstr(&outputSayang, "The written score exceeds 50 sheets and stops. Please check the length and conversion settings of the MIDI file. If you think this is an error, please contact the developer.");
						}
						errorNow = true;
						return;
					}
				}
				if (lOnoffVerticalDivision) { iCurrentWriteVertical = 0; isFirstChord = true; }
				
				vector<bool> isWritten (curchord, false);
				bool tempIOnoffVerticalDivision = lOnoffVerticalDivision;
				if (lOnoffVerticalDivision && (curchord < ceil(lengthHorizontalDivision/10000))) {
					if (!lOnOff_candidate) {
						if (!isEnglish) {
							append_warning_to_vectorstr(&outputSayang, "이 악보의 화음은 " + to_string(curchord) + "개이므로 " + to_string(ceil(lengthHorizontalDivision/10000)) + "분할할 수 없습니다. 화음 분할 기능이 비활성화됩니다.");
						} else {
							append_warning_to_vectorstr(&outputSayang, "The chord in this score is " + to_string(curchord) + ", so cannot be divided into " + to_string(ceil(lengthHorizontalDivision/10000)) + ". Chord division function is disabled.");
						}
					}
					tempIOnoffVerticalDivision = false;
				}
				
				bool isCheckWritten = false;
				while (!isCheckWritten) {
					if (tempIOnoffVerticalDivision) {
						iCurrentWriteVertical++;
						if (iCurrentWriteVertical > 50) {
							if (!isEnglish) {
								append_error_to_vectorstr(&outputSayang, "작성된 악보가 50장을 초과하여 중지합니다. 미디 파일의 길이 및 변환 설정을 확인해 주세요. 오류라고 생각되신다면 개발자에게 문의해 주세요.");
							} else {
								append_error_to_vectorstr(&outputSayang, "The written score exceeds 50 sheets and stops. Please check the length and conversion settings of the MIDI file. If you think this is an error, please contact the developer.");
							}
							errorNow = true;
							return;
						}
					}
					
					if (!isMML) {
						outputtxt = "<?xml version='1.0' encoding='utf-8'?>\n";
						outputtxt += "<ms2>\n";
					} else {
						outputtxt = "[Settings]\nEncoding = ks_c_5601-1987\nTitle = \nSource = \nMemo = \n";
					}
					isFirstChannel = true;
					int iWriteChordPos = -1;
					bool isSustainWritten = false;
					
					int curWrittenCharacter = 0;
					int curChordPos = 0;
					int curChordDir = 1;
					
					while (!tempIOnoffVerticalDivision || curWrittenCharacter < 10000) {
						if (iCountCharacterPseudoSustain > 0 && iWriteChordPos == -1) {
							iWriteChordPos++;
							if (isFirstChannel) {
								if (!isMML) {
									outputtxt += "<melody>\n";
									outputtxt += "<![CDATA[\n";
								} else {
									outputtxt += "[Channel" + to_string(iWriteChordPos+1) + "]\n";
								}
							} else {
								if (!isMML) {
									outputtxt += "<chord index=\"" + to_string(iWriteChordPos) + "\">\n";
									outputtxt += "<![CDATA[\n";
								} else {
									outputtxt += "[Channel" + to_string(iWriteChordPos+1) + "]";
								}
							}
							curWrittenCharacter += iCountCharacterPseudoSustain;
							outputtxt += cCharacterPseudoSustain;
							isSustainWritten = true;
							if (!isMML) {
								outputtxt += "\n]]>\n";
								if (isFirstChannel) {
									outputtxt += "</melody>\n";
									isFirstChannel = false;
								} else {
									outputtxt += "</chord>\n";
								}
							} else {
								outputtxt += "\n";
							}
						} else {
							if (isWritten[curChordPos] == false) {
								iWriteChordPos++;
								if (isFirstChannel) {
									if (!isMML) {
										outputtxt += "<melody>\n";
										outputtxt += "<![CDATA[\n";
									} else {
										outputtxt += "[Channel" + to_string(iWriteChordPos+1) + "]\n";
									}
								} else {
									if (!isMML) {
										outputtxt += "<chord index=\"" + to_string(iWriteChordPos) + "\">\n";
										outputtxt += "<![CDATA[\n";
									} else {
										outputtxt += "[Channel" + to_string(iWriteChordPos+1) + "[";
									}
								}
								curWrittenCharacter += iCountCharacter[curChordPos];
								isWritten[curChordPos] = true;
								outputtxt += cCharacter[curChordPos];
								if (!isMML) {
									outputtxt += "\n]]>\n";
									if (isFirstChannel) {
										outputtxt += "</melody>\n";
										isFirstChannel = false;
									} else {
										outputtxt += "</chord>\n";
									}
								} else {
									outputtxt += "\n";
								}
							}
						}
						
						if (iCountCharacterPseudoSustain > 0 && isSustainWritten) {
							isSustainWritten = false;
							continue;
						}
						
						curChordPos += curChordDir;
						if (curChordPos >= curchord) { break; }
						if (curChordPos < 0       ) { break; }
						
						if (!tempIOnoffVerticalDivision) {
							continue;
						} else if ( iWriteChordPos == 9 ) {
							break;
						} else {
							if (isWritten[curChordPos]) {
								continue;
							} else {
								if (curWrittenCharacter+iCountCharacter[curChordPos] <= 10000) {
									continue;
								} else {
									if (curChordDir==1) {
										int tempChordIndex = curchord-1;
										while (true) {
											if (isWritten[tempChordIndex] == false) { break; }
											else { tempChordIndex--; }
											if (tempChordIndex < 0) { break; }
										}
										if (tempChordIndex < 0) {
											break;
										} else if (curWrittenCharacter+iCountCharacter[tempChordIndex] <= 10000) {
											curChordPos = tempChordIndex;
											curChordDir = -1;
										} else {
											break;
										}
									} else {
										break;
									}
								}
							}
						}
					}
					
					if (!isMML) { outputtxt += "</ms2>\n"; }
					
					// currentName = ((message.guild)?("guild_"+message.channel.id):("dm_"+message.author.id));
					string outputFileName = "";
					if (!lOnOff_candidate) {
						outputFileName = originalFileName;//"converted"; //user2filename[currentName].substring(0, user2filename[currentName].size()-4);
					} else {
						outputFileName = "_" + originalFileName;
					}
					
					if ((command == "악보" && iFixedInstrument[0] != -1) || (command == "합주악보")) {
						outputFileName += "_" + instr2name(supportedInstrList[0]);
					}
	
					string fileExtension = ".ms2mml";
					if (isMML) { fileExtension = ".mml"; }
					if (!lOnoffHorizontalDivision && !tempIOnoffVerticalDivision) {
						outputFileName += fileExtension;
					} else if (lOnoffHorizontalDivision && !tempIOnoffVerticalDivision && lHorizontalDivisionFinished && iCurrentWriteHorizontal==1) {
						outputFileName += fileExtension;
					} else if (lOnoffHorizontalDivision && !tempIOnoffVerticalDivision) {
						string tempstring = to_string(iCurrentWriteHorizontal);
						if (iCurrentWriteHorizontal < 10) { tempstring = "0" + tempstring; }
						outputFileName += "_" + tempstring + fileExtension;
					} else if (!lOnoffHorizontalDivision && tempIOnoffVerticalDivision) {
						isCheckWritten = true;
						for (int i=0; i<curchord; i++) { if (isWritten[i] == false) { isCheckWritten = false; } }
						if (isCheckWritten && iCurrentWriteVertical==1) {
							outputFileName += fileExtension;
						} else {
							outputFileName += "_chord" + to_string(iCurrentWriteVertical) + fileExtension;
						}
					}
					
					if (curWrittenCharacter <= 3000) {
						outputFileName = "3000_" + outputFileName;
						writtenScoreList[writtenScoreList.size()-1][iInstrMain].push_back(3000);
					} else if (curWrittenCharacter <= 5000) {
						outputFileName = "5000_" + outputFileName;
						writtenScoreList[writtenScoreList.size()-1][iInstrMain].push_back(5000);
					} else {
						outputFileName = "10000_" + outputFileName;
						writtenScoreList[writtenScoreList.size()-1][iInstrMain].push_back(10000);
					}
					
					ofstream outputMidiMs2mml(outputFileName);
					outputMidiMs2mml << outputtxt;
					iHorizontalTryCount++;
					writtenFileList[writtenFileList.size()-1][iInstrMain].push_back(outputFileName);
					writtenCharacterList[writtenCharacterList.size()-1][iInstrMain].push_back(curWrittenCharacter);
					writtenChordList[writtenChordList.size()-1][iInstrMain].push_back(iWriteChordPos+1);
					
					isCheckWritten = true;
					for (int i=0; i<curchord; i++) { if (isWritten[i] == false) { isCheckWritten = false; break; } }
				}
			}
		}
	}
}