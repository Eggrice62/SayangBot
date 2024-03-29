### SayangBot2 릴리즈 및 구버전 SayangBot 지원 (2022/8/1 ~ )
- 기존 SayangBot 사용 명령어 입력 시 SayangBot2가 동작합니다.
- 명령어 맨 앞에 . 을 붙이면 구버전 SayangBot이 동작합니다.
- 구버전 대비 글자수 10~30% 저감 및 속도 향상!!

# SayangBot

![](./image/201218sayang.png)  
메이플스토리2 악보를 간편하게 만들 수 있는 프로그램 (Discord 봇) 입니다.  
미디 파일을 메이플스토리2 악보로 즉시 바꾸어 줍니다.  
메이플 월드의 새로운 음악가가 되어 보세요!  
This is a Discord bot that allows you to easily create MapleStory 2 sheet music.  
This bot instantly converts MIDI files into MapleStory 2 sheet music.  
Become a new musician in Maple World!  
![](./image/sendmidi.png)

# 사용법 / How to use

## 사양봇 서버 참여 or 직접 서버 초대
- (권장) Discord에서 사양봇 서식지 ( https://discord.gg/zDKPbSJcPA ) 서버에 참여하신 뒤, 서버에 있는 SayangBot에게 개인 메시지 (DM) 를 보내서 사용하실 수 있습니다.
- (권장하지 않음) 위 서버 참여가 어려우신 경우, 사양봇 소환 링크 ( https://bit.ly/3hYvjKm ) 를 통해 자신의 서버에 사양봇을 소환할 수 있습니다. 그러나 소환한 봇을 방치할 경우 봇이 탈주할 수 있습니다.

## 미디 파일 준비 / Prepare MIDI file

만들고자 하는 음악의 MIDI 파일이 있어야 합니다.  
MIDI 파일은 직접 만들 수도 있지만, 유명한 곡일 경우 다른 사람이 만들어둔 것을 인터넷 검색으로 찾을 수 있습니다.  
Google 등으로 <곡 이름> midi 와 같이 검색하거나, Musescore 등의 전문 사이트에서 MIDI 파일을 찾아 보세요.  
You must have a MIDI file of the music you want to create.  
You can also create your own MIDI files, but if they're famous songs, you can search the internet for others made by others.  
Search for MIDI on Google, etc., or find it on specialized sites such as Musescore.

## 미디 파일 업로드 / Upload MIDI file

![](./image/sendmidi3.png)  
SayangBot이 있는 Discord 서버에 미디 파일을 올리시면, SayangBot이 자동으로 인식하여 작동을 준비합니다.  
Just upload your MIDI file to the Discord server with SayangBot, and SayangBot will automatically recognize it and prepare to operate.

## 미디 파일 내용 확인 / Check MIDI file contents

![](./image/info.png)  
Discord 채팅으로 %정보 라고 입력해 보세요.  
미디 파일의 BPM, 재생 시간, 사용 악기, 텍스트 메모 등을 간단히 확인할 수 있습니다.  
Try entering %info in Discord chat.  
You can easily check a MIDI file's BPM, duration, instrument used, text notes, and more.

## 악보 작성 (독주) / Converting into sheet music (solo)

![](./image/sendmidi.png)  
Discord 채팅으로 %악보 라고 입력해 보세요.  
혼자 연주할 수 있는 악보가 제작됩니다.  
Try typing %solo in Discord chat.  
A sheet music that can be played alone is produced.

## 악보 작성 (합주) / Converting into sheet music (ensemble)

![](./image/sendmidi2new.png)  
Discord 채팅으로 %합주악보 라고 입력해 보세요.  
파티원들과 합주를 할 수 있도록 악기별 악보가 따로따로 제작됩니다.  
Try typing %ensemble in Discord chat.  
Sheet music for each instrument is produced separately so that you can perform with party members.

## 악보 사용법 / How to use sheet music

![](./image/import.png)  
빈 악보를 더블클릭하여 악보 제작 화면을 열고, 우측 하단의 파일 불러오기 버튼으로 방금 만들어진 ms2mml 파일을 가져옵니다.  
Double-click the blank sheet to open the sheet music creation screen, and import the just created ms2mml file by clicking the "파일 불러오기" button at the bottom right.

## 옵션 사용법 / How to use option for conversion

%도움말 악보 를 입력하여 옵션 목록과 사용법을 참고해 보세요.  
Enter %Help Score to see a list of options and how to use them.

# 문제 해결 및 팁 / Troubleshooting and Tips

## 봇이 응답하지 않는 경우 / If the bot is not responding

개발자가 8시간 이내로 봇을 재부팅할 것이니 기다려 주십시오.  
그리고 봇이 사망한 원인을 1~2일 내로 해결하여 업데이트할 것입니다.  
Please be patient as the developer will reboot the bot within 8 hours.  
And developer will update the cause of the bot's death by addressing it within 1-2 days.

## 변환에 실패할 경우 / If the conversion fails

개발자가 1~2일 내로 패치할 것이니 기다려 주십시오.  
SayangBot은 2021.9.24부터 베타 테스트 중이며 오늘도 개발자가 지속적으로 오류를 수정하고 기능을 개선하고 있습니다. 현재 대부분의 MIDI 파일을 오류 없이 악보로 변환 가능한 것으로 여겨지고 있으나, 간혹 예상치 못한 원인으로 처리가 안 될 수 있습니다. 이때는 이미 개발자에게 자동으로 관련 정보가 전송되었을 것이며, 원인을 파악한 뒤 해당 파일도 처리가 가능하도록 패치할 것입니다.  
Developers will patch it in 1-2 days. So please wait.  
SayangBot is in beta testing starting on September 24, 2021, and today, developers are constantly fixing errors and improving features. Currently, it is considered that most MIDI files can be converted into sheet music without errors, but sometimes, processing may not be possible due to unexpected reasons. In this case, the relevant information has already been sent to the developer automatically, and after determining the cause, developers will patch the file so that it can be processed.

## 글자 수 줄이기 / How to reduce the number of characters

- %악보 해상도 5 등으로 해상도를 낮추어 보세요. 해상도가 낮을수록 글자 수가 줄어들지만 악보의 품질이 낮아집니다. 해상도는 0~6의 정수로 입력할 수 있으며, 입력하지 않을 경우의 기본값은 6입니다.
- %악보 템포뭉개기 3 등으로 템포 변환 이벤트의 개수를 줄여 보세요. 템포뭉개기가 높을수록 글자 수가 줄어들지만 악보의 품질이 낮아집니다. 템포뭉개기는 0~6의 정수로 입력할 수 있으며, 입력하지 않을 경우의 기본값은 0입니다.
- 서스테인 페달 이벤트가 포함된 악보일 경우, %악보 서스테인뭉개기 3 등으로 서스테인 이벤트의 개수를 줄여 보세요. 서스테인뭉개기가 높을수록 글자 수가 줄어들지만 악보의 품질이 낮아집니다. 서스테인뭉개기는 0~6의 정수로 입력할 수 있으며, 입력하지 않을 경우의 기본값은 0입니다.
- 셋잇단음표가 많이 등장하거나 스윙 리듬이 있는 곡의 경우, %악보 셋잇단 등과 같이 사용하면 글자수가 줄어들 수도 있습니다.
> 위 옵션은 모두 중복해서 사용 가능합니다. (예시 : %악보 셋잇단 해상도 4 템포뭉개기 6 서스테인뭉개기 6)

# 피드백 보내기 / Feedback

SayangBot이 있는 Discord 서버에서 %피드백 <하실 말씀> 을 입력하면 개발자에게 전송됩니다.  
혹은 Discord에서 계란밥#9331 으로 연락주셔도 괜찮습니다.  
자유롭게 적어서 보내주시면 개발에 큰 도움이 됩니다. 감사합니다.  
Enter %feedback "what you want to say" on the Discord server with SayangBot and it will be sent to the developer.  
Or you can contact me on Discord (계란밥#9331).  
It will be a great help for development. thank you.

# 업데이트 내역

## 2023/1/29
- 서스테인 붙은 악보는 파일명 앞에 SUSTAIN 붙여서 강조
- 도움말 <명령어> 그대로 치면 아 그거 그렇게 하는 거 아닌데 로 회신
- 조옮김 추가 (비밀 기능으로 도움말에 아직 안 넣음)

## 2023/1/19
- 일부 셋잇단음표 인식 기능 추가 (자동 적용, 단 페이지 수가 늘어나는 경우 셋잇단음표 처리는 포기함)
- SayangBot1의 경우 사용자 선호에 따라 DrumEtc에 출력되는 음표를 변경
- 최소템포 옵션 추가
- 최적화 전후 악보의 길이가 달라질 경우 에러 출력
- 길이가 0인 음표는 무시하도록 변경
- 정보 출력 시 MIDI 파일에 내장된 텍스트에 줄바꿈 문자가 있으면 결과가 안 나오는 버그를 수정
- 독주악보 출력 시 기본 템포 범위 밖의 값을 변환하는 과정에서 오차가 생겨 무한루프에 빠지는 버그를 수정
- 템포 지정 없이 입력된 음표가 템포 변환 시 증발하는 버그를 수정
- 입력한 시작/종료 시간에 템포 변환이 적용되지 않는 버그를 수정
- 10장을 초과하는 독주 악보에 대해 뜬금없이 합주 경고를 출력하는 버그를 수정

## 2023/1/18
- 1만자에 맞게 분할할 때 서스테인 글자수를 고려하지 않아 오작동하는 오류를 수정
- 서스테인 채널 마지막 이벤트의 시점이 잘못 출력되는 오류를 수정

## 2023/1/17
- 합주악보 명령어 사용 시 큰북/작은북/심벌즈를 제외한 드럼 노트를 DrumEtc파일에 출력
- 구버전 사양봇에 신규 악기 출시 내용 업데이트
- 볼륨감소 옵션 사용 시 의도치 않게 모든 음표의 볼륨이 1 이하로 모기소리 되는 것을 수정
- SayangBot2 출력 결과가 2,000자를 초과할 경우 다운로드가 불가능한 오류 수정
- 특정 상황에서 출력할 음표가 없을 때 변환 실패하는 것을 수정

## 2022/7/6
- 동일 트랙의 악기 변경 이벤트만 사용하는 옵션 (트랙악기분리) 추가
- 모션 사용 시 최적화 단계에서 싱크가 작살나는 버그 수정
- 짧은 악보 최적화하다가 버그 걸려서 죽는 것 수정
- 템포 이벤트가 없을 때 버그 걸려서 죽는 것 수정

## 2022/6/22
- SayangBot2 BETA 테스트 시작
- 엔진 교체 (nodejs봇 -> python봇/c++변환모듈)
- python의 안정성과 c++의 높은 처리속도 획득
- 최적화 일부 도입으로 악보 글자 수 평균 10~30% 저감
- 템포 32 미만 혹은 255 초과 구간 처리 방식 변경
- 필요한 악보 종류 (고급/중급/초급 개수) 안내 추가

## 2022/2/2
- 1만자 제한에 맞게 분할하는 과정에서 원본 음표 길이가 수정되어 악보에 출력되지 않는 오류를 수정 (2d array는 slice 해도 deepcopy가 안됨?!)
- 최고음 옵션 수치가 셈여림으로 연결되는 오류를 수정

## 2021/12/15
- 템포 32 미만 혹은 255 초과인 구간이 제대로 출력되지 않는 오류를 수정

## 2021/11/26
- 11/7 업데이트 내용 중 템포 관련 내용이 셋잇단 옵션에 적용되지 않는 오류를 수정
- 이름이 mid인 파일을 보내면 서버가 사망하는 오류를 수정
- 서버가 사망하더라도 새로운 서버가 가동될 수 있도록 실행 방식을 변경 (코드에는 나와있지 않음)

## 2021/11/7
- 템포가 32 미만 혹은 255를 초과하는 MIDI 파일을 처리할 때, 템포 변경 이벤트가 나타나지 않는 시점까지의 음표들이 잘못된 길이로 처리되는 오류를 수정
- AnyConv에서 생성한 MIDI 파일에 대한 안내 메시지를 추가
- 모션 출력 시 기본 모션 (m1) 만 출력되던 것을 랜덤한 모션 (m1~m5) 으로 지정하여 출력하도록 변경

## 2021/10/9
- 플룻과 색소폰의 악기 이름이 잘못 출력되는 오류를 수정
- 독주악보가 여러 장 나왔을 경우 화음분할에 대한 도움말을 추가
- help 명령어로 영어 도움말 요청 시 한국어 도움말을 보는 방법 설명을 추가
- 하이퍼링크 유튜브 스팸 갖다버리고 여기 도움말로 연결

## 2021/10/6

- 2개 이상의 포트 (17개 이상의 채널) 를 사용하는 대규모 MIDI 파일 처리 시 잘못된 악기가 배정되는 오류를 수정

# 업데이트 목표 사항
- musescore의 소스 코드를 참고하여 셋잇단음표 등의 강건하고 정밀한 음표 인식
