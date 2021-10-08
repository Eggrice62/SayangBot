const logger = require('@greencoast/logger');
const TTSPlayer = require('../../classes/TTSPlayer');
const prefix = process.env.PREFIX || require('../../../config/settings.json').prefix;
const { updatePresence, executeCommand } = require('../../common/utils');
const fetch = require("node-fetch");
const fs = require('fs');
const performance = require('perf_hooks').performance;
var zip = new require('node-zip')();

let request = require(`request`);
let midiParser  = require('midi-parser-js');
const { MessageEmbed } = require('discord.js');

var currentName = '';
var iMaxTrack = 128;
var iMaxChannel = 256;

var user2filename = {};
var user2time = {};
var user2processtime = {};

var isBojagi = [];

var instrumentName2num = {};
instrumentName2num['피아노'] = [0, 1];
instrumentName2num['기타'] = [24];
instrumentName2num['클라리넷'] = [71];
instrumentName2num['하프'] = [46];
instrumentName2num['팀파니'] = [47];
instrumentName2num['일렉트릭기타'] = [29, 30];
instrumentName2num['베이스기타'] = [33];
instrumentName2num['탐탐'] = [117];
instrumentName2num['바이올린'] = [40, 41, 48, 49];
instrumentName2num['첼로'] = [42, 43];
instrumentName2num['팬플루트'] = [75];
instrumentName2num['색소폰'] = [65, 66, 67, 64];
instrumentName2num['트롬본'] = [57, 58];
instrumentName2num['트럼펫'] = [56, 59, 60, 61];
instrumentName2num['오카리나'] = [79, 78];
instrumentName2num['어쿠스틱베이스'] = [32];
instrumentName2num['비브라폰'] = [11, 98];
instrumentName2num['전자피아노'] = [4];
instrumentName2num['스틸드럼'] = [114];
instrumentName2num['피크베이스기타'] = [34];
instrumentName2num['오보에'] = [68, 69, 70];
instrumentName2num['피치카토바이올린'] = [45, 105, 107];
instrumentName2num['하프시코드'] = [6, 7];
instrumentName2num['하모니카'] = [22];
instrumentName2num['실로폰'] = [13, 14];
instrumentName2num['리코더'] = [74];
instrumentName2num['첼레스타'] = [8, 9, 10];
instrumentName2num['심벌즈'] = [129];
instrumentName2num['큰북'] = [130];
instrumentName2num['작은북'] = [131];
instrumentName2num['프렛리스베이스기타'] = [35];
instrumentName2num['마림바'] = [12];
instrumentName2num['플루트'] = [73, 72];

const handleDebug = (info) => {
  logger.debug(info);
};

const handleError = (error) => {
  logger.error(error);
};

const handleGuildCreate = (guild, client) => {
  logger.info(`Joined ${guild.name} guild!`);
  updatePresence(client);
  guild.ttsPlayer = new TTSPlayer(guild);
  guild.ttsPlayer.setLang('ko')
      .then()
      .catch();
};

const handleGuildDelete = (guild, client) => {
  logger.info(`Left ${guild.name} guild!`);
  updatePresence(client);
  guild.ttsPlayer = null;
};

const handleGuildUnavailable = (guild) => {
  logger.warn(`Guild ${guild.name} is currently unavailable!`);
};

const handleInvalidated = () => {
  logger.error('Client connection invalidated, terminating execution with code 1.');
  process.exit(1);
};

