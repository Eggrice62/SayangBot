#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>

using namespace std;

/*struct midiNote {
	int track;
	int channel;
	int pitch;
	int velocity;
	int starttime;
	int endtime;
	int instrument;
};*/
	
struct customMidiEvent {
	int tick;
	int type;
	int metaType;
	int channel = -1;
	vector<int> data;
};

struct midiTextEvent {
	int tick;
	string content;
};

vector<string> outputSayang;

int iMaxTrack = 128;
int iMaxChannel = 256;

bool iOnoffSustain = true;
bool iOnoffMotion = false;
bool lOnoffHorizontalDivision = true;
bool lOnoffVerticalDivision = false;
bool lOnoffMotionSpeedTempo = false;
bool isMML = false;
bool isTriplet = false;
bool isAutoEnhance = true;
bool isAutoDaecheInstrument = true;
bool isStrict = false;
bool isEnglish = false;

double cooldownMotionInQuarter = 7;
double volumeAdd1 = 0.;
double volumeMul2 = 1.;
double volumeAdd3 = 0.;
double tempoMulti = 1.;

int pitchLowerBound = -999;
int pitchUpperBound = 999;
int noteResolution = 64;
int tempoResolution = 1;
int susResolution = 1;
int iOnoffDrum = 0;
int iMethodDup = 1;
int lengthHorizontalDivision = 10000;
int iFixedTempo = -1;
int maxCutLength = 0;
int timeStartNote = 0;
int timeEndNote = 2147483647;

bool lOnOff_candidate = false;
vector<vector<vector<string>>> writtenFileList = {};
vector<vector<vector<int>>> writtenCharacterList = {};
vector<vector<vector<int>>> writtenScoreList = {};
vector<vector<vector<int>>> writtenChordList = {};
vector<bool> isNotPercurssion = {};

string command;
bool errorNow = false;

vector<int> iFixedInstrument (1, -1);

int tracks = 1;
int tickperquarter = 320;
	
vector<vector<int>> numNoteTrackChannel (iMaxTrack, vector<int> (iMaxChannel, 0));
vector<vector<vector<vector<int>>>> sustainTrackChannel (iMaxTrack, vector<vector<vector<int>>> (iMaxChannel, vector<vector<int>> {}));
vector<vector<vector<vector<int>>>> sustainTrackChannel_orig (iMaxTrack, vector<vector<vector<int>>> (iMaxChannel, vector<vector<int>> {}));
vector<vector<int>> Notes {};
vector<vector<int>> Notes_orig {};
vector<vector<vector<vector<int>>>> instrumentTrackChannel (iMaxTrack, vector<vector<vector<int>>> (iMaxChannel, vector<vector<int>> {}));
vector<string> trackName (iMaxTrack, "");
vector<int> iMaxKeySignatureTrack (iMaxTrack, 0);
vector<int> iPort (iMaxTrack, 0);
int maxLengthTrack = 0;
int maxEndNote = 0;
int iMaxTempo = 0;
int iMaxTimeSignature = 0;
int iMaxText = 0;
int iMaxCopyright = 0;
int iMaxKeySignature = 0;
bool isWarned255 = false;
bool isWarned11 = false;
bool isWarned = false;
bool isDrumWarned = false;
bool isLengthWarned = false;
bool isChordWarned = false;
bool is24NoteWarned = false;
vector<vector<vector<int>>> currentchannelvolume (iMaxChannel, vector<vector<int>> (1, vector<int> {0, 127}));
vector<vector<vector<int>>> currentchannelinstrument (iMaxChannel, vector<vector<int>> {});
vector<vector<int>> tempomat (iMaxTempo, vector<int> {-1, -1});
vector<vector<int>> tempomat_orig (iMaxTempo, vector<int> {-1, -1});
vector<vector<int>> tempomat_orig_orig (iMaxTempo, vector<int> {-1, -1});
vector<vector<int>> tempomat_before_transform (iMaxTempo, vector<int> {-1, -1});
vector<vector<int>> susmat (iMaxTempo, vector<int> {-1, -1});
vector<vector<int>> susmat_orig (iMaxTempo, vector<int> {-1, -1});
int currentTempoIndex = 0;
double maxCurrentTimeInReal = 0.;
vector<vector<int>> timesigmat (iMaxTimeSignature, vector<int> {-1, -1});
vector<midiTextEvent> textMat {};
int currentTextIndex = 0;
vector<midiTextEvent> copyrightMat (iMaxCopyright, midiTextEvent {-1, ""});
int currentCopyrightIndex = 0;
// var timekeymat = createArray(tracks, iMaxKeySignature, 2);
// for (i=0; i<tracks; i++) { for (j=0; j<iMaxKeySignature; j++) { for (k=0; k<2; k++) { timekeymat[i][j][k] = -1; } } }
vector<int> cuttableMat;
vector<int> icuttable;

