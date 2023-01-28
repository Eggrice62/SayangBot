#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

int args1i, args2i;
double args1d;

void read_input() {
	vector<string> args;
	string tempinputstr;
	ifstream fileinput("00input.sayang");
	while (getline(fileinput, tempinputstr, ' ')) {
		args.push_back(tempinputstr);
	}
	command = args[0];
	args.erase(args.begin());
	// stdout_var(args);
	// cout << command << endl;
	
	if (args.size() > 0) {
		if (args.back() == "영어") {
			isEnglish = true;
			args.pop_back();
		}
	}
	
	while (args.size() > 0) {
		if (args[0] == "엄격하게") {
			isStrict = true; args.erase(args.begin());
		} else if (args[0] == "트랙") {
			stringstream ssInt(args[1]);
			ssInt >> args1i;
			if (!ssInt.fail()) {
				if (1<=args1i && args1i<=iMaxTrack) {
					for (ichannel=0; ichannel<iMaxChannel; ichannel++) { listTrackChannel[args1i-1][ichannel] = true; }
				} else {
					if (!isEnglish) {
						append_warning_to_vectorstr(&outputSayang, "입력한 트랙 번호 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 트랙 번호는 1부터 " + to_string(iMaxTrack) + " 사이의 정수로 입력해 주세요.");
					} else {
						append_warning_to_vectorstr(&outputSayang, "The entered track number " + args[1] + " is invalid and will be ignored. Enter the track number as an integer between 1 and " + to_string(iMaxTrack) + ".");
					}
				}
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 트랙 번호 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 트랙 번호는 1부터 " + to_string(iMaxTrack) + " 사이의 정수로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered track number " + args[1] + " is invalid and will be ignored. Enter the track number as an integer between 1 and " + to_string(iMaxTrack) + ".");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "채널") {
			stringstream ssInt(args[1]);
			ssInt >> args1i;
			if (!ssInt.fail()) {
				if (1<=args1i && args1i<=iMaxChannel) {
					for (ichannel=0; ichannel<iMaxChannel; ichannel++) { listTrackChannel[itrack][args1i-1] = true; }
				} else {
					if (!isEnglish) {
						append_warning_to_vectorstr(&outputSayang, "입력한 채널 번호 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 채널 번호는 1부터 " + to_string(iMaxChannel) + " 사이의 정수로 입력해 주세요.");
					} else {
						append_warning_to_vectorstr(&outputSayang, "The entered channel number " + args[1] + " is invalid and will be ignored. Enter the channel number as an integer between 1 and " + to_string(iMaxChannel) + ".");
					}
				}
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 채널 번호 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 채널 번호는 1부터 " + to_string(iMaxChannel) + " 사이의 정수로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered channel number " + args[1] + " is invalid and will be ignored. Enter the channel number as an integer between 1 and " + to_string(iMaxChannel) + ".");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "트랙채널") {
			stringstream ssInt(args[1]);
			stringstream ssInt2(args[2]);
			ssInt >> args1i;
			ssInt2 >> args2i;
			if (!ssInt.fail() && !ssInt2.fail()) {
				if (1<=args1i && args1i<=iMaxTrack && 1<=args2i && args2i<=iMaxChannel) {
					listTrackChannel[args1i-1][args2i-1] = true;
				} else {
					if (!isEnglish) {
						append_warning_to_vectorstr(&outputSayang, "입력한 트랙채널 번호 " + args[1] + "-" + args[2] + "이(가) 적절하지 않아 무시됩니다. 트랙 번호는 1부터 " + to_string(iMaxTrack) + " 사이의 정수로 입력해 주세요. 채널 번호는 1부터 " + to_string(iMaxChannel) + " 사이의 정수로 입력해 주세요.");
					} else {
						append_warning_to_vectorstr(&outputSayang, "The entered track/channel number " + args[1] + "-" + args[2] + " is invalid and will be ignored. Enter the track number as an integer between 1 and " + to_string(iMaxTrack) + ". Enter the channel number as an integer between 1 and " + to_string(iMaxChannel) + ".");
					}
				}
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 트랙채널 번호 " + args[1] + "-" + args[2] + "이(가) 적절하지 않아 무시됩니다. 트랙 번호는 1부터 " + to_string(iMaxTrack) + " 사이의 정수로 입력해 주세요. 채널 번호는 1부터 " + to_string(iMaxChannel) + " 사이의 정수로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered track/channel number " + args[1] + "-" + args[2] + " is invalid and will be ignored. Enter the track number as an integer between 1 and " + to_string(iMaxTrack) + ". Enter the channel number as an integer between 1 and " + to_string(iMaxChannel) + ".");
				}
			}
			args.erase(args.begin()); args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "서스테인") {
			if (args[1] == "켜기" or args[1] == "켬" or args[1] == "온" or args[1] == "on" or args[1] == "o") {
				iOnoffSustain = true;
			} else if (args[1] == "끄기" or args[1] == "끔" or args[1] == "오프" or args[1] == "off" or args[1] == "x") {
				iOnoffSustain = false;
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 서스테인 스위치 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 서스테인 스위치는 켜기, 끄기, 켬, 끔, 온, 오프, on, off, o, x로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered sustain switch " + args[1] + " is invalid and will be ignored. Enter the sustain switch as 켜기, 끄기, 켬, 끔, 온, 오프, on, off, o, or x");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "모션") {
			if (args[1] == "켜기" or args[1] == "켬" or args[1] == "온" or args[1] == "on" or args[1] == "o") {
				iOnoffMotion = true;
			} else if (args[1] == "끄기" or args[1] == "끔" or args[1] == "오프" or args[1] == "off" or args[1] == "x") {
				iOnoffMotion = false;
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 모션 스위치 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 모션 스위치는 켜기, 끄기, 켬, 끔, 온, 오프, on, off, o, x로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered motion switch " + args[1] + " is invalid and will be ignored. Enter the motion switch as 켜기, 끄기, 켬, 끔, 온, 오프, on, off, o, or x");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "시작") {
			stringstream ssInt(args[1]);
			ssInt >> args1i;
			if (!ssInt.fail()) {
				timeStartNote = args1i;
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 시작 시간 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 시작 시간은 틱 단위로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered start time " + args[1] + " is invalid and will be ignored. Please enter the start time in ticks.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "종료") {
			stringstream ssInt(args[1]);
			ssInt >> args1i;
			if (!ssInt.fail()) {
				timeEndNote = args1i;
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 종료 시간 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 종료 시간은 틱 단위로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered end time " + args[1] + " is invalid and will be ignored. Please enter the end time in ticks.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "최저음") {
			stringstream ssInt(args[1]);
			ssInt >> args1i;
			if (!ssInt.fail()) {
				pitchLowerBound = args1i;
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 최저음 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 최저음은 가온다(C4)=60을 기준으로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered lowest note " + args[1] + " is invalid and will be ignored. Please enter the lowest note as a guideline (C4)=60.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "최고음") {
			stringstream ssInt(args[1]);
			ssInt >> args1i;
			if (!ssInt.fail()) {
				pitchUpperBound = args1i;
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 최고음 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 최고음 가온다(C4)=60을 기준으로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered highest note " + args[1] + " is invalid and will be ignored. Please enter the highest note as a guideline (C4)=60.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "음표깎기") {
			stringstream ssDouble(args[1]);
			ssDouble >> args1d;
			if (!ssDouble.fail()) {
				if (args1d>=0. && args1d<=100.) {
					maxCutLength = args1d/100.;
				} else {
					if (!isEnglish) {
						append_warning_to_vectorstr(&outputSayang, "입력한 음표깎기 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 음표깎기는 0~100 사이의 숫자로 입력해 주세요.");
					} else {
						append_warning_to_vectorstr(&outputSayang, "The entered note cutoff length " + args[1] + " is invalid and will be ignored. Please enter a number between 0 and 100 for note cutoff length.");
					}
				}
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 음표깎기 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 음표깎기는 0~100 사이의 숫자로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered note cutoff length " + args[1] + " is invalid and will be ignored. Please enter a number between 0 and 100 for note cutoff length.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "해상도") {
			stringstream ssInt(args[1]);
			ssInt >> args1i;
			if (!ssInt.fail()) {
				if (0<=args1i && args1i<=6) {
					noteResolution = myPow(2,args1i);
				} else {
					if (!isEnglish) {
						append_warning_to_vectorstr(&outputSayang, "입력한 해상도 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 해상도는 0~6 사이의 정수로 입력해 주세요.");
					} else {
						append_warning_to_vectorstr(&outputSayang, "The entered resolution " + args[1] + " is invalid and will be ignored. Enter the resolution as an integer between 0 and 6.");
					}
				}
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 해상도 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 해상도는 0~6 사이의 정수로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered resolution " + args[1] + " is invalid and will be ignored. Enter the resolution as an integer between 0 and 6.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "템포뭉개기") {
			stringstream ssInt(args[1]);
			ssInt >> args1i;
			if (!ssInt.fail()) {
				if (0<=args1i && args1i<=6) {
					tempoResolution = myPow(2,args1i);
				} else {
					if (!isEnglish) {
						append_warning_to_vectorstr(&outputSayang, "입력한 템포뭉개기 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 템포뭉개기는 0~6 사이의 정수로 입력해 주세요.");
					} else {
						append_warning_to_vectorstr(&outputSayang, "The entered mergetempo " + args[1] + " is invalid and will be ignored. Enter the mergetempo as an integer between 0 and 6.");
					}
				}
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 템포뭉개기 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 템포뭉개기는 0~6 사이의 정수로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered mergetempo " + args[1] + " is invalid and will be ignored. Enter the mergetempo as an integer between 0 and 6.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "서스테인뭉개기") {
			stringstream ssInt(args[1]);
			ssInt >> args1i;
			if (!ssInt.fail()) {
				if (0<=args1i && args1i<=6) {
					susResolution = myPow(2,args1i);
				} else {
					if (!isEnglish) {
						append_warning_to_vectorstr(&outputSayang, "입력한 서스테인뭉개기 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 서스테인뭉개기는 0~6 사이의 정수로 입력해 주세요.");
					} else {
						append_warning_to_vectorstr(&outputSayang, "The entered mergesustain " + args[1] + " is invalid and will be ignored. Enter the mergesustain as an integer between 0 and 6.");
					}
				}
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 서스테인뭉개기 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 서스테인뭉개기는 0~6 사이의 정수로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered mergesustain " + args[1] + " is invalid and will be ignored. Enter the mergesustain as an integer between 0 and 6.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "모션쿨") {
			stringstream ssDouble(args[1]);
			ssDouble >> args1d;
			if (!ssDouble.fail()) {
				if (0<=args1d) {
					cooldownMotionInQuarter = args1d;
				} else {
					if (!isEnglish) {
						append_warning_to_vectorstr(&outputSayang, "입력한 모션 쿨타임 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 모션 쿨타임은 0 이상의 정수로 입력해 주세요.");
					} else {
						append_warning_to_vectorstr(&outputSayang, "The entered motion cooldown " + args[1] + " is invalid and will be ignored. Enter the motion cooldown time as an integer greater than or equal to 0.");
					}
				}
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 모션 쿨타임 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 모션 쿨타임은 0 이상의 정수로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered motion cooldown " + args[1] + " is invalid and will be ignored. Enter the motion cooldown time as an integer greater than or equal to 0.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "볼륨1") {
			stringstream ssDouble(args[1]);
			ssDouble >> args1d;
			if (!ssDouble.fail()) {
				volumeAdd1 = args1d;
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 볼륨 보정치 1 (" + args[1] + ") 이(가) 적절하지 않아 무시됩니다. 볼륨 보정치는 숫자로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered volume correction value 1 (" + args[1] + ") is invalid and will be ignored. Please enter the volume correction value as a number.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "볼륨2") {
			stringstream ssDouble(args[1]);
			ssDouble >> args1d;
			if (!ssDouble.fail()) {
				volumeMul2 = args1d;
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 볼륨 보정치 2 (" + args[1] + ") 이(가) 적절하지 않아 무시됩니다. 볼륨 보정치는 숫자로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered volume correction value 2 (" + args[1] + ") is invalid and will be ignored. Please enter the volume correction value as a number.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "볼륨3") {
			stringstream ssDouble(args[1]);
			ssDouble >> args1d;
			if (!ssDouble.fail()) {
				volumeAdd3 = args1d;
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 볼륨 보정치 3 (" + args[1] + ") 이(가) 적절하지 않아 무시됩니다. 볼륨 보정치는 숫자로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered volume correction value 3 (" + args[1] + ") is invalid and will be ignored. Please enter the volume correction value as a number.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "고정볼륨") {
			stringstream ssDouble(args[1]);
			ssDouble >> args1d;
			if (!ssDouble.fail()) {
				volumeMul2 = 0;
				volumeAdd3 = args1d;
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 고정 볼륨 (" + args[1] + ") 이(가) 적절하지 않아 무시됩니다. 고정 볼륨은 숫자로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered fixed volume (" + args[1] + ") is invalid and will be ignored. Please enter the fixed volume as a number.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "볼륨증가") {
			volumeMul2 = 0.5;
			volumeAdd3 = 8;
			args.erase(args.begin());
		} else if (args[0] == "볼륨감소") {
			volumeMul2 = 0.5;
			volumeAdd3 = 0;
			args.erase(args.begin());
		} else if (args[0] == "드럼제외") {
			iOnoffDrum = 1;
			args.erase(args.begin());
		} else if (args[0] == "드럼만") {
			iOnoffDrum = 2;
			args.erase(args.begin());
		} else if (args[0] == "중복제거") {
			stringstream ssInt(args[1]);
			ssInt >> args1i;
			if (!ssInt.fail()) {
				if (0<=args1i && args1i<=2) {
					iMethodDup = args1i;
				} else {
					if (!isEnglish) {
						append_warning_to_vectorstr(&outputSayang, "입력한 중복 음표 처리 기법 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 중복 음표 처리 기법은 0~2 사이의 정수로 입력해 주세요.");
					} else {
						append_warning_to_vectorstr(&outputSayang, "The entered duplicate note handling method " + args[1] + " is invalid and will be ignored. Please enter an integer between 0 and 2 for the duplicate note processing method.");
					}
				}
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 중복 음표 처리 기법 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 중복 음표 처리 기법은 0~2 사이의 정수로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered duplicate note handling method " + args[1] + " is invalid and will be ignored. Please enter an integer between 0 and 2 for the duplicate note processing method.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "길이분할") {
			if (!isEnglish) {
				append_warning_to_vectorstr(&outputSayang, "이제 길이분할 옵션은 기본으로 적용되며 입력하지 않으셔도 됩니다. 길이분할 기능을 끄려면 1만자무시 옵션을 사용해 주세요.");
			} else {
				append_warning_to_vectorstr(&outputSayang, "Now the lengthdivision option is applied by default and you do not need to enter it. To turn off the lengthdivision function, please use the option ignore10000.");
			}
			args.erase(args.begin());
		} else if (args[0] == "1만자무시") {
			lOnoffHorizontalDivision = false;
			args.erase(args.begin());
		} else if (args[0] == "화음분할") {
			lOnoffHorizontalDivision = false;
			lOnoffVerticalDivision = true;
			args.erase(args.begin());
		} else if (args[0] == "모션속도템포") {
			lOnoffMotionSpeedTempo = true;
			args.erase(args.begin());
		} else if (args[0] == "고정템포") {
			stringstream ssInt(args[1]);
			ssInt >> args1i;
			if (!ssInt.fail()) {
				if ((args1i >= 8 && args1i < 32) or (args1i > 255 && args1i <= 510)) {
					if (!isEnglish) {
						append_warning_to_vectorstr(&outputSayang, "입력한 고정 템포 (" + args[1] + ") 는 메이플스토리2에서 지원하는 템포 범위 (32~255) 를 벗어납니다. 하지만 SayangBot이 적당히 바꾸어서 만들어 드릴게요. 악보 코드의 템포가 입력값과 달라도 버그가 아닙니다!");
					} else {
						append_warning_to_vectorstr(&outputSayang, "The entered fixed tempo (" + args[1] + ") is outside the tempo range (32~255) supported by MapleStory 2. But SayangBot will change it appropriately and make it for you. It's not a bug if (the tempo of the score chord is different from the input value!");
					}
				} else if (args1i < 8 or args1i > 510) {
					if (!isEnglish) {
						append_warning_to_vectorstr(&outputSayang, "입력한 고정 템포 (" + args[1] + ") 는 메이플스토리2에서 지원하는 템포 범위 (32~255) 를 벗어납니다. 악보는 만들어지지만 인게임에서 템포 코드가 적용되지 않을 수 있습니다.");
					} else {
						append_warning_to_vectorstr(&outputSayang, "The entered fixed tempo (" + args[1] + ") is outside the tempo range (32~255) supported by MapleStory 2. Scores are created, but tempo chords may not be applied in-game.");
					}
				}
				iFixedTempo = args1i;
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 고정 템포 (" + args[1] + ") 이(가) 적절하지 않아 무시됩니다. 고정 템포는 숫자로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered fixed tempo (" + args[1] + ") is invalid and will be ignored. Please enter a fixed tempo as a number.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "최소템포") {
			stringstream ssInt(args[1]);
			ssInt >> args1i;
			if (!ssInt.fail()) {
				if ((args1i >= 8 && args1i < 32) or (args1i > 255 && args1i <= 510)) {
					if (!isEnglish) {
						append_warning_to_vectorstr(&outputSayang, "입력한 최소 템포 (" + args[1] + ") 는 메이플스토리2에서 지원하는 템포 범위 (32~255) 를 벗어납니다. 하지만 SayangBot이 적당히 바꾸어서 만들어 드릴게요. 악보 코드의 템포가 입력값과 달라도 버그가 아닙니다!");
					} else {
						append_warning_to_vectorstr(&outputSayang, "The entered fixed tempo (" + args[1] + ") is outside the tempo range (32~255) supported by MapleStory 2. But SayangBot will change it appropriately and make it for you. It's not a bug if (the tempo of the score chord is different from the input value!");
					}
				} else if (args1i < 8 or args1i > 510) {
					if (!isEnglish) {
						append_warning_to_vectorstr(&outputSayang, "입력한 최소 템포 (" + args[1] + ") 는 메이플스토리2에서 지원하는 템포 범위 (32~255) 를 벗어납니다. 악보는 만들어지지만 인게임에서 템포 코드가 적용되지 않을 수 있습니다.");
					} else {
						append_warning_to_vectorstr(&outputSayang, "The entered fixed tempo (" + args[1] + ") is outside the tempo range (32~255) supported by MapleStory 2. Scores are created, but tempo chords may not be applied in-game.");
					}
				}
				iMinTempo = args1i;
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 최소 템포 (" + args[1] + ") 이(가) 적절하지 않아 무시됩니다. 최소 템포는 숫자로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered fixed tempo (" + args[1] + ") is invalid and will be ignored. Please enter a fixed tempo as a number.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "조옮김") {
			stringstream ssInt(args[1]);
			ssInt >> args1i;
			if (!ssInt.fail()) {
				iPitchShift = args1i;
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 조옮김 (" + args[1] + ") 이(가) 적절하지 않아 무시됩니다. 조옮김은 숫자로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered transpose (" + args[1] + ") is invalid and will be ignored. Please enter a transpose as a number.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "악기") {
			vector<int> instrumentKeyList = instrumentName2num(args[1]);
			if (instrumentKeyList[0] != -1) {
				iFixedInstrument = instrumentName2num(args[1]);
			} else {
				bool isInMidi = false;
				int iNumFixed = -1;
				for (iInstr=0; iInstr<128; iInstr++) {
					if (instr2name(iInstr)==args[1]) {
						isInMidi = true;
						iNumFixed = iInstr;
					}
				}
				if (isInMidi) {
					vector<int> iFixedInstrument (1, iNumFixed);
				} else {
					if (!isEnglish) {
						tempMessage = "입력한 악기 이름 (" + args[1] + ") 은 SayangBot이 모르는 악기이므로 무시됩니다. SayangBot이 지원하는 악기는 ";
					} else {
						tempMessage = "The entered instrument name (" + args[1] + ") will be ignored as SayangBot does not know it. The instruments supported by SayangBot are) { ";
					}
					tempMessage += "피아노, 기타, 클라리넷, 하프, 팀파니, 일렉트릭기타, 베이스기타, 탐탐, 바이올린, 첼로, 팬플루트, 색소폰, 트롬본, 트럼펫, 오카리나, 어쿠스틱베이스, 비브라폰, 전자피아노, 스틸드럼, 피크베이스기타, 오보에, 피치카토바이올린, 하프시코드, 하모니카, 실로폰, 리코더, 첼레스타, 심벌즈, 큰북, 작은북, 프렛리스베이스기타, 마림바, 플루트, 홍키통크피아노";
					for (iInstr=0; iInstr<128; iInstr++) {
						tempMessage += ", ";
						tempMessage += instr2name(iInstr);
					}
					tempMessage += ".";
					append_warning_to_vectorstr(&outputSayang, tempMessage);
					errorNow = true;
					return;
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "mml") {
			isMML = true;
			args.erase(args.begin());
		} else if (args[0] == "셋잇단") {
			isTriplet = true;
			args.erase(args.begin());
		} else if (args[0] == "자동볼륨증가") {
			if (args[1] == "켜기" or args[1] == "켬" or args[1] == "온" or args[1] == "on" or args[1] == "o") {
				isAutoEnhance = true;
			} else if (args[1] == "끄기" or args[1] == "끔" or args[1] == "오프" or args[1] == "off" or args[1] == "x") {
				isAutoEnhance = false;
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 자동볼륨증가 스위치 " + args[1] + "이(가) 적절하지 않아 무시됩니다. 자동볼륨증가 스위치는 켜기, 끄기, 켬, 끔, 온, 오프, on, off, o, x로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered auto volume increase switch " + args[1] + " is invalid and will be ignored. Enter the auto volume increase switch as 켜기, 끄기, 켬, 끔, 온, 오프, on, off, o, or x");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "대체악기사용안함") {
			isAutoDaecheInstrument = false;
			args.erase(args.begin());
		} else if (args[0] == "트랙악기분리") {
			lSameTrackInstrument = true;
			args.erase(args.begin());
		} else if (args[0] == "템포배수") {
			stringstream ssDouble(args[1]);
			ssDouble >> args1d;
			if (!ssDouble.fail()) {
				tempoMulti = args1d;
			} else {
				if (!isEnglish) {
					append_warning_to_vectorstr(&outputSayang, "입력한 템포 배수 (" + args[1] + ") 이(가) 적절하지 않아 무시됩니다. 템포 배수는 숫자로 입력해 주세요.");
				} else {
					append_warning_to_vectorstr(&outputSayang, "The entered tempo multiplier (" + args[1] + ") is invalid and will be ignored. Please enter the tempo multiplier as a number.");
				}
			}
			args.erase(args.begin()); args.erase(args.begin());
		} else if (args[0] == "원본파일이름") {
			originalFileName = args[1];
			args.erase(args.begin()); args.erase(args.begin());
		} else {
			if (!isEnglish) {
				append_warning_to_vectorstr(&outputSayang, "입력한 옵션 " + args[0] + "은(는) 아직 안 만들었으므로 무시됩니다."); args.erase(args.begin());
			} else {
				append_warning_to_vectorstr(&outputSayang, "The entered option " + args[0] + " is ignored because it has not been developed yet."); args.erase(args.begin());
			}
		}
	}
	
	if (command == "악보" && iFixedInstrument[0] == -1 && iOnoffDrum == 0) {
		iOnoffDrum = 1;
	}
	
	int countTrackChannel = 0;
	for (itrack=0; itrack<iMaxTrack; itrack++) {
		for (ichannel=0; ichannel<iMaxChannel; ichannel++) {
			if (listTrackChannel[itrack][ichannel]) { countTrackChannel++; }
		}
	}
	if (iOnoffDrum == 0) {
		if (countTrackChannel == 0) {
			for (itrack=0; itrack<iMaxTrack; itrack++) {
				for (ichannel=0; ichannel<iMaxChannel; ichannel++) {
					listTrackChannel[itrack][ichannel] = true;
				}
			}
		}
	} else if (iOnoffDrum == 1) {
		if (countTrackChannel != 0) {
			for (itrack=0; itrack<iMaxTrack; itrack++) {
				for (ichannel=0; ichannel<iMaxChannel; ichannel++) {
					if (ichannel%16 != 9) { continue; }
					listTrackChannel[itrack][ichannel] = false;
				}
			}
		} else if (countTrackChannel == 0) {
			for (itrack=0; itrack<iMaxTrack; itrack++) {
				for (ichannel=0; ichannel<iMaxChannel; ichannel++) {
					if (ichannel%16 == 9) { continue; }
					listTrackChannel[itrack][ichannel] = true;
				}
			}
		}
	} else if (iOnoffDrum == 2) {
		if (countTrackChannel != 0) {
			for (itrack=0; itrack<iMaxTrack; itrack++) {
				for (ichannel=0; ichannel<iMaxChannel; ichannel++) {
					if (ichannel%16 == 9) { continue; }
					listTrackChannel[itrack][ichannel] = false;
				}
			}
		} else if (countTrackChannel == 0) {
			for (itrack=0; itrack<iMaxTrack; itrack++) {
				for (ichannel=0; ichannel<iMaxChannel; ichannel++) {
					if (ichannel%16 != 9) { continue; }
					listTrackChannel[itrack][ichannel] = true;
				}
			}
		}
	}
	
	if (command == "합주악보") {
		lOnoffHorizontalDivision = false;
		lOnoffVerticalDivision = true;
	}
	
	if (timeStartNote > timeEndNote) {
		if (!isEnglish) {
			append_error_to_vectorstr(&outputSayang, "입력한 시작/종료 시간 범위가 적절하지 않습니다.");
		} else {
			append_error_to_vectorstr(&outputSayang, "The start/end time range entered is invalid.");
		}
		errorNow = true;
		return;
	}
	
	if (pitchUpperBound < pitchLowerBound) {
		if (!isEnglish) {
			append_error_to_vectorstr(&outputSayang, "입력한 최저음/최고음 범위가 적절하지 않습니다.");
		} else {
			append_error_to_vectorstr(&outputSayang, "The lowest/highest pitch range entered is invalid.");
		}
		errorNow = true;
		return;
	}
	
	if (command == "합주악보" && iFixedInstrument[0] != -1) {
		if (!isEnglish) {
			append_error_to_vectorstr(&outputSayang, "합주악보 명령어를 사용하면 SayangBot이 자동으로 악기별 악보를 모두 출력합니다. 악기 지정 출력 옵션은 필요하지 않습니다. 의도하신대로 입력하신 게 맞나요..?");
		} else {
			append_error_to_vectorstr(&outputSayang, "When you use the ensemble sheet music command, SayangBot will automatically output all sheet music for each instrument. The instrument-specific output option is not required. Is it correct that you entered it as intended..?");
		}
		errorNow = true;
		return;
	}
	
	if (command == "합주악보" && iOnoffDrum != 0) {
		if (!isEnglish) {
			append_error_to_vectorstr(&outputSayang, "합주악보 명령어와 드럼제외/드럼만 옵션의 조합은 지원하지 않습니다. 합주악보 명령어 대신 악보 명령어와 함께 사용해 주세요.");
		} else {
			append_error_to_vectorstr(&outputSayang, "Combination of ensemble command and drumexclude/drumonly option is not supported. Please use it with the solo command instead of the ensemble command.");
		}
		errorNow = true;
		return;
	}
	
	if (command == "합주악보" && iOnoffMotion) {
		if (!isEnglish) {
			append_warning_to_vectorstr(&outputSayang, "합주악보 출력 시 모션 제어 기능은 아직 완성되지 않았습니다. 일부 연주자에 모션이 적용되지 않을 수 있습니다.");
		} else {
			append_warning_to_vectorstr(&outputSayang, "The motion control function when outputting ensemble sheet music is not yet complete. Motion may not be applied to some players.");
		}
	}
	
	vector<string> inputSayang;
	inputSayang.push_back("SayangBot2 Input Summary");
	
	inputSayang.push_back("iOnoffSustain");
	inputSayang.push_back(to_string(iOnoffSustain));
	inputSayang.push_back("iOnoffMotion");
	inputSayang.push_back(to_string(iOnoffMotion));
	inputSayang.push_back("lOnoffHorizontalDivision");
	inputSayang.push_back(to_string(lOnoffHorizontalDivision));
	inputSayang.push_back("lOnoffVerticalDivision");
	inputSayang.push_back(to_string(lOnoffVerticalDivision));
	inputSayang.push_back("lOnoffMotionSpeedTempo");
	inputSayang.push_back(to_string(lOnoffMotionSpeedTempo));
	inputSayang.push_back("isMML");
	inputSayang.push_back(to_string(isMML));
	inputSayang.push_back("isTriplet");
	inputSayang.push_back(to_string(isTriplet));
	inputSayang.push_back("isAutoEnhance");
	inputSayang.push_back(to_string(isAutoEnhance));
	inputSayang.push_back("isAutoDaecheInstrument");
	inputSayang.push_back(to_string(isAutoDaecheInstrument));
	inputSayang.push_back("isStrict");
	inputSayang.push_back(to_string(isStrict));
	inputSayang.push_back("isEnglish");
	inputSayang.push_back(to_string(isEnglish));
	inputSayang.push_back("lSameTrackInstrument");
	inputSayang.push_back(to_string(lSameTrackInstrument));
	
	inputSayang.push_back("cooldownMotionInQuarter");
	inputSayang.push_back(to_string(cooldownMotionInQuarter));
	inputSayang.push_back("volumeAdd1");
	inputSayang.push_back(to_string(volumeAdd1));
	inputSayang.push_back("volumeMul2");
	inputSayang.push_back(to_string(volumeMul2));
	inputSayang.push_back("volumeAdd3");
	inputSayang.push_back(to_string(volumeAdd3));
	inputSayang.push_back("tempoMulti");
	inputSayang.push_back(to_string(tempoMulti));
	
	inputSayang.push_back("pitchLowerBound");
	inputSayang.push_back(to_string(pitchLowerBound));
	inputSayang.push_back("pitchUpperBound");
	inputSayang.push_back(to_string(pitchUpperBound));
	inputSayang.push_back("noteResolution");
	inputSayang.push_back(to_string(noteResolution));
	inputSayang.push_back("tempoResolution");
	inputSayang.push_back(to_string(tempoResolution));
	inputSayang.push_back("susResolution");
	inputSayang.push_back(to_string(susResolution));
	inputSayang.push_back("iOnoffDrum");
	inputSayang.push_back(to_string(iOnoffDrum));
	inputSayang.push_back("iMethodDup");
	inputSayang.push_back(to_string(iMethodDup));
	inputSayang.push_back("lengthHorizontalDivision");
	inputSayang.push_back(to_string(lengthHorizontalDivision));
	inputSayang.push_back("iFixedTempo");
	inputSayang.push_back(to_string(iFixedTempo));
	inputSayang.push_back("iMinTempo");
	inputSayang.push_back(to_string(iMinTempo));
	inputSayang.push_back("maxCutLength");
	inputSayang.push_back(to_string(maxCutLength));
	inputSayang.push_back("timeStartNote");
	inputSayang.push_back(to_string(timeStartNote));
	inputSayang.push_back("timeEndNote");
	inputSayang.push_back(to_string(timeEndNote));
	
	inputSayang.push_back("errorNow");
	inputSayang.push_back(to_string(errorNow));
	
	inputSayang.push_back("iFixedInstrument");
	for (iInstr=0; iInstr<iFixedInstrument.size(); iInstr++) {
		inputSayang.push_back(to_string(iFixedInstrument[iInstr]));
	}
	
	write_file_from_string_vector_with_endl("01input.sayang_"+to_string(MYPE), inputSayang);
	
	return;
}