const handleMessage = (message, client) => {
	if (message.author.bot) {
		return;
	}
	
	if (message.attachments.first()) {
		if (message.attachments.first().name.toLowerCase().includes('mid')) {
			currentName = ((message.guild)?('guild_'+message.channel.id):('dm_'+message.author.id))+'.mid';
			downloadmidi(message.attachments.first().url, currentName);// at the top of your file
			downloadmidi(message.attachments.first().url, message.attachments.first().name);// at the top of your file
			user2filename[((message.guild)?('guild_'+message.channel.id):('dm_'+message.author.id))] = message.attachments.first().name;
			user2processtime[((message.guild)?('guild_'+message.channel.id):('dm_'+message.author.id))] = 0;
			const exampleEmbed = new MessageEmbed()
				.setColor('#8cffa9')
				.setTitle('미디 파일 인식됨')
				.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
				.setDescription(message.attachments.first().name+'\n아래 명령어를 사용할 수 있습니다.')
				.addFields(
					{ name: prefix+'정보', value: '파일 내용 간단 확인' },
					{ name: prefix+'악보', value: '독주용 악보로 제작' },
					{ name: prefix+'합주악보', value: '합주를 위한 악기별 악보로 제작' },
					{ name: '전체 명령어 도움말', value: prefix+'도움말\n'+prefix+'도움말 <명령어>' },
					{ name: 'English help', value: prefix+'help\n'+prefix+'help <command>' },
				)
				.setFooter('문의 : 눈꽃빙빙빙 (계란밥#9331)', 'https://i.imgur.com/82dLPkv.png');
			message.channel.send(exampleEmbed);
			client.users.fetch('364432570005323796', false).then((user) => {
				const exampleEmbed = new MessageEmbed()
					.setColor('#8cffa9')
					.setAuthor(message.author.username + ' (' + message.author.id + ')', message.author.displayAvatarURL())
					.setTitle(message.attachments.first().name)
					.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
					.setDescription(((message.guild)?(message.guild.name+'-'+message.channel.name):'DM'));
				user.send(exampleEmbed);
			});
		}
	}

	if ((message.guild && message.content[0] == prefix)  || (!message.guild && message.author.id != '364432570005323796')) {
		client.users.fetch('364432570005323796', false).then((user) => {
			const exampleEmbed = new MessageEmbed()
				.setColor('#8cffa9')
				.setAuthor(message.author.username + ' (' + message.author.id + ')', message.author.displayAvatarURL())
				.setTitle(message.content)
				.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
				.setDescription(((message.guild)?(message.guild.name+'-'+message.channel.name):'DM'));
			user.send(exampleEmbed);
		});
	}
	
	if (message.content[0] != prefix) { return; }
			
	user2time[((message.guild)?('guild_'+message.channel.id):('dm_'+message.author.id))] = Date.now();
	
	isEnglish = false;
	
	args = message.content.slice(prefix.length).trim().toLowerCase().split(/ +/);
	command = args.shift().toLowerCase();
	
	if (/^[A-Za-z]*$/.test(command)) { isEnglish = true; }
	if (command == 'info') {
		command = '정보';
	} else if (command == 'solo') {
		command = '악보';
	} else if (command == 'ensemble') {
		command = '합주악보'
	}
	
	for (iarg=0; iarg<args.length; iarg++) {
		curArg = args[iarg];
		if (curArg == 'track') {
			args[iarg] = '트랙';
		} else if (curArg == 'channel') {
			args[iarg] = '채널';
		} else if (curArg == 'trackchannel') {
			args[iarg] = '트랙채널';
		} else if (curArg == 'sustain') {
			args[iarg] = '서스테인';
		} else if (curArg == 'motion') {
			args[iarg] = '모션';
		} else if (curArg == 'start') {
			args[iarg] = '시작';
		} else if (curArg == 'end') {
			args[iarg] = '종료';
		} else if (curArg == 'lowest') {
			args[iarg] = '최저음';
		} else if (curArg == 'highest') {
			args[iarg] = '최고음';
		} else if (curArg == 'cutoff') {
			args[iarg] = '음표깎기';
		} else if (curArg == 'resolution') {
			args[iarg] = '해상도';
		} else if (curArg == 'mergetempo') {
			args[iarg] = '템포뭉개기';
		} else if (curArg == 'mergesustain') {
			args[iarg] = '서스테인뭉개기';
		} else if (curArg == 'motioncooltime') {
			args[iarg] = '모션쿨';
		} else if (curArg == 'volume1') {
			args[iarg] = '볼륨1';
		} else if (curArg == 'volume2') {
			args[iarg] = '볼륨2';
		} else if (curArg == 'volume3') {
			args[iarg] = '볼륨3';
		} else if (curArg == 'fixedvolume') {
			args[iarg] = '고정볼륨';
		} else if (curArg == 'volumeincrease') {
			args[iarg] = '볼륨증가';
		} else if (curArg == 'volumedecrease') {
			args[iarg] = '볼륨감소';
		} else if (curArg == 'drumexclude') {
			args[iarg] = '드럼제외';
		} else if (curArg == 'drumonly') {
			args[iarg] = '드럼만';
		} else if (curArg == 'removeduplicates') {
			args[iarg] = '중복제거';
		} else if (curArg == 'lengthdivision') {
			args[iarg] = '길이분할';
		} else if (curArg == 'ignore10000') {
			args[iarg] = '1만자무시';
		} else if (curArg == 'chorddivision') {
			args[iarg] = '화음분할';
		} else if (curArg == 'motionspeed') {
			args[iarg] = '모션속도템포';
		} else if (curArg == 'fixedtempo') {
			args[iarg] = '고정템포';
		} else if (curArg == 'instrument') {
			args[iarg] = '악기';
		} else if (curArg == 'triplet') {
			args[iarg] = '셋잇단';
		} else if (curArg == 'autovolumeincrease') {
			args[iarg] = '자동볼륨증가';
		} else if (curArg == 'disablesubstitution') {
			args[iarg] = '대체악기사용안함';
		}
	}
	
	if (command == '피드백') {
		const exampleEmbed = new MessageEmbed()
			.setColor('#8cffa9')
			.setTitle('피드백 전송 완료')
			.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
			.setDescription('보내주신 피드백이 정상적으로 전달되었습니다. 도와주셔서 감사합니다.');
		message.channel.send(exampleEmbed);
		return;
	}
	
	if (command == 'feedback') {
		const exampleEmbed = new MessageEmbed()
			.setColor('#8cffa9')
			.setTitle('Feedback sent')
			.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
			.setDescription('Your feedback has been successfully delivered. Thank you for your help.');
		message.channel.send(exampleEmbed);
		return;
	}
	
	if (command == '도움말') {
		if (args.length == 0) {
			const exampleEmbed = new MessageEmbed()
				.setColor('#8cffa9')
				.setTitle('SayangBot')
				.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
				.setDescription('MIDI 파일 (*.mid) 을 ms2mml로 변환하는 봇입니다.\n이 채팅방에 미디 파일을 올리면 SayangBot이 자동으로 인식하여 작동합니다.\n미디 파일 공개가 꺼려지시면 개인 메시지 (DM)에서도 작동합니다.\n다만 보내신 mid 파일은 처리를 위해 제 컴퓨터에 저장됩니다. 도용을 하지는 않겠지만 그래도 참고해 주십시오.\n'+prefix+'도움말 <명령어> 로 세부 내용을 확인하세요!\nex) ' + prefix + '도움말 악보')
				.addFields(
					{ name: prefix+'정보', value: '미디 파일의 전반적인 정보'},//, inline: true },
					{ name: prefix+'악보', value: '전체 음표를 독주악보 1개로 변환'},
					{ name: prefix+'합주악보', value: '합주를 위한 악기별 악보들로 변환'},
					{ name: prefix+'팁', value: '출력된 악보에 문제가 있다면 참고해 보세요!'},
					{ name: prefix+'피드백', value: '' + prefix + '피드백 으로 시작하는 메시지는 모두 개발자에게 자동으로 전달됩니다. 자유롭게 적어서 보내주시면 개발에 큰 도움이 됩니다. 감사합니다.'});
			message.channel.send(exampleEmbed);
		} else {
			if (args[0] == '정보') {
				const exampleEmbed = new MessageEmbed()
					.setColor('#8cffa9')
					.setTitle('SayangBot 명령어 (' + prefix + args[0] + ')')
					.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
					.setDescription('미디 파일의 전반적인 정보를 확인할 수 있습니다.')
					.addFields(
						{ name: '파일 이름', value: '현재 사용 중인 mid 파일입니다. 해당 파일은 새로운 파일이 업로드되거나, SayangBot이 재부팅될 때까지 유효합니다. DM에서 사용 중인 파일은 다른 사람의 DM이나 서버에서 사용하는 파일의 영향을 받지 않습니다. 순서 걱정 없이 천천히 작업해 보세요.'},//, inline: true },
						{ name: 'BPM', value: '곡의 빠르기를 대략적으로 확인할 수 있습니다. 메이플스토리2에서 사용 가능한 템포 코드 범위는 32~255이지만, SayangBot은 8~510까지 알아서 바꿔 드립니다.'},
						{ name: '박자', value: '곡이 몇분의 몇박자인지를 확인할 수 있습니다. 사실 쓸데없는 정보입니다.'},
						{ name: '재생 시간', value: '곡의 재생 시간을 시/분/초 단위로 확인할 수 있습니다. 출력된 악보의 재생 시간은 템포 코드의 적용 상황에 따라 조금 오차가 생길 수 있습니다.'},
						{ name: '곡 전체 틱 / 4분음표 틱', value: '그렇습니다.'},
						{ name: '트랙 정보', value: '파일에 들어 있는 트랙의 개수, 사용하는 채널, 악기, 음표 개수, 서스테인 유무 등을 확인할 수 있습니다. 합주악보를 만드실 때 자세하게 살펴보시길 추천드립니다.'},
						{ name: '텍스트 이벤트', value: 'mid 파일의 제작자가 적어 놓은 메시지들입니다.'},
						{ name: '저작권 정보', value: 'mid 파일에 기록되어 있는 저작권 정보입니다.'});
				message.channel.send(exampleEmbed);
			} else if (args[0] == '악보') {
				const exampleEmbed = new MessageEmbed()
					.setColor('#8cffa9')
					.setTitle('SayangBot 명령어 (' + prefix + args[0] + ')')
					.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
					.setDescription('미디 파일에 들어있는 모든 음표를 악기 구분 없이 출력합니다. 독주용 악보를 만들거나, 글자수 문제로 2~3인 합주로 나눠서 만들 때 사용 가능합니다.\n악보 명령어에서 사용 가능한 옵션은 다음과 같습니다.\nex) ' + prefix + '악보 <옵션1> <옵션2> ... <옵션n>')
					.addFields(
						{ name: '셋잇단', value: '셋잇단음표가 많은 곡 (특히 스윙) 에 맞게 템포를 바꾸어 처리합니다.\nex) ' + prefix + '악보 셋잇단'},
						{ name: '1만자무시', value: '1만 자 이상의 악보를 길이 분할 없이 출력합니다.\nex) ' + prefix + '악보 1만자무시'},
						{ name: '화음분할', value: '악보의 화음을 분할하여 합주용 악보들로 만듭니다.\nex) ' + prefix + '악보 화음분할'},
						{ name: '악기', value: 'mid 파일에 기록된 음표 중 특정 악기의 음표만 출력합니다.\nex) ' + prefix + '악보 악기 프렛리스베이스기타'},
						{ name: '서스테인', value: '서스테인 페달 on/off 스위치입니다.\n기본값은 on입니다.\nex) ' + prefix + '악보 서스테인 on : 서스테인도 악보에 출력합니다.\n서스테인 이벤트를 주기적으로 초기화하도록 올바르게 구성하지 않으면 MIDI 동시발음수를 초과하여 연주 소리가 들리지 않을 수 있습니다.\n서스테인에 적합하지 않은 악기 (일렉기타, 첼로 등) 를 사용할 경우 주변 유저들에게 귀갱으로 인해 차단당할 수 있습니다.'},
						{ name: '모션', value: '자동 모션 on/off 스위치입니다. 연주를 일정 시간 멈출 경우 캐릭터가 모션을 하지 않고 정지합니다.\n기본값은 off입니다.\nex) ' + prefix + '악보 모션 on : 자동 모션 코드를 악보에 출력합니다.'},
						{ name: '해상도', value: '악보의 품질을 조정합니다.\n0~6 중 입력하는 숫자가 높을수록 듣기에 자연스러워지지만 악보가 길어집니다.\n기본값은 6입니다.\nex) ' + prefix + '악보 해상도 4 : 기본 설정으로 했을 때보다 짧아진 악보가 나옵니다.'},
						{ name: '템포뭉개기', value: '템포 조정 이벤트의 개수를 조절합니다.\n0~6 중 입력하는 숫자가 낮을수록 듣기에 자연스러워지지만 악보가 길어집니다.\n기본값은 0입니다.\nex) ' + prefix + '악보 템포뭉개기 3 : 기본 설정으로 했을 때보다 짧아진 악보가 나옵니다.'},
						{ name: '서스테인뭉개기', value: '서스테인 페달 이벤트의 개수를 조절합니다.\n0~6 중 입력하는 숫자가 낮을수록 듣기에 자연스러워지지만 악보가 길어집니다.\n기본값은 0입니다.\nex) ' + prefix + '악보 서스테인뭉개기 3 : 기본 설정으로 했을 때보다 짧아진 악보가 나옵니다.'},
						{ name: '볼륨증가', value: '볼륨을 V15 방향으로 높입니다.\nex) ' + prefix + '악보 볼륨증가'},
						{ name: '볼륨감소', value: '볼륨을 V0 방향으로 낮춥니다.\nex) ' + prefix + '악보 볼륨감소'},
						{ name: '고정볼륨', value: 'mid 파일에 기록된 볼륨을 무시하고 입력한 볼륨으로 고정합니다.\nex) ' + prefix + '악보 고정볼륨 14\n단위는 메이플스토리의 V코드 (V값 1당 미디 벨로시티 8) 로 입력합니다.'},
						{ name: '볼륨1/볼륨2/볼륨3', value: '볼륨을 직접 자세하게 조정합니다.\nex) ' + prefix + '악보 볼륨1 3 볼륨2 0.5 볼륨3 -5 : 볼륨을 3만큼 높이고, 0.5를 곱하고, 5만큼 낮춥니다.\n단위는 메이플스토리의 V코드 (V값 1당 미디 벨로시티 8) 로 입력합니다.'},
						{ name: '고정템포', value: 'mid 파일에 기록된 템포를 무시하고 입력한 템포로 고정합니다.\nex) ' + prefix + '악보 고정템포 180'},
						{ name: '드럼제외', value: '드럼 코드 (채널 10 등) 의 음표는 제외하고 출력합니다.\nex) ' + prefix + '악보 드럼제외'},
						{ name: '드럼만', value: '드럼 코드 (채널 10 등) 의 음표만 선택하여 출력합니다.\nex) ' + prefix + '악보 드럼만'},
						{ name: '트랙', value: '지정한 트랙을 출력 범위에 추가합니다.\nex) ' + prefix + '악보 트랙 3 트랙 5 : 3번 트랙, 5번 트랙의 음표만 악보에 출력합니다.'},
						{ name: '채널', value: '지정한 채널을 출력 범위에 추가합니다.\nex) ' + prefix + '악보 채널 7 채널 9 : 7번 채널, 9번 채널의 음표만 악보에 출력합니다.'},
						{ name: '트랙채널', value: '지정한 트랙-채널을 출력 범위에 추가합니다.\nex) ' + prefix + '악보 트랙채널 3 1 트랙채널 2 9 : 3번 트랙 중 1번 채널 음표, 2번 트랙 중 9번 채널 음표만 악보에 출력합니다.'},
						{ name: '시작', value: '지정한 시간 이후의 음표만 출력합니다.\n단위는 미디 틱(tick)이므로 초보자가 사용하기 어려울 수 있습니다.\nex) ' + prefix + '악보 시작 4800 : 4800틱 이후의 음표만 출력합니다.'},
						{ name: '종료', value: '지정한 시간 이전의 음표만 출력합니다.\n단위는 미디 틱(tick)이므로 초보자가 사용하기 어려울 수 있습니다.\nex) ' + prefix + '악보 종료 24000 : 24000틱 이전의 음표만 출력합니다.'},
						{ name: '최저음', value: '지정한 음높이 이상의 음표만 출력합니다.\n단위는 가온다(C4)를 60으로 하여 반음 당 1씩 계산하여 입력합니다.\nex) ' + prefix + '악보 최저음 35 : B1 이상의 음높이만 출력합니다.'},
						{ name: '최고음', value: '지정한 음높이 이하의 음표만 출력합니다.\n단위는 가온다(C4)를 60으로 하여 반음 당 1씩 계산하여 입력합니다.\nex) ' + prefix + '악보 최고음 68 : G+4 이하의 음높이만 출력합니다.'},
						{ name: '음표깎기', value: '살짝만 겹쳐서 화음 개수를 늘리는 음표가 있을 경우 음표 길이를 조금 줄여서 화음 개수를 줄입니다.\n범위는 0 (깎지 않음) ~ 100 (모두 깎음) 중 입력할 수 있습니다.\nex) ' + prefix + '악보 음표깎기 10 : 음표 길이의 10%까지 깎습니다.'},
						{ name: 'mml', value: 'ms2mml이 아닌 mml 파일로 출력합니다.\nex) ' + prefix + '악보 mml'});
					message.channel.send(exampleEmbed);
			} else if (args[0] == '합주악보') {
				const exampleEmbed = new MessageEmbed()
					.setColor('#8cffa9')
					.setTitle('SayangBot 명령어 (' + prefix + args[0] + ')')
					.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
					.setDescription('미디 파일로부터 합주 악보를 자동 생성합니다.\n합주악보 명령어에서 사용 가능한 옵션은 다음과 같습니다.\nex) ' + prefix + '악보 <옵션1> <옵션2> ... <옵션n>')
					.addFields(
						{ name: '셋잇단', value: '셋잇단음표가 많은 곡 (특히 스윙) 에 맞게 템포를 바꾸어 처리합니다.\nex) ' + prefix + '합주악보 셋잇단'},
						{ name: '서스테인', value: '서스테인 페달 on/off 스위치입니다.\n기본값은 on입니다.\nex) ' + prefix + '합주악보 서스테인 on : 서스테인도 악보에 출력합니다.\n서스테인 이벤트를 주기적으로 초기화하도록 올바르게 구성하지 않으면 MIDI 동시발음수를 초과하여 연주 소리가 들리지 않을 수 있습니다.\n서스테인에 적합하지 않은 악기 (일렉기타, 첼로 등) 를 사용할 경우 주변 유저들에게 귀갱으로 인해 차단당할 수 있습니다.'},
						{ name: '모션', value: '자동 모션 on/off 스위치입니다. 연주를 일정 시간 멈출 경우 캐릭터가 모션을 하지 않고 정지합니다.\n기본값은 off입니다.\nex) ' + prefix + '합주악보 모션 on : 자동 모션 코드를 악보에 출력합니다.'},
						{ name: '해상도', value: '악보의 품질을 조정합니다.\n0~6 중 입력하는 숫자가 높을수록 듣기에 자연스러워지지만 악보가 길어집니다.\n기본값은 6입니다.\nex) ' + prefix + '합주악보 해상도 4 : 기본 설정으로 했을 때보다 짧아진 악보가 나옵니다.'},
						{ name: '템포뭉개기', value: '템포 조정 이벤트의 개수를 조절합니다.\n0~6 중 입력하는 숫자가 낮을수록 듣기에 자연스러워지지만 악보가 길어집니다.\n기본값은 0입니다.\nex) ' + prefix + '합주악보 템포뭉개기 3 : 기본 설정으로 했을 때보다 짧아진 악보가 나옵니다.'},
						{ name: '서스테인뭉개기', value: '서스테인 페달 이벤트의 개수를 조절합니다.\n0~6 중 입력하는 숫자가 낮을수록 듣기에 자연스러워지지만 악보가 길어집니다.\n기본값은 0입니다.\nex) ' + prefix + '합주악보 서스테인뭉개기 3 : 기본 설정으로 했을 때보다 짧아진 악보가 나옵니다.'},
						{ name: '볼륨증가', value: '볼륨을 V15 방향으로 높입니다.\nex) ' + prefix + '합주악보 볼륨증가'},
						{ name: '볼륨감소', value: '볼륨을 V0 방향으로 낮춥니다.\nex) ' + prefix + '합주악보 볼륨감소'},
						{ name: '고정볼륨', value: 'mid 파일에 기록된 볼륨을 무시하고 입력한 볼륨으로 고정합니다.\nex) ' + prefix + '합주악보 고정볼륨 14\n단위는 메이플스토리의 V코드 (V값 1당 미디 벨로시티 8) 로 입력합니다.'},
						{ name: '볼륨1/볼륨2/볼륨3', value: '볼륨을 직접 자세하게 조정합니다.\nex) ' + prefix + '합주악보 볼륨1 3 볼륨2 0.5 볼륨3 -5 : 볼륨을 3만큼 높이고, 0.5를 곱하고, 5만큼 낮춥니다.\n단위는 메이플스토리의 V코드 (V값 1당 미디 벨로시티 8) 로 입력합니다.'},
						{ name: '고정템포', value: 'mid 파일에 기록된 템포를 무시하고 입력한 템포로 고정합니다.\nex) ' + prefix + '합주악보 고정템포 180'},
						{ name: '시작', value: '지정한 시간 이후의 음표만 출력합니다.\n단위는 미디 틱(tick)이므로 초보자가 사용하기 어려울 수 있습니다.\nex) ' + prefix + '합주악보 시작 4800 : 4800틱 이후의 음표만 출력합니다.'},
						{ name: '종료', value: '지정한 시간 이전의 음표만 출력합니다.\n단위는 미디 틱(tick)이므로 초보자가 사용하기 어려울 수 있습니다.\nex) ' + prefix + '합주악보 종료 24000 : 24000틱 이전의 음표만 출력합니다.'},
						{ name: '최저음', value: '지정한 음높이 이상의 음표만 출력합니다.\n단위는 가온다(C4)를 60으로 하여 반음 당 1씩 계산하여 입력합니다.\nex) ' + prefix + '합주악보 최저음 35 : B1 이상의 음높이만 출력합니다.'},
						{ name: '최고음', value: '지정한 음높이 이하의 음표만 출력합니다.\n단위는 가온다(C4)를 60으로 하여 반음 당 1씩 계산하여 입력합니다.\nex) ' + prefix + '합주악보 최고음 68 : G+4 이하의 음높이만 출력합니다.'},
						{ name: '음표깎기', value: '살짝만 겹쳐서 화음 개수를 늘리는 음표가 있을 경우 음표 길이를 조금 줄여서 화음 개수를 줄입니다.\n범위는 0 (깎지 않음) ~ 100 (모두 깎음) 중 입력할 수 있습니다.\nex) ' + prefix + '합주악보 음표깎기 10 : 음표 길이의 10%까지 깎습니다.'},
						{ name: 'mml', value: 'ms2mml이 아닌 mml 파일로 출력합니다.\nex) ' + prefix + '합주악보 mml'});
					message.channel.send(exampleEmbed);
			} else if (args[0] == '팁') {
				const exampleEmbed = new MessageEmbed()
					.setColor('#8cffa9')
					.setTitle('SayangBot 명령어 (' + prefix + args[0] + ')')
					.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
					.setDescription('출력된 악보에 문제가 있을 때 시도해볼 수 있는 해결책입니다.')
					.addFields(
						{ name: '글자 수 줄이기', value: '' + prefix + '악보 해상도 5 등으로 해상도를 낮추어 보세요. 해상도 옵션을 입력하지 않은 경우 기본값 6으로 적용되고 있으며, 낮게 입력할수록 글자수가 줄어들고 악보의 품질이 낮아집니다.\n' + prefix + '악보 템포뭉개기 2 등으로 템포뭉개기 기능을 사용해 보세요. 템포뭉개기 옵션을 입력하지 않은 경우 기본값 0으로 적용되고 있으며, 높게 입력할수록 글자수가 줄어들고 변속의 개수가 적어집니다.'},
						{ name: '악보에 이상한 음이?', value: '' + prefix + '악보 명령어로 독주악보를 만들 때 드럼제외 옵션을 입력하지 않으면 화음에 맞지 않는 드럼코드 음이 섞여서 의도하지 않은 소리가 날 수 있습니다. ' + prefix + '악보 드럼제외 와 같이 사용해 보세요.'},
						{ name: '합주 볼륨 개별 조절', value: '' + prefix + '합주악보 명령어 사용 시에는 모든 악기가 별다른 볼륨 수정 없이 파일 내용대로 출력됩니다. 특정 악기에만 볼륨 설정을 적용하여 개별 출력하려면 ' + prefix + '악보 <원하는 악기> 볼륨증가/볼륨감소/고정볼륨 등의 옵션을 적옹해 보세요. 볼륨을 더 자세히 조정하려면 ' + prefix + '도움말 악보 에서 볼륨1/볼륨2/볼륨3 기능을 참고해 보세요.'},
						{ name: '봇이 응답하지 않을 때', value: '' + prefix + '악보 변환은 미디 파일의 크기에 따라 1분까지도 소요될 수 있습니다. 만약 변환 명령어에 1분 이상 응답이 없거나, 미디 파일을 보내도 반응이 없는 경우, 봇 자체가 죽은 것입니다. 이때는 개발자가 수 시간 내로 오류를 수정한 뒤 봇을 다시 시작할 것입니다. 9/24 이후로 꾸준히 패치 중입니다. 조금만 기다려 주세요...'},
						{ name: '파일이 열리지 않음 & 싱크가 어긋남', value: '열리지 않는 미디 파일에 대해서는 다른 프로그램 (3mle, Musescore 등) 에서의 문제 여부를 확인하신 뒤 ' + prefix + '피드백 명령어로 제보해주시길 부탁드립니다.'},
						{ name: '버그 제보', value: '' + prefix + '피드백 으로 시작하는 메시지는 모두 개발자에게 자동으로 전달됩니다. 자유롭게 적어서 보내주시면 개발에 큰 도움이 됩니다. 감사합니다.'});
					message.channel.send(exampleEmbed);
			} else if (args[0] == '피드백') {
				const exampleEmbed = new MessageEmbed()
					.setColor('#8cffa9')
					.setTitle('SayangBot 명령어 (' + prefix + args[0] + ')')
					.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
					.setDescription('' + prefix + '피드백 으로 시작하는 메시지는 모두 개발자에게 자동으로 전달됩니다. 자유롭게 적어서 보내주시면 개발에 큰 도움이 됩니다. 감사합니다.');
					message.channel.send(exampleEmbed);
			} else {
				sendError(message.channel, '' + command + (checkBatchimEnding(command)?'은':'는') + ' 없는 명령어입니다. 명령어 목록은 ' + prefix + '도움말 을 참고해 주세요.');
			}
		}
		return;
	}
	if (command == '팁') {
		const exampleEmbed = new MessageEmbed()
			.setColor('#8cffa9')
			.setTitle('팁')
			.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
			.setDescription('출력된 악보에 문제가 있을 때 시도해볼 수 있는 해결책입니다.')
			.addFields(
				{ name: '글자 수 줄이기', value: '' + prefix + '악보 해상도 5 등으로 해상도를 낮추어 보세요. 해상도 옵션을 입력하지 않은 경우 기본값 6으로 적용되고 있으며, 낮게 입력할수록 글자수가 줄어들고 악보의 품질이 낮아집니다.\n' + prefix + '악보 템포뭉개기 2 등으로 템포뭉개기 기능을 사용해 보세요. 템포뭉개기 옵션을 입력하지 않은 경우 기본값 0으로 적용되고 있으며, 높게 입력할수록 글자수가 줄어들고 변속의 개수가 적어집니다.'},
				{ name: '합주 볼륨 개별 조절', value: '' + prefix + '합주악보 명령어 사용 시에는 모든 악기가 별다른 볼륨 수정 없이 파일 내용대로 출력됩니다. 특정 악기에만 볼륨 설정을 적용하여 개별 출력하려면 ' + prefix + '악보 <원하는 악기> 볼륨증가/볼륨감소/고정볼륨 등의 옵션을 적옹해 보세요. 볼륨을 더 자세히 조정하려면 ' + prefix + '도움말 악보 에서 볼륨1/볼륨2/볼륨3 기능을 참고해 보세요.'},
				{ name: '악보에 이상한 음이?', value: '' + prefix + '악보 명령어로 독주악보를 만들 때 드럼제외 옵션을 입력하지 않으면 화음에 맞지 않는 드럼코드 음이 섞여서 의도하지 않은 소리가 날 수 있습니다. ' + prefix + '악보 드럼제외 와 같이 사용해 보세요.'},
				{ name: '봇이 응답하지 않을 때', value: '' + prefix + '악보 변환은 미디 파일의 크기에 따라 1분까지도 소요될 수 있습니다. 만약 변환 명령어에 1분 이상 응답이 없거나, 미디 파일을 보내도 반응이 없는 경우, 봇 자체가 죽은 것입니다. 이때는 개발자가 수 시간 내로 오류를 수정한 뒤 봇을 다시 시작할 것입니다. 9/24 이후로 꾸준히 패치 중입니다. 조금만 기다려 주세요...'},
				{ name: '파일이 열리지 않음 & 싱크가 어긋남', value: '열리지 않는 미디 파일에 대해서는 다른 프로그램 (3mle, Musescore 등) 에서의 문제 여부를 확인하신 뒤 ' + prefix + '피드백 명령어로 제보해주시길 부탁드립니다.'},
				{ name: '버그 제보', value: '' + prefix + '피드백 으로 시작하는 메시지는 모두 개발자에게 자동으로 전달됩니다. 자유롭게 적어서 보내주시면 개발에 큰 도움이 됩니다. 감사합니다.'});
			message.channel.send(exampleEmbed);
			return;
	}
	
	if (command == 'help') {
		if (args.length == 0) {
			const exampleEmbed = new MessageEmbed()
				.setColor('#8cffa9')
				.setTitle('SayangBot')
				.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
				.setDescription('한국어 도움말은 ' + prefix + '도움말 을 참고하십시오.\nThis bot converts MIDI files (*.mid) to ms2mml.\nIf you upload a MIDI file to this chat room, SayangBot will automatically recognize it and operate it.\nIf you are reluctant to publish the MIDI file, it will also work in private message (DM).\nBut, the mid file you send is saved on my computer for processing.\nCheck details with'+prefix+'help <command>\nex) ' + prefix + 'help solo')
				.addFields(
					{ name: prefix+'info', value: 'General information about MIDI files'},//, inline: true },
					{ name: prefix+'solo', value: 'Convert all notes to 1 solo sheet'},
					{ name: prefix+'ensemble', value: 'Converted to sheet for each instrument for ensemble'},
					{ name: prefix+'tip', value: 'If there is a problem with the converted sheet music, please refer to it!'},
					{ name: prefix+'feedback', value: 'Any messages that start with ' + prefix + 'Feedback are automatically forwarded to the developer. Feel write it down and send it. it will be a great help for development. Thank you.'});
			message.channel.send(exampleEmbed);
		} else {
			if (args[0] == 'info') {
				const exampleEmbed = new MessageEmbed()
					.setColor('#8cffa9')
					.setTitle('SayangBot command (' + prefix + args[0] + ')')
					.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
					.setDescription('You can check the overall information of the MIDI file.')
					.addFields(
						{ name: '파일 이름', value: 'This is the mid file currently in use. The file will remain in effect until a new file is uploaded or SayangBot reboots. Files in use in a DM are not affected by other people"s DMs or files used by the server.'},//, inline: true },
						{ name: 'BPM', value: 'You can roughly check the tempo of the song. The available tempo chord range in MapleStory 2 is 32~255, but SayangBot can handle 8~510 automatically.'},
						{ name: '박자', value: 'You can check the time signatures of a song. This is actually useless information.'},
						{ name: '재생 시간', value: 'You can check the playback time of a song in hours/minutes/seconds. The playing time of the printed sheet music may be slightly different depending on the converted tempo chord.'},
						{ name: '곡 전체 틱 / 4분음표 틱', value: 'Yes it is.'},
						{ name: '트랙 정보', value: 'You can check the number of tracks in the file, the channels used, the instruments, the number of notes, and whether or not sustain event is present. Take a closer look when making ensemble sheet music.'},
						{ name: '텍스트 이벤트', value: 'The messages written by the creator of the MIDI file.'},
						{ name: '저작권 정보', value: 'Copyright information recorded in the mid file.'});
				message.channel.send(exampleEmbed);
			} else if (args[0] == 'solo') {
				const exampleEmbed = new MessageEmbed()
					.setColor('#8cffa9')
					.setTitle('SayangBot command (' + prefix + args[0] + ')')
					.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
					.setDescription('All notes in a MIDI file are output regardless of instrument. It can be used to make sheet music for solo use, or to divide it into two or three-person ensembles due to the number of characters.\nThe options available in the solo command are below.\nex) ' + prefix + 'solo <Option 1> <Option 2> ... <Option n>')
					.addFields(
						{ name: 'triplet', value: 'For songs with many triplets (especially swing), change the tempo to handle it.\nex) ' + prefix + 'solo triplet'},
						{ name: 'ignore10000', value: 'It outputs sheet music with more than 10,000 characters without length division.\nex) ' + prefix + 'solo ignore10000'},
						{ name: 'chorddivision', value: 'Divide the chords of the sheet music to make sheet music for ensemble.\nex) ' + prefix + 'solo chorddivision'},
						{ name: 'instrument', value: 'Among the notes recorded in the mid file, only the notes of a specific instrument are output.\nex) ' + prefix + 'solo instrument fretlessbassguitar'},
						{ name: 'sustain', value: 'This is a sustain pedal on/off switch.\nThe default is on.\nex) ' + prefix + 'solo sustain on : Outputs sustain to the score as well.\nIf not configured correctly to initialize sustain events periodically, MIDI polyphony If the number is exceeded, some sound may not be heard.\nIf you use an instrument that is not suitable for sustain (electric guitar, cello, etc.), users around you may be block you.'},
						{ name: 'motion', value: 'Automatic motion on/off switch. If the performance is stopped for a certain amount of time, the character will stop motion.\nThe default is off.\nex) ' + prefix + 'solo motion on : Automatic motion code is output to the score.'},
						{ name: 'resolution', value: 'Adjusts the quality of the score.\nThe higher the number from 0~6, the more natural it sounds, but the longer the score.\nThe default is 6.\nex) ' + prefix + 'solo resolution 4 : The sheet music will be shorter than default.'},
						{ name: 'mergetempo', value: 'Adjusts the number of tempo events.\nThe lower the number from 0 to 6, the more natural it sounds, but the longer the score.\nThe default is 0.\nex) ' + prefix + 'solo mergetempo 3 : The sheet music will be shorter than default.'},
						{ name: 'mergesustain', value: 'Adjusts the number of sustain pedal events.\nThe lower the number from 0 to 6, the more natural it sounds, but the longer the score.\nThe default is 0.\nex) ' + prefix + 'solo mergesustain 3 : The sheet music is shorter than the default setting.'},
						{ name: 'volumeincrease', value: 'Increase the volume towards V15.\nex) ' + prefix + 'solo volumeincrease'},
						{ name: 'volumedecrease', value: 'Decrease the volume towards V0.\nex) ' + prefix + 'solo volumedecrease'},
						{ name: 'fixedvolume', value: 'Ignores the volume recorded in the mid file and fixes it to the entered volume.\nex) ' + prefix + 'solo fixedvolume 14\nThe unit is MapleStory"s V chord (8 MIDI velocity per 1 V value).'},
						{ name: 'volume1/volume2/volume3', value: 'Adjust the volume in custom way.\nex) ' + prefix + 'solo volume1 3 volume2 0.5 volume3 -5 : Increase the volume by 3, multiply by 0.5, and decrease it by 5.\nThe unit is MapleStory"s unit. Input as V chord (8 MIDI velocity per 1 V value).'},
						{ name: 'fixedtempo', value: 'The tempo recorded in the mid file is ignored and fixed at the entered tempo.\nex) ' + prefix + 'solo fixedtempo 180'},
						{ name: 'drumexclude', value: 'The notes of the drum chord (channel 10, etc.) are excluded.\nex) ' + prefix + 'solo drumexclude'},
						{ name: 'drumonly', value: 'Only the notes of the drum chord (channel 10, etc.) are selected and output.\nex) ' + prefix + 'solo drumonly'},
						{ name: 'track', value: 'Adds the specified track to the output range.\nex) ' + prefix + 'solo track 3 track 5: Only the notes of Track 3 and Track 5 are output to the score.'},
						{ name: 'channel', value: 'Adds the specified channel to the output range.\nex) ' + prefix + 'solo channel 7 channel 9 : Only the notes of channels 7 and 9 are output to the score.'},
						{ name: 'trackchannel', value: 'Adds the specified track-channel to the output range.\nex) ' + prefix + 'solo trackchannel 3 1 trackchannel 2 9 : Only the note of channel 1 of track 3 and the note of channel 9 of track 2 are output to the score.'},
						{ name: 'start', value: 'Only notes after the specified time are output.\nThe unit is MIDI ticks, so it may be difficult for beginners to use.\nex) ' + prefix + 'solo start 4800 : Only notes after 4800 ticks are output.'},
						{ name: 'end', value: 'Only notes before the specified time are output.\nThe unit is MIDI ticks, so it may be difficult for beginners to use.\nex) ' + prefix + 'solo end 24000 : Only the notes before 24000 ticks are output.'},
						{ name: 'lowest', value: 'Only notes higher than the specified pitch are output.\nThe unit is calculated by calculating 1 per semitone with the C4 being 60.\nex) ' + prefix + 'solo lowest 35 : Only pitches above B1 are output.'},
						{ name: 'highest', value: 'Only notes below than the specified pitch are output.\nThe unit is calculated by calculating 1 per semitone with the C4 being 60.\nex) ' + prefix + 'solo highest 68 : Only pitches below G+4 are output.'},
						{ name: 'cutoff', value: 'If there are notes that slightly overlap so that increase the number of chords, shorten the length of the notes slightly to reduce the number of chords.\nThe range is 0 (uncut) to 100 (all cut).\nex) ' + prefix + 'solo cutoff 10 : Cuts up to 10% of the note length.'},
						{ name: 'mml', value: 'Output to mml file, not ms2mml.\nex) ' + prefix + 'solo mml'});
					message.channel.send(exampleEmbed);
			} else if (args[0] == 'ensemble') {
				const exampleEmbed = new MessageEmbed()
					.setColor('#8cffa9')
					.setTitle('SayangBot command (' + prefix + args[0] + ')')
					.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
					.setDescription('Automatically create ensemble scores from MIDI files.\nThe options available in the ensemble command are below.\nex) ' + prefix + 'ensemble <Option 1> <Option 2> ... <Option n>')
					.addFields(
						{ name: 'triplet', value: 'For songs with many triplets (especially swing), change the tempo to handle it.\nex) ' + prefix + 'ensemble triplet'},
						{ name: 'sustain', value: 'This is a sustain pedal on/off switch.\nThe default is on.\nex) ' + prefix + 'ensemble sustain on : Outputs sustain to the score as well.\nIf not configured correctly to initialize sustain events periodically, MIDI polyphony If the number is exceeded, some sound may not be heard.\nIf you use an instrument that is not suitable for sustain (electric guitar, cello, etc.), users around you may be block you.'},
						{ name: 'motion', value: 'Automatic motion on/off switch. If the performance is stopped for a certain amount of time, the character will stop motion.\nThe default is off.\nex) ' + prefix + 'ensemble motion on : Automatic motion code is output to the score.'},
						{ name: 'resolution', value: 'Adjusts the quality of the score.\nThe higher the number from 0~6, the more natural it sounds, but the longer the score.\nThe default is 6.\nex) ' + prefix + 'ensemble resolution 4 : The sheet music will be shorter than default.'},
						{ name: 'mergetempo', value: 'Adjusts the number of tempo events.\nThe lower the number from 0 to 6, the more natural it sounds, but the longer the score.\nThe default is 0.\nex) ' + prefix + 'ensemble mergetempo 3 : The sheet music will be shorter than default.'},
						{ name: 'mergesustain', value: 'Adjusts the number of sustain pedal events.\nThe lower the number from 0 to 6, the more natural it sounds, but the longer the score.\nThe default is 0.\nex) ' + prefix + 'ensemble mergesustain 3 : The sheet music is shorter than the default setting.'},
						{ name: 'volumeincrease', value: 'Increase the volume towards V15.\nex) ' + prefix + 'ensemble volumeincrease'},
						{ name: 'volumedecrease', value: 'Decrease the volume towards V0.\nex) ' + prefix + 'ensemble volumedecrease'},
						{ name: 'fixedvolume', value: 'Ignores the volume recorded in the mid file and fixes it to the entered volume.\nex) ' + prefix + 'ensemble fixedvolume 14\nThe unit is MapleStory"s V chord (8 MIDI velocity per 1 V value).'},
						{ name: 'volume1/volume2/volume3', value: 'Adjust the volume in custom way.\nex) ' + prefix + 'ensemble volume1 3 volume2 0.5 volume3 -5 : Increase the volume by 3, multiply by 0.5, and decrease it by 5.\nThe unit is MapleStory"s unit. Input as V chord (8 MIDI velocity per 1 V value).'},
						{ name: 'fixedtempo', value: 'The tempo recorded in the mid file is ignored and fixed at the entered tempo.\nex) ' + prefix + 'ensemble fixedtempo 180'},
						{ name: 'start', value: 'Only notes after the specified time are output.\nThe unit is MIDI ticks, so it may be difficult for beginners to use.\nex) ' + prefix + 'ensemble start 4800 : Only notes after 4800 ticks are output.'},
						{ name: 'end', value: 'Only notes before the specified time are output.\nThe unit is MIDI ticks, so it may be difficult for beginners to use.\nex) ' + prefix + 'ensemble end 24000 : Only the notes before 24000 ticks are output.'},
						{ name: 'lowest', value: 'Only notes higher than the specified pitch are output.\nThe unit is calculated by calculating 1 per semitone with the C4 being 60.\nex) ' + prefix + 'ensemble lowest 35 : Only pitches above B1 are output.'},
						{ name: 'highest', value: 'Only notes below than the specified pitch are output.\nThe unit is calculated by calculating 1 per semitone with the C4 being 60.\nex) ' + prefix + 'ensemble highest 68 : Only pitches below G+4 are output.'},
						{ name: 'cutoff', value: 'If there are notes that slightly overlap so that increase the number of chords, shorten the length of the notes slightly to reduce the number of chords.\nThe range is 0 (uncut) to 100 (all cut).\nex) ' + prefix + 'ensemble cutoff 10 : Cuts up to 10% of the note length.'},
						{ name: 'mml', value: 'Output to mml file, not ms2mml.\nex) ' + prefix + 'ensemble mml'});
					message.channel.send(exampleEmbed);
			} else if (args[0] == 'tip') {
				const exampleEmbed = new MessageEmbed()
					.setColor('#8cffa9')
					.setTitle('Tip')
					.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
					.setDescription('This is general solution you can try when there is a problem with the printed sheet music.')
					.addFields(
						{ name: 'Reducing the number of characters', value: '' + prefix + 'ensemble resolution 5 Try lowering the resolution, etc. If no resolution option is entered, the default value of 6 is applied. The lower the input, the fewer characters and the lower the quality of the score.\n' + prefix + 'ensemble mergetempo 2 Try using the mergetempo option, etc. If the mergetempo option is not entered, the default value of 0 is applied, and the higher the input, the fewer characters.'},
						{ name: 'Individual volume control in ensemble', value: '' + prefix + 'When using the ensemble sheet music command, all instruments are output as the file contents without changing the volume. If you want to apply the volume setting to only a specific instrument and output it individually, try adding options such as ' + prefix + 'ensemble <desired instrument> volumeincrease/volumedecrease/fixedvolume. If you want to adjust the volume in more detail, refer to the ' + prefix + 'help solo for the detailed function.'},
						{ name: 'Strange notes in the sheet music?', value: '' + prefix + 'If you do not enter the option to exclude drums when creating solo score with the solo command, drum chord notes that do not match the chord may be mixed, resulting in an unintended sound. Try ' + prefix + 'ensemble drumexclude'},
						{ name: 'When the bot is not responding', value: '' + prefix + 'Score conversion may take up to a minute, depending on the size of the MIDI file. If there is no response to the conversion command for more than 1 minute, or if there is no response even after sending a MIDI file, the bot itself is dead. In this case, the developer will fix the error within a few hours and restart the bot. It"s been patching steadily since 9/24. wait please...'},
						{ name: 'File won"t open & out of sync', value: 'I have reviewed this bug for a while, but as of now, I have only confirmed that the MIDI file with the problem does not open with other programs or opens strangely. If you have time, it would be of great help if you provide additional information with the ' + prefix + 'feedback <content> command. I"m curious as to which program the mid file was made with...'},
						{ name: 'Bug report', value: 'Any messages that start with ' + prefix + 'feedback are automatically forwarded to the developer. Feel write it down and send it. it will be a great help for development. Thank you.'});
					message.channel.send(exampleEmbed);
			} else if (args[0] == 'feedback') {
				const exampleEmbed = new MessageEmbed()
					.setColor('#8cffa9')
					.setTitle('SayangBot command (' + prefix + args[0] + ')')
					.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
					.setDescription('Any messages that start with ' + prefix + 'feedback are automatically forwarded to the developer. Feel write it down and send it. it will be a great help for development. Thank you.');
					message.channel.send(exampleEmbed);
			} else {
				sendError(message.channel, '' + command + ' does not exist. For a list of commands, refer to the ' + prefix + 'help');
			}
		}
		return;
	}
	if (command == 'tip') {
		const exampleEmbed = new MessageEmbed()
			.setColor('#8cffa9')
			.setTitle('Tip')
			.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
			.setDescription('This is general solution you can try when there is a problem with the printed sheet music.')
			.addFields(
				{ name: 'Reducing the number of characters', value: '' + prefix + 'ensemble resolution 5 Try lowering the resolution, etc. If no resolution option is entered, the default value of 6 is applied. The lower the input, the fewer characters and the lower the quality of the score.\n' + prefix + 'ensemble mergetempo 2 Try using the mergetempo option, etc. If the mergetempo option is not entered, the default value of 0 is applied, and the higher the input, the fewer characters.'},
				{ name: 'Individual volume control in ensemble', value: '' + prefix + 'When using the ensemble sheet music command, all instruments are output as the file contents without changing the volume. If you want to apply the volume setting to only a specific instrument and output it individually, try adding options such as ' + prefix + 'ensemble <desired instrument> volumeincrease/volumedecrease/fixedvolume. If you want to adjust the volume in more detail, refer to the ' + prefix + 'help solo for the detailed function.'},
				{ name: 'Strange notes in the sheet music?', value: '' + prefix + 'If you do not enter the option to exclude drums when creating solo score with the solo command, drum chord notes that do not match the chord may be mixed, resulting in an unintended sound. Try ' + prefix + 'ensemble drumexclude'},
				{ name: 'When the bot is not responding', value: 'Score conversion may take up to a minute, depending on the size of the MIDI file. If there is no response to the conversion command for more than 1 minute, or if there is no response even after sending a MIDI file, the bot itself is dead. In this case, the developer will fix the error within a few hours and restart the bot. It"s been patching steadily since 9/24. wait please...'},
				{ name: 'File won"t open & out of sync', value: 'I have reviewed this bug for a while, but as of now, I have only confirmed that the MIDI file with the problem does not open with other programs or opens strangely. If you have time, it would be of great help if you provide additional information with the ' + prefix + 'feedback <content> command. I"m curious as to which program the mid file was made with...'},
				{ name: 'Bug report', value: 'Any messages that start with ' + prefix + 'feedback are automatically forwarded to the developer. Feel write it down and send it. it will be a great help for development. Thank you.'});
			message.channel.send(exampleEmbed);
			return;
	}
	
	if (command == 'user2filename') {
		client.users.fetch('364432570005323796', false).then((user) => {
			keysList = Object.keys(user2filename);
			tempMessage = '**Current status of user2filename**';
			for (i=0; i<keysList.length; i++) {
				tempMessage += "\n" + String(keysList[i]) + ':' + String(user2filename[keysList[i]]);
			}
			user.send(tempMessage);
		});
		return;
	}
	
	if (command == 'user2time') {
		client.users.fetch('364432570005323796', false).then((user) => {
			keysList = Object.keys(user2time);
			tempMessage = '**Current status of user2time**';
			for (i=0; i<keysList.length; i++) {
				tempMessage += "\n" + String(keysList[i]) + ':' + String(user2time[keysList[i]]);
			}
			user.send(tempMessage);
		});
		return;
	}
	
	if (command == 'user2processtime') {
		client.users.fetch('364432570005323796', false).then((user) => {
			keysList = Object.keys(user2time);
			tempMessage = '**Current status of user2processtime**';
			for (i=0; i<keysList.length; i++) {
				tempMessage += "\n" + String(keysList[i]) + ':' + String(user2processtime[keysList[i]]);
			}
			user.send(tempMessage);
		});
		return;
	}
	
	if (command == 'instrumentName2num') {
		client.users.fetch('364432570005323796', false).then((user) => {
			keysList = Object.keys(instrumentName2num);
			tempMessage = '**Current status of instrumentName2num**';
			for (i=0; i<keysList.length; i++) {
				tempMessage += "\n" + String(keysList[i]) + ':' + String(instrumentName2num[keysList[i]]);
			}
			user.send(tempMessage);
		});
		return;
	}
	
	isStrict = false;
	listTrackChannel = createArray(iMaxTrack, iMaxChannel);
	for (itrack=0; itrack<iMaxTrack; itrack++) {
		for (ichannel=0; ichannel<iMaxChannel; ichannel++) {
			listTrackChannel[itrack][ichannel] = false;
		}
	}
	
	iOnoffSustain = true;
	iOnoffMotion = false;
	cooldownMotionInQuarter = 7;
	timeStartNote = 0;
	timeEndNote = Infinity;
	pitchLowerBound = -999;
	pitchUpperBound = 999;
	noteResolution = 64;
	tempoResolution = 1;
	susResolution = 1;
	volumeAdd1 = 0.;
	volumeMul2 = 1.;
	volumeAdd3 = 0.;
	iOnoffDrum = 0;
	iMethodDup = 1;
	lOnoffHorizontalDivision = true;
	lOnoffVerticalDivision = false;
	if (command=='합주악보') {
		lOnoffHorizontalDivision = false;
		lOnoffVerticalDivision = true;
	}
	lOnoffMotionSpeedTempo = false;
	lengthHorizontalDivision = 10000;
	iFixedTempo = -1;
	iFixedInstrument = -1;
	isMML = false;
	isSusCompress = false;
	maxCutLength = 0;
	isTriplet = false;
	isAutoEnhance = true;
	isAutoDaecheInstrument = true;
	tempoMulti = 1.;
	tlimit1 = 5000;
	tlimit2 = 12000;
	
	if (message.content[0] != prefix) { return; }
	
	if (!(command == '정보' || command == 'tempomat' || command == 'timedivision' || command == 'timesigmat'
		|| command == 'sustaintrackchannel' || command == 'notes' || command == 'notes6' || command == 'cuttablemat' || command == 'currentchannelinstrument' || command == 'currentchannelvolume'
		|| command == '악보'|| command == '합주악보')) {
		if (!isEnglish) {
			sendError(message.channel, '' + command + (checkBatchimEnding(command)?'은':'는') + ' 없는 명령어입니다. 명령어 목록은 ' + prefix + '도움말 을 참고해 주세요.');
		} else {
			sendError(message.channel, '' + command + (checkBatchimEnding(command)?' is':' is') + ' not a valid command for SayangBot. For a list of commands, see ' + prefix + 'help');
		}
		return;
	}
	
	if (!(Object.keys(user2filename).includes(((message.guild)?('guild_'+message.channel.id):('dm_'+message.author.id))))) {
		if (!isEnglish) {
			tempMessage = '인식된 미디 파일이 없습니다. 미디 파일이 첨부된 메시지를 전송해 주세요.'
		} else {
			tempMessage = 'The MIDI file is not recognized yet. Please send a message with the MIDI file attached.'
		}
		sendError(message.channel, tempMessage);
		return;
	}
	
	while (args.length > 0) {
		if (args[0] == '엄격하게') {
			isStrict = true; args.shift();
		} else if (args[0] == '트랙') {
			if (Number.isInteger(Number(args[1]))) {
				if (1<=Number(args[1]) && Number(args[1])<=iMaxTrack) {
					for (ichannel=0; ichannel<iMaxChannel; ichannel++) { listTrackChannel[Number(args[1])-1][ichannel] = true; }
				} else {
					if (!isEnglish) {
						sendWarning(message.channel, '입력한 트랙 번호 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 트랙 번호는 1부터 ' + iMaxTrack + ' 사이의 정수로 입력해 주세요.');
					} else {
						sendWarning(message.channel, 'The entered track number ' + args[1] + ' is invalid and will be ignored. Enter the track number as an integer between 1 and ' + iMaxTrack + '.');
					}
				}
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 트랙 번호 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 트랙 번호는 1부터 ' + iMaxTrack + ' 사이의 정수로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered track number ' + args[1] + ' is invalid and will be ignored. Enter the track number as an integer between 1 and ' + iMaxTrack + '.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '채널') {
			if (Number.isInteger(Number(args[1]))) {
				if (1<=Number(args[1]) && Number(args[1])<=iMaxChannel) {
					for (ichannel=0; ichannel<iMaxChannel; ichannel++) { listTrackChannel[itrack][Number(args[1])-1] = true; }
				} else {
					if (!isEnglish) {
						sendWarning(message.channel, '입력한 채널 번호 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 채널 번호는 1부터 ' + iMaxChannel + ' 사이의 정수로 입력해 주세요.');
					} else {
						sendWarning(message.channel, 'The entered channel number ' + args[1] + ' is invalid and will be ignored. Enter the channel number as an integer between 1 and ' + iMaxChannel + '.');
					}
				}
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 채널 번호 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 채널 번호는 1부터 ' + iMaxChannel + ' 사이의 정수로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered channel number ' + args[1] + ' is invalid and will be ignored. Enter the channel number as an integer between 1 and ' + iMaxChannel + '.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '트랙채널') {
			if (Number.isInteger(Number(args[1])) && Number.isInteger(Number(args[1]))) {
				if (1<=Number(args[1]) && Number(args[1])<=iMaxTrack && 1<=Number(args[2]) && Number(args[2])<=iMaxChannel) {
					listTrackChannel[args[1]-1][args[2]-1] = true;
				} else {
					if (!isEnglish) {
						sendWarning(message.channel, '입력한 트랙채널 번호 ' + args[1] + '-' + args[2] + '이(가) 적절하지 않아 무시됩니다. 트랙 번호는 1부터 ' + iMaxTrack + ' 사이의 정수로 입력해 주세요. 채널 번호는 1부터 ' + iMaxChannel + ' 사이의 정수로 입력해 주세요.');
					} else {
						sendWarning(message.channel, 'The entered track/channel number ' + args[1] + '-' + args[2] + ' is invalid and will be ignored. Enter the track number as an integer between 1 and ' + iMaxTrack + '. Enter the channel number as an integer between 1 and ' + iMaxChannel + '.');
					}
				}
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 트랙채널 번호 ' + args[1] + '-' + args[2] + '이(가) 적절하지 않아 무시됩니다. 트랙 번호는 1부터 ' + iMaxTrack + ' 사이의 정수로 입력해 주세요. 채널 번호는 1부터 ' + iMaxChannel + ' 사이의 정수로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered track/channel number ' + args[1] + '-' + args[2] + ' is invalid and will be ignored. Enter the track number as an integer between 1 and ' + iMaxTrack + '. Enter the channel number as an integer between 1 and ' + iMaxChannel + '.');
				}
			}
			args.shift(); args.shift(); args.shift();
		} else if (args[0] == '서스테인') {
			if (args[1] == '켜기' || args[1] == '켬' || args[1] == '온' || args[1] == 'on' || args[1] == 'o') {
				iOnoffSustain = true;
			} else if (args[1] == '끄기' || args[1] == '끔' || args[1] == '오프' || args[1] == 'off' || args[1] == 'x') {
				iOnoffSustain = false;
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 서스테인 스위치 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 서스테인 스위치는 켜기, 끄기, 켬, 끔, 온, 오프, on, off, o, x로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered sustain switch ' + args[1] + ' is invalid and will be ignored. Enter the sustain switch as 켜기, 끄기, 켬, 끔, 온, 오프, on, off, o, or x');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '모션') {
			if (args[1] == '켜기' || args[1] == '켬' || args[1] == '온' || args[1] == 'on' || args[1] == 'o') {
				iOnoffMotion = true;
			} else if (args[1] == '끄기' || args[1] == '끔' || args[1] == '오프' || args[1] == 'off' || args[1] == 'x') {
				iOnoffMotion = false;
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 모션 스위치 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 모션 스위치는 켜기, 끄기, 켬, 끔, 온, 오프, on, off, o, x로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered motion switch ' + args[1] + ' is invalid and will be ignored. Enter the motion switch as 켜기, 끄기, 켬, 끔, 온, 오프, on, off, o, or x');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '시작') {
			if (!Number.isNaN(args[1])) {
				timeStartNote = Number(args[1]);
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 시작 시간 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 시작 시간은 틱 단위로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered start time ' + args[1] + ' is invalid and will be ignored. Please enter the start time in ticks.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '종료') {
			if (!Number.isNaN(args[1])) {
				timeEndNote = Number(args[1]);
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 종료 시간 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 종료 시간은 틱 단위로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered end time ' + args[1] + ' is invalid and will be ignored. Please enter the end time in ticks.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '최저음') {
			if (Number.isInteger(Number(args[1]))) {
				pitchLowerBound = Number(args[1]);
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 최저음 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 최저음은 가온다(C4)=60을 기준으로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered lowest note ' + args[1] + ' is invalid and will be ignored. Please enter the lowest note as a guideline (C4)=60.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '최고음') {
			if (Number.isInteger(Number(args[1]))) {
				pitchUpperBound = Number(args[1]);
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 최고음 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 최고음 가온다(C4)=60을 기준으로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered highest note ' + args[1] + ' is invalid and will be ignored. Please enter the highest note as a guideline (C4)=60.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '음표깎기') {
			if (!Number.isNaN(args[1])) {
				if (Number(args[1])>=0 && Number(args[1])<=100) {
					maxCutLength = Number(args[1])/100;
				} else {
					if (!isEnglish) {
						sendWarning(message.channel, '입력한 음표깎기 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 음표깎기는 0~100 사이의 숫자로 입력해 주세요.');
					} else {
						sendWarning(message.channel, 'The entered note cutoff length ' + args[1] + ' is invalid and will be ignored. Please enter a number between 0 and 100 for note cutoff length.');
					}
				}
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 음표깎기 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 음표깎기는 0~100 사이의 숫자로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered note cutoff length ' + args[1] + ' is invalid and will be ignored. Please enter a number between 0 and 100 for note cutoff length.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '해상도') {
			if (Number.isInteger(Number(args[1]))) {
				if (0<=Number(args[1]) && Number(args[1])<=6) {
					noteResolution = Math.pow(2, Number(args[1]));
				} else {
					if (!isEnglish) {
						sendWarning(message.channel, '입력한 해상도 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 해상도는 0~6 사이의 정수로 입력해 주세요.');
					} else {
						sendWarning(message.channel, 'The entered resolution ' + args[1] + ' is invalid and will be ignored. Enter the resolution as an integer between 0 and 6.');
					}
				}
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 해상도 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 해상도는 0~6 사이의 정수로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered resolution ' + args[1] + ' is invalid and will be ignored. Enter the resolution as an integer between 0 and 6.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '템포뭉개기') {
			if (Number.isInteger(Number(args[1]))) {
				if (0<=Number(args[1]) && Number(args[1])<=6) {
					tempoResolution = Math.pow(2, Number(args[1]));
				} else {
					if (!isEnglish) {
						sendWarning(message.channel, '입력한 템포뭉개기 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 템포뭉개기는 0~6 사이의 정수로 입력해 주세요.');
					} else {
						sendWarning(message.channel, 'The entered mergetempo ' + args[1] + ' is invalid and will be ignored. Enter the mergetempo as an integer between 0 and 6.');
					}
				}
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 템포뭉개기 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 템포뭉개기는 0~6 사이의 정수로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered mergetempo ' + args[1] + ' is invalid and will be ignored. Enter the mergetempo as an integer between 0 and 6.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '서스테인뭉개기') {
			if (Number.isInteger(Number(args[1]))) {
				if (0<=Number(args[1]) && Number(args[1])<=6) {
					susResolution = Math.pow(2, Number(args[1]));
				} else {
					if (!isEnglish) {
						sendWarning(message.channel, '입력한 서스테인뭉개기 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 서스테인뭉개기는 0~6 사이의 정수로 입력해 주세요.');
					} else {
						sendWarning(message.channel, 'The entered mergesustain ' + args[1] + ' is invalid and will be ignored. Enter the mergesustain as an integer between 0 and 6.');
					}
				}
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 서스테인뭉개기 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 서스테인뭉개기는 0~6 사이의 정수로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered mergesustain ' + args[1] + ' is invalid and will be ignored. Enter the mergesustain as an integer between 0 and 6.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '모션쿨') {
			if (Number.isInteger(Number(args[1]))) {
				if (0<=Number(args[1])) {
					cooldownMotionInQuarter = Number(args[1]);
				} else {
					if (!isEnglish) {
						sendWarning(message.channel, '입력한 모션 쿨타임 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 모션 쿨타임은 0 이상의 정수로 입력해 주세요.');
					} else {
						sendWarning(message.channel, 'The entered motion cooldown ' + args[1] + ' is invalid and will be ignored. Enter the motion cooldown time as an integer greater than or equal to 0.');
					}
				}
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 모션 쿨타임 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 모션 쿨타임은 0 이상의 정수로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered motion cooldown ' + args[1] + ' is invalid and will be ignored. Enter the motion cooldown time as an integer greater than or equal to 0.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '볼륨1') {
			if (!Number.isNaN(Number(args[1]))) {
				volumeAdd1 = Number(args[1]);
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 볼륨 보정치 1 (' + args[1] + ') 이(가) 적절하지 않아 무시됩니다. 볼륨 보정치는 숫자로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered volume correction value 1 (' + args[1] + ') is invalid and will be ignored. Please enter the volume correction value as a number.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '볼륨2') {
			if (!Number.isNaN(Number(args[1]))) {
				volumeMul2 = Number(args[1]);
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 볼륨 보정치 2 (' + args[1] + ') 이(가) 적절하지 않아 무시됩니다. 볼륨 보정치는 숫자로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered volume correction value 2 (' + args[1] + ') is invalid and will be ignored. Please enter the volume correction value as a number.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '볼륨3') {
			if (!Number.isNaN(Number(args[1]))) {
				volumeAdd3 = Number(args[1]);
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 볼륨 보정치 3 (' + args[1] + ') 이(가) 적절하지 않아 무시됩니다. 볼륨 보정치는 숫자로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered volume correction value 3 (' + args[1] + ') is invalid and will be ignored. Please enter the volume correction value as a number.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '고정볼륨') {
			if (!Number.isNaN(Number(args[1]))) {
				volumeMul2 = 0;
				volumeAdd3 = Number(args[1]);
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 고정 볼륨 (' + args[1] + ') 이(가) 적절하지 않아 무시됩니다. 고정 볼륨은 숫자로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered fixed volume (' + args[1] + ') is invalid and will be ignored. Please enter the fixed volume as a number.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '볼륨증가') {
			volumeMul2 = 0.5;
			volumeAdd3 = 8;
			args.shift();
		} else if (args[0] == '볼륨감소') {
			volumeMul2 = 0.5;
			volumeAdd3 = -8;
			args.shift();
		} else if (args[0] == '드럼제외') {
			iOnoffDrum = 1;
			args.shift();
		} else if (args[0] == '드럼만') {
			iOnoffDrum = 2;
			args.shift();
		} else if (args[0] == '중복제거') {
			if (Number.isInteger(Number(args[1]))) {
				if (0<=Number(args[1]) && Number(args[1])<=2) {
					iMethodDup = Number(args[1]);
				} else {
					if (!isEnglish) {
						sendWarning(message.channel, '입력한 중복 음표 처리 기법 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 중복 음표 처리 기법은 0~2 사이의 정수로 입력해 주세요.');
					} else {
						sendWarning(message.channel, 'The entered duplicate note handling method ' + args[1] + ' is invalid and will be ignored. Please enter an integer between 0 and 2 for the duplicate note processing method.');
					}
				}
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 중복 음표 처리 기법 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 중복 음표 처리 기법은 0~2 사이의 정수로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered duplicate note handling method ' + args[1] + ' is invalid and will be ignored. Please enter an integer between 0 and 2 for the duplicate note processing method.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '길이분할') {
			if (!isEnglish) {
				sendWarning(message.channel, '이제 길이분할 옵션은 기본으로 적용되며 입력하지 않으셔도 됩니다. 길이분할 기능을 끄려면 1만자무시 옵션을 사용해 주세요.');
			} else {
				sendWarning(message.channel, 'Now the lengthdivision option is applied by default and you do not need to enter it. To turn off the lengthdivision function, please use the option ignore10000.');
			}
			args.shift();
		} else if (args[0] == '1만자무시') {
			lOnoffHorizontalDivision = false;
			args.shift();
		} else if (args[0] == '화음분할') {
			lOnoffHorizontalDivision = false;
			lOnoffVerticalDivision = true;
			args.shift();
		} else if (args[0] == '모션속도템포') {
			lOnoffMotionSpeedTempo = true;
			args.shift();
		} else if (args[0] == '고정템포') {
			if (!Number.isNaN(Number(args[1]))) {
				if ((Number(args[1]) >= 8 && Number(args[1]) < 32) || (Number(args[1]) > 255 && Number(args[1]) <= 510)) {
					if (!isEnglish) {
						sendWarning(message.channel, '입력한 고정 템포 (' + args[1] + ') 는 메이플스토리2에서 지원하는 템포 범위 (32~255) 를 벗어납니다. 하지만 SayangBot이 적당히 바꾸어서 만들어 드릴게요. 악보 코드의 템포가 입력값과 달라도 버그가 아닙니다!');
					} else {
						sendWarning(message.channel, 'The entered fixed tempo (' + args[1] + ') is outside the tempo range (32~255) supported by MapleStory 2. But SayangBot will change it appropriately and make it for you. It"s not a bug if the tempo of the score chord is different from the input value!');
					}
				} else if (Number(args[1]) < 8 || Number(args[1]) > 510) {
					if (!isEnglish) {
						sendWarning(message.channel, '입력한 고정 템포 (' + args[1] + ') 는 메이플스토리2에서 지원하는 템포 범위 (32~255) 를 벗어납니다. 악보는 만들어지지만 인게임에서 템포 코드가 적용되지 않을 수 있습니다.');
					} else {
						sendWarning(message.channel, 'The entered fixed tempo (' + args[1] + ') is outside the tempo range (32~255) supported by MapleStory 2. Scores are created, but tempo chords may not be applied in-game.');
					}
				}
				iFixedTempo = Number(args[1]);
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 고정 템포 (' + args[1] + ') 이(가) 적절하지 않아 무시됩니다. 고정 템포는 숫자로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered fixed tempo (' + args[1] + ') is invalid and will be ignored. Please enter a fixed tempo as a number.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '악기') {
			instrumentKeyList = Object.keys(instrumentName2num);
			if (instrumentKeyList.includes(args[1])) {
				iFixedInstrument = instrumentName2num[args[1]];
			} else {
				isInMidi = false;
				for (iInstr=0; iInstr<128; iInstr++) {
					if (instr2name(iInstr).toLowerCase()==args[1]) {
						isInMidi = true;
						iNumFixed = iInstr;
					}
				}
				if (isInMidi) {
					iFixedInstrument = [iNumFixed];
				} else {
					if (!isEnglish) {
						tempMessage = '입력한 악기 이름 (' + args[1] + ') 은 SayangBot이 모르는 악기이므로 무시됩니다. SayangBot이 지원하는 악기는 ';
					} else {
						tempMessage = 'The entered instrument name (' + args[1] + ') will be ignored as SayangBot does not know it. The instruments supported by SayangBot are: ';
					}
					for (iInstr=0; iInstr<instrumentKeyList.length; iInstr++) {
						if (iInstr!=0) { tempMessage += ', '; }
						tempMessage += instrumentKeyList[iInstr];
					}
					for (iInstr=0; iInstr<128; iInstr++) {
						tempMessage += ', ';
						tempMessage += instr2name(iInstr);
					}
					tempMessage += '.'
					sendWarning(message.channel, tempMessage);
					return;
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == 'mml') {
			isMML = true;
			args.shift();
		} else if (args[0] == '셋잇단') {
			isTriplet = true;
			args.shift();
		} else if (args[0] == '자동볼륨증가') {
			if (args[1] == '켜기' || args[1] == '켬' || args[1] == '온' || args[1] == 'on' || args[1] == 'o') {
				isAutoEnhance = true;
			} else if (args[1] == '끄기' || args[1] == '끔' || args[1] == '오프' || args[1] == 'off' || args[1] == 'x') {
				isAutoEnhance = false;
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 자동볼륨증가 스위치 ' + args[1] + '이(가) 적절하지 않아 무시됩니다. 자동볼륨증가 스위치는 켜기, 끄기, 켬, 끔, 온, 오프, on, off, o, x로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered auto volume increase switch ' + args[1] + ' is invalid and will be ignored. Enter the auto volume increase switch as 켜기, 끄기, 켬, 끔, 온, 오프, on, off, o, or x');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '대체악기사용안함') {
			isAutoDaecheInstrument = false;
			args.shift();
		} else if (args[0] == '템포배수') {
			if (!Number.isNaN(Number(args[1]))) {
				tempoMulti = Number(args[1]);
			} else {
				if (!isEnglish) {
					sendWarning(message.channel, '입력한 템포 배수 (' + args[1] + ') 이(가) 적절하지 않아 무시됩니다. 템포 배수는 숫자로 입력해 주세요.');
				} else {
					sendWarning(message.channel, 'The entered tempo multiplier (' + args[1] + ') is invalid and will be ignored. Please enter the tempo multiplier as a number.');
				}
			}
			args.shift(); args.shift();
		} else if (args[0] == '소요시간무시') {
			sendWarning(message.channel, '변환을 시작합니다. 소요시간무시 옵션의 지나친 사용으로 인해 서버 운영에 무리가 있을 경우, 어느날 갑자기 옵션이 사라질 수 있습니다...');
			tlimit1 *= Infinity;
			tlimit2 *= Infinity;
			args.shift();
		} else {
			if (!isEnglish) {
				sendWarning(message.channel, '입력한 옵션 ' + args[0] + '은(는) 아직 안 만들었으므로 무시됩니다.'); args.shift();
			} else {
				sendWarning(message.channel, 'The entered option ' + args[0] + ' is ignored because it has not been developed yet.'); args.shift();
			}
		}
	}
	
	if (user2processtime[((message.guild)?('guild_'+message.channel.id):('dm_'+message.author.id))] > tlimit1) {
		if (!isEnglish) {
			sendError(message.channel, '죄송합니다, 이 파일은 현재의 SayangBot이 처리하기에는 너무 큽니다. 현재 SayangBot은 통상적인 프로그램으로 제작한 mid 파일에는 문제없이 사용 가능하나, 처리 시간이 오래 걸리는 파일은 서버 안정성을 위해 실행을 차단하고 있습니다. 소요시간무시 옵션을 사용하시거나, 다른 파일을 사용해 주십시오.');
		} else {
			sendError(message.channel, 'Sorry, this file is too large for the current SayangBot to handle. Currently, SayangBot can be used without any problem in mid files made with regular programs, but files that take a long time to process, such as blackMIDI / live performance / mp3 conversion, are blocked for server stability. Please use a different file.');
		}
		return;
	}
	
	if (command == '악보' && iFixedInstrument == -1 && iOnoffDrum == 0) {
		iOnoffDrum = 1;
	}
	
	countTrackChannel = 0;
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
	
	if (timeStartNote > timeEndNote) {
		if (!isEnglish) {
			sendError(message.channel, '입력한 시작/종료 시간 범위가 적절하지 않습니다.');
		} else {
			sendError(message.channel, 'The start/end time range entered is invalid.');
		}
		console.log([timeStartNote, timeEndNote]);
		return;
	}
	
	if (pitchUpperBound < pitchLowerBound) {
		if (!isEnglish) {
			sendError(message.channel, '입력한 최저음/최고음 범위가 적절하지 않습니다.');
		} else {
			sendError(message.channel, 'The lowest/highest pitch range entered is invalid.');
		}
		return;
	}
	
	if (command == '합주악보' && iFixedInstrument != -1) {
		if (!isEnglish) {
			sendError(message.channel, '합주악보 명령어를 사용하면 SayangBot이 자동으로 악기별 악보를 모두 출력합니다. 악기 지정 출력 옵션은 필요하지 않습니다. 의도하신대로 입력하신 게 맞나요..?');
		} else {
			sendError(message.channel, 'When you use the ensemble sheet music command, SayangBot will automatically output all sheet music for each instrument. The instrument-specific output option is not required. Is it correct that you entered it as intended..?');
		}
		return;
	}
	
	if (command == '합주악보' && iOnoffDrum != 0) {
		if (!isEnglish) {
			sendError(message.channel, '합주악보 명령어와 드럼제외/드럼만 옵션의 조합은 지원하지 않습니다. 합주악보 명령어 대신 악보 명령어와 함께 사용해 주세요.');
		} else {
			sendError(message.channel, 'Combination of ensemble command and drumexclude/drumonly option is not supported. Please use it with the ' + prefix + 'solo command instead of the ' + prefix + 'ensemble command.');
		}
		return;
	}
	
	if (command == '합주악보' && iOnoffMotion) {
		if (!isEnglish) {
			sendWarning(message.channel, '합주악보 출력 시 모션 제어 기능은 아직 완성되지 않았습니다. 일부 연주자에 모션이 적용되지 않을 수 있습니다.');
		} else {
			sendWarning(message.channel, 'The motion control function when outputting ensemble sheet music is not yet complete. Motion may not be applied to some players.');
		}
	}
	
	if (command == '정보' || command == 'tempomat' || command == 'timedivision' || command == 'timesigmat'
		|| command == 'sustaintrackchannel' || command == 'notes' || command == 'notes6' || command == 'cuttablemat'
		 || command == '악보'|| command == '합주악보' || command == 'currentchannelinstrument' || command == 'currentchannelvolume') {
		currentName = ((message.guild)?('guild_'+message.channel.id):('dm_'+message.author.id))+'.mid';
		const fileExists = (file) => {
			return new Promise((resolve) => {
				fs.access(currentName, fs.constants.F_OK, (err) => {
				err ? resolve(false) : resolve(true)
				});
			})
		}
		if (fileExists) {
			fs.readFile(currentName, 'base64', function (err,data) {
				try {
					var midiArray = midiParser.parse(data);
					if (midiArray==false) {
						if (!isEnglish) {
							sendError(message.channel, '파일을 읽지 못했습니다. 명령어를 너무 빨리 입력하시면 간혹 렉 때문에 일시적으로 오류가 발생할 수 있습니다. 미디 파일을 올린 뒤 2초 후에 명령어를 입력해 보세요. 그래도 소용이 없다면, 입력하신 파일은 표준 MIDI 포맷이 아니거나 손상되었습니다 (파일 헤더가 0x4D546864으로 시작하지 않음). 만약 다른 프로그램에서 정상적으로 열리는 파일임에도 이 에러가 발생한다면 개발자에게 제보 부탁드립니다...');
						} else {
							sendError(message.channel, 'Failed to read file. If you enter a command too quickly, sometimes an error may occur temporarily due to lag. After uploading the MIDI file, try entering the command 2 seconds later. If that doesn"t work, the file you entered is not in standard MIDI format or is corrupt (file header does not start with 0x4D546864). If this error occurs even though the file is normally opened in another program, please report it to the developer...');
						}
						return;
					} else if (midiArray==-1) {
						if (!isEnglish) {
							sendError(message.channel, '현재 SayangBot이 읽을 수 없는 MIDI 파일입니다 (트랙 헤더 오류). 다른 프로그램으로 MIDI 파일을 재출력해서 사용하시거나, 업데이트를 기다려 주세요...');
						} else {
							sendError(message.channel, 'This is a MIDI file that SayangBot cannot currently read (track header error). Re-output the MIDI file with another program and use it, or wait for the update...');
						}
						return;
					}
					var numNoteTrackChannel = createArray(midiArray.track.length, iMaxChannel);
				} catch (error) {
					console.log(midiArray);
					console.error(error);
					if (!isEnglish) {
						sendError(message.channel, '파일을 읽지 못했습니다. 명령어를 너무 빨리 입력하시면 간혹 렉 때문에 일시적으로 오류가 발생할 수 있습니다. 미디 파일을 올린 뒤 2초 후에 명령어를 입력해 보세요. 그래도 소용이 없다면, 입력하신 파일은 표준 MIDI 포맷이 아니거나 손상되었습니다 (파일 헤더가 0x4D546864으로 시작하지 않음). 만약 다른 프로그램에서 정상적으로 열리는 파일임에도 이 에러가 발생한다면 개발자에게 제보 부탁드립니다...');
					} else {
						sendError(message.channel, 'Failed to read file. If you enter a command too quickly, sometimes an error may occur temporarily due to lag. After uploading the MIDI file, try entering the command 2 seconds later. If that doesn"t work, the file you entered is not in standard MIDI format or is corrupt (file header does not start with 0x4D546864). If this error occurs even though the file is normally opened in another program, please report it to the developer...');
					}
					return;
				}
				timeStartInvest = performance.now();
				
				var midiArray = midiParser.parse(data);
				var numNoteTrackChannel = createArray(midiArray.track.length, iMaxChannel);
				for (itrack=0; itrack<midiArray.track.length; itrack++) {
					for (ichannel=0; ichannel<iMaxChannel; ichannel++) {
						numNoteTrackChannel[itrack][ichannel] = 0;
					}
				}
				var sustainTrackChannel = createArray(midiArray.track.length, iMaxChannel, 0);
				var Notes = createArray(0);
				var instrumentTrackChannel = createArray(midiArray.track.length, iMaxChannel, 0);
				var trackName = createArray(midiArray.track.length);
				for (i=0; i<midiArray.track.length; i++) { trackName[i] = ''; }
				var iMaxKeySignatureTrack = createArray(midiArray.track.length);
				var iPort = createArray(midiArray.track.length);
				var maxLengthTrack = 0;
				var maxEndNote = 0;
				iMaxTempo = 0;
				iMaxTimeSignature = 0;
				iMaxText = 0;
				iMaxCopyright = 0;
				iMaxKeySignature = 0;
				isWarned255 = false;
				isWarned11 = false;
				isWarned = false;
				isDrumWarned = false;
				isLengthWarned = false;
				isChordWarned = false;
				is24NoteWarned = false;
				var currentchannelvolume = createArray(iMaxChannel, 0);
				for (ichannel=0; ichannel<iMaxChannel; ichannel++) { currentchannelvolume[ichannel] = currentchannelvolume[ichannel].concat([[0, 127]]); }
				var currentchannelinstrument = createArray(iMaxChannel, 0);
				for (ichannel=0; ichannel<iMaxChannel; ichannel++) { currentchannelinstrument[ichannel] = currentchannelinstrument[ichannel].concat([[0, 0]]); }
				
				for (itrack=0; itrack<midiArray.track.length; itrack++) {
					var currentTime = 0;
					iMaxKeySignatureTrack[itrack] = 0;
					iPort[itrack] = 0;
					for (ievent=0; ievent<midiArray.track[itrack].event.length; ievent++) {
						currentTime += midiArray.track[itrack].event[ievent].deltaTime;
						if (midiArray.track[itrack].event[ievent].type == 255) {
							if (midiArray.track[itrack].event[ievent].metaType == 81) {
								iMaxTempo++;
							} else if (midiArray.track[itrack].event[ievent].metaType == 3) {
								trackName[itrack] = ' - ' + midiArray.track[itrack].event[ievent].data;
							} else if (midiArray.track[itrack].event[ievent].metaType == 88) {
								if (itrack!=0) { console.log('Warning : time signature event in non-zero track'); continue; }
								iMaxTimeSignature++;
							} else if (midiArray.track[itrack].event[ievent].metaType == 1) {
								iMaxText++;
							} else if (midiArray.track[itrack].event[ievent].metaType == 2) {
								iMaxCopyright++;
							} else if (midiArray.track[itrack].event[ievent].metaType == 89) {
								iMaxKeySignatureTrack[itrack]++;
							} else if (midiArray.track[itrack].event[ievent].metaType == 47) {
								maxLengthTrack = (maxLengthTrack>currentTime)?maxLengthTrack:currentTime;
								continue;
							} else if (midiArray.track[itrack].event[ievent].metaType == 33) {
								iPort[itrack] = midiArray.track[itrack].event[ievent].data;
							} else if (midiArray.track[itrack].event[ievent].metaType == 127) {
								// musescore event
								// data 67, 123, 1 = chord
							} else { // unknown (text)
								iMaxText++;
							}
						} else if (midiArray.track[itrack].event[ievent].type == 11) {
							if (midiArray.track[itrack].event[ievent].data[0] == 64) {
								// sustain
							} else if (midiArray.track[itrack].event[ievent].data[0] == 7) {
								ichannel = midiArray.track[itrack].event[ievent].channel+iPort[itrack]*16;
								currentchannelvolume[ichannel] = currentchannelvolume[ichannel].concat([[currentTime, midiArray.track[itrack].event[ievent].data[1]]]);
							} else if (midiArray.track[itrack].event[ievent].data[0] == 2) {
								// breath control (ignore)
								continue;
							} else if (midiArray.track[itrack].event[ievent].data[0] == 10) {
								// pan (ignore)
								continue;
							} else if (midiArray.track[itrack].event[ievent].data[0] == 91) {
								// reverb (ignore)
								continue;
							} else if (midiArray.track[itrack].event[ievent].data[0] == 93) {
								// chorus (ignore)
								continue;
							} else if (midiArray.track[itrack].event[ievent].data[0] == 121) {
								// reset all (ignore)
								continue;
							} else if (midiArray.track[itrack].event[ievent].data[0] == 123) {
								// off all TBD
								continue;
							} else if (midiArray.track[itrack].event[ievent].data[0] == 11) {
								// expression controller (ignore)
								continue;
							} else if (midiArray.track[itrack].event[ievent].data[0] == 99) {
								// unknown (ignore)
								continue;
							} else if (isStrict && ~isWarned11) {
								sendWarning(message.channel, '파일에 포함된 미디 이벤트(타입 11, 메타타입 ' + midiArray.track[itrack].event[ievent].data[0] + ')는 뭔지 몰라서 무시하겠습니다...');
								isWarned11 = true;
								console.log(11);
								console.log(midiArray.track[itrack].event[ievent]);
								console.log(midiArray.track[itrack].event[ievent].data);
							}
						} else if (midiArray.track[itrack].event[ievent].type == 12) {
							// instrument (moved below)
						} else if (midiArray.track[itrack].event[ievent].type == 9) {
							if (midiArray.track[itrack].event[ievent].data[1] > 0) {
								ichannel = midiArray.track[itrack].event[ievent].channel+iPort[itrack]*16;
								numNoteTrackChannel[itrack][ichannel] += 1;
							}
						} else if (isStrict && isWarned) {
							sendWarning(message.channel, '파일에 포함된 미디 이벤트(타입 ' + midiArray.track[itrack].event[ievent].type + ')는 뭔지 몰라서 무시하겠습니다...');
							isWarned = true;
							console.log(midiArray.track[itrack].event[ievent].type);
							console.log(midiArray.track[itrack].event[ievent]);
							console.log(midiArray.track[itrack].event[ievent].data);
						}
						if (currentTime <0) {// && currentTime <= 140000) {
							console.log(currentTime);
							console.log(midiArray.track[itrack].event[ievent]);
						}
					}
					iMaxKeySignature = (iMaxKeySignature > iMaxKeySignatureTrack[itrack]) ? iMaxKeySignature : iMaxKeySignatureTrack[itrack];
				}
				
				var tempomat = createArray(iMaxTempo, 2);
				for (i=0; i<iMaxTempo; i++) { tempomat[i][0] = -1; tempomat[i][1] = -1; }
				var currentTempoIndex = 0;
				var maxCurrentTimeInReal = 0.;
				var timesigmat = createArray(iMaxTimeSignature, 2);
				// for (i=0; i<midiArray.track.length; i++) { 
				for (j=0; j<iMaxTimeSignature; j++) { for (k=0; k<2; k++) { timesigmat[j][k] = -1; } }
				// }
				var textMat = createArray(iMaxText, 2);
				var currentTextIndex = 0;
				var copyrightMat = createArray(iMaxCopyright, 2);
				var currentCopyrightIndex = 0;
				var timekeymat = createArray(midiArray.track.length, iMaxKeySignature, 2);
				for (i=0; i<midiArray.track.length; i++) { for (j=0; j<iMaxKeySignature; j++) { for (k=0; k<2; k++) { timekeymat[i][j][k] = -1; } } }
				
				for (itrack=0; itrack<midiArray.track.length; itrack++) {
					var currentTime = 0;
					var currentTempo = 120;
					for (ievent=0; ievent<midiArray.track[itrack].event.length; ievent++) {
						currentTime += midiArray.track[itrack].event[ievent].deltaTime;
						if (midiArray.track[itrack].event[ievent].type == 255) {
							if (midiArray.track[itrack].event[ievent].metaType == 81) {
								var thisTempo = midiArray.track[itrack].event[ievent].data;
								var tempoValue = Math.floor(60*1000000/thisTempo);
								tempomat[currentTempoIndex][0] = currentTime;
								tempomat[currentTempoIndex][1] = tempoValue*tempoMulti;
								currentTempoIndex++;
								currentTempo = thisTempo;
							}
						} else if (midiArray.track[itrack].event[ievent].type == 12) {
							ichannel = midiArray.track[itrack].event[ievent].channel+iPort[itrack]*16;
							targetPatch = midiArray.track[itrack].event[ievent].data;
							if (ichannel%16 == 9) { targetPatch = 128; }
							currentchannelinstrument[ichannel] = currentchannelinstrument[ichannel].concat([[currentTime, targetPatch]]);
						}
					}
				}
				
				tempomat.sort(sortFunctionByFirst);
				isRemoved = true;
				while (isRemoved) {
					isRemoved = false;
					for (i=0; i<tempomat.length-1; i++) {
						if (tempomat[i][0] == tempomat[i+1][0]) {
							isRemoved = true;
							newtempomat = createArray(0);
							for (i2=0; i2<tempomat.length; i2++) {
								if (i2!=i) {
									newtempomat = newtempomat.concat([tempomat[i2]]);
								}
							}
							tempomat = newtempomat;
						}
					}
				}
				
				if (iFixedTempo != -1) {
					tempomat = [[0, iFixedTempo]];
					iMaxTempo = 1;
				}
				
				for (ichannel=0; ichannel<iMaxChannel; ichannel++) {
					currentchannelvolume[ichannel] = currentchannelvolume[ichannel].sort(sortFunctionByFirst);
					currentchannelinstrument[ichannel] = currentchannelinstrument[ichannel].sort(sortFunctionByFirst);
				}
				timeMidInvest = performance.now();
				if ((timeMidInvest-timeStartInvest > tlimit1 && (command=='악보' || command=='합주악보')) || (timeMidInvest-timeStartInvest > tlimit2)) {
					if (!isEnglish) {
						sendError(message.channel, '죄송합니다, 이 파일은 현재의 SayangBot이 처리하기에는 너무 큽니다. 현재 SayangBot은 통상적인 프로그램으로 제작한 mid 파일에는 문제없이 사용 가능하나, 처리 시간이 오래 걸리는 파일은 서버 안정성을 위해 실행을 차단하고 있습니다. 소요시간무시 옵션을 사용하시거나, 다른 파일을 사용해 주십시오.');
					} else {
						sendError(message.channel, 'Sorry, this file is too large for the current SayangBot to handle. Currently, SayangBot can be used without any problem in mid files made with regular programs, but files that take a long time to process, such as blackMIDI / live performance / mp3 conversion, are blocked for server stability. Please use a different file.');
					}
					return;
				}
				
				for (itrack=0; itrack<midiArray.track.length; itrack++) {
					var currentTime = 0;
					var currentTimeInReal = 0.;
					var currentTempo = 120;
					var currentTempoIndex = 0;
					var currentTimeSigIndex = 0;
					var currentTimeKeyIndex = 0;
					
					for (ievent=0; ievent<midiArray.track[itrack].event.length; ievent++) {
						currentTime += midiArray.track[itrack].event[ievent].deltaTime;
						if (currentTempoIndex < tempomat.length) {
							if (currentTime >= tempomat[currentTempoIndex][0]) {
								diffTimeBeforeChange = 60/currentTempo*(tempomat[currentTempoIndex][0] - (currentTime-midiArray.track[itrack].event[ievent].deltaTime))/midiArray.timeDivision;
								diffTimeAfterChange = 60/tempomat[currentTempoIndex][1]*(currentTime - tempomat[currentTempoIndex][0])/midiArray.timeDivision;
								currentTempo = tempomat[currentTempoIndex][1];
								currentTempoIndex++;
							} else {
								diffTimeBeforeChange = 60/currentTempo*midiArray.track[itrack].event[ievent].deltaTime/midiArray.timeDivision;
								diffTimeAfterChange = 0;
							}
						} else {
							diffTimeBeforeChange = 60/currentTempo*midiArray.track[itrack].event[ievent].deltaTime/midiArray.timeDivision;
							diffTimeAfterChange = 0;
						}
						if (currentTimeInReal + diffTimeBeforeChange + diffTimeAfterChange < 86400) {
							currentTimeInReal += (diffTimeBeforeChange + diffTimeAfterChange);
						}
						if (midiArray.track[itrack].event[ievent].type == 255) {
							if (midiArray.track[itrack].event[ievent].metaType == 88) {
								if (itrack!=0) { continue; }
								timesigmat[currentTimeSigIndex][0] = currentTime;
								timesigmat[currentTimeSigIndex][1] = midiArray.track[itrack].event[ievent].data;
								currentTimeSigIndex++;
							} else if (midiArray.track[itrack].event[ievent].metaType == 1) {
								textMat[currentTextIndex][0] = currentTimeInReal;
								textMat[currentTextIndex][1] = midiArray.track[itrack].event[ievent].data.replace(/\n/g, ' ');
								currentTextIndex++;
							} else if (midiArray.track[itrack].event[ievent].metaType == 2) {
								copyrightMat[currentCopyrightIndex][0] = currentTimeInReal;
								copyrightMat[currentCopyrightIndex][1] = midiArray.track[itrack].event[ievent].data;
								currentCopyrightIndex++;
							} else if (midiArray.track[itrack].event[ievent].metaType == 89) {
								timekeymat[itrack][currentTimeKeyIndex][0] = currentTime;
								timekeymat[itrack][currentTimeKeyIndex][1] = midiArray.track[itrack].event[ievent].data;
								currentTimeKeyIndex++;
							} else if (midiArray.track[itrack].event[ievent].metaType == 81) {
								// tempo
							} else if (midiArray.track[itrack].event[ievent].metaType == 3) {
								// track name
							} else if (midiArray.track[itrack].event[ievent].metaType == 47) {
								// end of track
							} else if (midiArray.track[itrack].event[ievent].metaType == 33) {
								// prefefined port
							} else if (midiArray.track[itrack].event[ievent].metaType == 127) {
								// musescore event
							} else {
								tempText = midiArray.track[itrack].event[ievent].data.toString();
								textMat[currentTextIndex][0] = currentTimeInReal;
								textMat[currentTextIndex][1] = tempText.replace(/\n/g, ' ');
								currentTextIndex++;
							}
						} else if (midiArray.track[itrack].event[ievent].type == 11) {
							if (midiArray.track[itrack].event[ievent].data[0] == 64) {
								ichannel = midiArray.track[itrack].event[ievent].channel+iPort[itrack]*16;
								currentSustain = midiArray.track[itrack].event[ievent].data[1];
								for (iInstr=0; iInstr<currentchannelinstrument[ichannel].length; iInstr++) {
									if (currentchannelinstrument[ichannel][iInstr][0] <= currentTime) { currentchannelinstrumentvalue = currentchannelinstrument[ichannel][iInstr][1]; }
								}
								icurrentinstrument = currentchannelinstrumentvalue;
								if (midiArray.track[itrack].event[ievent].channel%16 == 9) {
									icurrentinstrument = 128;
								}
								sustainTrackChannel[itrack][ichannel] = sustainTrackChannel[itrack][ichannel].concat([[currentTime, currentSustain, icurrentinstrument]]);
							}
						} else if (midiArray.track[itrack].event[ievent].type == 12) {
							// moved above
						} else if (midiArray.track[itrack].event[ievent].type == 9) {
							if (currentTimeInReal > 86400) {
								if (!is24NoteWarned) {
									if (!isEnglish) {
										sendWarning(message.channel, '미디 파일에 포함된 음표 중 재생 시간 24시간 이후의 음표는 무시합니다. 미디 파일에 오류가 있을 수도 있습니다. 다른 사항이 있으시면 개발자에게 문의 바랍니다.');
									} else {
										sendWarning(message.channel, 'Notes after 24 hours of playing time are ignored. There may be errors in the MIDI file. If you have any other questions, please contact the developer.');
									}
									is24NoteWarned = true;
								}
								continue;
							}
							if (midiArray.track[itrack].event[ievent].data[1] > 0) {
								ichannel = midiArray.track[itrack].event[ievent].channel+iPort[itrack]*16;
								icurrentpitch = midiArray.track[itrack].event[ievent].data[0];
								currentchannelvolumevalue = 100;
								for (ivol=0; ivol<currentchannelvolume[ichannel].length; ivol++) {
									if (currentchannelvolume[ichannel][ivol][0] <= currentTime) { currentchannelvolumevalue = currentchannelvolume[ichannel][ivol][1]; }
									else { break; }
								}
								icurrentvolume = Math.floor(currentchannelvolumevalue/100 * midiArray.track[itrack].event[ievent].data[1]);
								currentchannelinstrumentvalue = 0;
								for (iInstr=0; iInstr<currentchannelinstrument[ichannel].length; iInstr++) {
									if (currentchannelinstrument[ichannel][iInstr][0] <= currentTime) { currentchannelinstrumentvalue = currentchannelinstrument[ichannel][iInstr][1]; }
								}
								icurrentinstrument = currentchannelinstrumentvalue;
								if (midiArray.track[itrack].event[ievent].channel%16 == 9) {
									icurrentinstrument = 128;
								}
								endTime = currentTime;
								for (iFindEnd=ievent+1; iFindEnd<midiArray.track[itrack].event.length; iFindEnd++) {
									endTime += midiArray.track[itrack].event[iFindEnd].deltaTime;
									if (midiArray.track[itrack].event[iFindEnd].type == 9) {
										if (midiArray.track[itrack].event[iFindEnd].channel+iPort[itrack]*16 == ichannel && midiArray.track[itrack].event[iFindEnd].data[0] == icurrentpitch) {
											break;
										}
									} else if (midiArray.track[itrack].event[iFindEnd].type == 8) {
										if (midiArray.track[itrack].event[iFindEnd].channel+iPort[itrack]*16 == ichannel && midiArray.track[itrack].event[iFindEnd].data[0] == icurrentpitch) {
											break;
										}
									} else if (midiArray.track[itrack].event[iFindEnd].type == 11) {
										if (midiArray.track[itrack].event[iFindEnd].data[0] == 123) {
											break;
										}
									} else if (midiArray.track[itrack].event[iFindEnd].type == 255) {
										if (midiArray.track[itrack].event[iFindEnd].metaType == 47) {
											break;
										}
									}
								}
								Notes = Notes.concat([[itrack, ichannel, icurrentpitch, icurrentvolume, currentTime, endTime, icurrentinstrument]]);
								maxEndNote = (maxEndNote>currentTime)?maxEndNote:currentTime;
							}
						}
					}
					maxCurrentTimeInReal = (maxCurrentTimeInReal > currentTimeInReal) ? maxCurrentTimeInReal : currentTimeInReal;
				}
				timeMidInvest = performance.now();
				if ((timeMidInvest-timeStartInvest > tlimit1 && (command=='악보' || command=='합주악보')) || (timeMidInvest-timeStartInvest > tlimit2)) {
					if (!isEnglish) {
						sendError(message.channel, '죄송합니다, 이 파일은 현재의 SayangBot이 처리하기에는 너무 큽니다. 현재 SayangBot은 통상적인 프로그램으로 제작한 mid 파일에는 문제없이 사용 가능하나, 처리 시간이 오래 걸리는 파일은 서버 안정성을 위해 실행을 차단하고 있습니다. 소요시간무시 옵션을 사용하시거나, 다른 파일을 사용해 주십시오.');
					} else {
						sendError(message.channel, 'Sorry, this file is too large for the current SayangBot to handle. Currently, SayangBot can be used without any problem in mid files made with regular programs, but files that take a long time to process, such as blackMIDI / live performance / mp3 conversion, are blocked for server stability. Please use a different file.');
					}
					return;
				}
				
				for (inote=0; inote<Notes.length; inote++) {
					Notes[inote][5] = (Notes[inote][5]<maxEndNote+12*midiArray.timeDivision)?Notes[inote][5]:maxEndNote+12*midiArray.timeDivision;
				}
				maxLengthTrack = (maxLengthTrack<maxEndNote+12*midiArray.timeDivision)?maxLengthTrack:maxEndNote+12*midiArray.timeDivision;
				maxCurrentTimeInReal = Math.ceil(maxCurrentTimeInReal);
				
				newSustainTrackChannel = createArray(midiArray.track.length, iMaxChannel, 0);
				for (i=0; i<midiArray.track.length; i++) {
					for (j=0; j<iMaxChannel; j++) {
						if (sustainTrackChannel[i][j].length == 0) { continue; }
						deathnote = createArray(sustainTrackChannel[i][j].length);
						for (k=1; k<sustainTrackChannel[i][j].length; k++) {
							deathnote[k] = 1;
						}
						deathnote[0] = 0;
						deathnote[sustainTrackChannel[i][j].length-1] = 0;
						for (k=0; k<sustainTrackChannel[i][j].length-1; k++) {
							for (l=k+1; l<sustainTrackChannel[i][j].length; l++) {
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
						for (k=0; k<sustainTrackChannel[i][j].length; k++) {
							if (deathnote[k]==0) {
								newSustainTrackChannel[i][j] = newSustainTrackChannel[i][j].concat([sustainTrackChannel[i][j][k]]);
							}
						}
					}
				}
				sustainTrackChannel = newSustainTrackChannel;
				timeMidInvest = performance.now();
				if ((timeMidInvest-timeStartInvest > tlimit1 && (command=='악보' || command=='합주악보')) || (timeMidInvest-timeStartInvest > tlimit2)) {
					if (!isEnglish) {
						sendError(message.channel, '죄송합니다, 이 파일은 현재의 SayangBot이 처리하기에는 너무 큽니다. 현재 SayangBot은 통상적인 프로그램으로 제작한 mid 파일에는 문제없이 사용 가능하나, 처리 시간이 오래 걸리는 파일은 서버 안정성을 위해 실행을 차단하고 있습니다. 소요시간무시 옵션을 사용하시거나, 다른 파일을 사용해 주십시오.');
					} else {
						sendError(message.channel, 'Sorry, this file is too large for the current SayangBot to handle. Currently, SayangBot can be used without any problem in mid files made with regular programs, but files that take a long time to process, such as blackMIDI / live performance / mp3 conversion, are blocked for server stability. Please use a different file.');
					}
					return;
				}
				
				isRestricted = false;
				for (i=0; i<midiArray.track.length; i++) {
					for (j=0; j<iMaxChannel; j++) {
						if (listTrackChannel[i][j]==false) {
							isRestricted = true;
						}
					}
				}
				for (i=0; i<midiArray.track.length; i++) {
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
				}
				timeMidInvest = performance.now();
				if ((timeMidInvest-timeStartInvest > tlimit1 && (command=='악보' || command=='합주악보')) || (timeMidInvest-timeStartInvest > tlimit2)) {
					if (!isEnglish) {
						sendError(message.channel, '죄송합니다, 이 파일은 현재의 SayangBot이 처리하기에는 너무 큽니다. 현재 SayangBot은 통상적인 프로그램으로 제작한 mid 파일에는 문제없이 사용 가능하나, 처리 시간이 오래 걸리는 파일은 서버 안정성을 위해 실행을 차단하고 있습니다. 소요시간무시 옵션을 사용하시거나, 다른 파일을 사용해 주십시오.');
					} else {
						sendError(message.channel, 'Sorry, this file is too large for the current SayangBot to handle. Currently, SayangBot can be used without any problem in mid files made with regular programs, but files that take a long time to process, such as blackMIDI / live performance / mp3 conversion, are blocked for server stability. Please use a different file.');
					}
					return;
				}
				
				if (command == 'tempomat') {
					for (itempo=0; itempo<tempomat.length; itempo++) {
						// if (tempomat[itempo][0] > 56000 && tempomat[itempo][0] < 57000) {
							console.log(tempomat[itempo]);
						// }
					}
					return;
				}
				if (command == 'timedivision') { console.log(midiArray.timeDivision); return; }
				if (command == 'timesigmat') { 
					// for (i=0; i<midiArray.track.length; i++) {
						for (j=0; j<iMaxTimeSignature; j++) {
							for (k=0; k<2; k++) {
								console.log(timesigmat[j][k])
							}
						}
					// }
					return;
				}
				if (command == 'sustaintrackchannel') { 
					for (i=0; i<midiArray.track.length; i++) {
						console.log('track ' + i.toString());
						for (j=0; j<iMaxChannel; j++) {
							if (sustainTrackChannel[i][j].length == 0) { continue; }
							console.log('channel ' + j.toString());
							for (k=0; k<sustainTrackChannel[i][j].length; k++) {
								console.log(sustainTrackChannel[i][j][k]);
							}
						}
					}
					return;
				}
				if (command == 'notes') {
					Notes = Notes.sort(sortFunctionByFifth);
					for (i=0; i<Notes.length; i++) {
						console.log(Notes[i]);
					}
					return;
				}
				if (command == 'notes6') {
					Notes = Notes.sort(sortFunctionByFifth);
					for (i=0; i<Notes.length; i++) {
						// if (Notes[i][4] > 56000 && Notes[i][5] < 57000) {
							console.log(Notes[i].slice(0,6));
						// }
					}
					return;
				}
				if (command == 'currentchannelinstrument') {
					console.log(currentchannelinstrument);
					return;
				}
				if (command == 'currentchannelvolume') {
					for (j=0; j<iMaxChannel; j++) {
						if (currentchannelvolume[j].length == 0) { continue; }
						console.log('channel ' + j.toString());
						for (k=0; k<currentchannelvolume[j].length; k++) {
							console.log(currentchannelvolume[j][k]);
						}
					}
					return;
				}
				
				for (inote=0; inote<Notes.length; inote++) {
					itrack = Notes[inote][0];
					ichannel = Notes[inote][1];
					if (instrumentTrackChannel[itrack][ichannel].length > 0) {
						if (instrumentTrackChannel[itrack][ichannel][instrumentTrackChannel[itrack][ichannel].length-1][1] != Notes[inote][6]) {
							instrumentTrackChannel[itrack][ichannel] = instrumentTrackChannel[itrack][ichannel].concat([[Notes[inote][4], Notes[inote][6]]]);
						}
					} else {
						instrumentTrackChannel[itrack][ichannel] = instrumentTrackChannel[itrack][ichannel].concat([[Notes[inote][4], Notes[inote][6]]]);
					}
				}
				
				textMat = textMat.sort(sortFunctionByFirst);
				
				timeMidInvest = performance.now();
				if ((timeMidInvest-timeStartInvest > tlimit1 && (command=='악보' || command=='합주악보')) || (timeMidInvest-timeStartInvest > tlimit2)) {
					if (!isEnglish) {
						sendError(message.channel, '죄송합니다, 이 파일은 현재의 SayangBot이 처리하기에는 너무 큽니다. 현재 SayangBot은 통상적인 프로그램으로 제작한 mid 파일에는 문제없이 사용 가능하나, 처리 시간이 오래 걸리는 파일은 서버 안정성을 위해 실행을 차단하고 있습니다. 소요시간무시 옵션을 사용하시거나, 다른 파일을 사용해 주십시오.');
					} else {
						sendError(message.channel, 'Sorry, this file is too large for the current SayangBot to handle. Currently, SayangBot can be used without any problem in mid files made with regular programs, but files that take a long time to process, such as blackMIDI / live performance / mp3 conversion, are blocked for server stability. Please use a different file.');
					}
					return;
				}
				
				Notes = Notes.sort(sortFunctionByFifth);
				cuttableMat = [];
				for (inote=0; inote<Notes.length; inote++) {
					if (cuttableMat.includes(Notes[inote][4])) { continue; }
					isInterrupted = false;
					for (jnote=0; jnote<inote; jnote++) {
						if (Notes[jnote][5] > Notes[inote][4] && Notes[jnote][4] < Notes[inote][4]) {
							isInterrupted = true;
							break;
						}
					}
					if (!isInterrupted) {
						cuttableMat = cuttableMat.concat([Notes[inote][4]]);
					}
				}
				for (inote=0; inote<Notes.length; inote++) {
					if (cuttableMat.includes(Notes[inote][5])) { continue; }
					isInterrupted = false;
					for (jnote=0; jnote<Notes.length; jnote++) {
						if (Notes[jnote][4] < Notes[inote][5] && Notes[jnote][5] > Notes[inote][5]) {
							isInterrupted = true;
							break;
						}
					}
					if (!isInterrupted) {
						cuttableMat = cuttableMat.concat([Notes[inote][5]]);
					}
				}
				cuttableMat = cuttableMat.concat([Infinity]);
				if (cuttableMat[0] != 0) {
					cuttableMat = [0].concat(cuttableMat);
				}
				if (command == 'cuttablemat') {
					console.log(cuttableMat);
				}
				timeEndInvest = performance.now();
				user2processtime[((message.guild)?('guild_'+message.channel.id):('dm_'+message.author.id))] = timeEndInvest-timeStartInvest;
				if (timeEndInvest-timeStartInvest > tlimit1 && !(command == '악보' || command == '합주악보')) {
					if (!isEnglish) {
						sendError(message.channel, '죄송합니다, 이 파일은 현재의 SayangBot이 처리하기에는 너무 큽니다. 현재 SayangBot은 통상적인 프로그램으로 제작한 mid 파일에는 문제없이 사용 가능하나, 처리 시간이 오래 걸리는 파일은 서버 안정성을 위해 실행을 차단하고 있습니다. 소요시간무시 옵션을 사용하시거나, 다른 파일을 사용해 주십시오.');
					} else {
						sendError(message.channel, 'Sorry, this file is too large for the current SayangBot to handle. Currently, SayangBot can be used without any problem in mid files made with regular programs, but files that take a long time to process, such as blackMIDI / live performance / mp3 conversion, are blocked for server stability. Please use a different file.');
					}
				}
				
				if (command == '악보' || command == '합주악보') {
					if (timeEndInvest-timeStartInvest > tlimit1) {
						if (!isEnglish) {
							sendError(message.channel, '죄송합니다, 이 파일은 현재의 SayangBot이 처리하기에는 너무 큽니다. 현재 SayangBot은 통상적인 프로그램으로 제작한 mid 파일에는 문제없이 사용 가능하나, 처리 시간이 오래 걸리는 파일은 서버 안정성을 위해 실행을 차단하고 있습니다. 소요시간무시 옵션을 사용하시거나, 다른 파일을 사용해 주십시오.');
						} else {
							sendError(message.channel, 'Sorry, this file is too large for the current SayangBot to handle. Currently, SayangBot can be used without any problem in mid files made with regular programs, but files that take a long time to process, such as blackMIDI / live performance / mp3 conversion, are blocked for server stability. Please use a different file.');
						}
						return;
					}
					if (isAutoEnhance && command == '악보' && iFixedInstrument == -1) {
						curAvgNote = 0;
						for (inote=0; inote<Notes.length; inote++) {
							if (Notes[inote][1]%16 != 9) {
								curAvgNote += Notes[inote][3];
							}
						}
						curAvgNote /= Notes.length;
						if (curAvgNote < 64) {
							if (!isEnglish) {
								sendWarning(message.channel, '전반적인 음량이 너무 작아서 자동으로 키워드렸습니다. 이 기능을 원하지 않으시면 ' + prefix + '악보 자동볼륨증가 off 와 같이 사용해 보세요.');
							} else {
								sendWarning(message.channel, 'The overall volume was too low, so it was automatically increased. If you do not want this function, try using it with ' + prefix + ' autovolumeincrease off.');
							}
							for (inote=0; inote<Notes.length; inote++) {
								if (Notes[inote][1]%16 != 9) {
									Notes[inote][3] = 127 - (127-Notes[inote][3])*0.5;
								}
							}
						}
					}
					
					Notes_before_command = Notes;
					tempomat_before_command = tempomat;
					sustainTrackChannel_before_command = sustainTrackChannel;
					warningInstrList = [];
					
					writtenFileList = [];
					writtenCharacterList = [];
					writtenChordList = [];
					
					if (command == '악보') {
						if (iFixedInstrument == -1) {
							for (inote=0; inote<Notes_before_command.length; inote++) {
								Notes_before_command[inote][6] = 0;
							}
							for (iii=0; iii<sustainTrackChannel_before_command.length; iii++) {
								for (jjj=0; jjj<sustainTrackChannel_before_command[iii].length; jjj++) {
									for (kkk=0; kkk<sustainTrackChannel_before_command[iii][jjj].length; kkk++) {
										sustainTrackChannel_before_command[iii][jjj][kkk][2] = 0;
									}
								}
							}
							listInstrItems = [[0]];
						} else {
							if (isAutoDaecheInstrument) {
								supportedInstrList = iFixedInstrument;
								listInstrItems = [supportedInstrList];
								isDaeched = false;
								for (inote=0; inote<Notes_before_command.length; inote++) {
									if (supportedInstrList.includes(Notes[inote][6]) && supportedInstrList[0] != Notes[inote][6]) {
										isDaeched = true;
										break;
									}
								}
								if (isDaeched) {
									if (!isEnglish) {
										tempMessage = '사용자 편의를 위해 악기 ';
									} else {
										tempMessage = 'For your convenience, ';
									}
									for (iInstr=1; iInstr<supportedInstrList.length; iInstr++) {
										if (iInstr != 1) { tempMessage += ', ' }
										tempMessage += instr2name(supportedInstrList[iInstr]);
									}
									if (!isEnglish) {
										tempMessage += '가 메이플스토리2 악기 ' + instr2name(supportedInstrList[0]) + " 악보로 통합되어 출력됩니다. 이 기능을 원하지 않으시면 '대체악기사용안함' 옵션을 사용해 주세요.";
									} else {
										tempMessage += ' will be automatically integrated into ' + instr2name(supportedInstrList[0]) + " If you do not want this feature, please use the 'disablesubstitution' option.";
									}
									sendWarning(message.channel, tempMessage);
								}
							} else {
								supportedInstrList = iFixedInstrument[0];
								listInstrItems = [[supportedInstrList]];
							}
						}
					} else if (command == '합주악보') {
						tempListInstr = [];
						for (inote=0; inote<Notes_before_command.length; inote++) {
							preInvestInstr = Notes_before_command[inote][6];
							if (Notes_before_command[inote][1]%16 == 9) { preInvestInstr = 128; }
							if (!tempListInstr.includes(preInvestInstr)) {
								tempListInstr = tempListInstr.concat([preInvestInstr]);
							}
						}
						tempListInstr.sort();
						listInstrItems = [];
						autoDaecheInformList = [];
						for (iInstr=0; iInstr<tempListInstr.length; iInstr++) {
							tempCurrentInstr = tempListInstr[iInstr];
							isMekong = false;
							listInstrKeys = Object.keys(instrumentName2num);
							if (isAutoDaecheInstrument) {
								for (iInstrInv=0; iInstrInv<listInstrKeys.length; iInstrInv++) {
									supportedInstrList = instrumentName2num[listInstrKeys[iInstrInv]];
									if (supportedInstrList.includes(tempCurrentInstr)) {
										isMekong = true;
										if (!(listInstrItems.includes(supportedInstrList))) {
											listInstrItems = listInstrItems.concat([supportedInstrList]);
										}
										if (supportedInstrList[0] != tempCurrentInstr) {
											if (!(autoDaecheInformList.includes(supportedInstrList))) {
												autoDaecheInformList = autoDaecheInformList.concat([supportedInstrList]);
											}
										}
										break;
									}
								}
							} else {
								for (iInstrInv=0; iInstrInv<listInstrKeys.length; iInstrInv++) {
									supportedInstrList = instrumentName2num[listInstrKeys[iInstrInv]];
									if (supportedInstrList[0] == tempCurrentInstr) {
										isMekong = true;
										if (!(listInstrItems.includes([supportedInstrList[0]]))) {
											listInstrItems = listInstrItems.concat([[supportedInstrList[0]]]);
										}
										break;
									}
								}
							}
							if (!isMekong) {
								if (tempCurrentInstr == 128) {
									isBasic = true;
									for (inote=0; inote<Notes_before_command.length; inote++) {
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
									if (!isBasic) {
										if (!isEnglish) {
											sendWarning(message.channel, '이 미디 파일에는 큰북/작은북/심벌즈와 더불어 메이플스토리2에 없는 타악기 소리도 포함되어 있습니다. 합주가 조금 허전해질 수 있습니다. 메이플스토리2에서 타악기 전체를 사용하는 방법에 대해서는 "사양"님에게 문의하십시오.');
										} else {
											sendWarning(message.channel, 'This MIDI file contains kick drum / snare drum / cymbal as well as percussion sounds not found in MapleStory 2. The ensemble can be a little empty. For information on how to use the entire percussion instrument in MapleStory 2, please contact "사양".');
										}
									}
									listInstrItems = listInstrItems.concat([[129], [130], [131]]);
								} else {
									listInstrItems = listInstrItems.concat([[tempCurrentInstr]]);
									warningInstrList = warningInstrList.concat([instr2name(tempCurrentInstr)]);
								}
							}
						}
						if (autoDaecheInformList.length > 0) {
							if (!isEnglish) {
								tempMessage = '사용자 편의를 위해 다음과 같이 통합된 악보가 출력됩니다.]';
							} else {
								tempMessage = 'For user convenience, the integrated sheet music is output as follows.]';
							}
							for (iDaeche=0; iDaeche<autoDaecheInformList.length; iDaeche++) {
								tempMessage += '\n' + instr2name(autoDaecheInformList[iDaeche][0]) + ' (';
								for (iInstr=1; iInstr<autoDaecheInformList[iDaeche].length; iInstr++) {
									tempMessage += '+'+instr2name(autoDaecheInformList[iDaeche][iInstr]);
									if (iInstr == autoDaecheInformList[iDaeche].length-1) { tempMessage += ')' }
								}
							}
							if (!isEnglish) {
								tempMessage += "\n[이 기능을 원하지 않으시면 '대체악기사용안함' 옵션을 사용해 주세요."
							} else {
								tempMessage += "\n[If you do not want this feature, please use the 'disablesubstitution' option."
							}
							sendWarning(message.channel, tempMessage);
						}
					}
					if (warningInstrList.length > 0) {
						if (!isEnglish) {
							tempMessage = '미디 파일에서 사용하는 악기 중 ';
						} else {
							tempMessage = 'Among the instruments used in MIDI file, ';
						}
						for (iWarningInstr=0; iWarningInstr<warningInstrList.length; iWarningInstr++) {
							if (iWarningInstr!=0) { tempMessage += ', '; }
							tempMessage += warningInstrList[iWarningInstr];
						}
						if (!isEnglish) {
							tempMessage += '는 메이플스토리2에 없습니다. 대체할만한 악기를 잘 찾아보세요!';
						} else {
							tempMessage += ' are not in MapleStory 2. Try to find a good replacement!';
						}
						sendWarning(message.channel, tempMessage);
					}
					
					for (iInstrMain=0; iInstrMain<listInstrItems.length; iInstrMain++) {
						supportedInstrList = listInstrItems[iInstrMain];
						tempomat = tempomat_before_command;
						sustainTrackChannel = sustainTrackChannel_before_command;
						Notes = [];
						for (inote=0; inote<Notes_before_command.length; inote++) {
							percCheck = false;
							if (supportedInstrList.includes(Notes_before_command[inote][6])) {
								percCheck = true;
							} else if (supportedInstrList[0]>128) {
								if (Notes_before_command[inote][6] == 128 && supportedInstrList[0] == 129 && Notes_before_command[inote][2] == 57) { percCheck = true; }
								if (Notes_before_command[inote][6] == 128 && supportedInstrList[0] == 129 && Notes_before_command[inote][2] == 49) { percCheck = true; }
								if (Notes_before_command[inote][6] == 128 && supportedInstrList[0] == 130 && Notes_before_command[inote][2] == 35) { percCheck = true; }
								if (Notes_before_command[inote][6] == 128 && supportedInstrList[0] == 130 && Notes_before_command[inote][2] == 36) { percCheck = true; }
								if (Notes_before_command[inote][6] == 128 && supportedInstrList[0] == 131 && Notes_before_command[inote][2] == 38) { percCheck = true; }
								if (Notes_before_command[inote][6] == 128 && supportedInstrList[0] == 131 && Notes_before_command[inote][2] == 40) { percCheck = true; }
							}
							if (percCheck) { Notes = Notes.concat([Notes_before_command[inote]]); }
						}
						
						if (Notes.length == 0) {
							if (command == '악보') {
								if (iFixedInstrument == -1) {
									if (!isEnglish) {
										sendError(message.channel, '미디 파일에 포함된 음표가 없거나 SayangBot이 읽지 못했습니다...');
									} else {
										sendError(message.channel, 'Either the MIDI file contains no notes or SayangBot couldn"t read it...');
									}
									return;
								} else {
									instrumentKeyList = Object.keys(instrumentName2num);
									for (iInstrInv=0; iInstrInv<instrumentKeyList.length; iInstrInv++) {
										if (instrumentName2num[instrumentKeyList[iInstrInv]] == supportedInstrList) {
											if (!isEnglish) {
												sendWarning(message.channel, '이 미디 파일에는 입력한 악기 (' + instrumentKeyList[iInstrInv] + ') 가 안 나오는 것 같습니다. ' + prefix + '정보 명령어로 악기 정보를 확인해 보세요. 저는 퇴근합니다.');
											} else {
												sendWarning(message.channel, 'The instrument (' + instrumentKeyList[iInstrInv] + ') doesn"t seem to appear in this MIDI file. Check the instrument information with the ' + prefix + 'info command.');
											}
											return;
										}
									}
									if (!isEnglish) {
										sendWarning(message.channel, '이 미디 파일에는 입력한 악기 (' + instr2name(supportedInstrList[0]) + ') 가 안 나오는 것 같습니다. ' + prefix + '정보 명령어로 악기 정보를 확인해 보세요. 저는 퇴근합니다.');
									} else {
										sendWarning(message.channel, 'The instrument (' + instr2name(supportedInstrList[0]) + ') doesn"t seem to appear in this MIDI file. Check the instrument information with the ' + prefix + 'info command.');
									}
									return;
								}
							} else if (command == '합주악보') {
								continue;
							}
						}
					
						if (lOnoffHorizontalDivision) {
							icuttable = createArray(3);
							tempCutIndex = 0;
							while (cuttableMat[tempCutIndex] < timeStartNote) {
								tempCutIndex++;
							}
							icuttable[0] = tempCutIndex;
							icuttable[1] = -1;
							tempCutIndex = cuttableMat.length-1;
							while (cuttableMat[tempCutIndex] > timeEndNote) {
								tempCutIndex--;
							}
							icuttable[2] = tempCutIndex;
							iCurrentWriteHorizontal = 0;
							Notes_orig = Notes;
							tempomat_orig_orig = tempomat;
							sustainTrackChannel_orig = sustainTrackChannel;
						}
						lHorizontalDivisionFinished = false;
						iHorizontalTryCount = 0;
						cuttableMat_orig = cuttableMat.slice();
						while (!lHorizontalDivisionFinished) {
							tickperquarter = midiArray.timeDivision;
							cuttableMat = cuttableMat_orig.slice();
							if (!lOnoffHorizontalDivision) {
								tempTimeStartNote = timeStartNote;
								tempTimeEndNote = timeEndNote;
							} else {
								Notes = Notes_orig;
								tempomat = tempomat_orig_orig;
								sustainTrackChannel = sustainTrackChannel_orig;
								if (icuttable[1] < 0 && icuttable[2] == cuttableMat.length-1) {
									tempTimeStartNote = cuttableMat[icuttable[0]];
									tempTimeEndNote = cuttableMat[icuttable[2]];
								} else {
									tempCuttableIndex = Math.floor((icuttable[1]+icuttable[2])/2);
									tempTimeStartNote = cuttableMat[icuttable[0]];
									tempTimeEndNote = cuttableMat[tempCuttableIndex];
								}
							}
							
							if (!isTriplet) {
								for (iCutCut=0; iCutCut<cuttableMat.length; iCutCut++) {
									cuttableMat[iCutCut] = Math.round((cuttableMat[iCutCut]-tempTimeStartNote)/(4*tickperquarter/noteResolution)) * (4*tickperquarter/noteResolution) + tempTimeStartNote;
								}
							} else if (isTriplet) {
								for (iCutCut=0; iCutCut<cuttableMat.length; iCutCut++) {
									cuttableMat[iCutCut] = Math.round((cuttableMat[iCutCut]-tempTimeStartNote)/(4*tickperquarter*2/3/noteResolution)) * (4*tickperquarter*2/3/noteResolution) + tempTimeStartNote;
								}
							}
							
							if (!isTriplet) {
								for (itempo=0; itempo<tempomat.length; itempo++) {
									tempomat[itempo][0] = (tempoResolution*4*tickperquarter/noteResolution)*Math.round((tempomat[itempo][0]-tempTimeStartNote)/(tempoResolution*4*tickperquarter/noteResolution)) + tempTimeStartNote;
									if (itempo!=0) {
										if (tempomat[itempo-1][1] > 255) {
											tempomat[itempo][0] = (2*tempoResolution*4*tickperquarter/noteResolution)*Math.round((tempomat[itempo][0]-tempTimeStartNote)/(2*tempoResolution*4*tickperquarter/noteResolution)) + tempTimeStartNote;
										}
									}
								}
							} else if (isTriplet) {
								for (itempo=0; itempo<tempomat.length; itempo++) {
									tempomat[itempo][0] = (tempoResolution*4*tickperquarter*2/3/noteResolution)*Math.round((tempomat[itempo][0]-tempTimeStartNote)/(tempoResolution*4*tickperquarter*2/3/noteResolution)) + tempTimeStartNote;
									if (itempo!=0) {
										if (tempomat[itempo-1][1] > 255) {
											tempomat[itempo][0] = (2*tempoResolution*4*tickperquarter*2/3/noteResolution)*Math.round((tempomat[itempo][0]-tempTimeStartNote)/(2*tempoResolution*4*tickperquarter*2/3/noteResolution)) + tempTimeStartNote;
										}
									}
								}
							}
							tempomat.sort(sortFunctionByFirst);
							isRemoved = true;
							while (isRemoved) {
								isRemoved = false;
								for (i=0; i<tempomat.length; i++) {
									if (tempomat[i][0] < tempTimeStartNote) {
										isRemoved = true;
										tempomat[i][0] = tempTimeStartNote;
										break;
									}
									if (i != tempomat.length-1) {
										if (tempomat[i][0] == tempomat[i+1][0]) {
											isRemoved = true;
										}
									}
									if (tempomat[i][0] < tempTimeStartNote) { isRemoved = true; }
									if (tempomat[i][0] > tempTimeEndNote) { isRemoved = true; }
									if (tempomat[i][0] > maxLengthTrack) { isRemoved = true; }
									if (isRemoved) {
										newtempomat = createArray(0);
										for (i2=0; i2<tempomat.length; i2++) {
											if (i2!=i) { newtempomat = newtempomat.concat([tempomat[i2]]); }
										}
										tempomat = newtempomat;
										break;
									}
								}
							}
							tempomat = tempomat.concat([[Infinity, 255]]);
							tempomat_orig = tempomat;
							// console.log(tempomat);
							
							susmat = [];
							if (iOnoffSustain) {
								for (itrack=0; itrack<midiArray.track.length; itrack++) {
									for (ichannel=0; ichannel<iMaxChannel; ichannel++) {
										if (!listTrackChannel[itrack][ichannel]) { continue; }
										for (isus=0; isus<sustainTrackChannel[itrack][ichannel].length; isus++) {
											if (sustainTrackChannel[itrack][ichannel][isus][0] < tempTimeStartNote) { continue; }
											if (sustainTrackChannel[itrack][ichannel][isus][0] > tempTimeEndNote) { continue; }
											if (sustainTrackChannel[itrack][ichannel][isus][0] > maxLengthTrack) { continue; }
											if (sustainTrackChannel[itrack][ichannel][isus][0] < timeStartNote) { continue; }
											if (sustainTrackChannel[itrack][ichannel][isus][0] > timeEndNote) { continue; }
											if (!supportedInstrList.includes(sustainTrackChannel[itrack][ichannel][isus][2])) { continue; }
											susmat = susmat.concat([sustainTrackChannel[itrack][ichannel][isus]]);
										}
									}
								}
							}
							if (!isTriplet) {
								for (isus=0; isus<susmat.length; isus++) {
									susmat[isus][0] = (susResolution*4*tickperquarter/noteResolution)*Math.round((susmat[isus][0]-tempTimeStartNote)/(susResolution*4*tickperquarter/noteResolution)) + tempTimeStartNote;
									currentTempo = 120;
									for (itempo=0; itempo<tempomat.length; itempo++) {
										if (susmat[isus][0] >= tempomat[itempo][0]) {
											currentTempo = tempomat[itempo][1];
										} else {
											break;
										}
									}
									if (currentTempo > 255) {
										susmat[isus][0] = (2*susResolution*4*tickperquarter/noteResolution)*Math.round((susmat[isus][0]-tempTimeStartNote)/(2*susResolution*4*tickperquarter/noteResolution)) + tempTimeStartNote;
									}
								}
							} else if (isTriplet) {
								for (isus=0; isus<susmat.length; isus++) {
									susmat[isus][0] = (susResolution*4*tickperquarter*2/3/noteResolution)*Math.round((susmat[isus][0]-tempTimeStartNote)/(susResolution*4*tickperquarter*2/3/noteResolution)) + tempTimeStartNote;
									currentTempo = 120;
									for (itempo=0; itempo<tempomat.length; itempo++) {
										if (susmat[isus][0] >= tempomat[itempo][0]) {
											currentTempo = tempomat[itempo][1];
										} else {
											break;
										}
									}
									if (currentTempo > 170) {
										susmat[isus][0] = (2*susResolution*4*tickperquarter/noteResolution)*Math.round((susmat[isus][0]-tempTimeStartNote)/(2*susResolution*4*tickperquarter/noteResolution)) + tempTimeStartNote;
									}
								}
							}
							susmat.sort(sortFunctionByFirstSecondThird);
							isRemoved = true;
							while (isRemoved) {
								isRemoved = false;
								for (i=0; i<susmat.length-1; i++) {
									if (susmat[i][0] == susmat[i+1][0] && (susmat[i][1]-63.5)*(susmat[i+1][1]-63.5)>0 && susmat[i][2] == susmat[i+1][2]) {
										isRemoved = true;
										newsusmat = createArray(0);
										for (i2=0; i2<susmat.length; i2++) {
											if (i2!=i) { newsusmat = newsusmat.concat([susmat[i2]]); }
										}
										susmat = newsusmat;
										break;
									}
								}
							}
							susmat = susmat.concat([[Infinity, 0, 0]]);
							susmat_orig = susmat;
							// console.log(susmat);
							
							// console.log(Notes.length);
							// console.log(Notes);
							if (maxCutLength>0) {
								cutOverlap = createArray(Notes.length);
								for (inote=0; inote<Notes.length; inote++) {
									cutOverlap[inote] = 0;
								}
								for (inote=0; inote<Notes.length; inote++) {
									for (jnote=0; jnote<Notes.length; jnote++) {
										if ((Notes[jnote][4] >= Notes[inote][5]-(Notes[inote][5]-Notes[inote][4])*maxCutLength) && (Notes[jnote][4] < Notes[inote][5])) {
											tempValue = (Notes[inote][5]-Notes[jnote][4])/(Notes[inote][5]-Notes[inote][4]);
											if (Notes[inote][5]-Notes[inote][4] == 0) { tempValue = 0; }
											cutOverlap[inote] = (cutOverlap[inote]>tempValue)?cutOverlap[inote]:tempValue;
										}
									}
									cutOverlap[inote] = (cutOverlap[inote]<maxCutLength)?cutOverlap[inote]:maxCutLength;
								}
								for (inote=0; inote<Notes.length; inote++) {
									Notes[inote][5] = Notes[inote][4] + (Notes[inote][5]-Notes[inote][4])*(1-cutOverlap[inote]);
								}
							}
							Notes3 = [];
							isThereDrum = false;
							isThereOther = false;
							for (inote=0; inote<Notes.length; inote++) {
								if (!listTrackChannel[Notes[inote][0]][Notes[inote][1]]) { continue; }
								if (Notes[inote][2] < pitchLowerBound) { continue; }
								if (Notes[inote][3] > pitchUpperBound) { continue; }
								if (Notes[inote][4] < tempTimeStartNote) { continue; }
								if (Notes[inote][5] > tempTimeEndNote) { continue; }
								if (Notes[inote][4] < timeStartNote) { continue; }
								if (Notes[inote][5] > timeEndNote) { continue; }
								Notes3 = Notes3.concat([Notes[inote]]);
								if (Notes[inote][1]%16 == 9) { isThereDrum = true; }
								if (Notes[inote][1]%16 != 9) { isThereOther = true; }
							}
							Notes = Notes3;
							if (iOnoffDrum == 0 && isThereDrum && isThereOther && !isDrumWarned) {
								sendWarning(message.channel, '드럼 코드와 일반 악기 코드를 같은 악보에 작성하게 됩니다. 의도하지 않은 소리가 날 수 있습니다. 드럼 코드를 제외하고 일반 악기 코드만 작성하려면 드럼제외 옵션을 사용하세요.');
								isDrumWarned = true;
							}
							if (!isTriplet) {
								for (inote=0; inote<Notes.length; inote++) {
									Notes[inote][4] = Math.round((Notes[inote][4]-tempTimeStartNote)/(4*tickperquarter/noteResolution)) * (4*tickperquarter/noteResolution) + tempTimeStartNote;
									Notes[inote][5] = Math.round((Notes[inote][5]-tempTimeStartNote)/(4*tickperquarter/noteResolution)) * (4*tickperquarter/noteResolution) + tempTimeStartNote;
								}
							} else if (isTriplet) {
								for (inote=0; inote<Notes.length; inote++) {
									Notes[inote][4] = Math.round((Notes[inote][4]-tempTimeStartNote)/(4*tickperquarter*2/3/noteResolution)) * (4*tickperquarter*2/3/noteResolution) + tempTimeStartNote;
									Notes[inote][5] = Math.round((Notes[inote][5]-tempTimeStartNote)/(4*tickperquarter*2/3/noteResolution)) * (4*tickperquarter*2/3/noteResolution) + tempTimeStartNote;
								}
							}
							if (iMethodDup == 1) {
								Notes.sort(sortFunctionByFifth);
								Notes3 = [];
								deathnote = createArray(Notes.length);
								for (inote=0; inote<Notes.length; inote++) {
									deathnote[inote] = 0;
								}
								for (inote=0; inote<Notes.length; inote++) {
									for (jnote=inote+1; jnote<Notes.length; jnote++) {
										if (Notes[jnote][2] == Notes[inote][2] && Notes[inote][4] == Notes[jnote][4]) {
											deathnote[jnote] = 1;
										}
									}
								}
								for (inote=0; inote<Notes.length; inote++) {
									if (deathnote[inote] == 0) {
										Notes3 = Notes3.concat([Notes[inote]]);
									}
								}
								Notes = Notes3;
							} else if (iMethodDup == 2) {
								if (!isEnglish) {
									// sendWarning(message.channel, '중복제거 2 옵션을 사용하면 SayangBot이 몇 분간 응답하지 않을 수 있습니다. 문제가 발생한 경우 개발자에게 문의해 주세요.');
								} else {
									// sendWarning(message.channel, 'The option may cause SayangBot to become unresponsive for several minutes. If you run into any problems, please contact the developer.');
								}
								initialLength = Notes.length;
								isRemoved = true;
								while (isRemoved) {
									isRemoved = false;
									console.log('중복 음표 제거 중 : ' + (initialLength - Notes.length) + '/' + initialLength);
									for (inote=0; inote<Notes.length; inote++) {
										for (jnote=0; jnote<Notes.length; jnote++) {
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
														Notes.splice(jnote, 1);
													}
												} else if (Notes[inote][5] > Notes[jnote][5]) {
													if (Notes[inote][3] <= Notes[jnote][3]) {
														Notes[inote][5] = Notes[jnote][4];
													} else if (Notes[inote][3] > Notes[jnote][3]) {
														Notes.splice(jnote, 1);
													}
												}
											} else if (Notes[inote][4] == Notes[jnote][4]) {
												if (Notes[inote][5] < Notes[jnote][5]) {
													if (Notes[inote][3] <= Notes[jnote][3]) {
														Notes.splice(inote, 1);
													} else if (Notes[inote][3] > Notes[jnote][3]) {
														Notes[jnote][4] = Notes[inote][5];
													}
												} else if (Notes[inote][5] == Notes[jnote][5]) {
													if (Notes[inote][3] <= Notes[jnote][3]) {
														Notes.splice(inote, 1);
													} else if (Notes[inote][3] > Notes[jnote][3]) {
														Notes.splice(jnote, 1);
													}
												} else if (Notes[inote][5] > Notes[jnote][5]) {
													if (Notes[inote][3] <= Notes[jnote][3]) {
														Notes[inote][4] = Notes[jnote][5];
													} else if (Notes[inote][3] > Notes[jnote][3]) {
														Notes.splice(jnote, 1);
													}
												}
											} else if (Notes[inote][4] > Notes[jnote][4]) {
												if (Notes[inote][5] < Notes[jnote][5]) {
													if (Notes[inote][3] <= Notes[jnote][3]) {
														Notes.splice(inote, 1);
													} else if (Notes[inote][3] > Notes[jnote][3]) {
														Notes[jnote][5] = Notes[inote][4];
													}
												} else if (Notes[inote][5] == Notes[jnote][5]) {
													if (Notes[inote][3] <= Notes[jnote][3]) {
														Notes.splice(inote, 1);
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
							Notes.sort(sortFunctionByFifth);
							// console.log(Notes.length);
							// console.log(Notes);
							
							currentTime = tempTimeStartNote;
							
							curoct = 4;
							curlen = 4;
							curvol = 8;
							currest = true;
							curchord = -1;
							
							outputtxt = "";
							outputtxtRollback = outputtxt;
							measureLength = outputtxt.length;
			
							lfinish = false;
							isFirstChannel = true;
							isplaying = false;
							iCountCharacter = [];
							cCharacter = [];
							iCountCharacterPseudoSustain = 0;
							
							while (!lfinish) {
								Notes2 = [];
								tempomat = tempomat_orig.slice();
								susmat = susmat_orig.slice();
								
								while (Notes.length>0) {
									curnote = Notes[0];
									
									if (curchord>=0) {
										starttime = curnote[4];
										endtime = curnote[5];
									} else if (curchord==-1) {
										if (susmat.length > 1) {
											starttime = susmat[0][0];
											endtime = susmat[0][0];
										} else {
											while (Notes.length>0) {
												Notes2 = Notes2.concat([Notes[0]]);
												Notes.shift();
											}
											break;
										}
									}
									
									if (outputtxt.length+iCountCharacterPseudoSustain>lengthHorizontalDivision) {
										if (Notes2.length == 0) {
											if (!isEnglish) {
												sendError(message.channel, '이 미디 파일에는 템포 조정 이벤트 / 서스테인 이벤트가 너무 많아서 1만자 악보에 맞게 출력할 수 없습니다. ' + prefix + '악보 템포뭉개기 2 서스테인뭉개기 2 등의 옵션으로 개수를 줄여 보세요. 실행을 종료합니다.');
											} else {
												sendError(message.channel, 'There are too many tempo events/sustain events in this MIDI file, so it can"t be output to fit 10,000-character sheet music. Try reducing the number with options such as ' + prefix + 'solo mergetempo 2 mergesustain 2, etc. Terminate execution.');
											}
											return;
										}
										if (curchord==-1) { curchord = 0; }
										outputtxt = outputtxtRollback;
										while (Notes.length>0) {
											Notes2 = Notes2.concat([Notes[0]]);
											Notes.shift();
										}
										break;
									}
									outputtxtRollback = outputtxt;
									
									if (currentTime - starttime > 0 && curchord >= 0) {
										Notes2 = Notes2.concat([curnote]);
										Notes.shift();
										continue;
									}
									
									isstart = false;
									issus = false;
							
									if (tempomat[0][0] <= susmat[0][0]) {
										if (starttime >= tempomat[0][0]) {
											starttime = tempomat[0][0];
											isstart = true;
										}
										if (endtime >= tempomat[0][0]) {
											endtime = tempomat[0][0];
										}
										if (starttime >= susmat[0][0]) {
											// starttime = susmat[0][0];
											issus = true;
										}
										// if (endtime >= susmat[0][0]) {
											// endtime = susmat[0][0];
										// }
									} else if (tempomat[0][0] > susmat[0][0] && curchord == -1) {
										if (starttime >= susmat[0][0]) {
											starttime = susmat[0][0];
											issus = true;
										}
										// if (endtime >= susmat[0][0]) {
											// endtime = susmat[0][0];
										// }
									} else {
										if (starttime >= tempomat[0][0]) {
											starttime = tempomat[0][0];
											isstart = true;
										}
										if (endtime >= tempomat[0][0]) {
											endtime = tempomat[0][0];
										}
										if (starttime >= susmat[0][0]) {
											// starttime = susmat[0][0];
											issus = true;
										}
										// if (endtime >= susmat[0][0]) {
											// endtime = susmat[0][0];
										// }
									}
									
									if (starttime - currentTime > 0) {
										// testlist = [testlist starttime-currentTime];
										restlist = get_length_list2_nonexpand(starttime-currentTime,tickperquarter,noteResolution);
										lintdotted = false;
										for (iListInd=0; iListInd<restlist.length; iListInd++) {
											if (lintdotted) {
												lintdotted = false;
												continue;
											}
											i = restlist[iListInd];
											if (i != curlen) {
												outputtxt += 'l' + (i);
												curlen = i;
											}
											if (iListInd == restlist.length-2) {
												if (restlist[iListInd+1]==i*2) {
													lintdotted = true;
												}
											}
											if (iOnoffMotion) {
												if (!currest && isFirstChannel && (starttime-currentTime)>tickperquarter*cooldownMotionInQuarter && isplaying) {
													outputtxt += 'm0';
													isplaying = false;
												}
											}
											currest = true;
											outputtxt += 'r';
											currentTime = currentTime + notetotime2(i,tickperquarter);
											if (lintdotted) {
												outputtxt += '.';
												currentTime = currentTime + notetotime2(i,tickperquarter)/2;
											}
										}
									}
									if (outputtxt.length+iCountCharacterPseudoSustain>lengthHorizontalDivision) {
										if (Notes2.length == 0) {
											if (!isEnglish) {
												sendError(message.channel, '이 미디 파일에는 템포 조정 이벤트 / 서스테인 이벤트가 너무 많아서 1만자 악보에 맞게 출력할 수 없습니다. ' + prefix + '악보 템포뭉개기 2 서스테인뭉개기 2 등의 옵션으로 개수를 줄여 보세요. 실행을 종료합니다.');
											} else {
												sendError(message.channel, 'There are too many tempo events/sustain events in this MIDI file, so it can"t be output to fit 10,000-character sheet music. Try reducing the number with options such as ' + prefix + 'solo mergetempo 2 mergesustain 2, etc. Terminate execution.');
											}
											return;
										}
										if (curchord==-1) { curchord = 0; }
										outputtxt = outputtxtRollback;
										while (Notes.length>0) {
											Notes2 = Notes2.concat([Notes[0]]);
											Notes.shift();
										}
										break;
									}
									outputtxtRollback = outputtxt;
									if (isstart && issus) {
										if (tempomat[0][0] != currentTime) {
											if (!isEnglish) {
												sendError(message.channel, '템포 변환 중 에러가 발생했습니다. 개발자에게 문의해 주세요...');
											} else {
												sendError(message.channel, 'An error occurred during tempo conversion. Please contact the developer...');
											}
											console.log(tempomat[0][0]);
											console.log(currentTime);
											return;
										}
										if (!isTriplet) {
											if (tempomat[0][1] > 255) {
												outputtxt += 't' + Math.floor(tempomat[0][1]/2);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]/2); }
												tickperquarter = midiArray.timeDivision*2;
											} else if (tempomat[0][1] < 8) {
												outputtxt += 't' + Math.floor(tempomat[0][1]*8);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]*8); }
												tickperquarter = midiArray.timeDivision/8;
											} else if (tempomat[0][1] < 16) {
												outputtxt += 't' + Math.floor(tempomat[0][1]*4);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]*4); }
												tickperquarter = midiArray.timeDivision/4;
											} else if (tempomat[0][1] < 32) {
												outputtxt += 't' + Math.floor(tempomat[0][1]*2);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]*2); }
												tickperquarter = midiArray.timeDivision/2;
											} else {
												outputtxt += 't' + Math.floor(tempomat[0][1]);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]); }
												tickperquarter = midiArray.timeDivision;
											}
										} else if (isTriplet) {
											if (tempomat[0][1] > 170) {
												outputtxt += 't' + Math.floor(tempomat[0][1]*3/4);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]*3/4); }
												tickperquarter = midiArray.timeDivision*4/3;
											} else if (tempomat[0][1] < 6) {
												outputtxt += 't' + Math.floor(tempomat[0][1]*12);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]*12); }
												tickperquarter = midiArray.timeDivision/12;
											} else if (tempomat[0][1] < 11) {
												outputtxt += 't' + Math.floor(tempomat[0][1]*6);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]*6); }
												tickperquarter = midiArray.timeDivision/6;
											} else if (tempomat[0][1] < 22) {
												outputtxt += 't' + Math.floor(tempomat[0][1]*3);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]*3); }
												tickperquarter = midiArray.timeDivision/3;
											} else {
												outputtxt += 't' + Math.floor(tempomat[0][1]*3/2);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]*3/2); }
												tickperquarter = midiArray.timeDivision*2/3;
											}
										}
										tempomat.shift();
										if (curchord == -1) {
											if (susmat[0][0] < currentTime) {
												if (!isEnglish) {
													sendError(message.channel, '서스테인 변환 중 에러가 발생했습니다. 개발자에게 문의해 주세요...');
												} else {
													sendError(message.channel, 'An error occurred during sustain conversion. Contact the developer...');
												}
												console.log(susmat[0][0]);
												console.log(currentTime);
												return;
											}
											if (susmat[0][1] >= 64) {
												outputtxt += 's1';
											} else {
												outputtxt += 's0';
											}
											susmat.shift();
										} else {
											while (susmat[0][0] <= starttime) {
												susmat.shift();
											}
										}
										continue;
									}
									if (outputtxt.length+iCountCharacterPseudoSustain>lengthHorizontalDivision) {
										if (Notes2.length == 0) {
											if (!isEnglish) {
												sendError(message.channel, '이 미디 파일에는 템포 조정 이벤트 / 서스테인 이벤트가 너무 많아서 1만자 악보에 맞게 출력할 수 없습니다. ' + prefix + '악보 템포뭉개기 2 서스테인뭉개기 2 등의 옵션으로 개수를 줄여 보세요. 실행을 종료합니다.');
											} else {
												sendError(message.channel, 'There are too many tempo events/sustain events in this MIDI file, so it can"t be output to fit 10,000-character sheet music. Try reducing the number with options such as ' + prefix + 'solo mergetempo 2 mergesustain 2, etc. Terminate execution.');
											}
											return;
										}
										if (curchord==-1) { curchord = 0; }
										outputtxt = outputtxtRollback;
										while (Notes.length>0) {
											Notes2 = Notes2.concat([Notes[0]]);
											Notes.shift();
										}
										break;
									}
									outputtxtRollback = outputtxt;
									if (isstart) {
										if (tempomat[0][0] != currentTime) {
											if (!isEnglish) {
												sendError(message.channel, '템포 변환 중 에러가 발생했습니다. 개발자에게 문의해 주세요...');
											} else {
												sendError(message.channel, 'An error occurred during tempo conversion. Please contact the developer...');
											}
											console.log(tempomat[0][0]);
											console.log(currentTime);
											return;
										}
										if (!isTriplet) {
											if (tempomat[0][1] > 255) {
												outputtxt += 't' + Math.floor(tempomat[0][1]/2);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]/2); }
												tickperquarter = midiArray.timeDivision*2;
											} else if (tempomat[0][1] < 8) {
												outputtxt += 't' + Math.floor(tempomat[0][1]*8);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]*8); }
												tickperquarter = midiArray.timeDivision/8;
											} else if (tempomat[0][1] < 16) {
												outputtxt += 't' + Math.floor(tempomat[0][1]*4);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]*4); }
												tickperquarter = midiArray.timeDivision/4;
											} else if (tempomat[0][1] < 32) {
												outputtxt += 't' + Math.floor(tempomat[0][1]*2);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]*2); }
												tickperquarter = midiArray.timeDivision/2;
											} else {
												outputtxt += 't' + Math.floor(tempomat[0][1]);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]); }
												tickperquarter = midiArray.timeDivision;
											}
										} else if (isTriplet) {
											if (tempomat[0][1] > 170) {
												outputtxt += 't' + Math.floor(tempomat[0][1]*3/4);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]*3/4); }
												tickperquarter = midiArray.timeDivision*4/3;
											} else if (tempomat[0][1] < 6) {
												outputtxt += 't' + Math.floor(tempomat[0][1]*12);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]*12); }
												tickperquarter = midiArray.timeDivision/12;
											} else if (tempomat[0][1] < 11) {
												outputtxt += 't' + Math.floor(tempomat[0][1]*6);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]*6); }
												tickperquarter = midiArray.timeDivision/6;
											} else if (tempomat[0][1] < 22) {
												outputtxt += 't' + Math.floor(tempomat[0][1]*3);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]*3); }
												tickperquarter = midiArray.timeDivision/3;
											} else {
												outputtxt += 't' + Math.floor(tempomat[0][1]*3/2);
												if (lOnoffMotionSpeedTempo && isFirstChannel) { outputtxt += 'p' + Math.floor(tempomat[0][1]*2/3); }
												tickperquarter = midiArray.timeDivision*2/3;
											}
										}
										tempomat.shift()
										continue;
									}
									if (outputtxt.length+iCountCharacterPseudoSustain>lengthHorizontalDivision) {
										if (Notes2.length == 0) {
											if (!isEnglish) {
												sendError(message.channel, '이 미디 파일에는 템포 조정 이벤트 / 서스테인 이벤트가 너무 많아서 1만자 악보에 맞게 출력할 수 없습니다. ' + prefix + '악보 템포뭉개기 2 서스테인뭉개기 2 등의 옵션으로 개수를 줄여 보세요. 실행을 종료합니다.');
											} else {
												sendError(message.channel, 'There are too many tempo events/sustain events in this MIDI file, so it can"t be output to fit 10,000-character sheet music. Try reducing the number with options such as ' + prefix + 'solo mergetempo 2 mergesustain 2, etc. Terminate execution.');
											}
											return;
										}
										if (curchord==-1) { curchord = 0; }
										outputtxt = outputtxtRollback;
										while (Notes.length>0) {
											Notes2 = Notes2.concat([Notes[0]]);
											Notes.shift();
										}
										break;
									}
									outputtxtRollback = outputtxt;
									if (issus) {
										if (curchord == -1) {	
											if (susmat[0][0] != currentTime) {
												if (!isEnglish) {
													sendError(message.channel, '서스테인 변환 중 에러가 발생했습니다. 개발자에게 문의해 주세요...');
												} else {
													sendError(message.channel, 'An error occurred during sustain conversion. Contact the developer...');
												}
												console.log(susmat[0][0]);
												console.log(currentTime);
												return;
											}
											if (susmat[0][1] >= 64) {
												outputtxt += 's1';
											} else {
												outputtxt += 's0';
											}
											susmat.shift();
										} else {
											while (susmat[0][0] <= starttime) {
												susmat.shift();
											}
										}
										continue;
									}
									if (outputtxt.length+iCountCharacterPseudoSustain>lengthHorizontalDivision) {
										if (Notes2.length == 0) {
											if (!isEnglish) {
												sendError(message.channel, '이 미디 파일에는 템포 조정 이벤트 / 서스테인 이벤트가 너무 많아서 1만자 악보에 맞게 출력할 수 없습니다. ' + prefix + '악보 템포뭉개기 2 서스테인뭉개기 2 등의 옵션으로 개수를 줄여 보세요. 실행을 종료합니다.');
											} else {
												sendError(message.channel, 'There are too many tempo events/sustain events in this MIDI file, so it can"t be output to fit 10,000-character sheet music. Try reducing the number with options such as ' + prefix + 'solo mergetempo 2 mergesustain 2, etc. Terminate execution.');
											}
											return;
										}
										if (curchord==-1) { curchord = 0; }
										outputtxt = outputtxtRollback;
										while (Notes.length>0) {
											Notes2 = Notes2.concat([Notes[0]]);
											Notes.shift();
										}
										break;
									}
									outputtxtRollback = outputtxt;
									
									if (curchord >= 0) {
										notenumber = curnote[2];
										volnumber = curnote[3];
										thisoct = Math.floor((notenumber-12)/12);
										thisvol = Math.floor(((volnumber+volumeAdd1*8)*volumeMul2+volumeAdd3*8)/8);
										thisvol = (thisvol>15)?15:thisvol;
										thisvol = (thisvol <0)?0 :thisvol;
										scalenumber = notenumber%12;
									} else {
										thisoct = curoct;
										thisvol = curvol;
									}
									
									// testlist = [testlist endtime-currentTime];
									notelist = get_length_list2_expand(endtime-currentTime,tickperquarter,noteResolution);
									tempCurrentTime = currentTime;
									for (ii=0; ii<notelist.length; ii++) {
										i = notelist[ii];
										tempCurrentTime += notetotime2(i,tickperquarter);
									}
									if ( tempCurrentTime > tempomat[0][0] ) {
										notelist = get_length_list2_nonexpand(endtime-currentTime,tickperquarter,noteResolution);
									}
									
									if (thisoct != curoct) {
										if (thisoct == curoct - 1) {
											outputtxt += '<';
											curoct = thisoct;
										} else if (thisoct == curoct + 1) {
											outputtxt += '>';
											curoct = thisoct;
										} else {
											outputtxt += 'o' + (thisoct);
											curoct = thisoct;
										}
									}
									if (thisvol != curvol) {
										outputtxt += 'v' + (thisvol);
										curvol = thisvol;
									}
									lfirst = true;
									lintdotted = false;
									for (ii=0; ii<notelist.length; ii++) {
										if (lintdotted) {
											lintdotted = false;
											continue;
										}
										i = notelist[ii];
										if (i != curlen) {
											outputtxt += 'l' + (i);
											curlen = i;
										}
										if (ii == notelist.length-2) {
											if (notelist[ii+1]==i*2) {
												lintdotted = true;
											}
										}
										if (!lfirst) {
											outputtxt += '&';
										} else {
											lfirst = false;
										}
										if (iOnoffMotion) {
											if (currest && isFirstChannel && !isplaying) {
												outputtxt += 'm1';
												isplaying = true;
											}
										}
										currest = false;
										outputtxt += scaletoname(scalenumber);
										currentTime = currentTime + notetotime2(i,tickperquarter);
										if (lintdotted) {
											outputtxt += '.';
											currentTime = currentTime + notetotime2(i,tickperquarter)/2;
										}
									}
									if (outputtxt.length+iCountCharacterPseudoSustain>lengthHorizontalDivision) {
										if (Notes2.length == 0) {
											if (!isEnglish) {
												sendError(message.channel, '이 미디 파일에는 템포 조정 이벤트 / 서스테인 이벤트가 너무 많아서 1만자 악보에 맞게 출력할 수 없습니다. ' + prefix + '악보 템포뭉개기 2 서스테인뭉개기 2 등의 옵션으로 개수를 줄여 보세요. 실행을 종료합니다.');
											} else {
												sendError(message.channel, 'There are too many tempo events/sustain events in this MIDI file, so it can"t be output to fit 10,000-character sheet music. Try reducing the number with options such as ' + prefix + 'solo mergetempo 2 mergesustain 2, etc. Terminate execution.');
											}
											return;
										}
										if (curchord==-1) { curchord = 0; }
										outputtxt = outputtxtRollback;
										while (Notes.length>0) {
											Notes2 = Notes2.concat([Notes[0]]);
											Notes.shift();
										}
										break;
									}
									outputtxtRollback = outputtxt;
									
									if (notelist.length == 0) {
										Notes2 = Notes2.concat([curnote]);
									}
									Notes.shift();
								}
								
								if (Notes2.length > 0) {
									Notes2.sort(sortFunctionByFifth);
									Notes = Notes2;
									Notes2 = [];
									currentTime = tempTimeStartNote;
									curoct = 4;
									curlen = 4;
									curvol = 8;
									currest = true;
									if (curchord>=0) {
										iCountCharacter = iCountCharacter.concat([outputtxt.length]);
										cCharacter = cCharacter.concat([outputtxt]);
										isFirstChannel = false;
									} else if (curchord==-1) {
										iCountCharacterPseudoSustain = outputtxt.length;
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
									iCountCharacter = iCountCharacter.concat([outputtxt.length]);
									cCharacter = cCharacter.concat([outputtxt]);
								}
								
							}
							iSumCountCharacter = 0;
							for (i=0; i<curchord; i++) { iSumCountCharacter += iCountCharacter[i]; }
							iSumCountCharacter += iCountCharacterPseudoSustain;
							
							lWrite = false;
							if (!lOnoffHorizontalDivision) {
								lHorizontalDivisionFinished = true;
								lWrite = true;
							} else {
								tempCutIndex = cuttableMat.length-1;
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
										tempCutIndex = cuttableMat.length-1;
										while (cuttableMat[tempCutIndex] > timeEndNote) {
											tempCutIndex--;
										}
										if ((icuttable[2] == tempCutIndex && iSumCountCharacter > lengthHorizontalDivision) || (icuttable[0] == icuttable[1])) {
											if (maxLengthTrack/midiArray.timeDivision/tempomat.length < 20) {
												if (!isEnglish) {
													sendError(message.channel, '이 파일은 1만자 악보에 맞게 자동 분할할 수 없습니다. 미디 파일에 템포 설정 이벤트가 상당히 많은 것으로 생각됩니다. 템포뭉개기 2 등의 옵션을 시도해 보세요.');
												} else {
													sendError(message.channel, 'This file cannot be automatically split to fit 10,000 character scores. I think there are quite a few tempo setting events in the MIDI file. Try options like mergetempo 2.');
												}
												return;
											} else {
												if (!isEnglish) {
													sendError(message.channel, '이 파일은 1만자 악보에 맞게 자동 분할할 수 없습니다. 실행을 종료합니다. 1만자무시 혹은 화음분할 옵션을 사용해 보세요.');
												} else {
													sendError(message.channel, 'This file cannot be automatically split to fit 10,000 character scores. Terminate execution. Try the ignore10000 or chorddivision option.');
												}
												return;
											}
										}
										lWrite = true;
										icuttable[0] = icuttable[1];
										icuttable[1] = -1;
										tempCutIndex = cuttableMat.length-1;
										while (cuttableMat[tempCutIndex] > timeEndNote) {
											tempCutIndex--;
										}
										icuttable[2] = tempCutIndex;
									}
								}
							}
							
							if (lWrite) {
								for (iMaxCheck=0; iMaxCheck<iCountCharacter.length; iMaxCheck++) {
									if (iCountCharacter[iMaxCheck] > 10000) {
										if (!isEnglish) {
											sendError(message.channel, '전체 화음 중 1개 이상의 화음 글자수가 1만자를 초과하여 변환할 수 없습니다. 글자수를 줄이는 옵션 (해상도, 템포뭉개기, 서스테인뭉개기 등) 을 시도해 보세요. 오류라고 생각되신다면 개발자에게 제보해 주세요.');
										} else {
											sendError(message.channel, 'One or more of the total chords cannot be converted because the number of chord characters exceeds 10,000. Try reducing the number of characters (resolution, mergetempo, mergesustain, etc.). If you think this is an error, please report it to the developer.');
										}
										return;
									}
									if (iCountCharacter[iMaxCheck] < 10000 && iCountCharacter[iMaxCheck]+iCountCharacterPseudoSustain > 10000) {
										if (!isEnglish) {
											sendError(message.channel, '서스테인 이벤트가 너무 많아 변환할 수 없습니다. 서스테인뭉개기 옵션으로 서스테인 이벤트의 개수를 줄여 보세요. 오류라고 생각되신다면 개발자에게 제보해 주세요.');
										} else {
											sendError(message.channel, 'There are too many sustain events to convert. Reduce the number of sustain events with the mergesustain option. If you think this is an error, please report it to the developer.');
										}
										return;
									}
								}
										
								if (lOnoffHorizontalDivision) {
									iCurrentWriteHorizontal++;
									if (iCurrentWriteHorizontal > 50) {
										if (!isEnglish) {
											sendError(message.channel, '작성된 악보가 50장을 초과하여 중지합니다. 미디 파일의 길이 및 변환 설정을 확인해 주세요. 오류라고 생각되신다면 개발자에게 문의해 주세요.');
										} else {
											sendError(message.channel, 'The written score exceeds 50 sheets and stops. Please check the length and conversion settings of the MIDI file. If you think this is an error, please contact the developer.');
										}
										return;
									}
								}
								if (lOnoffVerticalDivision) { iCurrentWriteVertical = 0; isFirstChord = true; }
								
								isWritten = createArray(curchord);
								for (i=0; i<curchord; i++) { isWritten[i] = false; }
								tempIOnoffVerticalDivision = lOnoffVerticalDivision;
								if (lOnoffVerticalDivision && (curchord < Math.ceil(lengthHorizontalDivision/10000))) {
									if (!isEnglish) {
										sendWarning(message.channel, '이 악보의 화음은 ' + curchord + '개이므로 ' + Math.ceil(lengthHorizontalDivision/10000) + '분할할 수 없습니다. 화음 분할 기능이 비활성화됩니다.');
									} else {
										sendWarning(message.channel, 'The chord in this score is ' + curchord + ', so cannot be divided into ' + Math.ceil(lengthHorizontalDivision/10000) + '. Chord division function is disabled.');
									}
									tempIOnoffVerticalDivision = false;
								}
								
								isCheckWritten = false;
								while (!isCheckWritten) {
									if (tempIOnoffVerticalDivision) {
										iCurrentWriteVertical++;
										if (iCurrentWriteVertical > 50) {
											if (!isEnglish) {
												sendError(message.channel, '작성된 악보가 50장을 초과하여 중지합니다. 미디 파일의 길이 및 변환 설정을 확인해 주세요. 오류라고 생각되신다면 개발자에게 문의해 주세요.');
											} else {
												sendError(message.channel, 'The written score exceeds 50 sheets and stops. Please check the length and conversion settings of the MIDI file. If you think this is an error, please contact the developer.');
											}
											return;
										}
									}
									
									if (!isMML) {
										outputtxt = '<?xml version="1.0" encoding="utf-8"?>' + "\n";
										outputtxt += '<ms2>' + "\n";
									} else {
										outputtxt = '[Settings]\nEncoding = ks_c_5601-1987\nTitle = \nSource = \nMemo = \n';
									}
									isFirstChannel = true;
									iWriteChordPos = -1;
									isSustainWritten = false;
									
									curWrittenCharacter = 0;
									curChordPos = 0;
									curChordDir = 1;
									
									while (!tempIOnoffVerticalDivision || curWrittenCharacter < 10000) {
										if (iCountCharacterPseudoSustain > 0 && iWriteChordPos == -1) {
											iWriteChordPos++;
											if (isFirstChannel) {
												if (!isMML) {
													outputtxt += '<melody>' + "\n";
													outputtxt += '<![CDATA[' + "\n";
												} else {
													outputtxt += '[Channel' + (iWriteChordPos+1) + ']\n';
												}
											} else {
												if (!isMML) {
													outputtxt += '<chord index="' + (iWriteChordPos) + '">' + "\n";
													outputtxt += '<![CDATA[' + "\n";
												} else {
													outputtxt += '[Channel' + (iWriteChordPos+1) + '[';
												}
											}
											curWrittenCharacter += iCountCharacterPseudoSustain;
											outputtxt += cCharacterPseudoSustain;
											isSustainWritten = true;
											if (!isMML) {
												outputtxt += "\n" + ']]>' + "\n";
												if (isFirstChannel) {
													outputtxt += '</melody>' + "\n";
													isFirstChannel = false;
												} else {
													outputtxt += '</chord>' + "\n";
												}
											} else {
												outputtxt += "\n";
											}
										} else {
											if (isWritten[curChordPos] == false) {
												iWriteChordPos++;
												if (isFirstChannel) {
													if (!isMML) {
														outputtxt += '<melody>' + "\n";
														outputtxt += '<![CDATA[' + "\n";
													} else {
														outputtxt += '[Channel' + (iWriteChordPos+1) + ']\n';
													}
												} else {
													if (!isMML) {
														outputtxt += '<chord index="' + (iWriteChordPos) + '">' + "\n";
														outputtxt += '<![CDATA[' + "\n";
													} else {
														outputtxt += '[Channel' + (iWriteChordPos+1) + '[';
													}
												}
												curWrittenCharacter += iCountCharacter[curChordPos];
												isWritten[curChordPos] = true;
												outputtxt += cCharacter[curChordPos];
												if (!isMML) {
													outputtxt += "\n" + ']]>' + "\n";
													if (isFirstChannel) {
														outputtxt += '</melody>' + "\n";
														isFirstChannel = false;
													} else {
														outputtxt += '</chord>' + "\n";
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
														tempChordIndex = curchord-1;
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
									
									if (!isMML) { outputtxt += '</ms2>' + "\n"; }
									
									currentName = ((message.guild)?('guild_'+message.channel.id):('dm_'+message.author.id));
									outputFileName = user2filename[currentName].substring(0, user2filename[currentName].length-4);
									
									if ((command == '악보' && iFixedInstrument != -1) || (command == '합주악보')) {
										outputFileName += '_' + instr2name(supportedInstrList[0]);
									}
				
									fileExtension = '.ms2mml';
									if (isMML) { fileExtension = '.mml'; }
									if (!lOnoffHorizontalDivision && !tempIOnoffVerticalDivision) {
										outputFileName += fileExtension;
									} else if (lOnoffHorizontalDivision && !tempIOnoffVerticalDivision && lHorizontalDivisionFinished && iCurrentWriteHorizontal==1) {
										outputFileName += fileExtension;
									} else if (lOnoffHorizontalDivision && !tempIOnoffVerticalDivision) {
										outputFileName += '_' + iCurrentWriteHorizontal.toString().padStart(2,"0") + fileExtension;
									} else if (!lOnoffHorizontalDivision && tempIOnoffVerticalDivision) {
										isCheckWritten = true;
										for (i=0; i<curchord; i++) { if (isWritten[i] == false) { isCheckWritten = false; } }
										if (isCheckWritten && iCurrentWriteVertical==1) {
											outputFileName += fileExtension;
										} else {
											outputFileName += '_chord' + iCurrentWriteVertical.toString() + fileExtension;
										}
									}
									fs.writeFileSync(outputFileName, outputtxt, function (err) {
										if (err) {
											if (!isEnglish) {
												sendError(message.channel, '파일 쓰기 중 에러가 발생했습니다. 개발자에게 문의해 주세요...');
											} else {
												sendError(message.channel, 'An error occurred while writing the file. Please contact the developer...');
											}
											console.dir(err);
											return;
										}
										iHorizontalTryCount++;
									});
									writtenFileList = writtenFileList.concat([outputFileName]);
									writtenCharacterList = writtenCharacterList.concat([curWrittenCharacter]);
									writtenChordList = writtenChordList.concat([iWriteChordPos+1]);
									if (curWrittenCharacter > 10000 && !isLengthWarned) {
										if (!isEnglish) {
											sendWarning(message.channel, '악보의 글자 수가 게임 내 제한인 10,000자를 초과합니다. 일부 음표가 연주되지 않을 수 있습니다. <해상도/템포뭉개기/서스테인뭉개기> 등의 옵션으로 악보 품질을 낮추고 길이를 줄여볼 수 있습니다. 혹은 길이분할 옵션을 사용해 여러 장의 독주 악보로 나눌 수 있습니다. 아니면 화음분할 옵션을 사용해 합주 악보로 나눌 수 있습니다.');
										} else {
											sendWarning(message.channel, 'The number of characters in the score exceeds the in-game limit of 10,000 characters. Some notes may not play. You can try lowering the quality of the score and shortening its length with options such as <resolution/mergetempo/mergesustain>. Alternatively, you can use the lengthdivision option to split it into multiple solo scores. Alternatively, you can use the chorddivision option to split it into ensemble scores.');
										}
										isLengthWarned = true;
									}
									if (iWriteChordPos+1 > 10 && !isChordWarned) {
										if (!isEnglish) {
											sendWarning(message.channel, '악보의 화음 수가 게임 내 제한인 10화음을 초과합니다. 게임에서 파일을 불러올 수는 있지만 11번째 이후 화음의 음표는 사용되지 않습니다. 화음분할 옵션을 사용해 합주 악보로 만들 수 있습니다.');
										} else {
											sendWarning(message.channel, 'The number of chords in the score exceeds the in-game limit of 10 chords. The game can load the file, but notes from the 11th and later chords are not used. You can use the chorddivision option to create an ensemble score.');
										}
										isChordWarned = true;
									}
									isCheckWritten = true;
									for (i=0; i<curchord; i++) { if (isWritten[i] == false) { isCheckWritten = false; break; } }
								}
								if (iHorizontalTryCount == 3 && !lHorizontalDivisionFinished) {
									if (!isEnglish) {
										sendWarning(message.channel, '출력된 악보가 3장을 초과합니다. 악보를 연속으로 재생하기 어려울 수 있습니다.');
									} else {
										sendWarning(message.channel, 'The printed sheet music exceeds 3 sheets. It may be difficult to play sheet music continuously.');
									}
								}
							}
						}
					}
					
					if (writtenFileList.length > 10 && writtenFileList.length <= 13) {
						if (!isEnglish) {
							sendWarning(message.channel, '생성된 합주 악보의 연주자 수가 10명을 초과합니다. 메이플스토리2에서 10명을 초과하는 합주는 2개 파티에 나뉘어 집결한 후 동시에 시작하여 연주한 사례가 있으나, 컴퓨터 2대를 동시에 사용하는 플레이어가 있어야 합니다. 악보 글자수를 줄여 인원수를 줄이려면 합주악보 해상도 5 서스테인뭉개기 2 등과 같이 사용해 보세요.');
						} else {
							sendWarning(message.channel, 'The number of players in the created ensemble score exceeds 10. In MapleStory 2, there is a case where an ensemble of more than 10 people was divided into two parties and assembled and then started and played at the same time, but there must be a player using two computers. If you want to reduce the number of characters by reducing the number of characters in the score, try using it with ' + prefix + 'ensemble resolution 5 mergesustain 2, etc.');
						}
					}
					if (writtenFileList.length > 13) {
						if (!isEnglish) {
							sendWarning(message.channel, '생성된 합주 악보의 연주자 수가 13명을 초과합니다. 메이플스토리2에서 13명을 초과하는 합주는 2개 파티를 사용하여 가능하기는 하나, MIDI의 한계로 인해 일부 악기가 재생되지 않는 것으로 알려져 있으니 주의하십시오.');
						} else {
							sendWarning(message.channel, 'The number of players in the created ensemble score exceeds 13. Ensembles with more than 13 people in MapleStory 2 are possible using 2 parties, but be aware that some instruments are known to not play due to MIDI limitations.');
						}
					}
					
					if (writtenFileList.length == 0) {
						if (!isEnglish) {
							sendError(message.channel, '미디 파일에 포함된 음표가 없거나 SayangBot이 읽지 못했습니다...');
						} else {
							sendError(message.channel, 'Midi file contains no notes or SayangBot couldn"t read it...');
						}
						return;
					} else if (writtenFileList.length == 1) {
						outputFileSendName = writtenFileList[0];
					} else {
						var zip2 = new require('node-zip')();
						for (iWritten=0; iWritten<writtenFileList.length; iWritten++) {
							zip2.file(writtenFileList[iWritten], fs.readFileSync(writtenFileList[iWritten]));
						}
						var data = zip2.generate({base64:false, compression:'DEFLATE'});
						currentName = ((message.guild)?('guild_'+message.channel.id):('dm_'+message.author.id));
						outputFileSendName = user2filename[currentName].substring(0, user2filename[currentName].length-4)+'.zip';
						fs.writeFileSync(outputFileSendName, data, 'binary');
					}
	
					const exampleEmbed = new MessageEmbed()
						.setColor('#8cffa9')
						.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
						.setFooter('문의 : 눈꽃빙빙빙 (계란밥#9331)', 'https://i.imgur.com/82dLPkv.png');
					if (!isMML) {
						exampleEmbed.setTitle('mid->ms2mml 변환 완료')
					} else {
						exampleEmbed.setTitle('mid->mml 변환 완료')
					}
					if (writtenFileList.length == 1) {
						exampleEmbed.setDescription('독주 악보');
					} else if (writtenFileList.length > 1 && (lOnoffVerticalDivision || command=='합주악보')) {
						exampleEmbed.setDescription(writtenFileList.length + '인 합주 악보');
					} else {
						exampleEmbed.setDescription('독주 악보 (' + writtenFileList.length + '장 연속 재생)');
					}
					for (iWritten=0; iWritten<writtenFileList.length; iWritten++) {
						tempMessage = '```ini' + `
	[글자 수] : ` + writtenCharacterList[iWritten] + `
	[화음 수] : ` + writtenChordList[iWritten] + '\n```';
						exampleEmbed.addField(writtenFileList[iWritten], tempMessage, false);
					}
					message.channel.send(exampleEmbed);
					message.channel.send('↓다운로드', { files : [outputFileSendName] });
				}
		
				if (command == '정보') {
					currentName = ((message.guild)?('guild_'+message.channel.id):('dm_'+message.author.id));
					isFirstMessage = true;

					tempMessage = '```' + `파일 이름
	` + user2filename[currentName];

					tempMessage += `
BPM`;
					if (tempomat.length == 0) {
						tempMessage += `
	정보 없음, 120으로 임의 고정`;
					} else if (tempomat.length == 1 && tempomat[0][0] == 0) {
						tempMessage += `
	` + tempomat[0][1].toString();
					} else if (tempomat.length == 1 && tempomat[0][0] != 0) {
						if (tempomat[0][1] == 120) {
							tempMessage += `
	` + tempomat[0][1].toString();
						} else {
							tempMessage += `
	120 -> ` + tempomat[0][1].toString();
						}
					} else {
						var tempoInfo = [120, 99999, -1, 120];
						for (i=0; i<tempomat.length; i++) {
							if (tempomat[i][0] == 0) { tempoInfo[0] = tempomat[i][1]; }
							tempoInfo[1] = (tempoInfo[1] < tempomat[i][1]) ? tempoInfo[1] : tempomat[i][1];
							tempoInfo[2] = (tempoInfo[2] > tempomat[i][1]) ? tempoInfo[2] : tempomat[i][1];
							tempoInfo[3] = tempomat[i][1];
						}
						tempMessage += `
	시작 : ` + tempoInfo[0] + `
	최소 : ` + tempoInfo[1] + `
	최대 : ` + tempoInfo[2] + `
	종료 : ` + tempoInfo[3];
					}
				
					tempMessage += `
박자
	`;
					if (iMaxTimeSignature>0) {
						for (i=0; i<iMaxTimeSignature; i++) {
							if (i!=0) { tempMessage += ' -> '; }
							tempMessage += timesigmat[i][1][0].toString() + '/' + Math.pow(2, timesigmat[i][1][1]).toString();
							if (i>5) { tempMessage += ' -> ...'; break; }
						}
					} else {
						tempMessage += '정보 없음';
					}
				
					tempMessage += `
재생 시간
	`;
					if (maxCurrentTimeInReal > 3600) {
						tempMessage += Math.floor(maxCurrentTimeInReal/3600).toString() + '시간 ';
						maxCurrentTimeInReal = maxCurrentTimeInReal % 3600;
					}
					if (maxCurrentTimeInReal > 60) {
						tempMessage += Math.floor(maxCurrentTimeInReal/60).toString() + '분 ';
						maxCurrentTimeInReal = maxCurrentTimeInReal % 60;
					}
					tempMessage += Math.round(maxCurrentTimeInReal.toString()*100)/100 + '초';
					
					tempMessage += `
곡 전체 틱 / 4분 음표 틱
	`;
					tempMessage += maxLengthTrack + ' / ' + midiArray.timeDivision;
				
					tempMessage += `
트랙 정보`;
					for (itrack=0; itrack<midiArray.track.length; itrack++) {
						tempMessage += `
	` + (itrack+1).toString() + `번 트랙` + trackName[itrack];
						for (ichannel=0; ichannel<iMaxChannel; ichannel++) {
							if (numNoteTrackChannel[itrack][ichannel] > 0) {
								tempMessage += `
		` + (ichannel+1+iPort[itrack]*16).toString() + `번 채널`
								tempMessage += `
			악기 : `;
								if (ichannel%16 == 9) {
									tempMessage += 'Drumset';
								} else {
									if (instrumentTrackChannel[itrack][ichannel].length == 0) {
										countOtherInst = 0;
										for (itrack2=0; itrack2<midiArray.track.length; itrack2++) {
											if (instrumentTrackChannel[itrack2][ichannel].length > 0) {
												countOtherInst++;
											}
										}
										if (countOtherInst == 1) {
											for (itrack2=0; itrack2<midiArray.track.length; itrack2++) {
												if (instrumentTrackChannel[itrack2][ichannel].length > 0) {
													for (iInstr=0; iInstr<instrumentTrackChannel[itrack2][ichannel].length; iInstr++) {
														if (iInstr!=0) { tempMessage += ' -> '; }
														tempMessage += instr2name(instrumentTrackChannel[itrack2][ichannel][iInstr][1]);
														if (iInstr>5) { tempMessage += ' -> ...'; break; }
													}
													tempMessage += ' (' + (itrack2+1).toString() + '번 트랙에서 추정함)';
												}
											}
										} else {
											tempMessage += '정보 없음';
										}
									} else {
										for (iInstr=0; iInstr<instrumentTrackChannel[itrack][ichannel].length; iInstr++) {
											if (iInstr!=0) {
												if (instr2name(instrumentTrackChannel[itrack][ichannel][iInstr][1])==instr2name(instrumentTrackChannel[itrack][ichannel][iInstr-1][1])) {
													continue;
												} else {
													tempMessage += ' -> ';
													tempMessage += instr2name(instrumentTrackChannel[itrack][ichannel][iInstr][1]);
												}
											} else {
												tempMessage += instr2name(instrumentTrackChannel[itrack][ichannel][iInstr][1]);
											}
											if (iInstr>5) { tempMessage += ' -> ...'; break; }
										}
									}
								}
								tempMessage += `
			음표 개수 : ` + numNoteTrackChannel[itrack][ichannel].toString();
								if (sustainTrackChannel[itrack][ichannel].length > 0) {
									tempMessage += `
			서스테인 이벤트 포함`;
								}
							}
						}
						if (tempMessage.length > 1500 && itrack != midiArray.track.length-1) {
							tempMessage += '```';
							const exampleEmbed = new MessageEmbed()
								.setColor('#8cffa9')
								.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
								.setDescription(tempMessage);
							if (isFirstMessage) { exampleEmbed.setTitle('사용 중인 미디 파일 정보'); }
							isFirstMessage = false;
							message.channel.send(exampleEmbed);
							tempMessage = '```';
						}
					}
				
					if (iMaxText > 0) {
						tempMessage += `
텍스트 이벤트`;
						for (i=0; i<iMaxText; i++) {
							tempMessage += `
	`;
							currentTimeInReal = textMat[i][0];
							if (currentTimeInReal > 3600) {
								tempMessage += Math.floor(currentTimeInReal/3600).toString() + '시간 ';
								currentTimeInReal = currentTimeInReal % 3600;
							}
							if (currentTimeInReal > 60) {
								tempMessage += Math.floor(currentTimeInReal/60).toString() + '분 ';
								currentTimeInReal = currentTimeInReal % 60;
							}
							tempMessage += Math.round(currentTimeInReal.toString()*100)/100 + '초 : ';
							tempMessage += textMat[i][1];
							if (i > 10) {
								tempMessage += `
	이하 생략`;
								if (tempMessage.length > 1500 && i != iMaxText-1) {
									tempMessage += '```';
									const exampleEmbed = new MessageEmbed()
										.setColor('#8cffa9')
										.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
										.setDescription(tempMessage);
									if (isFirstMessage) { exampleEmbed.setTitle('사용 중인 미디 파일 정보'); }
									isFirstMessage = false;
									message.channel.send(exampleEmbed);
									// message.channel.send(tempMessage);
									tempMessage = '```';
								}
								break;
							}
							if (tempMessage.length > 1500 && i != iMaxText-1) {
								tempMessage += '```';
								const exampleEmbed = new MessageEmbed()
									.setColor('#8cffa9')
									.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
									.setDescription(tempMessage);
								if (isFirstMessage) { exampleEmbed.setTitle('사용 중인 미디 파일 정보'); }
								isFirstMessage = false;
								message.channel.send(exampleEmbed);
								// message.channel.send(tempMessage);
								tempMessage = '```';
							}
						}
					}
				
					if (iMaxCopyright > 0) {
						tempMessage += `
저작권 정보`;
						for (i=0; i<iMaxCopyright; i++) {
							tempMessage += `
	` + copyrightMat[i][1];
						}
					}
	
					tempMessage += '```';
					const exampleEmbed = new MessageEmbed()
						.setColor('#8cffa9')
						.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
						.setDescription(tempMessage)
						.setFooter('문의 : 눈꽃빙빙빙 (계란밥#9331)', 'https://i.imgur.com/82dLPkv.png');
					if (isFirstMessage) { exampleEmbed.setTitle('사용 중인 미디 파일 정보'); }
					message.channel.send(exampleEmbed);
					// message.channel.send(tempMessage);
				}
			});
		} else {
			if (!isEnglish) {
				sendError(message.channel, '인식된 미디 파일이 없습니다. 미디 파일이 첨부된 메시지를 전송해 주세요.');
			} else {
				sendError(message.channel, 'The MIDI file is not recognized yet. Please send a message with the MIDI file attached.');
			}
			return;
		}
	} else {
		if (!isEnglish) {
			sendError(message.channel, '' + command + (checkBatchimEnding(command)?'은':'는') + ' 없는 명령어입니다. 명령어 목록은 ' + prefix + '도움말 을 참고해 주세요.');
		} else {
			sendError(message.channel, '' + command + (checkBatchimEnding(command)?' is':' is') + ' not a valid command for SayangBot. For a list of commands, see ' + prefix + 'help');
		}
	}
};