string outputtxt;
string outputtxtRollback;

string originalFileName;

void read_input(int a, char *b);
void parse_midi();
void do_command();

int itrack;
int ichannel;
vector<vector<bool>> listTrackChannel(iMaxTrack, vector<bool> (iMaxChannel, false));

int iInstr;

string tempMessage;

void write_file_from_string_vector(string path, vector<string> vectorstr) {
	ofstream outputFile(path.data());
	if ( outputFile.is_open() ) {
		for (int i=0; i<vectorstr.size(); i++) {
			outputFile << vectorstr[i];
		}
		outputFile.close();
	}
	return;
}

void write_file_from_string_vector_with_endl(string path, vector<string> vectorstr) {
	ofstream outputFile(path.data());
	if ( outputFile.is_open() ) {
		for (int i=0; i<vectorstr.size(); i++) {
			outputFile << vectorstr[i] << '\n';
		}
		outputFile.close();
	}
	return;
}

void append_error_to_vectorstr(vector<string> *v, string s) {
	v->push_back("Error\n");
	v->push_back(s+"\n");
	return;
}

void append_warning_to_vectorstr(vector<string> *v, string s) {
	v->push_back("Warning\n");
	v->push_back(s+"\n");
	return;
}

void append_information_to_vectorstr(vector<string> *v, string s) {
	v->push_back("Information\n");
	v->push_back(s+"\n");
	return;
}

void append_file_to_vectorstr(vector<string> *v, string s) {
	v->push_back("FileName\n");
	v->push_back(s+"\n");
	return;
}

void append_text_to_vectorstr(vector<string> *v, string s) {
	v->push_back("PlainText\n");
	v->push_back(s+"\n");
	return;
}

