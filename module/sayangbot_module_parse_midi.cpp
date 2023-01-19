#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include "sayangbot_module_transform_tempo.cpp"

using namespace std;
using namespace smf;

void parse_midi() {
	if (errorNow) { return; }
	
	tracks = 1;
	tickperquarter = 320;
	
	numNoteTrackChannel = vector<vector<int>> (iMaxTrack, vector<int> (iMaxChannel, 0));
	sustainTrackChannel = vector<vector<vector<vector<int>>>> (iMaxTrack, vector<vector<vector<int>>> (iMaxChannel, vector<vector<int>> {}));
	sustainTrackChannel_orig = vector<vector<vector<vector<int>>>> (iMaxTrack, vector<vector<vector<int>>> (iMaxChannel, vector<vector<int>> {}));
	Notes = vector<vector<int>> {};
	Notes_orig = vector<vector<int>> {};
	instrumentTrackChannel = vector<vector<vector<vector<int>>>> (iMaxTrack, vector<vector<vector<int>>> (iMaxChannel, vector<vector<int>> {}));
	trackName = vector<string> (iMaxTrack, "");
	iMaxKeySignatureTrack = vector<int> (iMaxTrack, 0);
	iPort = vector<int> (iMaxTrack, 0);
	
	maxLengthTrack = 0;
	maxEndNote = 0;
	iMaxTempo = 0;
	iMaxTimeSignature = 0;
	iMaxText = 0;
	iMaxCopyright = 0;
	iMaxKeySignature = 0;
	
	currentchannelvolume = vector<vector<vector<int>>> (iMaxChannel, vector<vector<int>> (1, vector<int> {0, 127}));
	currentchannelinstrument = vector<vector<vector<int>>> (iMaxChannel, vector<vector<int>> {});
	tempomat = vector<vector<int>> (iMaxTempo, vector<int> {-1, -1});
	tempomat_orig = vector<vector<int>> (iMaxTempo, vector<int> {-1, -1});
	tempomat_orig_orig = vector<vector<int>> (iMaxTempo, vector<int> {-1, -1});
	tempomat_before_transform = vector<vector<int>> (iMaxTempo, vector<int> {-1, -1});
	susmat = vector<vector<int>> (iMaxTempo, vector<int> {-1, -1});
	susmat_orig = vector<vector<int>> (iMaxTempo, vector<int> {-1, -1});
	
	currentTempoIndex = 0;
	maxCurrentTimeInReal = 0.;
	timesigmat = vector<vector<int>> (iMaxTimeSignature, vector<int> {-1, -1});
	textMat = vector<midiTextEvent> {};
	currentTextIndex = 0;
	copyrightMat = vector<midiTextEvent> (iMaxCopyright, midiTextEvent {-1, ""});
	currentCopyrightIndex = 0;
	
	cuttableMat = vector<int> {};
	icuttable = vector<int> {};
	
	MidiFile midifile;
	bool readStatus = midifile.read("current.midi_now");
	
	midifile.doTimeAnalysis();
	midifile.linkNotePairs();
	if (readStatus == false) {
		if (!isEnglish) {
			append_error_to_vectorstr(&outputSayang, "현재 SayangBot이 읽을 수 없는 MIDI 파일입니다 (헤더 오류). 다른 프로그램으로 MIDI 파일을 재출력해서 사용하시거나, 업데이트를 기다려 주세요...");
		} else {
			append_error_to_vectorstr(&outputSayang, "This is a MIDI file that SayangBot cannot currently read (track header error). Re-output the MIDI file with another program and use it, or wait for the update...");
		}
		errorNow = true;
		return;
	}
	
	tracks = midifile.getTrackCount();
	
	vector<vector<customMidiEvent>> customMidifile (tracks, vector<customMidiEvent> {});
	
	ofstream outputMidi01("02midi.sayang_"+to_string(MYPE));
	outputMidi01 << "TPQ: " << midifile.getTicksPerQuarterNote() << '\n';
	if (tracks > 1) outputMidi01 << "TRACKS: " << tracks << '\n';
	for (int track=0; track<tracks; track++) {
		if (tracks > 1) outputMidi01 << "\nTrack " << track << '\n';
		outputMidi01 << "Tick\tSeconds\tDur\tMessage" << '\n';
		for (int event=0; event<midifile[track].size(); event++) {
			outputMidi01 << dec << midifile[track][event].tick;
			outputMidi01 << '\t' << dec << midifile[track][event].seconds;
			outputMidi01 << '\t';
			if (midifile[track][event].isNoteOn())
				outputMidi01 << midifile[track][event].getDurationInSeconds();
			outputMidi01 << '\t' << dec;
			for (int i=0; i<midifile[track][event].size(); i++)
				outputMidi01 << (int)midifile[track][event][i] << ' ';
			outputMidi01 << '\n';
			
			if (midifile[track][event][0] == 255) {
				customMidiEvent tempMidiEvent;
				tempMidiEvent.tick = midifile[track][event].tick;
				tempMidiEvent.type = midifile[track][event][0];
				tempMidiEvent.metaType = midifile[track][event][1];
				for (int idata=2; idata<midifile[track][event].size(); idata++) {
					tempMidiEvent.data.push_back(midifile[track][event][idata]);
				}
				customMidifile[track].push_back(tempMidiEvent);
			} else {
				customMidiEvent tempMidiEvent;
				tempMidiEvent.tick = midifile[track][event].tick;
				tempMidiEvent.type = midifile[track][event][0]/16;
				tempMidiEvent.channel = midifile[track][event][0]%16;
				tempMidiEvent.metaType = -1;
				for (int idata=1; idata<midifile[track][event].size(); idata++) {
					tempMidiEvent.data.push_back(midifile[track][event][idata]);
				}
				customMidifile[track].push_back(tempMidiEvent);
			}
		}
	}
	
	tickperquarter = midifile.getTicksPerQuarterNote();
		
	for (itrack=0; itrack<tracks; itrack++) {
		int currentTime = 0;
		iMaxKeySignatureTrack[itrack] = 0;
		iPort[itrack] = 0;
		for (int ievent=0; ievent<customMidifile[itrack].size(); ievent++) {
			currentTime = customMidifile[itrack][ievent].tick;
			if (customMidifile[itrack][ievent].type == 255) {
				if (customMidifile[itrack][ievent].metaType == 81) {
					iMaxTempo++;
				} else if (customMidifile[itrack][ievent].metaType == 3) {
					trackName[itrack] = " - " + vectorint2str(customMidifile[itrack][ievent].data);
				} else if (customMidifile[itrack][ievent].metaType == 88) {
					if (itrack!=0) { cout << "Warning : time signature event in non-zero track" << endl; continue; }
					iMaxTimeSignature++;
				} else if (customMidifile[itrack][ievent].metaType == 1) {
					iMaxText++;
				} else if (customMidifile[itrack][ievent].metaType == 2) {
					iMaxCopyright++;
				} else if (customMidifile[itrack][ievent].metaType == 89) {
					iMaxKeySignatureTrack[itrack]++;
				} else if (customMidifile[itrack][ievent].metaType == 47) {
					maxLengthTrack = (maxLengthTrack>currentTime)?maxLengthTrack:currentTime;
					continue;
				} else if (customMidifile[itrack][ievent].metaType == 33) {
					iPort[itrack] = customMidifile[itrack][ievent].data[1];
				} else if (customMidifile[itrack][ievent].metaType == 127) {
					// musescore event
					// data 67, 123, 1 = chord
				} else { // unknown (text)
					iMaxText++;
				}
			} else if (customMidifile[itrack][ievent].type == 11) {
				if (customMidifile[itrack][ievent].data[0] == 64) {
					// sustain
				} else if (customMidifile[itrack][ievent].data[0] == 7) {
					ichannel = customMidifile[itrack][ievent].channel+iPort[itrack]*16;
					currentchannelvolume[ichannel].push_back(vector<int> {currentTime, customMidifile[itrack][ievent].data[1]});
				} else if (customMidifile[itrack][ievent].data[0] == 2) {
					// breath control (ignore)
					continue;
				} else if (customMidifile[itrack][ievent].data[0] == 10) {
					// pan (ignore)
					continue;
				} else if (customMidifile[itrack][ievent].data[0] == 91) {
					// reverb (ignore)
					continue;
				} else if (customMidifile[itrack][ievent].data[0] == 93) {
					// chorus (ignore)
					continue;
				} else if (customMidifile[itrack][ievent].data[0] == 121) {
					// reset all (ignore)
					continue;
				} else if (customMidifile[itrack][ievent].data[0] == 123) {
					// off all TBD
					continue;
				} else if (customMidifile[itrack][ievent].data[0] == 11) {
					// expression controller (ignore)
					continue;
				} else if (customMidifile[itrack][ievent].data[0] == 99) {
					// unknown (ignore)
					continue;
				// } else if (isStrict && ~isWarned11) {
					// sendWarning(message.channel, '파일에 포함된 미디 이벤트(타입 11, 메타타입 ' + customMidifile[itrack][ievent].data[0] + ')는 뭔지 몰라서 무시하겠습니다...');
					// isWarned11 = true;
					// console.log(11);
					// console.log(customMidifile[track][itrack].event[ievent]);
					// console.log(customMidifile[itrack][ievent].data);
				}
			} else if (customMidifile[itrack][ievent].type == 12) {
				// instrument (moved below)
			} else if (customMidifile[itrack][ievent].type == 9) {
				if (customMidifile[itrack][ievent].data[1] > 0) {
					ichannel = customMidifile[itrack][ievent].channel+iPort[itrack]*16;
					numNoteTrackChannel[itrack][ichannel] += 1;
				}
			// } else if (isStrict && isWarned) {
				// sendWarning(message.channel, '파일에 포함된 미디 이벤트(타입 ' + customMidifile[itrack][ievent].type + ')는 뭔지 몰라서 무시하겠습니다...');
				// isWarned = true;
				// console.log(customMidifile[itrack][ievent].type);
				// console.log(customMidifile[track][itrack].event[ievent]);
				// console.log(customMidifile[itrack][ievent].data);
			}
		}
		iMaxKeySignature = (iMaxKeySignature > iMaxKeySignatureTrack[itrack]) ? iMaxKeySignature : iMaxKeySignatureTrack[itrack];
	}
	
	tempomat = vector<vector<int>> (iMaxTempo, vector<int> {-1, -1});
	timesigmat = vector<vector<int>> (iMaxTimeSignature, vector<int> {-1, -1});
	midiTextEvent tempTextEvent;
	tempTextEvent.tick = -1; tempTextEvent.content == "";
	textMat = vector<midiTextEvent> (iMaxText, tempTextEvent);
	copyrightMat = vector<midiTextEvent> (iMaxCopyright, midiTextEvent {-1, ""});
	
	for (itrack=0; itrack<tracks; itrack++) {
		int currentTime = 0;
		int currentTempo = 120;
		for (int ievent=0; ievent<customMidifile[itrack].size(); ievent++) {
			currentTime = customMidifile[itrack][ievent].tick;
			if (customMidifile[itrack][ievent].type == 255) {
				if (customMidifile[itrack][ievent].metaType == 81) {
					int thisTempo = customMidifile[itrack][ievent].data[1]*myPow(16,4) + customMidifile[itrack][ievent].data[2]*myPow(16,2) + customMidifile[itrack][ievent].data[3]*myPow(16,0);
					int tempoValue = (60*1000000/thisTempo);
					tempomat[currentTempoIndex][0] = currentTime;
					tempomat[currentTempoIndex][1] = tempoValue*tempoMulti;
					currentTempoIndex++;
					currentTempo = thisTempo;
				}
			} else if (customMidifile[itrack][ievent].type == 12) {
				ichannel = customMidifile[itrack][ievent].channel+iPort[itrack]*16;
				int targetPatch = customMidifile[itrack][ievent].data[0];
				if (ichannel%16 == 9) { targetPatch = 128; }
				currentchannelinstrument[ichannel].push_back(vector<int> {currentTime, targetPatch, itrack});
			}
		}
	}
	
	stable_sort(tempomat.begin(), tempomat.end(), sortby1stcol);
	bool isRemoved = true;
	int removedElement = 0;
	while (isRemoved) {
		isRemoved = false;
		for (int i=0; i+1<tempomat.size(); i++) {
			if (tempomat[i][0] == tempomat[i+1][0]) {
				isRemoved = true;
				removedElement++;
				vector<vector<int>> newtempomat {};
				for (int i2=0; i2<tempomat.size(); i2++) {
					if (i2!=i) {
						newtempomat.push_back(tempomat[i2]);
					}
				}
				tempomat = newtempomat;
			}
		}
	}
	iMaxTempo -= removedElement;
	
	if (tempomat.size() == 0 || tempomat[0][0] != 0) {
		tempomat.insert(tempomat.begin(), vector<int> {0, 2});
		tempomat[0][1] = 120;
	}
	
	if (iMinTempo != -1) {
		for (int iTempo=0; iTempo<tempomat.size(); iTempo++) {
			if (tempomat[iTempo][1] < iMinTempo) {
				tempomat[iTempo][1] = iMinTempo;
			}
		}
	}
	
	if (iFixedTempo != -1) {
		tempomat = vector<vector<int>> (1, vector<int> {0, iFixedTempo});
		iMaxTempo = 1;
	}
	
	for (ichannel=0; ichannel<iMaxChannel; ichannel++) {
		stable_sort(currentchannelvolume[ichannel].begin(), currentchannelvolume[ichannel].end(), sortby1stcol);
		stable_sort(currentchannelinstrument[ichannel].begin(), currentchannelinstrument[ichannel].end(), sortby1stcol);
		if (currentchannelinstrument[ichannel].size() == 0) { currentchannelinstrument[ichannel].push_back(vector<int> {0, 0, -1}); }
	}
	
	for (itrack=0; itrack<tracks; itrack++) {
		int currentTime = 0;
		double currentTimeInReal = 0.;
		int currentTempo = 120;
		int currentTempoIndex = 0;
		int currentTimeSigIndex = 0;
		int currentTimeKeyIndex = 0;
		double diffTimeBeforeChange = 0.;
		double diffTimeAfterChange = 0.;
		
		for (int ievent=0; ievent<customMidifile[itrack].size(); ievent++) {
			currentTime = customMidifile[itrack][ievent].tick;
			int tempDeltaTime = 0;
			if (ievent>0) { tempDeltaTime = customMidifile[itrack][ievent].tick - customMidifile[itrack][ievent-1].tick; }
			if (currentTempoIndex < tempomat.size()) {
				if (currentTime >= tempomat[currentTempoIndex][0]) { // while no need?
					diffTimeBeforeChange = 60./currentTempo*(tempomat[currentTempoIndex][0] - (currentTime-tempDeltaTime))/midifile.getTicksPerQuarterNote();
					diffTimeAfterChange = 60./tempomat[currentTempoIndex][1]*(currentTime - tempomat[currentTempoIndex][0])/midifile.getTicksPerQuarterNote();
					currentTempo = tempomat[currentTempoIndex][1];
					currentTempoIndex++;
				} else {
					diffTimeBeforeChange = 60./currentTempo*tempDeltaTime/midifile.getTicksPerQuarterNote();
					diffTimeAfterChange = 0.;
				}
			} else {
				diffTimeBeforeChange = 60./currentTempo*tempDeltaTime/midifile.getTicksPerQuarterNote();
				diffTimeAfterChange = 0.;
			}
			if (currentTimeInReal + diffTimeBeforeChange + diffTimeAfterChange < 86400.) {
				currentTimeInReal += (diffTimeBeforeChange + diffTimeAfterChange);
			}
			if (customMidifile[itrack][ievent].type == 255) {
				if (customMidifile[itrack][ievent].metaType == 88) {
					if (itrack!=0) { continue; }
					timesigmat[currentTimeSigIndex][0] = currentTime;
					timesigmat[currentTimeSigIndex][1] = (customMidifile[itrack][ievent].data[0]);
					currentTimeSigIndex++;
				} else if (customMidifile[itrack][ievent].metaType == 1) {
					textMat[currentTextIndex].tick = currentTimeInReal;
					textMat[currentTextIndex].content = vectorint2str(customMidifile[itrack][ievent].data);
					currentTextIndex++;
				} else if (customMidifile[itrack][ievent].metaType == 2) {
					copyrightMat[currentCopyrightIndex].tick = currentTimeInReal;
					copyrightMat[currentCopyrightIndex].content = vectorint2str(customMidifile[itrack][ievent].data);
					currentCopyrightIndex++;
				} else if (customMidifile[itrack][ievent].metaType == 89) {
					// timekeymat[itrack][currentTimeKeyIndex][0] = currentTime;
					// timekeymat[itrack][currentTimeKeyIndex][1] = (customMidifile[itrack][ievent].data[0]);
					currentTimeKeyIndex++;
				} else if (customMidifile[itrack][ievent].metaType == 81) {
					// tempo
				} else if (customMidifile[itrack][ievent].metaType == 3) {
					// track name
				} else if (customMidifile[itrack][ievent].metaType == 47) {
					// end of track
				} else if (customMidifile[itrack][ievent].metaType == 33) {
					// prefefined port
				} else if (customMidifile[itrack][ievent].metaType == 127) {
					// musescore event
				} else {
					string tempText = vectorint2str(customMidifile[itrack][ievent].data);
					textMat[currentTextIndex].tick = currentTimeInReal;
					textMat[currentTextIndex].content = tempText;
					currentTextIndex++;
				}
			} else if (customMidifile[itrack][ievent].type == 11) {
				if (customMidifile[itrack][ievent].data[0] == 64) {
					ichannel = customMidifile[itrack][ievent].channel+iPort[itrack]*16;
					int currentSustain = customMidifile[itrack][ievent].data[1];
					int currentchannelinstrumentvalue = 0;
					for (iInstr=0; iInstr<currentchannelinstrument[ichannel].size(); iInstr++) {
						bool isSameTrack = true;
						if (lSameTrackInstrument) {
							isSameTrack = (currentchannelinstrument[ichannel][iInstr][2] == itrack);
						}
						if ((currentchannelinstrument[ichannel][iInstr][0] <= currentTime) && isSameTrack) { currentchannelinstrumentvalue = currentchannelinstrument[ichannel][iInstr][1]; }
					}
					int icurrentinstrument = currentchannelinstrumentvalue;
					if (customMidifile[itrack][ievent].channel%16 == 9) {
						icurrentinstrument = 128;
					}
					sustainTrackChannel[itrack][ichannel].push_back(vector<int> {currentTime, currentSustain, icurrentinstrument});
				}
			} else if (customMidifile[itrack][ievent].type == 12) {
				// moved above
			} else if (customMidifile[itrack][ievent].type == 9) {
				if (currentTimeInReal > 86400) {
					if (!is24NoteWarned) {
						if (!isEnglish) {
							append_warning_to_vectorstr(&outputSayang, "미디 파일에 포함된 음표 중 재생 시간 24시간 이후의 음표는 무시합니다. 미디 파일에 오류가 있을 수도 있습니다. 다른 사항이 있으시면 개발자에게 문의 바랍니다.");
						} else {
							append_warning_to_vectorstr(&outputSayang, "Notes after 24 hours of playing time are ignored. There may be errors in the MIDI file. If you have any other questions, please contact the developer.");
						}
						is24NoteWarned = true;
					}
					continue;
				}
				if (customMidifile[itrack][ievent].data[1] > 0) {
					ichannel = customMidifile[itrack][ievent].channel+iPort[itrack]*16;
					int icurrentpitch = customMidifile[itrack][ievent].data[0];
					double currentchannelvolumevalue = 100.;
					for (int ivol=0; ivol<currentchannelvolume[ichannel].size(); ivol++) {
						if (currentchannelvolume[ichannel][ivol][0] <= currentTime) { currentchannelvolumevalue = currentchannelvolume[ichannel][ivol][1]; }
						else { break; }
					}
					int icurrentvolume = (int) (currentchannelvolumevalue/100. * customMidifile[itrack][ievent].data[1]);
					int currentchannelinstrumentvalue = 0;
					for (iInstr=0; iInstr<currentchannelinstrument[ichannel].size(); iInstr++) {
						bool isSameTrack = true;
						if (lSameTrackInstrument) {
							isSameTrack = (currentchannelinstrument[ichannel][iInstr][2] == itrack);
						}
						if ((currentchannelinstrument[ichannel][iInstr][0] <= currentTime) && isSameTrack) { currentchannelinstrumentvalue = currentchannelinstrument[ichannel][iInstr][1]; }
					}
					int icurrentinstrument = currentchannelinstrumentvalue;
					if (customMidifile[itrack][ievent].channel%16 == 9) {
						icurrentinstrument = 128;
					}
					int endTime = currentTime;
					for (int iFindEnd=ievent+1; iFindEnd<customMidifile[itrack].size(); iFindEnd++) {
						endTime = customMidifile[itrack][iFindEnd].tick;
						if (customMidifile[itrack][iFindEnd].type == 9) {
							if (customMidifile[itrack][iFindEnd].channel+iPort[itrack]*16 == ichannel && customMidifile[itrack][iFindEnd].data[0] == icurrentpitch) {
								break;
							}
						} else if (customMidifile[itrack][iFindEnd].type == 8) {
							if (customMidifile[itrack][iFindEnd].channel+iPort[itrack]*16 == ichannel && customMidifile[itrack][iFindEnd].data[0] == icurrentpitch) {
								break;
							}
						} else if (customMidifile[itrack][iFindEnd].type == 11) {
							if (customMidifile[itrack][iFindEnd].data[0] == 123) {
								break;
							}
						} else if (customMidifile[itrack][iFindEnd].type == 255) {
							if (customMidifile[itrack][iFindEnd].metaType == 47) {
								break;
							}
						}
					}
					if (endTime == currentTime) { continue; }
					vector<int> tempnote {itrack, ichannel, icurrentpitch, icurrentvolume, currentTime, endTime, icurrentinstrument};
					/*tempnote.track = itrack;
					tempnote.channel = ichannel;
					tempnote.pitch = icurrentpitch;
					tempnote.velocity = icurrentvolume;
					tempnote.starttime = currentTime;
					tempnote.endtime = endTime;
					tempnote.instrument = icurrentinstrument;*/
					Notes.push_back(tempnote);
					maxEndNote = (maxEndNote>currentTime)?maxEndNote:currentTime;
				}
			}
		}
		maxCurrentTimeInReal = (maxCurrentTimeInReal > currentTimeInReal) ? maxCurrentTimeInReal : currentTimeInReal;
	}
	
	for (int inote=0; inote<Notes.size(); inote++) {
		Notes[inote][5] = (Notes[inote][5]<maxEndNote+12*midifile.getTicksPerQuarterNote())?Notes[inote][5]:maxEndNote+12*midifile.getTicksPerQuarterNote();
	}
	maxLengthTrack = (maxLengthTrack<maxEndNote+12*midifile.getTicksPerQuarterNote())?maxLengthTrack:maxEndNote+12*midifile.getTicksPerQuarterNote();
	maxCurrentTimeInReal = ceil(maxCurrentTimeInReal);
	
	vector<vector<vector<vector<int>>>> newSustainTrackChannel (tracks, vector<vector<vector<int>>> (iMaxChannel, vector<vector<int>> {}));
	for (int i=0; i<tracks; i++) {
		for (int j=0; j<iMaxChannel; j++) {
			if (sustainTrackChannel[i][j].size() == 0) { continue; }
			vector<int> deathnote (sustainTrackChannel[i][j].size(), 1);
			deathnote[0] = 0;
			deathnote[sustainTrackChannel[i][j].size()-1] = 0;
			for (int k=0; k<sustainTrackChannel[i][j].size()-1; k++) {
				for (int l=k+1; l<sustainTrackChannel[i][j].size(); l++) {
					if (sustainTrackChannel[i][j][k][2] != sustainTrackChannel[i][j][l][2]) {
						continue;
					} else {
						if ((sustainTrackChannel[i][j][k][1]-63.5)*(sustainTrackChannel[i][j][l][1]-63.5)<0) {
							deathnote[l] = 0;
						}
						break;
					}
				}
				if (deathnote[k]!=0) {
					continue;
				}
			}
			for (int k=0; k<sustainTrackChannel[i][j].size(); k++) {
				if (deathnote[k]==0) {
					newSustainTrackChannel[i][j].push_back(sustainTrackChannel[i][j][k]);
				}
			}
		}
	}
	sustainTrackChannel = newSustainTrackChannel;
	
	/*isRestricted = false;
	for (i=0; i<tracks; i++) {
		for (j=0; j<iMaxChannel; j++) {
			if (listTrackChannel[i][j]==false) {
				isRestricted = true;
			}
		}
	}
	for (i=0; i<tracks; i++) {
		for (j=0; j<iMaxChannel; j++) {
			if (sustainTrackChannel[i][j].length == 0) { continue; }
			for (k=0; k<sustainTrackChannel[i][j].length; k++) {
				deathnotemax = 0;
				deathnotemin = sustainTrackChannel[i][j][k][0];
				for (inote=0; inote<Notes.length; inote++) {
					curnote = Notes[inote];
					if (i==curnote[0]) {
						if (command=='악보' && iFixedInstrument==-1) {
							if (isRestricted) {
								if (j==curnote[1]) {
									if (sustainTrackChannel[i][j][k][0] < curnote[5] && sustainTrackChannel[i][j][k][0] > curnote[4]) {
										deathnotemin = (deathnotemin>curnote[4])?curnote[4]:deathnotemin;
									}
									if (sustainTrackChannel[i][j][k][0] > curnote[5]) {
										deathnotemax = (deathnotemax<curnote[5])?curnote[5]:deathnotemax;
									}
								}
							} else {
								if (sustainTrackChannel[i][j][k][0] < curnote[5] && sustainTrackChannel[i][j][k][0] > curnote[4]) {
									deathnotemin = (deathnotemin>curnote[4])?curnote[4]:deathnotemin;
								}
								if (sustainTrackChannel[i][j][k][0] > curnote[5]) {
									deathnotemax = (deathnotemax<curnote[5])?curnote[5]:deathnotemax;
								}
							}
						} else {
							if (sustainTrackChannel[i][j][k][2]==curnote[6]) {
								if (sustainTrackChannel[i][j][k][0] < curnote[5] && sustainTrackChannel[i][j][k][0] > curnote[4]) {
									deathnotemin = (deathnotemin>curnote[4])?curnote[4]:deathnotemin;
								}
								if (sustainTrackChannel[i][j][k][0] > curnote[5]) {
									deathnotemax = (deathnotemax<curnote[5])?curnote[5]:deathnotemax;
								}
							}
						}
					}
				}
				sustainTrackChannel[i][j][k][0] = (deathnotemin>deathnotemax)?deathnotemin:deathnotemax;
			}
		}
	}*/
	
	ofstream outputMidiTempo("03tempomat.sayang_"+to_string(MYPE));
	for (int iTempo=0; iTempo<iMaxTempo; iTempo++) {
		outputMidiTempo << tempomat[iTempo][0] << " " << tempomat[iTempo][1] << '\n';
	}
	
	ofstream outputMidiTimedivision("04timedivision.sayang_"+to_string(MYPE));
	outputMidiTimedivision << midifile.getTicksPerQuarterNote() << '\n';
	
	/*if (command == 'timesigmat') { 
		// for (i=0; i<tracks; i++) {
			for (j=0; j<iMaxTimeSignature; j++) {
				for (k=0; k<2; k++) {
					console.log(timesigmat[j][k])
				}
			}
		// }
		return;
	}*/
	
	ofstream outputMidiSustainTrackChannel("05sustaintrackchannel.sayang_"+to_string(MYPE));
	for (int i=0; i<tracks; i++) {
		outputMidiSustainTrackChannel << "track " << i << '\n';
		for (int j=0; j<iMaxChannel; j++) {
			if (sustainTrackChannel[i][j].size() == 0) { continue; }
			outputMidiSustainTrackChannel << "channel " << j << '\n';
			for (int k=0; k<sustainTrackChannel[i][j].size(); k++) {
				outputMidiSustainTrackChannel << sustainTrackChannel[i][j][k][0] << '\t' << sustainTrackChannel[i][j][k][1] << '\t' << sustainTrackChannel[i][j][k][2] << '\n';
			}
		}
	}
	
	ofstream outputMidiNotes("06notes.sayang_"+to_string(MYPE));
	int tempNoteSize = Notes.size();
	for (int i=0; i<tempNoteSize; i++) {
		outputMidiNotes << Notes[i][0] << '\t' << Notes[i][1] << '\t' << Notes[i][2] << '\t' << Notes[i][3] << '\t' << Notes[i][4] << '\t' << Notes[i][5] << '\t' << Notes[i][6] << '\n';
	}
	
	stable_sort(Notes.begin(), Notes.end(), sortFunctionByStarttime);
	
	ofstream outputMidiNotesSorted("07notes_sorted.sayang_"+to_string(MYPE));
	for (int i=0; i<Notes.size(); i++) {
		outputMidiNotesSorted << Notes[i][0] << '\t' << Notes[i][1] << '\t' << Notes[i][2] << '\t' << Notes[i][3] << '\t' << Notes[i][4] << '\t' << Notes[i][5] << '\t' << Notes[i][6] << '\n';
	}
	
	ofstream outputMidiInstr("08currentchannelinstrument.sayang_"+to_string(MYPE));
	for (int ichannel=0; ichannel<iMaxChannel; ichannel++) {
		outputMidiInstr << "channel " << ichannel << '\n';
		for (int iTempo=0; iTempo<currentchannelinstrument[ichannel].size(); iTempo++) {
			outputMidiInstr << currentchannelinstrument[ichannel][iTempo][0] << " " << currentchannelinstrument[ichannel][iTempo][1] << " " << currentchannelinstrument[ichannel][iTempo][2] << '\n';
		}
	}
	
	ofstream outputMidiVolume("09currentchannelvolume.sayang_"+to_string(MYPE));
	for (int ichannel=0; ichannel<iMaxChannel; ichannel++) {
		outputMidiVolume << "channel " << ichannel << '\n';
		for (int iTempo=0; iTempo<currentchannelvolume[ichannel].size(); iTempo++) {
			outputMidiVolume << currentchannelvolume[ichannel][iTempo][0] << " " << currentchannelvolume[ichannel][iTempo][1] << '\n';
		}
	}
	
	for (int inote=0; inote<Notes.size(); inote++) {
		itrack = Notes[inote][0];
		ichannel = Notes[inote][1];
		if (instrumentTrackChannel[itrack][ichannel].size() > 0) {
			if (instrumentTrackChannel[itrack][ichannel][instrumentTrackChannel[itrack][ichannel].size()-1][1] != Notes[inote][6]) {
				instrumentTrackChannel[itrack][ichannel].push_back(vector<int> {Notes[inote][5], Notes[inote][6]});
			}
		} else {
			instrumentTrackChannel[itrack][ichannel].push_back(vector<int> {Notes[inote][5], Notes[inote][6]});
		}
	}
	
	stable_sort(textMat.begin(), textMat.end(), sortFunctionByTexttime);
	
	for (int inote=0; inote<Notes.size(); inote++) {
		if (count(cuttableMat.begin(), cuttableMat.end(), Notes[inote][5])) { continue; }
		bool isInterrupted = false;
		// if (Notes[inote][5]%(4*tickperquarter/noteResolution) != 0) {
			// isInterrupted = true;
			// continue;
		// }
		for (int jnote=0; jnote<inote; jnote++) {
			if (Notes[jnote][5] > Notes[inote][5] && Notes[jnote][4] < Notes[inote][5]) {
				isInterrupted = true;
				break;
			}
		}
		if (!isInterrupted) {
			cuttableMat.push_back(Notes[inote][5]);
		}
	}
	for (int inote=0; inote<Notes.size(); inote++) {
		if (count(cuttableMat.begin(), cuttableMat.end(), Notes[inote][4])) { continue; }
		bool isInterrupted = false;
		// if (Notes[inote][4]%(4*tickperquarter/noteResolution) != 0) {
			// isInterrupted = true;
			// continue;
		// }
		for (int jnote=0; jnote<Notes.size(); jnote++) {
			if (Notes[jnote][4] < Notes[inote][4] && Notes[jnote][5] > Notes[inote][4]) {
				isInterrupted = true;
				break;
			}
		}
		if (!isInterrupted) {
			cuttableMat.push_back(Notes[inote][4]);
		}
	}
	for (int i=0; i<tracks; i++) {
		for (int j=0; j<iMaxChannel; j++) {
			if (sustainTrackChannel[i][j].size() == 0) { continue; }
			for (int k=0; k<sustainTrackChannel[i][j].size(); k++) {
				bool isInterrupted = false;
				// if (sustainTrackChannel[i][j][k][0]%(4*tickperquarter/noteResolution) != 0) {
					// isInterrupted = true;
					// continue;
				// }
				for (int jnote=0; jnote<Notes.size(); jnote++) {
					if (Notes[jnote][4] < sustainTrackChannel[i][j][k][0] && Notes[jnote][5] > sustainTrackChannel[i][j][k][0]) {
						isInterrupted = true;
						break;
					}
				}
				if (!isInterrupted) {
					cuttableMat.push_back(sustainTrackChannel[i][j][k][0]);
				}
			}
		}
	}
	cuttableMat.push_back(86400*2*midifile.getTicksPerQuarterNote());
	if (cuttableMat.size() == 0) {
		cuttableMat.push_back(0);
	}
	if (cuttableMat[0] != 0) {
		cuttableMat.insert(cuttableMat.begin(), 0);
	}
	
	stable_sort(cuttableMat.begin(), cuttableMat.end());
	
	ofstream outputMidiCuttable("10cuttablemat.sayang_"+to_string(MYPE));
	for (int i=0; i<cuttableMat.size(); i++) {
		outputMidiCuttable << cuttableMat[i] << '\n';
	}
	
	//////////////////// below 32 over 255
	
	tempomat_before_transform = tempomat;
	
	for (int iTempo=0; iTempo<iMaxTempo; iTempo++) {
		tempomat[iTempo][0] = transform_tempo(tempomat[iTempo][0], tempomat_before_transform);
		while (tempomat[iTempo][1] < 32) { tempomat[iTempo][1] *= 2; }
		while (tempomat[iTempo][1] > 255) { tempomat[iTempo][1] /= 2; }
	}
	
	ofstream outputMidiTempo2("03tempomat_transformed.sayang_"+to_string(MYPE));
	for (int iTempo=0; iTempo<iMaxTempo; iTempo++) {
		outputMidiTempo2 << tempomat[iTempo][0] << " " << tempomat[iTempo][1] << '\n';
	}
	
	for (int i=0; i<tracks; i++) {
		for (int j=0; j<iMaxChannel; j++) {
			if (sustainTrackChannel[i][j].size() == 0) { continue; }
			for (int k=0; k<sustainTrackChannel[i][j].size(); k++) {
				sustainTrackChannel[i][j][k][0] = transform_tempo(sustainTrackChannel[i][j][k][0], tempomat_before_transform);
			}
		}
	}
	
	ofstream outputMidiSustainTrackChannel2("05sustaintrackchannel_transformed.sayang_"+to_string(MYPE));
	for (int i=0; i<tracks; i++) {
		outputMidiSustainTrackChannel2 << "track " << i << '\n';
		for (int j=0; j<iMaxChannel; j++) {
			if (sustainTrackChannel[i][j].size() == 0) { continue; }
			outputMidiSustainTrackChannel2 << "channel " << j << '\n';
			for (int k=0; k<sustainTrackChannel[i][j].size(); k++) {
				outputMidiSustainTrackChannel2 << sustainTrackChannel[i][j][k][0] << '\t' << sustainTrackChannel[i][j][k][1] << '\t' << sustainTrackChannel[i][j][k][2] << '\n';
			}
		}
	}
	
	for (int i=0; i<tempNoteSize; i++) {
		Notes[i][4] = transform_tempo(Notes[i][4], tempomat_before_transform);
		Notes[i][5] = transform_tempo(Notes[i][5], tempomat_before_transform);
	}
	
	ofstream outputMidiNotes2("06notes_transformed.sayang_"+to_string(MYPE));
	tempNoteSize = Notes.size();
	for (int i=0; i<tempNoteSize; i++) {
		outputMidiNotes2 << Notes[i][0] << '\t' << Notes[i][1] << '\t' << Notes[i][2] << '\t' << Notes[i][3] << '\t' << Notes[i][4] << '\t' << Notes[i][5] << '\t' << Notes[i][6] << '\n';
	}
	
	quarterMat = vector<int> {};
	int currentQuarter = 0;
	if (cuttableMat.size() > 0) {
		while (currentQuarter < cuttableMat[cuttableMat.size()-1]) {
			quarterMat.push_back(currentQuarter);
			currentQuarter += tickperquarter;
		}
	}
	quarterMat.push_back(currentQuarter);
	for (int i=0; i<quarterMat.size(); i++) {
		quarterMat[i] = transform_tempo(quarterMat[i], tempomat_before_transform);
	}
	
	ofstream outputMidiQuarter("10quartermat_transformed.sayang_"+to_string(MYPE));
	for (int i=0; i<quarterMat.size(); i++) {
		outputMidiQuarter << quarterMat[i] << '\n';
	}
	
	for (int i=0; i<cuttableMat.size(); i++) {
		cuttableMat[i] = transform_tempo(cuttableMat[i], tempomat_before_transform);
	}
	
	ofstream outputMidiCuttable2("10cuttablemat_transformed.sayang_"+to_string(MYPE));
	for (int i=0; i<cuttableMat.size(); i++) {
		outputMidiCuttable2 << cuttableMat[i] << '\n';
	}
	
	if (!lOnOff_candidate) {
		maxLengthTrack = transform_tempo(maxLengthTrack, tempomat_before_transform);
		timeStartNote = transform_tempo(timeStartNote, tempomat_before_transform);
		timeEndNote = transform_tempo(timeEndNote, tempomat_before_transform);
	}

	return;
}