const handleReady = (client) => {
  logger.info('Connected to Discord! - Ready.');
  updatePresence(client);

  client.guilds.cache.each((guild) => {
    guild.ttsPlayer = new TTSPlayer(guild);
    guild.ttsPlayer.setLang('ko')
        .then()
        .catch();
  });
};

const handleWarn = (info) => {
  logger.warn(info);
};

module.exports = {
  handleDebug,
  handleError,
  handleGuildCreate,
  handleGuildDelete,
  handleGuildUnavailable,
  handleInvalidated,
  handleMessage,
  handleReady,
  handleWarn
};

function checkBatchimEnding(word) {
  return false;
}

function downloadmidi(url, path) {
	request.get(url)
		.on('error', console.error)
		.pipe(fs.createWriteStream(path));
}

function createArray(length) {
    var arr = new Array(length || 0),
        i = length;

    if (arguments.length > 1) {
        var args = Array.prototype.slice.call(arguments, 1);
        while(i--) arr[length-1 - i] = createArray.apply(this, args);
    }

    return arr;
}

function sortFunctionByFirst(a, b) {
    if (a[0] === b[0]) {
        return 0;
    }
    else {
        return (a[0] < b[0]) ? -1 : 1;
    }
}

function sortFunctionByFifth(a, b) {
    if (a[4] === b[4]) {
        return 0;
    }
    else {
        return (a[4] < b[4]) ? -1 : 1;
    }
}