vector<int> instrumentName2num(string s) {
	if (s.compare("피아노")==0) {vector<int> v = {0, 1}; return v;}
	if (s.compare("기타")==0) {vector<int> v = {24}; return v;}
	if (s.compare("클라리넷")==0) {vector<int> v = {71}; return v;}
	if (s.compare("하프")==0) {vector<int> v = {46}; return v;}
	if (s.compare("팀파니")==0) {vector<int> v = {47}; return v;}
	if (s.compare("일렉트릭기타")==0) {vector<int> v = {29, 30}; return v;}
	if (s.compare("베이스기타")==0) {vector<int> v = {33}; return v;}
	if (s.compare("탐탐")==0) {vector<int> v = {117}; return v;}
	if (s.compare("바이올린")==0) {vector<int> v = {40, 41, 48, 49}; return v;}
	if (s.compare("첼로")==0) {vector<int> v = {42, 43}; return v;}
	if (s.compare("팬플루트")==0) {vector<int> v = {75}; return v;}
	if (s.compare("색소폰")==0) {vector<int> v = {65, 66, 67, 64}; return v;}
	if (s.compare("트롬본")==0) {vector<int> v = {57, 58}; return v;}
	if (s.compare("트럼펫")==0) {vector<int> v = {56, 59, 61}; return v;}
	if (s.compare("오카리나")==0) {vector<int> v = {79, 78}; return v;}
	if (s.compare("어쿠스틱베이스")==0) {vector<int> v = {32}; return v;}
	if (s.compare("비브라폰")==0) {vector<int> v = {11, 98}; return v;}
	if (s.compare("전자피아노")==0) {vector<int> v = {4}; return v;}
	if (s.compare("스틸드럼")==0) {vector<int> v = {114}; return v;}
	if (s.compare("피크베이스기타")==0) {vector<int> v = {34}; return v;}
	if (s.compare("오보에")==0) {vector<int> v = {68, 69, 70}; return v;}
	if (s.compare("피치카토바이올린")==0) {vector<int> v = {45, 105, 107}; return v;}
	if (s.compare("하프시코드")==0) {vector<int> v = {6, 7}; return v;}
	if (s.compare("하모니카")==0) {vector<int> v = {22}; return v;}
	if (s.compare("실로폰")==0) {vector<int> v = {13, 14}; return v;}
	if (s.compare("리코더")==0) {vector<int> v = {74}; return v;}
	if (s.compare("첼레스타")==0) {vector<int> v = {8, 9, 10}; return v;}
	if (s.compare("심벌즈")==0) {vector<int> v = {129}; return v;}
	if (s.compare("큰북")==0) {vector<int> v = {130}; return v;}
	if (s.compare("작은북")==0) {vector<int> v = {131}; return v;}
	if (s.compare("프렛리스베이스기타")==0) {vector<int> v = {35}; return v;}
	if (s.compare("마림바")==0) {vector<int> v = {12}; return v;}
	if (s.compare("플루트")==0) {vector<int> v = {73, 72}; return v;}
	if (s.compare("홍키통크피아노")==0) {vector<int> v = {3}; return v;}
	if (s.compare("프렌치호른")==0) {vector<int> v = {60}; return v;}
	vector<int> v = {-1}; return v;
}

vector<string> instrumentNameList {"피아노", "기타", "클라리넷", "하프", "팀파니",
"일렉트릭기타", "베이스기타", "탐탐", "바이올린", "첼로",
"팬플루트", "색소폰", "트롬본", "트럼펫", "오카리나",
"어쿠스틱베이스", "비브라폰", "전자피아노", "스틸드럼", "피크베이스기타",
"오보에", "피치카토바이올린", "하프시코드", "하모니카", "실로폰",
"리코더", "첼레스타", "심벌즈", "큰북", "작은북",
"프렛리스베이스기타", "마림바", "플루트", "홍키통크피아노", "프렌치호른"};

