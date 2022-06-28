#include <iostream>
#include <fstream>
#include <vector>
#include "midifile/include/MidiFile.h"
#include "midifile/src/MidiFile.cpp"
#include "midifile/src/MidiEvent.cpp"
#include "midifile/src/MidiEventList.cpp"
#include "midifile/src/MidiMessage.cpp"
#include "midifile/src/Binasc.cpp"
#include "sayangbot_module.hpp"
#include "sayangbot_module_read_input.cpp"
#include "sayangbot_module_parse_midi.cpp"
#include "sayangbot_module_do_information.cpp"
#include "sayangbot_module_do_ms2mml.cpp"
#include "sayangbot_module_send_ms2mml_files.cpp"

#include <chrono>
/*#include <thread>*/

using namespace std;

int main(int argc, char *argv[]) {
	auto start = chrono::steady_clock::now();
	read_input();
	auto t1 = chrono::steady_clock::now();
	    cout << "1 Elapsed time in milliseconds: "
        << chrono::duration_cast<chrono::milliseconds>(t1 - start).count()
        << " ms" << endl;
	parse_midi();
	auto t2 = chrono::steady_clock::now();
	    cout << "2 Elapsed time in milliseconds: "
        << chrono::duration_cast<chrono::milliseconds>(t2 - t1).count()
        << " ms" << endl;
	if (command == "정보") {
		do_information();
	} else {
		do_ms2mml();
	}
	auto t3 = chrono::steady_clock::now();
	    cout << "3 Elapsed time in milliseconds: "
        << chrono::duration_cast<chrono::milliseconds>(t3 - t2).count()
        << " ms" << endl; t1 = t3;
		
	if (command != "정보" && !errorNow) {
		lOnOff_candidate = true;
		
		parse_midi();
		auto t2 = chrono::steady_clock::now();
	    cout << "4 Elapsed time in milliseconds: "
        << chrono::duration_cast<chrono::milliseconds>(t2 - t1).count()
        << " ms" << endl;
		do_ms2mml();
		auto t3 = chrono::steady_clock::now();
	    cout << "5 Elapsed time in milliseconds: "
        << chrono::duration_cast<chrono::milliseconds>(t3 - t2).count()
        << " ms" << endl;
		
		send_ms2mml_files();
	}
	append_information_to_vectorstr(&outputSayang, "SayangBot2 BETA 테스트 기간입니다. (~2022/7/31)$newline$버그가 있다면 편히 제보해주시면 큰 도움이 됩니다. 감사합니다.");
	write_file_from_string_vector("99output.sayang", outputSayang);
	return 0;
	
	/*string sssss = "s0s1t138o1v13el16&el32&e.l64ro4l4f+l16&f+l32&f+.l64ro6l16g+bf+t111<g+bf+t138g+bf+<g+bf+g+bf+<g+bf+g+bf+<g+bf+s0s1t69<l8et138>eb>eg+bt69>et138dc+<bg+es0s1t98o1v14ao3l16a>c+l8eav13ev12c+s0s1o1v11ao3l16a>c+l8eaec+s0s1l4c+l8es0s1l4bl8g+s0s1l4a>l8c+s0s1l4g+<l16b>fs0s1<l8f+o6f+o4l16eds0s1l4el8c+s0s1deo2g+s0s1o4v13l4c+l32a>l64c+<e<l16as0s1>l8f>l32f+.g+f+.l16c+<g+s0s1l8a>l32f.f+f.l8c+s0s1<l16g+el32f.>f+g+.<l16b>fs0s1>l4c+<l16eds0s1v15l8c+<v14dcs0<l16al64&al32rl64rv13l32f+bag+v14l16el64&el32rl64rv13l32f+bag+v14l16el64&el32rl64rv13l8fv14l16f+l64&f+l32rl64rv13l32f+gf+fv14l16f+l64&f+l32rl64rv11l32f+gf+fl16f+l64&f+l32rl64rv13l8fv15dv13l32b>dc+gf+ba+>dc+ag+g<v15l8a<v14bv15l16el64&el32rl64rv14l32ef+ed+v15l16el64&el32rl64rv11l32ef+ed+l16el64&el32rl64rv14l8del32g+a>c+ea>c+ea>c+ea>c+<v15l8ao4v14es0s1v15a>v14l32aa+ags0s1<v15l8a>v14l32ca+ags0s1<v15l8av14gs0s1t88fgs0l16grs0s1l8ft71gs0l16drs0s1t73l8c<al16a+rs0t57>v15l4cv14l16cc+s0s1t75<l4a+>l8gs0s1ft87gl16aa+s0s1l8fd.<>l16cs0s1t83<l8a+l16&a+l32&a+.l64ro5l8cs0s1<aa+>l16cd+s0s1l8dd+f+s0s1l16gal8a+l32cdc<a+s0s1>l16c<a+l8al32&a.l64rl16gs0s1l8fl16&fl32&f.l64rv12l8a+s0s1l16f>c<v11l8a+g+s0t92<v15a+v13l32d+rfr>crdrd+rfrgrg+ra+r>crt85<v15l8a+v14l16dl32cr>a>d<grf>v13c<d+rda+crt80<a+>f<art84v15l8g<v13l32d+rfro5crdrd+rfrgrg+ra+r>cr<v15l8fv14l16do3l32cro7c<a+agfd+dc<a+agfd+dd+fs0s1<l8gl16&gl32&g.l64r>d+l32g<l64a+l16a+s0s1l8b>l32b.>d<b.l16gds0s1l8d+l32b.>c<b.l8gs0s1l16d<a+l32b.o6cd.<l8fs0ct84o3v11b>ct90v12dt104d+v13gt120>cv14d+t134ft67v15gl16&gl32&g.l64r<l8dt192<l4gl8gggrl4gggf+f+f+l8g+rt160l4gl8rbs0s1t220>cd+t250g+a+t140>l16cd+s0s1>c<a+g+e<a+bs0s1>ceg+a+>ces0s1>c<a+g+fecs0s1t160<a+g+gfd+es0s1fgfefcs0s1<a+g+gfefs0s1gg+a+>c<d+c+s0s1t170cd+g+a+>cd+s0s1>c<a+g+e<a+bs0s1t180>ceg+a+>ces0s1>c<a+g+fecs0s1<a+b>cc+<a+gs0s1t170d+efgd+<a+s0s1t174>g+d+c<g+d+es0s1fed+dt180l8c+<bs0s1t140>l16cd+g+a+>cd+s0s1t150>c<a+g+e<a+bs0s1t160>ceg+a+>ces0s1t170>c<a+g+fecs0s1<a+g+gfd+es0s1fgfefcs0s1t180<a+g+gfefs0s1t165gg+a+>c<d+c+s0s1cd+g+a+>cd+s0s1t180>c<a+g+e<a+bs0s1>ceg+a+>ces0s1>c<a+g+fecs0s1t190<a+b>cc+<a+gs0s1d+efgt160l8fd+";
	vector<string> resultsssss = opt_ms2mml(sssss);*/
}