function sortFunctionByFifthSixth(a, b) {
    if (a[4] === b[4]) {
		if (a[5] === b[5]) {
			return 0;
		}
		else {
			return (a[5] < b[5]) ? -1 : 1;
		}
    }
    else {
        return (a[4] < b[4]) ? -1 : 1;
    }
}

function sortFunctionByFirstSecondThird(a, b) {
    if (a[0] === b[0]) {
		if (a[1] === b[1]) {
			if (a[2] === b[2]) {
				return 0;
			}
			else {
				return (a[2] < b[2]) ? -1 : 1;
			}
		}
		else {
			return (a[1] < b[1]) ? -1 : 1;
		}
    }
    else {
        return (a[0] < b[0]) ? -1 : 1;
    }
}

function keysig2text(a) {
    if (a/256 <= 7) {
		ss=['C','G','D', 'A', 'E','B',  'F#', 'C#'];
		dataStr = ss[a/256+1];
		if (a%256 == 0) { dataStr += ' Major'; }
		else if (a%256 == 1) { dataStr += ' Minor'; }
	} else if (a/256 >= 249) {
		ss=['F','Bb','Eb','Ab','Db','Gb','Cb'];
		dataStr = ss[255-a/256+1];
		if (a%256 == 0) { dataStr += ' Major'; }
		else if (a%256 == 1) { dataStr += ' Minor'; }
	} else { dataStr = '알 수 없음'; }
	return dataStr;
}