string instr2name(int a) {
	vector<string> instrList = {"AcousticGrandPiano",
"BrightAcousticPiano",
"ElectricGrandPiano",
"Honky-tonkPiano",
"ElectricPiano1",
"ElectricPiano2",
"Harpsichord",
"Clavi",
"Celesta",
"Glockenspiel",
"MusicBox",
"Vibraphone",
"Marimba",
"Xylophone",
"TubularBells",
"Dulcimer",
"DrawbarOrgan",
"PercussiveOrgan",
"RockOrgan",
"ChurchOrgan",
"ReedOrgan",
"Accordion",
"Harmonica",
"TangoAccordion",
"AcousticGuitar(nylon)",
"AcousticGuitar(steel)",
"ElectricGuitar(jazz)",
"ElectricGuitar(clean)",
"ElectricGuitar(muted)",
"OverdrivenGuitar",
"DistortionGuitar",
"GuitarHarmonics",
"AcousticBass",
"ElectricBass(finger)",
"ElectricBass(pick)",
"FretlessBass",
"SlapBass1",
"SlapBass2",
"SynthBass1",
"SynthBass2",
"Violin",
"Viola",
"Cello",
"Contrabass",
"TremoloStrings",
"PizzicatoStrings",
"OrchestralHarp",
"Timpani",
"StringEnsemble1",
"StringEnsemble2",
"SynthStrings1",
"SynthStrings2",
"ChoirAahs",
"VoiceOohs",
"SynthVoice",
"OrchestraHit",
"Trumpet",
"Trombone",
"Tuba",
"MutedTrumpet",
"FrenchHorn",
"BrassSection",
"SynthBrass1",
"SynthBrass2",
"SopranoSax",
"AltoSax",
"TenorSax",
"BaritoneSax",
"Oboe",
"EnglishHorn",
"Bassoon",
"Clarinet",
"Piccolo",
"Flute",
"Recorder",
"PanFlute",
"Blownbottle",
"Shakuhachi",
"Whistle",
"Ocarina",
"Lead1(square)",
"Lead2(sawtooth)",
"Lead3(calliope)",
"Lead4(chiff)",
"Lead5(charang)",
"Lead6(voice)",
"Lead7(fifths)",
"Lead8(bass+lead)",
"Pad1(newage)",
"Pad2(warm)",
"Pad3(polysynth)",
"Pad4(choir)",
"Pad5(bowed)",
"Pad6(metallic)",
"Pad7(halo)",
"Pad8(sweep)",
"FX1(rain)",
"FX2(soundtrack)",
"FX3(crystal)",
"FX4(atmosphere)",
"FX5(brightness)",
"FX6(goblins)",
"FX7(echoes)",
"FX8(sci-fi)",
"Sitar",
"Banjo",
"Shamisen",
"Koto",
"Kalimba",
"Bagpipe",
"Fiddle",
"Shanai",
"TinkleBell",
"Agogô",
"SteelDrums",
"Woodblock",
"TaikoDrum",
"MelodicTom",
"SynthDrum",
"ReverseCymbal",
"GuitarFretNoise",
"BreathNoise",
"Seashore",
"BirdTweet",
"TelephoneRing",
"Helicopter",
"Applause",
"Gunshot",
"Drumset",
"Cymbal",
"BigDrum",
"SmallDrum"};
	return instrList[a];
}

void stdout_var(vector<int> v) {
	for (int i=0; i<v.size(); i++) {
		printf("%d\n",v[i]);
	}
}

void stdout_var(vector<string> v) {
	for (int i=0; i<v.size(); i++) {
		printf("%s\n",v[i].c_str());
	}
}

int myPow(int x, unsigned int p)
{
	if (p == 0) return 1;
	if (p == 1) return x;
	
	int tmp = myPow(x, p/2);
	if (p%2 == 0) return tmp * tmp;
	else return x * tmp * tmp;
}

string vectorint2str(vector<int> v) {
	string s = "";
	for (int i=1; i<v.size(); i++) { // midi data [0] = size
		s += (char) v[i];
	}
	return s;
}

bool sortby1stcol(const vector<int>& v1, const vector<int>& v2) {
	return v1[0] < v2[0];
}

bool sortFunctionByStarttime(const vector<int>& v1, const vector<int>& v2) {
	return v1[4] < v2[4];
}

bool sortFunctionByTexttime(const midiTextEvent& v1, const midiTextEvent& v2) {
	return v1.tick < v2.tick;
}

string scaletoname(int scalenumber) {
	string outname = "";
	if (scalenumber ==  -1) { outname = "c-"; }
	if (scalenumber ==  0) { outname = "c"; }
	if (scalenumber ==  1) { outname = "c+"; }
	if (scalenumber ==  2) { outname = "d"; }
	if (scalenumber ==  3) { outname = "d+"; }
	if (scalenumber ==  4) { outname = "e"; }
	if (scalenumber ==  5) { outname = "f"; }
	if (scalenumber ==  6) { outname = "f+"; }
	if (scalenumber ==  7) { outname = "g"; }
	if (scalenumber ==  8) { outname = "g+"; }
	if (scalenumber ==  9) { outname = "a"; }
	if (scalenumber == 10) { outname = "a+"; }
	if (scalenumber == 11) { outname = "b"; }
	if (scalenumber == 12) { outname = "b+"; }
	
	return outname;
}

int notetotime2(int note, int tpq) {
	return tpq*4/note;
}

