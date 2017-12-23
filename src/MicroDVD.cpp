

#include "MicroDVD.h"
#include <iostream>
#include "SubtitleApp.h"
#include <regex>



SubtitleContainer    MicroDVD::Load(istream& f) {

    float fps = m_app->askUserForFloat("Enter fps: ") ;
    if(fps <= 0) {
        throw runtime_error("Invalid fps");
    }

    SubtitleContainer subtitles ;

    while(!f.eof()) {

        Subtitle subtitle ;

        string line = "";
        getline(f, line);

        if("" == line)
            continue;

        smatch results;
        regex_match(line, results, regex("\\{(\\d+)\\}\\{(\\d+)\\}(.*)"));

        if(results.size() != 4) {
            throw runtime_error("Error reading subtitles at line " + to_string(subtitles.size() + 1) );
        }

        subtitle.appearanceTime.miliseconds = (int) ( stoi(results[1]) / fps * 1000 );
        subtitle.removalTime.miliseconds = (int) ( stoi(results[2]) / fps * 1000 );
        subtitle.text = results[3] ;
        replaceAll(subtitle.text, "|", "\n");

        subtitles.push_back( subtitle );
    }

    return subtitles ;
}

void    MicroDVD::Save(ostream& f, const SubtitleContainer& subtitles ) {

    // ask user for fps
    float fps = m_app->askUserForFloat("Enter fps: ") ;
    if(fps <= 0) {
        throw runtime_error("Invalid fps");
    }

    for(auto sbt : subtitles) {

        // timestamps
        int fps1 = sbt.appearanceTime.miliseconds / 1000.0f * fps ;
        int fps2 = sbt.removalTime.miliseconds / 1000.0f * fps ;

        f << "{" << fps1 << "}{" << fps2 << "}" ;

        // subtitle text

        // replace new line with pipe '|'
        auto text = sbt.text ;
        replaceAll(text, "\n", "|");

        f << text ;

        f << endl ;
    }

}

string  MicroDVD::GetExtensionForSaving() {

    return ".TXT" ;

}

string  MicroDVD::GetFormatDescription() {
	
	return "MicroDVD";
	
}

void MicroDVD::replaceAll(string &str, const string &from, const string &to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

//TimeStamp   MicroDVD::ReadTimeStamp( const string& str ) {
//
//
//}