function instr2name(a) {
	instrList = ['AcousticGrandPiano',
'BrightAcousticPiano',
'ElectricGrandPiano',
'Honky-tonkPiano',
'ElectricPiano1',
'ElectricPiano2',
'Harpsichord',
'Clavi',
'Celesta',
'Glockenspiel',
'MusicBox',
'Vibraphone',
'Marimba',
'Xylophone',
'TubularBells',
'Dulcimer',
'DrawbarOrgan',
'PercussiveOrgan',
'RockOrgan',
'ChurchOrgan',
'ReedOrgan',
'Accordion',
'Harmonica',
'TangoAccordion',
'AcousticGuitar(nylon)',
'AcousticGuitar(steel)',
'ElectricGuitar(jazz)',
'ElectricGuitar(clean)',
'ElectricGuitar(muted)',
'OverdrivenGuitar',
'DistortionGuitar',
'GuitarHarmonics',
'AcousticBass',
'ElectricBass(finger)',
'ElectricBass(pick)',
'FretlessBass',
'SlapBass1',
'SlapBass2',
'SynthBass1',
'SynthBass2',
'Violin',
'Viola',
'Cello',
'Contrabass',
'TremoloStrings',
'PizzicatoStrings',
'OrchestralHarp',
'Timpani',
'StringEnsemble1',
'StringEnsemble2',
'SynthStrings1',
'SynthStrings2',
'ChoirAahs',
'VoiceOohs',
'SynthVoice',
'OrchestraHit',
'Trumpet',
'Trombone',
'Tuba',
'MutedTrumpet',
'FrenchHorn',
'BrassSection',
'SynthBrass1',
'SynthBrass2',
'SopranoSax',
'AltoSax',
'TenorSax',
'BaritoneSax',
'Oboe',
'EnglishHorn',
'Bassoon',
'Clarinet',
'Piccolo',
'Flute',
'Recorder',
'PanFlute',
'Blownbottle',
'Shakuhachi',
'Whistle',
'Ocarina',
'Lead1(square)',
'Lead2(sawtooth)',
'Lead3(calliope)',
'Lead4(chiff)',
'Lead5(charang)',
'Lead6(voice)',
'Lead7(fifths)',
'Lead8(bass+lead)',
'Pad1(newage)',
'Pad2(warm)',
'Pad3(polysynth)',
'Pad4(choir)',
'Pad5(bowed)',
'Pad6(metallic)',
'Pad7(halo)',
'Pad8(sweep)',
'FX1(rain)',
'FX2(soundtrack)',
'FX3(crystal)',
'FX4(atmosphere)',
'FX5(brightness)',
'FX6(goblins)',
'FX7(echoes)',
'FX8(sci-fi)',
'Sitar',
'Banjo',
'Shamisen',
'Koto',
'Kalimba',
'Bagpipe',
'Fiddle',
'Shanai',
'TinkleBell',
'Agogô',
'SteelDrums',
'Woodblock',
'TaikoDrum',
'MelodicTom',
'SynthDrum',
'ReverseCymbal',
'GuitarFretNoise',
'BreathNoise',
'Seashore',
'BirdTweet',
'TelephoneRing',
'Helicopter',
'Applause',
'Gunshot',
'Drumset',
'Cymbal',
'BigDrum',
'SmallDrum'];
	return instrList[a];
}

