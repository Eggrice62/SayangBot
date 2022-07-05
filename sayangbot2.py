import discord
from datetime import datetime
import os
import requests
import time
from shutil import copyfile
from os.path import exists

client = discord.Client()

@client.event
async def on_ready():
    print('We have logged in as {0.user}'.format(client))

@client.event
async def on_message(message):
	if message.author.bot:
		return
		
	if message.author == client.user:
		return
	
	if len(message.attachments)>0:
		if message.attachments[0].filename.lower().endswith('mid') or message.attachments[0].filename.lower().endswith('midi'):
			os.makedirs('db/'+(('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id))), exist_ok=True)
			currentName = datetime.now().strftime("%Y_%m_%d-%I_%M_%S_%p_") + message.attachments[0].filename
			downloadmidi(message.attachments[0].url, 'db/'+(('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id))) + '/' + currentName)# at the top of your file
			if message.attachments[0].filename[:7]=='AnyConv':
				await sendError(message.channel, '죄송합니다, AnyConv에서 생성한 MIDI 파일은 실행을 차단하고 있습니다. 일반적으로 MIDI 파일은 Musescore, NWC 등의 악보 프로그램이나 Cakewalk, FL Studio와 같은 시퀀싱 프로그램에서 생성하는 경우가 가장 많으며, SayangBot은 이러한 파일에는 대체로 문제없이 사용 가능한 상황입니다. 하지만 AnyConv에서 변환한 MIDI 파일은 음원을 인식하여 변환하는 과정에서 128분음표가 등장하는 등 필요 이상으로 복잡해지는 것으로 파악되고 있고, 이 때문에 처리 시간이 과다하게 소요되는 것을 막기 위해 실행을 차단하게 되었습니다. 가급적 mp3 등 다른 확장자를 변환한 것이 아닌, 처음부터 mid 확장자로 입수하신 파일을 사용해주시길 부탁드리겠습니다.')
				return
			user2filename[(('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id)))] = currentName
			user2processtime[(('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id)))] = 0
			exampleEmbed = discord.Embed(color=0x8cffa9,title='미디 파일 인식됨',url='https://github.com/Eggrice62/SayangBot' ,description=message.attachments[0].filename+'\n아래 명령어를 사용할 수 있습니다.')
			exampleEmbed.add_field(inline=False, name= prefix+'정보', value= '파일 내용 간단 확인')
			exampleEmbed.add_field(inline=False, name= prefix+'악보', value= '독주용 악보로 제작')
			exampleEmbed.add_field(inline=False, name= prefix+'합주악보', value= '합주를 위한 악기별 악보로 제작')
			exampleEmbed.add_field(inline=False, name= '전체 명령어 도움말', value= prefix+'도움말\n'+prefix+'도움말 <명령어>')
			exampleEmbed.add_field(inline=False, name= 'English help', value= prefix+'help\n'+prefix+'help <command>')
			exampleEmbed.set_footer(text='문의 : 눈꽃빙빙빙 (계란밥#9331)', icon_url='https://i.imgur.com/82dLPkv.png')
			### await message.channel.send(embed=exampleEmbed) 나중에 풀것
			user = await client.fetch_user('364432570005323796')
			exampleEmbed = discord.Embed(color=0x8cffa9,title=message.attachments[0].filename,url='https://github.com/Eggrice62/SayangBot' ,description=((message.guild.name+'-'+message.channel.name) if (message.guild) else 'DM'))
			exampleEmbed.set_author(name=str(message.author.name) + ' (' + str(message.author.id) + ')', icon_url=message.author.avatar_url)
			await user.send(embed=exampleEmbed)

	if (message.guild is not None and message.content.startswith(prefix)) or (not message.guild and message.author.id != 364432570005323796):
		user = await client.fetch_user('364432570005323796')
		exampleEmbed = discord.Embed(color=0x8cffa9,title=message.content,url='https://github.com/Eggrice62/SayangBot' ,description=((message.guild.name+'-'+message.channel.name) if (message.guild) else 'DM'))
		exampleEmbed.set_author(name=message.author.name + ' (' + str(message.author.id) + ')', icon_url=message.author.avatar_url)
		await user.send(embed=exampleEmbed)
	
	if not message.content.startswith(prefix):
		return
			
	user2time[(('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id)))] = time.time()
	
	isEnglish = False
	
	args = message.content[len(prefix):].strip().lower().split(' ')
	command = args.pop(0).lower()
	
	if command.encode().isalpha():
		isEnglish = True
	if command == 'info':
		command = '정보'
	elif command == 'solo':
		command = '악보'
	elif command == 'ensemble':
		command = '합주악보'
	
	for iarg in range(len(args)):
		curArg = args[iarg]
		if curArg == 'track':
			args[iarg] = '트랙'
		elif curArg == 'channel':
			args[iarg] = '채널'
		elif curArg == 'trackchannel':
			args[iarg] = '트랙채널'
		elif curArg == 'sustain':
			args[iarg] = '서스테인'
		elif curArg == 'motion':
			args[iarg] = '모션'
		elif curArg == 'start':
			args[iarg] = '시작'
		elif curArg == 'end':
			args[iarg] = '종료'
		elif curArg == 'lowest':
			args[iarg] = '최저음'
		elif curArg == 'highest':
			args[iarg] = '최고음'
		elif curArg == 'cutoff':
			args[iarg] = '음표깎기'
		elif curArg == 'resolution':
			args[iarg] = '해상도'
		elif curArg == 'mergetempo':
			args[iarg] = '템포뭉개기'
		elif curArg == 'mergesustain':
			args[iarg] = '서스테인뭉개기'
		elif curArg == 'motioncooltime':
			args[iarg] = '모션쿨'
		elif curArg == 'volume1':
			args[iarg] = '볼륨1'
		elif curArg == 'volume2':
			args[iarg] = '볼륨2'
		elif curArg == 'volume3':
			args[iarg] = '볼륨3'
		elif curArg == 'fixedvolume':
			args[iarg] = '고정볼륨'
		elif curArg == 'volumeincrease':
			args[iarg] = '볼륨증가'
		elif curArg == 'volumedecrease':
			args[iarg] = '볼륨감소'
		elif curArg == 'drumexclude':
			args[iarg] = '드럼제외'
		elif curArg == 'drumonly':
			args[iarg] = '드럼만'
		elif curArg == 'removeduplicates':
			args[iarg] = '중복제거'
		elif curArg == 'lengthdivision':
			args[iarg] = '길이분할'
		elif curArg == 'ignore10000':
			args[iarg] = '1만자무시'
		elif curArg == 'chorddivision':
			args[iarg] = '화음분할'
		elif curArg == 'motionspeed':
			args[iarg] = '모션속도템포'
		elif curArg == 'fixedtempo':
			args[iarg] = '고정템포'
		elif curArg == 'instrument':
			args[iarg] = '악기'
		elif curArg == 'triplet':
			args[iarg] = '셋잇단'
		elif curArg == 'autovolumeincrease':
			args[iarg] = '자동볼륨증가'
		elif curArg == 'disablesubstitution':
			args[iarg] = '대체악기사용안함'
		# elif curArg == 'dontrearrangechord':
			# args[iarg] = '화음섞지않기'
	
	if command == '피드백':
		exampleEmbed = discord.Embed(color=0x8cffa9,title='피드백 전송 완료',url='https://github.com/Eggrice62/SayangBot' ,description='보내주신 피드백이 정상적으로 전달되었습니다. 도와주셔서 감사합니다.')
		await message.channel.send(embed=exampleEmbed)
		return
	
	if command == 'feedback':
		exampleEmbed = discord.Embed(color=0x8cffa9,title='Feedback sent',url='https://github.com/Eggrice62/SayangBot' ,description='Your feedback has been successfully delivered. Thank you for your help.')
		await message.channel.send(embed=exampleEmbed)
		return

	if message.content.startswith('%s도움말'%prefix):
		if command == '도움말':
			if len(args) == 0:
				exampleEmbed = discord.Embed(title='SayangBot',color=0x8cffa9,url='https://github.com/Eggrice62/SayantBot',description='MIDI 파일 (*.mid) 을 ms2mml로 변환하는 봇입니다.\n이 채팅방에 미디 파일을 올리면 SayangBot이 자동으로 인식하여 작동합니다.\n미디 파일 공개가 꺼려지시면 개인 메시지 (DM)에서도 작동합니다.\n다만 보내신 mid 파일은 처리를 위해 제 컴퓨터에 저장됩니다. 도용을 하지는 않겠지만 그래도 참고해 주십시오.\n'+prefix+'도움말 <명령어> 로 세부 내용을 확인하세요!\nex) ' + prefix + '도움말 악보')
				exampleEmbed.add_field(inline=False, name= prefix+'정보', value= '미디 파일의 전반적인 정보')
				exampleEmbed.add_field(inline=False, name= prefix+'악보', value= '전체 음표를 독주악보 1개로 변환')
				exampleEmbed.add_field(inline=False, name= prefix+'합주악보', value= '합주를 위한 악기별 악보들로 변환')
				exampleEmbed.add_field(inline=False, name= prefix+'팁', value= '출력된 악보에 문제가 있다면 참고해 보세요!')
				exampleEmbed.add_field(inline=False, name= prefix+'피드백', value= '' + prefix + '피드백 으로 시작하는 메시지는 모두 개발자에게 자동으로 전달됩니다. 자유롭게 적어서 보내주시면 개발에 큰 도움이 됩니다. 감사합니다.')
				await message.channel.send(embed=exampleEmbed)
			else:
				if args[0] == '정보':
					exampleEmbed = discord.Embed(color=0x8cffa9,title='SayangBot 명령어 (' + prefix + args[0] + ')',url='https://github.com/Eggrice62/SayangBot',description='미디 파일의 전반적인 정보를 확인할 수 있습니다.')
					exampleEmbed.add_field(inline=False,name= '파일 이름', value= '현재 사용 중인 mid 파일입니다. 해당 파일은 새로운 파일이 업로드되거나, SayangBot이 재부팅될 때까지 유효합니다. DM에서 사용 중인 파일은 다른 사람의 DM이나 서버에서 사용하는 파일의 영향을 받지 않습니다. 순서 걱정 없이 천천히 작업해 보세요.')
					exampleEmbed.add_field(inline=False,name= 'BPM', value= '곡의 빠르기를 대략적으로 확인할 수 있습니다. 메이플스토리2에서 사용 가능한 템포 코드 범위는 32~255이지만, SayangBot은 8~510까지 알아서 바꿔 드립니다.')
					exampleEmbed.add_field(inline=False,name= '박자', value= '곡이 몇분의 몇박자인지를 확인할 수 있습니다. 사실 쓸데없는 정보입니다.')
					exampleEmbed.add_field(inline=False,name= '재생 시간', value= '곡의 재생 시간을 시/분/초 단위로 확인할 수 있습니다. 출력된 악보의 재생 시간은 템포 코드의 적용 상황에 따라 조금 오차가 생길 수 있습니다.')
					exampleEmbed.add_field(inline=False,name= '곡 전체 틱 / 4분음표 틱', value= '그렇습니다.')
					exampleEmbed.add_field(inline=False,name= '트랙 정보', value= '파일에 들어 있는 트랙의 개수, 사용하는 채널, 악기, 음표 개수, 서스테인 유무 등을 확인할 수 있습니다. 합주악보를 만드실 때 자세하게 살펴보시길 추천드립니다.')
					exampleEmbed.add_field(inline=False,name= '텍스트 이벤트', value= 'mid 파일의 제작자가 적어 놓은 메시지들입니다.')
					exampleEmbed.add_field(inline=False,name= '저작권 정보', value= 'mid 파일에 기록되어 있는 저작권 정보입니다.')
					await message.channel.send(embed=exampleEmbed)
				elif args[0] == '악보':
					exampleEmbed = discord.Embed(color=0x8cffa9,title='SayangBot 명령어 (' + prefix + args[0] + ')',url='https://github.com/Eggrice62/SayangBot',description='미디 파일에 들어있는 모든 음표를 악기 구분 없이 출력합니다. 독주용 악보를 만들거나, 글자수 문제로 2~3인 합주로 나눠서 만들 때 사용 가능합니다.\n악보 명령어에서 사용 가능한 옵션은 다음과 같습니다.\nex) ' + prefix + '악보 <옵션1> <옵션2> ... <옵션n>')
					exampleEmbed.add_field(inline=False,name= '셋잇단', value= '셋잇단음표가 많은 곡 (특히 스윙) 에 맞게 템포를 바꾸어 처리합니다.\nex) ' + prefix + '악보 셋잇단')
					exampleEmbed.add_field(inline=False,name= '1만자무시', value= '1만 자 이상의 악보를 길이 분할 없이 출력합니다.\nex) ' + prefix + '악보 1만자무시')
					exampleEmbed.add_field(inline=False,name= '화음분할', value= '악보의 화음을 분할하여 합주용 악보들로 만듭니다.\nex) ' + prefix + '악보 화음분할')
					exampleEmbed.add_field(inline=False,name= '악기', value= 'mid 파일에 기록된 음표 중 특정 악기의 음표만 출력합니다.\nex) ' + prefix + '악보 악기 프렛리스베이스기타')
					exampleEmbed.add_field(inline=False,name= '서스테인', value= '서스테인 페달 on/off 스위치입니다.\n기본값은 on입니다.\nex) ' + prefix + '악보 서스테인 on : 서스테인도 악보에 출력합니다.\n서스테인 이벤트를 주기적으로 초기화하도록 올바르게 구성하지 않으면 MIDI 동시발음수를 초과하여 연주 소리가 들리지 않을 수 있습니다.\n서스테인에 적합하지 않은 악기 (일렉기타, 첼로 등) 를 사용할 경우 주변 유저들에게 귀갱으로 인해 차단당할 수 있습니다.')
					exampleEmbed.add_field(inline=False,name= '모션', value= '자동 모션 on/off 스위치입니다. 연주를 일정 시간 멈출 경우 캐릭터가 모션을 하지 않고 정지합니다.\n기본값은 off입니다.\nex) ' + prefix + '악보 모션 on : 자동 모션 코드를 악보에 출력합니다.')
					exampleEmbed.add_field(inline=False,name= '해상도', value= '악보의 품질을 조정합니다.\n0~6 중 입력하는 숫자가 높을수록 듣기에 자연스러워지지만 악보가 길어집니다.\n기본값은 6입니다.\nex) ' + prefix + '악보 해상도 4 : 기본 설정으로 했을 때보다 짧아진 악보가 나옵니다.')
					exampleEmbed.add_field(inline=False,name= '템포뭉개기', value= '템포 조정 이벤트의 개수를 조절합니다.\n0~6 중 입력하는 숫자가 낮을수록 듣기에 자연스러워지지만 악보가 길어집니다.\n기본값은 0입니다.\nex) ' + prefix + '악보 템포뭉개기 3 : 기본 설정으로 했을 때보다 짧아진 악보가 나옵니다.')
					exampleEmbed.add_field(inline=False,name= '서스테인뭉개기', value= '서스테인 페달 이벤트의 개수를 조절합니다.\n0~6 중 입력하는 숫자가 낮을수록 듣기에 자연스러워지지만 악보가 길어집니다.\n기본값은 0입니다.\nex) ' + prefix + '악보 서스테인뭉개기 3 : 기본 설정으로 했을 때보다 짧아진 악보가 나옵니다.')
					exampleEmbed.add_field(inline=False,name= '볼륨증가', value= '볼륨을 V15 방향으로 높입니다.\nex) ' + prefix + '악보 볼륨증가')
					exampleEmbed.add_field(inline=False,name= '볼륨감소', value= '볼륨을 V0 방향으로 낮춥니다.\nex) ' + prefix + '악보 볼륨감소')
					exampleEmbed.add_field(inline=False,name= '고정볼륨', value= 'mid 파일에 기록된 볼륨을 무시하고 입력한 볼륨으로 고정합니다.\nex) ' + prefix + '악보 고정볼륨 14\n단위는 메이플스토리의 V코드 (V값 1당 미디 벨로시티 8) 로 입력합니다.')
					exampleEmbed.add_field(inline=False,name= '볼륨1/볼륨2/볼륨3', value= '볼륨을 직접 자세하게 조정합니다.\nex) ' + prefix + '악보 볼륨1 3 볼륨2 0.5 볼륨3 -5 : 볼륨을 3만큼 높이고, 0.5를 곱하고, 5만큼 낮춥니다.\n단위는 메이플스토리의 V코드 (V값 1당 미디 벨로시티 8) 로 입력합니다.')
					exampleEmbed.add_field(inline=False,name= '고정템포', value= 'mid 파일에 기록된 템포를 무시하고 입력한 템포로 고정합니다.\nex) ' + prefix + '악보 고정템포 180')
					exampleEmbed.add_field(inline=False,name= '드럼제외', value= '드럼 코드 (채널 10 등) 의 음표는 제외하고 출력합니다.\nex) ' + prefix + '악보 드럼제외')
					exampleEmbed.add_field(inline=False,name= '드럼만', value= '드럼 코드 (채널 10 등) 의 음표만 선택하여 출력합니다.\nex) ' + prefix + '악보 드럼만')
					exampleEmbed.add_field(inline=False,name= '트랙', value= '지정한 트랙을 출력 범위에 추가합니다.\nex) ' + prefix + '악보 트랙 3 트랙 5 : 3번 트랙, 5번 트랙의 음표만 악보에 출력합니다.')
					exampleEmbed.add_field(inline=False,name= '채널', value= '지정한 채널을 출력 범위에 추가합니다.\nex) ' + prefix + '악보 채널 7 채널 9 : 7번 채널, 9번 채널의 음표만 악보에 출력합니다.')
					exampleEmbed.add_field(inline=False,name= '트랙채널', value= '지정한 트랙-채널을 출력 범위에 추가합니다.\nex) ' + prefix + '악보 트랙채널 3 1 트랙채널 2 9 : 3번 트랙 중 1번 채널 음표, 2번 트랙 중 9번 채널 음표만 악보에 출력합니다.')
					# exampleEmbed.add_field(inline=False,name= '시작', value= '지정한 시간 이후의 음표만 출력합니다.\n단위는 미디 틱(tick)이므로 초보자가 사용하기 어려울 수 있습니다.\nex) ' + prefix + '악보 시작 4800 : 4800틱 이후의 음표만 출력합니다.')
					# exampleEmbed.add_field(inline=False,name= '종료', value= '지정한 시간 이전의 음표만 출력합니다.\n단위는 미디 틱(tick)이므로 초보자가 사용하기 어려울 수 있습니다.\nex) ' + prefix + '악보 종료 24000 : 24000틱 이전의 음표만 출력합니다.')
					exampleEmbed.add_field(inline=False,name= '최저음', value= '지정한 음높이 이상의 음표만 출력합니다.\n단위는 가온다(C4)를 60으로 하여 반음 당 1씩 계산하여 입력합니다.\nex) ' + prefix + '악보 최저음 35 : B1 이상의 음높이만 출력합니다.')
					exampleEmbed.add_field(inline=False,name= '최고음', value= '지정한 음높이 이하의 음표만 출력합니다.\n단위는 가온다(C4)를 60으로 하여 반음 당 1씩 계산하여 입력합니다.\nex) ' + prefix + '악보 최고음 68 : G+4 이하의 음높이만 출력합니다.')
					# exampleEmbed.add_field(inline=False,name= '음표깎기', value= '살짝만 겹쳐서 화음 개수를 늘리는 음표가 있을 경우 음표 길이를 조금 줄여서 화음 개수를 줄입니다.\n범위는 0 (깎지 않음) ~ 100 (모두 깎음) 중 입력할 수 있습니다.\nex) ' + prefix + '악보 음표깎기 10 : 음표 길이의 10%까지 깎습니다.')
					exampleEmbed.add_field(inline=False,name= 'mml', value= 'ms2mml이 아닌 mml 파일로 출력합니다.\nex) ' + prefix + '악보 mml')
					await message.channel.send(embed=exampleEmbed)
				elif args[0] == '합주악보':
					exampleEmbed = discord.Embed(color=0x8cffa9,title='SayangBot 명령어 (' + prefix + args[0] + ')',url='https://github.com/Eggrice62/SayangBot',description='미디 파일로부터 합주 악보를 자동 생성합니다.\n합주악보 명령어에서 사용 가능한 옵션은 다음과 같습니다.\nex) ' + prefix + '악보 <옵션1> <옵션2> ... <옵션n>')
					exampleEmbed.add_field(inline=False,name= '셋잇단', value= '셋잇단음표가 많은 곡 (특히 스윙) 에 맞게 템포를 바꾸어 처리합니다.\nex) ' + prefix + '합주악보 셋잇단')
					exampleEmbed.add_field(inline=False,name= '서스테인', value= '서스테인 페달 on/off 스위치입니다.\n기본값은 on입니다.\nex) ' + prefix + '합주악보 서스테인 on : 서스테인도 악보에 출력합니다.\n서스테인 이벤트를 주기적으로 초기화하도록 올바르게 구성하지 않으면 MIDI 동시발음수를 초과하여 연주 소리가 들리지 않을 수 있습니다.\n서스테인에 적합하지 않은 악기 (일렉기타, 첼로 등) 를 사용할 경우 주변 유저들에게 귀갱으로 인해 차단당할 수 있습니다.')
					exampleEmbed.add_field(inline=False,name= '모션', value= '자동 모션 on/off 스위치입니다. 연주를 일정 시간 멈출 경우 캐릭터가 모션을 하지 않고 정지합니다.\n기본값은 off입니다.\nex) ' + prefix + '합주악보 모션 on : 자동 모션 코드를 악보에 출력합니다.')
					exampleEmbed.add_field(inline=False,name= '해상도', value= '악보의 품질을 조정합니다.\n0~6 중 입력하는 숫자가 높을수록 듣기에 자연스러워지지만 악보가 길어집니다.\n기본값은 6입니다.\nex) ' + prefix + '합주악보 해상도 4 : 기본 설정으로 했을 때보다 짧아진 악보가 나옵니다.')
					exampleEmbed.add_field(inline=False,name= '템포뭉개기', value= '템포 조정 이벤트의 개수를 조절합니다.\n0~6 중 입력하는 숫자가 낮을수록 듣기에 자연스러워지지만 악보가 길어집니다.\n기본값은 0입니다.\nex) ' + prefix + '합주악보 템포뭉개기 3 : 기본 설정으로 했을 때보다 짧아진 악보가 나옵니다.')
					exampleEmbed.add_field(inline=False,name= '서스테인뭉개기', value= '서스테인 페달 이벤트의 개수를 조절합니다.\n0~6 중 입력하는 숫자가 낮을수록 듣기에 자연스러워지지만 악보가 길어집니다.\n기본값은 0입니다.\nex) ' + prefix + '합주악보 서스테인뭉개기 3 : 기본 설정으로 했을 때보다 짧아진 악보가 나옵니다.')
					exampleEmbed.add_field(inline=False,name= '볼륨증가', value= '볼륨을 V15 방향으로 높입니다.\nex) ' + prefix + '합주악보 볼륨증가')
					exampleEmbed.add_field(inline=False,name= '볼륨감소', value= '볼륨을 V0 방향으로 낮춥니다.\nex) ' + prefix + '합주악보 볼륨감소')
					exampleEmbed.add_field(inline=False,name= '고정볼륨', value= 'mid 파일에 기록된 볼륨을 무시하고 입력한 볼륨으로 고정합니다.\nex) ' + prefix + '합주악보 고정볼륨 14\n단위는 메이플스토리의 V코드 (V값 1당 미디 벨로시티 8) 로 입력합니다.')
					exampleEmbed.add_field(inline=False,name= '볼륨1/볼륨2/볼륨3', value= '볼륨을 직접 자세하게 조정합니다.\nex) ' + prefix + '합주악보 볼륨1 3 볼륨2 0.5 볼륨3 -5 : 볼륨을 3만큼 높이고, 0.5를 곱하고, 5만큼 낮춥니다.\n단위는 메이플스토리의 V코드 (V값 1당 미디 벨로시티 8) 로 입력합니다.')
					exampleEmbed.add_field(inline=False,name= '고정템포', value= 'mid 파일에 기록된 템포를 무시하고 입력한 템포로 고정합니다.\nex) ' + prefix + '합주악보 고정템포 180')
					# exampleEmbed.add_field(inline=False,name= '시작', value= '지정한 시간 이후의 음표만 출력합니다.\n단위는 미디 틱(tick)이므로 초보자가 사용하기 어려울 수 있습니다.\nex) ' + prefix + '합주악보 시작 4800 : 4800틱 이후의 음표만 출력합니다.')
					# exampleEmbed.add_field(inline=False,name= '종료', value= '지정한 시간 이전의 음표만 출력합니다.\n단위는 미디 틱(tick)이므로 초보자가 사용하기 어려울 수 있습니다.\nex) ' + prefix + '합주악보 종료 24000 : 24000틱 이전의 음표만 출력합니다.')
					exampleEmbed.add_field(inline=False,name= '최저음', value= '지정한 음높이 이상의 음표만 출력합니다.\n단위는 가온다(C4)를 60으로 하여 반음 당 1씩 계산하여 입력합니다.\nex) ' + prefix + '합주악보 최저음 35 : B1 이상의 음높이만 출력합니다.')
					exampleEmbed.add_field(inline=False,name= '최고음', value= '지정한 음높이 이하의 음표만 출력합니다.\n단위는 가온다(C4)를 60으로 하여 반음 당 1씩 계산하여 입력합니다.\nex) ' + prefix + '합주악보 최고음 68 : G+4 이하의 음높이만 출력합니다.')
					# exampleEmbed.add_field(inline=False,name= '음표깎기', value= '살짝만 겹쳐서 화음 개수를 늘리는 음표가 있을 경우 음표 길이를 조금 줄여서 화음 개수를 줄입니다.\n범위는 0 (깎지 않음) ~ 100 (모두 깎음) 중 입력할 수 있습니다.\nex) ' + prefix + '합주악보 음표깎기 10 : 음표 길이의 10%까지 깎습니다.')
					exampleEmbed.add_field(inline=False,name= 'mml', value= 'ms2mml이 아닌 mml 파일로 출력합니다.\nex) ' + prefix + '합주악보 mml')
					await message.channel.send(embed=exampleEmbed)
				elif args[0] == '팁':
					exampleEmbed = discord.Embed(color=0x8cffa9,title='SayangBot 명령어 (' + prefix + args[0] + ')',url='https://github.com/Eggrice62/SayangBot',description='출력된 악보에 문제가 있을 때 시도해볼 수 있는 해결책입니다.')
					exampleEmbed.add_field(inline=False,name= '글자 수 줄이기', value= '' + prefix + '악보 해상도 5 등으로 해상도를 낮추어 보세요. 해상도 옵션을 입력하지 않은 경우 기본값 6으로 적용되고 있으며, 낮게 입력할수록 글자수가 줄어들고 악보의 품질이 낮아집니다.\n' + prefix + '악보 템포뭉개기 2 등으로 템포뭉개기 기능을 사용해 보세요. 템포뭉개기 옵션을 입력하지 않은 경우 기본값 0으로 적용되고 있으며, 높게 입력할수록 글자수가 줄어들고 변속의 개수가 적어집니다.')
					exampleEmbed.add_field(inline=False,name= '악보에 이상한 음이?', value= '' + prefix + '악보 명령어로 독주악보를 만들 때 드럼제외 옵션을 입력하지 않으면 화음에 맞지 않는 드럼코드 음이 섞여서 의도하지 않은 소리가 날 수 있습니다. ' + prefix + '악보 드럼제외 와 같이 사용해 보세요.')
					exampleEmbed.add_field(inline=False,name= '합주 볼륨 개별 조절', value= '' + prefix + '합주악보 명령어 사용 시에는 모든 악기가 별다른 볼륨 수정 없이 파일 내용대로 출력됩니다. 특정 악기에만 볼륨 설정을 적용하여 개별 출력하려면 ' + prefix + '악보 <원하는 악기> 볼륨증가/볼륨감소/고정볼륨 등의 옵션을 적옹해 보세요. 볼륨을 더 자세히 조정하려면 ' + prefix + '도움말 악보 에서 볼륨1/볼륨2/볼륨3 기능을 참고해 보세요.')
					exampleEmbed.add_field(inline=False,name= '봇이 응답하지 않을 때', value= '' + prefix + '악보 변환은 미디 파일의 크기에 따라 1분까지도 소요될 수 있습니다. 만약 변환 명령어에 1분 이상 응답이 없거나, 미디 파일을 보내도 반응이 없는 경우, 봇 자체가 죽은 것입니다. 이때는 개발자가 수 시간 내로 오류를 수정한 뒤 봇을 다시 시작할 것입니다. 9/24 이후로 꾸준히 패치 중입니다. 조금만 기다려 주세요...')
					exampleEmbed.add_field(inline=False,name= '파일이 열리지 않음 & 싱크가 어긋남', value= '열리지 않는 미디 파일에 대해서는 다른 프로그램 (3mle, Musescore 등) 에서의 문제 여부를 확인하신 뒤 ' + prefix + '피드백 명령어로 제보해주시길 부탁드립니다.')
					exampleEmbed.add_field(inline=False,name= '버그 제보', value= '' + prefix + '피드백 으로 시작하는 메시지는 모두 개발자에게 자동으로 전달됩니다. 자유롭게 적어서 보내주시면 개발에 큰 도움이 됩니다. 감사합니다.')
					await message.channel.send(embed=exampleEmbed)
				elif args[0] == '피드백':
					exampleEmbed = discord.Embed(color=0x8cffa9,title='SayangBot 명령어 (' + prefix + args[0] + ')',url='https://github.com/Eggrice62/SayangBot',description='' + prefix + '피드백 으로 시작하는 메시지는 모두 개발자에게 자동으로 전달됩니다. 자유롭게 적어서 보내주시면 개발에 큰 도움이 됩니다. 감사합니다.')
					await message.channel.send(embed=exampleEmbed)
				else:
					await sendError(message.channel, '' + args[0] + (' 은' if checkBatchimEnding(args[0]) else '는') + ' 없는 명령어입니다. 명령어 목록은 ' + prefix + '도움말 을 참고해 주세요.')
		return
	if command == '팁':
		exampleEmbed = discord.Embed(color=0x8cffa9,title='팁',url='https://github.com/Eggrice62/SayangBot',description='출력된 악보에 문제가 있을 때 시도해볼 수 있는 해결책입니다.')
		exampleEmbed.add_field(inline=False,name= '글자 수 줄이기', value= '' + prefix + '악보 해상도 5 등으로 해상도를 낮추어 보세요. 해상도 옵션을 입력하지 않은 경우 기본값 6으로 적용되고 있으며, 낮게 입력할수록 글자수가 줄어들고 악보의 품질이 낮아집니다.\n' + prefix + '악보 템포뭉개기 2 등으로 템포뭉개기 기능을 사용해 보세요. 템포뭉개기 옵션을 입력하지 않은 경우 기본값 0으로 적용되고 있으며, 높게 입력할수록 글자수가 줄어들고 변속의 개수가 적어집니다.')
		exampleEmbed.add_field(inline=False,name= '합주 볼륨 개별 조절', value= '' + prefix + '합주악보 명령어 사용 시에는 모든 악기가 별다른 볼륨 수정 없이 파일 내용대로 출력됩니다. 특정 악기에만 볼륨 설정을 적용하여 개별 출력하려면 ' + prefix + '악보 <원하는 악기> 볼륨증가/볼륨감소/고정볼륨 등의 옵션을 적옹해 보세요. 볼륨을 더 자세히 조정하려면 ' + prefix + '도움말 악보 에서 볼륨1/볼륨2/볼륨3 기능을 참고해 보세요.')
		exampleEmbed.add_field(inline=False,name= '악보에 이상한 음이?', value= '' + prefix + '악보 명령어로 독주악보를 만들 때 드럼제외 옵션을 입력하지 않으면 화음에 맞지 않는 드럼코드 음이 섞여서 의도하지 않은 소리가 날 수 있습니다. ' + prefix + '악보 드럼제외 와 같이 사용해 보세요.')
		exampleEmbed.add_field(inline=False,name= '봇이 응답하지 않을 때', value= '' + prefix + '악보 변환은 미디 파일의 크기에 따라 1분까지도 소요될 수 있습니다. 만약 변환 명령어에 1분 이상 응답이 없거나, 미디 파일을 보내도 반응이 없는 경우, 봇 자체가 죽은 것입니다. 이때는 개발자가 수 시간 내로 오류를 수정한 뒤 봇을 다시 시작할 것입니다. 9/24 이후로 꾸준히 패치 중입니다. 조금만 기다려 주세요...')
		exampleEmbed.add_field(inline=False,name= '파일이 열리지 않음 & 싱크가 어긋남', value= '열리지 않는 미디 파일에 대해서는 다른 프로그램 (3mle, Musescore 등) 에서의 문제 여부를 확인하신 뒤 ' + prefix + '피드백 명령어로 제보해주시길 부탁드립니다.')
		exampleEmbed.add_field(inline=False,name= '버그 제보', value= '' + prefix + '피드백 으로 시작하는 메시지는 모두 개발자에게 자동으로 전달됩니다. 자유롭게 적어서 보내주시면 개발에 큰 도움이 됩니다. 감사합니다.')
		await message.channel.send(embed=exampleEmbed)
		return
	
	if command == 'help':
		if len(args) == 0:
			exampleEmbed = discord.Embed(color=0x8cffa9,title='SayangBot',url='https://github.com/Eggrice62/SayangBot',description='한국어 도움말은 ' + prefix + '도움말 을 참고하십시오.\nThis bot converts MIDI files (*.mid) to ms2mml.\nIf you upload a MIDI file to this chat room, SayangBot will automatically recognize it and operate it.\nIf you are reluctant to publish the MIDI file, it will also work in private message (DM).\nBut, the mid file you send is saved on my computer for processing.\nCheck details with'+prefix+'help <command>\nex) ' + prefix + 'help solo')
			exampleEmbed.add_field(inline=False,name= prefix+'info', value= 'General information about MIDI files')
			exampleEmbed.add_field(inline=False,name= prefix+'solo', value= 'Convert all notes to 1 solo sheet')
			exampleEmbed.add_field(inline=False,name= prefix+'ensemble', value= 'Converted to sheet for each instrument for ensemble')
			exampleEmbed.add_field(inline=False,name= prefix+'tip', value= 'If there is a problem with the converted sheet music, please refer to it!')
			exampleEmbed.add_field(inline=False,name= prefix+'feedback', value= 'Any messages that start with ' + prefix + 'Feedback are automatically forwarded to the developer. Feel write it down and send it. it will be a great help for development. Thank you.')
			await message.channel.send(embed=exampleEmbed)
		else:
			if args[0] == 'info':
				exampleEmbed = discord.Embed(color=0x8cffa9,title='SayangBot command (' + prefix + args[0] + ')',url='https://github.com/Eggrice62/SayangBot',description='You can check the overall information of the MIDI file.')
				exampleEmbed.add_field(inline=False,name= '파일 이름', value= 'This is the mid file currently in use. The file will remain in effect until a new file is uploaded or SayangBot reboots. Files in use in a DM are not affected by other people"s DMs or files used by the server.')
				exampleEmbed.add_field(inline=False,name= 'BPM', value= 'You can roughly check the tempo of the song. The available tempo chord range in MapleStory 2 is 32~255, but SayangBot can handle 8~510 automatically.')
				exampleEmbed.add_field(inline=False,name= '박자', value= 'You can check the time signatures of a song. This is actually useless information.')
				exampleEmbed.add_field(inline=False,name= '재생 시간', value= 'You can check the playback time of a song in hours/minutes/seconds. The playing time of the printed sheet music may be slightly different depending on the converted tempo chord.')
				exampleEmbed.add_field(inline=False,name= '곡 전체 틱 / 4분음표 틱', value= 'Yes it is.')
				exampleEmbed.add_field(inline=False,name= '트랙 정보', value= 'You can check the number of tracks in the file, the channels used, the instruments, the number of notes, and whether or not sustain event is present. Take a closer look when making ensemble sheet music.')
				exampleEmbed.add_field(inline=False,name= '텍스트 이벤트', value= 'The messages written by the creator of the MIDI file.')
				exampleEmbed.add_field(inline=False,name= '저작권 정보', value= 'Copyright information recorded in the mid file.')
				await message.channel.send(embed=exampleEmbed)
			elif args[0] == 'solo':
				exampleEmbed = discord.Embed(color=0x8cffa9,title='SayangBot command (' + prefix + args[0] + ')',url='https://github.com/Eggrice62/SayangBot',description='All notes in a MIDI file are output regardless of instrument. It can be used to make sheet music for solo use, or to divide it into two or three-person ensembles due to the number of characters.\nThe options available in the solo command are below.\nex) ' + prefix + 'solo <Option 1> <Option 2> ... <Option n>')
				exampleEmbed.add_field(inline=False,name= 'triplet', value= 'For songs with many triplets (especially swing), change the tempo to handle it.\nex) ' + prefix + 'solo triplet')
				exampleEmbed.add_field(inline=False,name= 'ignore10000', value= 'It outputs sheet music with more than 10,000 characters without length division.\nex) ' + prefix + 'solo ignore10000')
				exampleEmbed.add_field(inline=False,name= 'chorddivision', value= 'Divide the chords of the sheet music to make sheet music for ensemble.\nex) ' + prefix + 'solo chorddivision')
				exampleEmbed.add_field(inline=False,name= 'instrument', value= 'Among the notes recorded in the mid file, only the notes of a specific instrument are output.\nex) ' + prefix + 'solo instrument fretlessbassguitar')
				exampleEmbed.add_field(inline=False,name= 'sustain', value= 'This is a sustain pedal on/off switch.\nThe default is on.\nex) ' + prefix + 'solo sustain on : Outputs sustain to the score as well.\nIf not configured correctly to initialize sustain events periodically, MIDI polyphony If the number is exceeded, some sound may not be heard.\nIf you use an instrument that is not suitable for sustain (electric guitar, cello, etc.), users around you may be block you.')
				exampleEmbed.add_field(inline=False,name= 'motion', value= 'Automatic motion on/off switch. If the performance is stopped for a certain amount of time, the character will stop motion.\nThe default is off.\nex) ' + prefix + 'solo motion on : Automatic motion code is output to the score.')
				exampleEmbed.add_field(inline=False,name= 'resolution', value= 'Adjusts the quality of the score.\nThe higher the number from 0~6, the more natural it sounds, but the longer the score.\nThe default is 6.\nex) ' + prefix + 'solo resolution 4 : The sheet music will be shorter than default.')
				exampleEmbed.add_field(inline=False,name= 'mergetempo', value= 'Adjusts the number of tempo events.\nThe lower the number from 0 to 6, the more natural it sounds, but the longer the score.\nThe default is 0.\nex) ' + prefix + 'solo mergetempo 3 : The sheet music will be shorter than default.')
				exampleEmbed.add_field(inline=False,name= 'mergesustain', value= 'Adjusts the number of sustain pedal events.\nThe lower the number from 0 to 6, the more natural it sounds, but the longer the score.\nThe default is 0.\nex) ' + prefix + 'solo mergesustain 3 : The sheet music is shorter than the default setting.')
				exampleEmbed.add_field(inline=False,name= 'volumeincrease', value= 'Increase the volume towards V15.\nex) ' + prefix + 'solo volumeincrease')
				exampleEmbed.add_field(inline=False,name= 'volumedecrease', value= 'Decrease the volume towards V0.\nex) ' + prefix + 'solo volumedecrease')
				exampleEmbed.add_field(inline=False,name= 'fixedvolume', value= 'Ignores the volume recorded in the mid file and fixes it to the entered volume.\nex) ' + prefix + 'solo fixedvolume 14\nThe unit is MapleStory"s V chord (8 MIDI velocity per 1 V value).')
				exampleEmbed.add_field(inline=False,name= 'volume1/volume2/volume3', value= 'Adjust the volume in custom way.\nex) ' + prefix + 'solo volume1 3 volume2 0.5 volume3 -5 : Increase the volume by 3, multiply by 0.5, and decrease it by 5.\nThe unit is MapleStory"s unit. Input as V chord (8 MIDI velocity per 1 V value).')
				exampleEmbed.add_field(inline=False,name= 'fixedtempo', value= 'The tempo recorded in the mid file is ignored and fixed at the entered tempo.\nex) ' + prefix + 'solo fixedtempo 180')
				exampleEmbed.add_field(inline=False,name= 'drumexclude', value= 'The notes of the drum chord (channel 10, etc.) are excluded.\nex) ' + prefix + 'solo drumexclude')
				exampleEmbed.add_field(inline=False,name= 'drumonly', value= 'Only the notes of the drum chord (channel 10, etc.) are selected and output.\nex) ' + prefix + 'solo drumonly')
				exampleEmbed.add_field(inline=False,name= 'track', value= 'Adds the specified track to the output range.\nex) ' + prefix + 'solo track 3 track 5: Only the notes of Track 3 and Track 5 are output to the score.')
				exampleEmbed.add_field(inline=False,name= 'channel', value= 'Adds the specified channel to the output range.\nex) ' + prefix + 'solo channel 7 channel 9 : Only the notes of channels 7 and 9 are output to the score.')
				exampleEmbed.add_field(inline=False,name= 'trackchannel', value= 'Adds the specified track-channel to the output range.\nex) ' + prefix + 'solo trackchannel 3 1 trackchannel 2 9 : Only the note of channel 1 of track 3 and the note of channel 9 of track 2 are output to the score.')
				exampleEmbed.add_field(inline=False,name= 'start', value= 'Only notes after the specified time are output.\nThe unit is MIDI ticks, so it may be difficult for beginners to use.\nex) ' + prefix + 'solo start 4800 : Only notes after 4800 ticks are output.')
				exampleEmbed.add_field(inline=False,name= 'end', value= 'Only notes before the specified time are output.\nThe unit is MIDI ticks, so it may be difficult for beginners to use.\nex) ' + prefix + 'solo end 24000 : Only the notes before 24000 ticks are output.')
				exampleEmbed.add_field(inline=False,name= 'lowest', value= 'Only notes higher than the specified pitch are output.\nThe unit is calculated by calculating 1 per semitone with the C4 being 60.\nex) ' + prefix + 'solo lowest 35 : Only pitches above B1 are output.')
				exampleEmbed.add_field(inline=False,name= 'highest', value= 'Only notes below than the specified pitch are output.\nThe unit is calculated by calculating 1 per semitone with the C4 being 60.\nex) ' + prefix + 'solo highest 68 : Only pitches below G+4 are output.')
				exampleEmbed.add_field(inline=False,name= 'cutoff', value= 'If there are notes that slightly overlap so that increase the number of chords, shorten the length of the notes slightly to reduce the number of chords.\nThe range is 0 (uncut) to 100 (all cut).\nex) ' + prefix + 'solo cutoff 10 : Cuts up to 10% of the note length.')
				exampleEmbed.add_field(inline=False,name= 'mml', value= 'Output to mml file, not ms2mml.\nex) ' + prefix + 'solo mml')
				await message.channel.send(embed=exampleEmbed)
			elif args[0] == 'ensemble':
				exampleEmbed = discord.Embed(color=0x8cffa9,title='SayangBot command (' + prefix + args[0] + ')',url='https://github.com/Eggrice62/SayangBot',description='Automatically create ensemble scores from MIDI files.\nThe options available in the ensemble command are below.\nex) ' + prefix + 'ensemble <Option 1> <Option 2> ... <Option n>')
				exampleEmbed.add_field(inline=False,name= 'triplet', value= 'For songs with many triplets (especially swing), change the tempo to handle it.\nex) ' + prefix + 'ensemble triplet')
				exampleEmbed.add_field(inline=False,name= 'sustain', value= 'This is a sustain pedal on/off switch.\nThe default is on.\nex) ' + prefix + 'ensemble sustain on : Outputs sustain to the score as well.\nIf not configured correctly to initialize sustain events periodically, MIDI polyphony If the number is exceeded, some sound may not be heard.\nIf you use an instrument that is not suitable for sustain (electric guitar, cello, etc.), users around you may be block you.')
				exampleEmbed.add_field(inline=False,name= 'motion', value= 'Automatic motion on/off switch. If the performance is stopped for a certain amount of time, the character will stop motion.\nThe default is off.\nex) ' + prefix + 'ensemble motion on : Automatic motion code is output to the score.')
				exampleEmbed.add_field(inline=False,name= 'resolution', value= 'Adjusts the quality of the score.\nThe higher the number from 0~6, the more natural it sounds, but the longer the score.\nThe default is 6.\nex) ' + prefix + 'ensemble resolution 4 : The sheet music will be shorter than default.')
				exampleEmbed.add_field(inline=False,name= 'mergetempo', value= 'Adjusts the number of tempo events.\nThe lower the number from 0 to 6, the more natural it sounds, but the longer the score.\nThe default is 0.\nex) ' + prefix + 'ensemble mergetempo 3 : The sheet music will be shorter than default.')
				exampleEmbed.add_field(inline=False,name= 'mergesustain', value= 'Adjusts the number of sustain pedal events.\nThe lower the number from 0 to 6, the more natural it sounds, but the longer the score.\nThe default is 0.\nex) ' + prefix + 'ensemble mergesustain 3 : The sheet music is shorter than the default setting.')
				exampleEmbed.add_field(inline=False,name= 'volumeincrease', value= 'Increase the volume towards V15.\nex) ' + prefix + 'ensemble volumeincrease')
				exampleEmbed.add_field(inline=False,name= 'volumedecrease', value= 'Decrease the volume towards V0.\nex) ' + prefix + 'ensemble volumedecrease')
				exampleEmbed.add_field(inline=False,name= 'fixedvolume', value= 'Ignores the volume recorded in the mid file and fixes it to the entered volume.\nex) ' + prefix + 'ensemble fixedvolume 14\nThe unit is MapleStory"s V chord (8 MIDI velocity per 1 V value).')
				exampleEmbed.add_field(inline=False,name= 'volume1/volume2/volume3', value= 'Adjust the volume in custom way.\nex) ' + prefix + 'ensemble volume1 3 volume2 0.5 volume3 -5 : Increase the volume by 3, multiply by 0.5, and decrease it by 5.\nThe unit is MapleStory"s unit. Input as V chord (8 MIDI velocity per 1 V value).')
				exampleEmbed.add_field(inline=False,name= 'fixedtempo', value= 'The tempo recorded in the mid file is ignored and fixed at the entered tempo.\nex) ' + prefix + 'ensemble fixedtempo 180')
				exampleEmbed.add_field(inline=False,name= 'start', value= 'Only notes after the specified time are output.\nThe unit is MIDI ticks, so it may be difficult for beginners to use.\nex) ' + prefix + 'ensemble start 4800 : Only notes after 4800 ticks are output.')
				exampleEmbed.add_field(inline=False,name= 'end', value= 'Only notes before the specified time are output.\nThe unit is MIDI ticks, so it may be difficult for beginners to use.\nex) ' + prefix + 'ensemble end 24000 : Only the notes before 24000 ticks are output.')
				exampleEmbed.add_field(inline=False,name= 'lowest', value= 'Only notes higher than the specified pitch are output.\nThe unit is calculated by calculating 1 per semitone with the C4 being 60.\nex) ' + prefix + 'ensemble lowest 35 : Only pitches above B1 are output.')
				exampleEmbed.add_field(inline=False,name= 'highest', value= 'Only notes below than the specified pitch are output.\nThe unit is calculated by calculating 1 per semitone with the C4 being 60.\nex) ' + prefix + 'ensemble highest 68 : Only pitches below G+4 are output.')
				exampleEmbed.add_field(inline=False,name= 'cutoff', value= 'If there are notes that slightly overlap so that increase the number of chords, shorten the length of the notes slightly to reduce the number of chords.\nThe range is 0 (uncut) to 100 (all cut).\nex) ' + prefix + 'ensemble cutoff 10 : Cuts up to 10% of the note length.')
				exampleEmbed.add_field(inline=False,name= 'mml', value= 'Output to mml file, not ms2mml.\nex) ' + prefix + 'ensemble mml')
				await message.channel.send(embed=exampleEmbed)
			elif args[0] == 'tip':
				exampleEmbed = discord.Embed(color=0x8cffa9,title='Tip',url='https://github.com/Eggrice62/SayangBot',description='This is general solution you can try when there is a problem with the printed sheet music.')
				exampleEmbed.add_field(inline=False,name= 'Reducing the number of characters', value= '' + prefix + 'ensemble resolution 5 Try lowering the resolution, etc. If no resolution option is entered, the default value of 6 is applied. The lower the input, the fewer characters and the lower the quality of the score.\n' + prefix + 'ensemble mergetempo 2 Try using the mergetempo option, etc. If the mergetempo option is not entered, the default value of 0 is applied, and the higher the input, the fewer characters.')
				exampleEmbed.add_field(inline=False,name= 'Individual volume control in ensemble', value= '' + prefix + 'When using the ensemble sheet music command, all instruments are output as the file contents without changing the volume. If you want to apply the volume setting to only a specific instrument and output it individually, try adding options such as ' + prefix + 'ensemble <desired instrument> volumeincrease/volumedecrease/fixedvolume. If you want to adjust the volume in more detail, refer to the ' + prefix + 'help solo for the detailed function.')
				exampleEmbed.add_field(inline=False,name= 'Strange notes in the sheet music?', value= '' + prefix + 'If you do not enter the option to exclude drums when creating solo score with the solo command, drum chord notes that do not match the chord may be mixed, resulting in an unintended sound. Try ' + prefix + 'ensemble drumexclude')
				exampleEmbed.add_field(inline=False,name= 'When the bot is not responding', value= '' + prefix + 'Score conversion may take up to a minute, depending on the size of the MIDI file. If there is no response to the conversion command for more than 1 minute, or if there is no response even after sending a MIDI file, the bot itself is dead. In this case, the developer will fix the error within a few hours and restart the bot. It"s been patching steadily since 9/24. wait please...')
				exampleEmbed.add_field(inline=False,name= 'File won"t open & out of sync', value= 'I have reviewed this bug for a while, but as of now, I have only confirmed that the MIDI file with the problem does not open with other programs or opens strangely. If you have time, it would be of great help if you provide additional information with the ' + prefix + 'feedback <content> command. I"m curious as to which program the mid file was made with...')
				exampleEmbed.add_field(inline=False,name= 'Bug report', value= 'Any messages that start with ' + prefix + 'feedback are automatically forwarded to the developer. Feel write it down and send it. it will be a great help for development. Thank you.')
				await message.channel.send(embed=exampleEmbed)
			elif args[0] == 'feedback':
				exampleEmbed = discord.Embed(color=0x8cffa9,title='SayangBot command (' + prefix + args[0] + ')',url='https://github.com/Eggrice62/SayangBot',description='Any messages that start with ' + prefix + 'feedback are automatically forwarded to the developer. Feel write it down and send it. it will be a great help for development. Thank you.')
				await message.channel.send(embed=exampleEmbed)
			else:
				await sendError(message.channel, '' + args[0] + ' does not exist. For a list of commands, refer to the ' + prefix + 'help')
		return
	if command == 'tip':
		exampleEmbed = discord.Embed(color=0x8cffa9,title='Tip',url='https://github.com/Eggrice62/SayangBot',description='This is general solution you can try when there is a problem with the printed sheet music.')
		exampleEmbed.add_field(inline=False,name= 'Reducing the number of characters', value= '' + prefix + 'ensemble resolution 5 Try lowering the resolution, etc. If no resolution option is entered, the default value of 6 is applied. The lower the input, the fewer characters and the lower the quality of the score.\n' + prefix + 'ensemble mergetempo 2 Try using the mergetempo option, etc. If the mergetempo option is not entered, the default value of 0 is applied, and the higher the input, the fewer characters.')
		exampleEmbed.add_field(inline=False,name= 'Individual volume control in ensemble', value= '' + prefix + 'When using the ensemble sheet music command, all instruments are output as the file contents without changing the volume. If you want to apply the volume settihttps://www.youtube.com/watch?v=sykrbTWfa_sng to only a specific instrument and output it individually, try adding options such as ' + prefix + 'ensemble <desired instrument> volumeincrease/volumedecrease/fixedvolume. If you want to adjust the volume in more detail, refer to the ' + prefix + 'help solo for the detailed function.')
		exampleEmbed.add_field(inline=False,name= 'Strange notes in the sheet music?', value= '' + prefix + 'If you do not enter the option to exclude drums when creating solo score with the solo command, drum chord notes that do not match the chord may be mixed, resulting in an unintended sound. Try ' + prefix + 'ensemble drumexclude')
		exampleEmbed.add_field(inline=False,name= 'When the bot is not responding', value= 'Score conversion may take up to a minute, depending on the size of the MIDI file. If there is no response to the conversion command for more than 1 minute, or if there is no response even after sending a MIDI file, the bot itself is dead. In this case, the developer will fix the error within a few hours and restart the bot. It"s been patching steadily since 9/24. wait please...')
		exampleEmbed.add_field(inline=False,name= 'File won"t open & out of sync', value= 'I have reviewed this bug for a while, but as of now, I have only confirmed that the MIDI file with the problem does not open with other programs or opens strangely. If you have time, it would be of great help if you provide additional information with the ' + prefix + 'feedback <content> command. I"m curious as to which program the mid file was made with...')
		exampleEmbed.add_field(inline=False,name= 'Bug report', value= 'Any messages that start with ' + prefix + 'feedback are automatically forwarded to the developer. Feel write it down and send it. it will be a great help for development. Thank you.')
		await message.channel.send(embed=exampleEmbed)
		return
	
	if command == 'user2filename':
		user = await client.fetch_user('364432570005323796')
		keysList = list(user2filename.keys())
		tempMessage = '**Current status of user2filename**'
		for i in range(len(keysList)):
			tempMessage += "\n" + str(keysList[i]) + ':' + str(user2filename[keysList[i]])
		await user.send(tempMessage)
		return
	
	if command == 'user2time':
		user = await client.fetch_user('364432570005323796')
		keysList = list(user2time.keys())
		tempMessage = '**Current status of user2time**'
		for i in range(len(keysList)):
			tempMessage += "\n" + str(keysList[i]) + ':' + str(user2time[keysList[i]])
		await user.send(tempMessage)
		return
	
	if command == 'user2processtime':
		user = await client.fetch_user('364432570005323796')
		keysList = list(user2processtime.keys())
		tempMessage = '**Current status of user2processtime**'
		for i in range(len(keysList)):
			tempMessage += "\n" + str(keysList[i]) + ':' + str(user2processtime[keysList[i]])
		await user.send(tempMessage)
		return
	
	if command == 'instrumentName2num':
		user = await client.fetch_user('364432570005323796')
		keysList = list(instrumentName2num.keys())
		tempMessage = '**Current status of instrumentName2num**'
		for i in range(len(keysList)):
			tempMessage += "\n" + str(keysList[i]) + ':' + str(instrumentName2num[keysList[i]])
		await user.send(tempMessage)
		return
	
	if command != '정보' and command != '악보' and command != '합주악보':
		if isEnglish is False:
			await sendError(message.channel, '' + command + (' 은' if checkBatchimEnding(command) else ' 는') + ' 없는 명령어입니다. 명령어 목록은 ' + prefix + '도움말 을 참고해 주세요.')
		else:
			await sendError(message.channel, '' + command + ' is' + ' not a valid command for SayangBot. For a list of commands, see ' + prefix + 'help')
		return
	
	try:
		devnull = user2filename[('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id))]
	except:
		if isEnglish is False:
			await sendError(message.channel, '인식된 미디 파일이 없습니다. 미디 파일이 첨부된 메시지를 전송해 주세요.')
		else:
			await sendError(message.channel, 'The MIDI file is not recognized yet. Please send a message with the MIDI file attached.')
		return
	
	os.system('ln -sf $PWD/module/sayangbot_module $PWD/db/%s/sayangbot_module'%(('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id))))
	os.system('ln -sf $PWD/db/%s/%s $PWD/db/%s/current.midi_now'%(('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id)),user2filename[('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id))],('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id))))
	os.system('rm db/%s/99output.sayang 2> /dev/null'%(('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id))))
	finput = open('db/%s/00input.sayang'%(('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id))),"w")
	finput.write(message.content[len(prefix):] + (' 영어' if isEnglish else '') + ' 원본파일이름 ' + user2filename[('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id))][23:-4])
	finput.close()
	if isEnglish is False:
		progressMessage = await message.channel.send("변환 중입니다. 변환이 완료되거나 120초 내 변환에 실패할 경우 다시 알려드리겠습니다.")
	else:
		progressMessage = await message.channel.send("Conversion in progress. If the conversion succeeds or the conversion fails within 2 minutes, we will notify you again.")
	os.system('cd db/%s/ && timeout 120s ./sayangbot_module'%((('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id)))))
	await progressMessage.delete()
	
	if exists('db/%s/99output.sayang'%(('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id)))):
		if command == '정보': await message.channel.send('파일 정보 (%s)' % user2filename[('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id))][23:])
		fsayang = open('db/%s/99output.sayang'%(('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id))))
		lines = fsayang.readlines()
		for i in range(len(lines)//2):
			if lines[2*i].strip() == 'Error':
				await sendError(message.channel, lines[2*i+1].strip().replace('$newline$','\n').replace('$prefix$',prefix))
				if (message.guild is not None and message.content.startswith(prefix)) or (not message.guild and message.author.id != 364432570005323796):
					user = await client.fetch_user('364432570005323796')
					exampleEmbed = discord.Embed(color=0xff0000,title="변환 실패",url='https://github.com/Eggrice62/SayangBot' ,description=((message.guild.name+'-'+message.channel.name) if (message.guild) else 'DM') + ' 실패')
					exampleEmbed.set_author(name=message.author.name + ' (' + str(message.author.id) + ')', icon_url=message.author.avatar_url)
					await user.send(embed=exampleEmbed)
			elif lines[2*i].strip() == 'Warning':
				await sendWarning(message.channel, lines[2*i+1].strip().replace('$newline$','\n').replace('$prefix$',prefix))
			elif lines[2*i].strip() == 'Information':
				await sendInformation(message.channel, lines[2*i+1].strip().replace('$newline$','\n').replace('$prefix$',prefix))
			elif lines[2*i].strip() == 'PlainText':
				await message.channel.send(lines[2*i+1].strip().replace('$newline$','\n').replace('$prefix$',prefix))
		for i in range(len(lines)//2):
			if lines[2*i].strip() == 'FileName':
				if lines[2*i+1].strip()[-3:] == 'zip':
					sendFileName = 'db/%s/%s.zip'%(((('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id)))), user2filename[('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id))][23:-4])
					os.system('cp db/%s/out100.zip %s'%(((('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id)))), sendFileName))
				else:
					sendFileName = 'db/%s/%s' % (('guild_'+str(message.channel.id)) if (message.guild) else ('dm_'+str(message.author.id)), lines[2*i+1].strip())
				await message.channel.send('↓다운로드', file=discord.File(sendFileName))
	else:
		if (message.guild is not None and message.content.startswith(prefix)) or (not message.guild and message.author.id != 364432570005323796):
			user = await client.fetch_user('364432570005323796')
			exampleEmbed = discord.Embed(color=0xff0000,title="변환 실패",url='https://github.com/Eggrice62/SayangBot' ,description=((message.guild.name+'-'+message.channel.name) if (message.guild) else 'DM') + ' 실패')
			exampleEmbed.set_author(name=message.author.name + ' (' + str(message.author.id) + ')', icon_url=message.author.avatar_url)
			await user.send(embed=exampleEmbed)
		if isEnglish is False:
			await sendError(message.channel, '죄송합니다, 버그 걸렸습니다. 업데이트를 기다려 주세요...')
		else:
			await sendError(message.channel, 'Sorry, I got a bug. Please wait for the update...')
		return

async def sendError(a, b):
	exampleEmbed = discord.Embed(color=0xff0000,title='에러',description='```css\n['+b+']```',url='https://github.com/Eggrice62/SayangBot')
	exampleEmbed.set_footer(text='문의 : 눈꽃빙빙빙 (계란밥#9331)', icon_url='https://i.imgur.com/82dLPkv.png')
	await a.send(embed=exampleEmbed)

async def sendWarning(a, b):
	exampleEmbed = discord.Embed(color=0xffa500,title='경고',description='```fix\n['+b+']```',url='https://github.com/Eggrice62/SayangBot')
	exampleEmbed.set_footer(text='문의 : 눈꽃빙빙빙 (계란밥#9331)', icon_url='https://i.imgur.com/82dLPkv.png')
	await a.send(embed=exampleEmbed)

async def sendInformation(a, b):
	exampleEmbed = discord.Embed(color=0x8cffa9,title='알림',description='```ini\n['+b+']```',url='https://github.com/Eggrice62/SayangBot')
	exampleEmbed.set_footer(text='문의 : 눈꽃빙빙빙 (계란밥#9331)', icon_url='https://i.imgur.com/82dLPkv.png')
	await a.send(embed=exampleEmbed)

def checkBatchimEnding(word):
	return False

def downloadmidi(url, path):
	response = requests.get(url)
	open(path, "wb").write(response.content)

user2filename = {}
user2time = {}
user2processtime = {}

instrumentName2num = {}
instrumentName2num['피아노'] = [0, 1]
instrumentName2num['기타'] = [24]
instrumentName2num['클라리넷'] = [71]
instrumentName2num['하프'] = [46]
instrumentName2num['팀파니'] = [47]
instrumentName2num['일렉트릭기타'] = [29, 30]
instrumentName2num['베이스기타'] = [33]
instrumentName2num['탐탐'] = [117]
instrumentName2num['바이올린'] = [40, 41, 48, 49]
instrumentName2num['첼로'] = [42, 43]
instrumentName2num['팬플루트'] = [75]
instrumentName2num['색소폰'] = [65, 66, 67, 64]
instrumentName2num['트롬본'] = [57, 58]
instrumentName2num['트럼펫'] = [56, 59, 60, 61]
instrumentName2num['오카리나'] = [79, 78]
instrumentName2num['어쿠스틱베이스'] = [32]
instrumentName2num['비브라폰'] = [11, 98]
instrumentName2num['전자피아노'] = [4]
instrumentName2num['스틸드럼'] = [114]
instrumentName2num['피크베이스기타'] = [34]
instrumentName2num['오보에'] = [68, 69, 70]
instrumentName2num['피치카토바이올린'] = [45, 105, 107]
instrumentName2num['하프시코드'] = [6, 7]
instrumentName2num['하모니카'] = [22]
instrumentName2num['실로폰'] = [13, 14]
instrumentName2num['리코더'] = [74]
instrumentName2num['첼레스타'] = [8, 9, 10]
instrumentName2num['심벌즈'] = [129]
instrumentName2num['큰북'] = [130]
instrumentName2num['작은북'] = [131]
instrumentName2num['프렛리스베이스기타'] = [35]
instrumentName2num['마림바'] = [12]
instrumentName2num['플루트'] = [73, 72]

prefix = '.%'
with open('secretToken') as f:
	lines = f.readlines()
	secretToken = lines[0].strip()
client.run(secretToken)