vector<int> get_length_list2_expand(int dur, int tpq, int resolution) {
	vector<int> larray {};
	int curres = 1;
	int tempResolution = 10;
	while (dur > 0 && curres <= resolution) {
		if (abs(dur - 4*tpq) < 4*tpq/tempResolution) {
		// if (abs(dur - 4*tpq) < 4*tpq/resolution && dur > 4*tpq) {
			larray.push_back(1);
			break;
		} else if (abs(dur - 3*tpq) < 3*tpq/tempResolution) {
		// } else if (abs(dur - 3*tpq) < 3*tpq/resolution && dur > 3*tpq) {
			larray.push_back(2);
			larray.push_back(4);
			break;
		} else if (abs(dur - 2*tpq) < 2*tpq/tempResolution) {
		// } else if (abs(dur - 2*tpq) < 2*tpq/resolution && dur > 2*tpq) {
			larray.push_back(2);
			break;
		} else if (abs(dur - 3*tpq/2) < 3*tpq/2/tempResolution) {
			larray.push_back(4);
			larray.push_back(8);
			break;
		} else if (abs(dur - tpq) < tpq/tempResolution) {
			larray.push_back(4);
			break;
		} else if (abs(dur - tpq/2) < tpq/2/tempResolution) {
			larray.push_back(8);
			break;
		// } else if (abs(dur - tpq/3) < tpq/3/resolution && dur > tpq/3) {
			// larray.push_back(12]);
			// break
		} else if (abs(dur - tpq/4) < tpq/4/tempResolution) {
			larray.push_back(16);
			break;
		// } else if (abs(dur - tpq/6) < tpq/6/resolution && dur > tpq/6
			// larray.push_back(24]);
			// break
		} else {
			if (dur >= 4*tpq / curres) {
				larray.push_back(curres);
				dur = dur - 4*tpq / curres;
			}
			if (dur < 4*tpq / curres) {
				curres = curres * 2;
			}
		}
	}
	if (larray.size() == 0) { larray = vector<int> {resolution}; }
	
	return larray;
}

vector<int> get_length_list2_nonexpand(int dur, int tpq, int resolution) {
	vector<int> larray {};
	int curres = 1;
	int tempResolution = 1;
	while (dur > 4*tpq/resolution/2 && curres <= resolution) {
		if (abs(dur - 4*tpq) < tempResolution*4*tpq/resolution && dur > 4*tpq) {
			larray.push_back(1);
			break;
		} else if (abs(dur - 3*tpq) < tempResolution*3*tpq/resolution && dur > 3*tpq) {
			larray.push_back(2);
			larray.push_back(4);
			break;
		} else if (abs(dur - 2*tpq) < tempResolution*2*tpq/resolution && dur > 2*tpq) {
			larray.push_back(2);
			break;
		} else if (abs(dur - 3*tpq/2) < tempResolution*3*tpq/2/resolution && dur > 3*tpq/2) {
			larray.push_back(4);
			larray.push_back(8);
			break;
		} else if (abs(dur - tpq) < tempResolution*tpq/resolution && dur > tpq) {
			larray.push_back(4);
			break;
		} else if (abs(dur - tpq/2) < tempResolution*tpq/2/resolution && dur > tpq/2) {
			larray.push_back(8);
			break;
		// } else if (abs(dur - tpq/3) < tempResolution*tpq/3/resolution && dur > tpq/3) {
			// larray.push_back(12]);
			// break
		} else if (abs(dur - tpq/4) < tempResolution*tpq/4/resolution && dur > tpq/4) {
			larray.push_back(16);
			break;
		// } else if (abs(dur - tpq/6) < tempResolution*tpq/6/resolution && dur > tpq/6
			// larray.push_back(24]);
			// break
		} else {
			if (dur >= 4*tpq / curres) {
				larray.push_back(curres);
				dur = dur - 4*tpq / curres;
			}
			if (dur < 4*tpq / curres) {
				curres = curres * 2;
			}
		}
	}
	
	return larray;
}