function scaletoname(scalenumber) {
	if (scalenumber ==  0) { outname = 'c'; }
	if (scalenumber ==  1) { outname = 'c+'; }
	if (scalenumber ==  2) { outname = 'd'; }
	if (scalenumber ==  3) { outname = 'd+'; }
	if (scalenumber ==  4) { outname = 'e'; }
	if (scalenumber ==  5) { outname = 'f'; }
	if (scalenumber ==  6) { outname = 'f+'; }
	if (scalenumber ==  7) { outname = 'g'; }
	if (scalenumber ==  8) { outname = 'g+'; }
	if (scalenumber ==  9) { outname = 'a'; }
	if (scalenumber == 10) { outname = 'a+'; }
	if (scalenumber == 11) { outname = 'b'; }
	
	return outname;
}

function notetotime2(note, tpq) {
	return tpq*4/note;
}

function get_length_list2_expand(dur,tpq,resolution) {
	larray = [];
	curres = 1;
	while (dur > 0 && curres <= resolution) {
		if (Math.abs(dur - 4*tpq) < 4*tpq/10) {
		// if (Math.abs(dur - 4*tpq) < 4*tpq/resolution && dur > 4*tpq) {
			larray = larray.concat([1]);
			break;
		} else if (Math.abs(dur - 3*tpq) < 3*tpq/10) {
		// } else if (Math.abs(dur - 3*tpq) < 3*tpq/resolution && dur > 3*tpq) {
			larray = larray.concat([2, 4]);
			break;
		} else if (Math.abs(dur - 2*tpq) < 2*tpq/10) {
		// } else if (Math.abs(dur - 2*tpq) < 2*tpq/resolution && dur > 2*tpq) {
			larray = larray.concat([2]);
			break;
		} else if (Math.abs(dur - 3*tpq/2) < 3*tpq/2/10) {
			larray = larray.concat([4, 8]);
			break;
		} else if (Math.abs(dur - tpq) < tpq/10) {
			larray = larray.concat([4]);
			break;
		} else if (Math.abs(dur - tpq/2) < tpq/2/10) {
			larray = larray.concat([8]);
			break;
		// } else if (Math.abs(dur - tpq/3) < tpq/3/resolution && dur > tpq/3) {
			// larray = larray.concat([12]);
			// break
		} else if (Math.abs(dur - tpq/4) < tpq/4/10) {
			larray = larray.concat([16]);
			break;
		// } else if (Math.abs(dur - tpq/6) < tpq/6/resolution && dur > tpq/6
			// larray = larray.concat([24]);
			// break
		} else {
			if (dur >= 4*tpq / curres) {
				larray = larray.concat([curres]);
				dur = dur - 4*tpq / curres;
			}
			if (dur < 4*tpq / curres) {
				curres = curres * 2;
			}
		}
	}
	if (larray.length == 0) { larray = [resolution]; }
	
	return larray;
}

function get_length_list2_nonexpand(dur,tpq,resolution) {
	larray = [];
	curres = 1;
	while (dur > 4*tpq/resolution/2 && curres <= resolution) {
		if (Math.abs(dur - 4*tpq) < 4*tpq/resolution && dur > 4*tpq) {
			larray = larray.concat([1]);
			break;
		} else if (Math.abs(dur - 3*tpq) < 3*tpq/resolution && dur > 3*tpq) {
			larray = larray.concat([2, 4]);
			break;
		} else if (Math.abs(dur - 2*tpq) < 2*tpq/resolution && dur > 2*tpq) {
			larray = larray.concat([2]);
			break;
		} else if (Math.abs(dur - 3*tpq/2) < 3*tpq/2/resolution && dur > 3*tpq/2) {
			larray = larray.concat([4, 8]);
			break;
		} else if (Math.abs(dur - tpq) < tpq/resolution && dur > tpq) {
			larray = larray.concat([4]);
			break;
		} else if (Math.abs(dur - tpq/2) < tpq/2/resolution && dur > tpq/2) {
			larray = larray.concat([8]);
			break;
		// } else if (Math.abs(dur - tpq/3) < tpq/3/resolution && dur > tpq/3) {
			// larray = larray.concat([12]);
			// break
		} else if (Math.abs(dur - tpq/4) < tpq/4/resolution && dur > tpq/4) {
			larray = larray.concat([16]);
			break;
		// } else if (Math.abs(dur - tpq/6) < tpq/6/resolution && dur > tpq/6
			// larray = larray.concat([24]);
			// break
		} else {
			if (dur >= 4*tpq / curres) {
				larray = larray.concat([curres]);
				dur = dur - 4*tpq / curres;
			}
			if (dur < 4*tpq / curres) {
				curres = curres * 2;
			}
		}
	}
	
	return larray;
}

function sortObjectByKeys(o) {
    return Object.keys(o).sort().reduce((r, k) => (r[k] = o[k], r), {});
}

function sendError(a, b) {
	const exampleEmbed = new MessageEmbed()
		.setColor('#ff0000')
		.setTitle('에러')
		.setDescription('```css\n['+b+']```')
		.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
		.setFooter('문의 : 눈꽃빙빙빙 (계란밥#9331)', 'https://i.imgur.com/82dLPkv.png');
	a.send(exampleEmbed);
    return;
}

function sendWarning(a, b) {
	const exampleEmbed = new MessageEmbed()
		.setColor('#ffa500')
		.setTitle('경고')
		.setDescription('```fix\n['+b+']```')
		.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
		.setFooter('문의 : 눈꽃빙빙빙 (계란밥#9331)', 'https://i.imgur.com/82dLPkv.png');
	a.send(exampleEmbed);
    return;
}

function sendInformation(a, b) {
	const exampleEmbed = new MessageEmbed()
		.setColor('#8cffa9')
		.setTitle('알림')
		.setDescription('```ini\n['+b+']```')
		.setURL('https://www.youtube.com/channel/UCB-vCi140r38njzhiQ-ykug')
		.setFooter('문의 : 눈꽃빙빙빙 (계란밥#9331)', 'https://i.imgur.com/82dLPkv.png');
	a.send(exampleEmbed);
    return;
}