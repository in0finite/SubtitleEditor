

#include "MPlayer.h"
#include <iostream>



SubtitleContainer    MPlayer::Load(istream& f) {

    SubtitleContainer subtitles ;

    int count = 0;
	int lineCount = 0;

    int lastTime = 0;

    while(!f.eof()) {

        Subtitle subtitle ;

		string line = "";

        // timestamps
		getline(f, line);
		lineCount++;


        auto pos = line.find(" ");
        if(pos == string::npos) {
            throw runtime_error("Error reading subtitles at line " + to_string(lineCount) );
        }

        float f1 = stof( line.substr(0, pos) );
        float f2 = stof( line.substr(pos) );

        subtitle.appearanceTime.miliseconds = lastTime + (int) (f1 * 1000) ;
        subtitle.removalTime.miliseconds = subtitle.appearanceTime.miliseconds + (int) (f2 * 1000) ;

        lastTime = subtitle.removalTime.miliseconds ;


        // read text until empty line is found, or until end of file
        subtitle.text = "" ;
        do {
            
			getline(f, line);
			lineCount++;

            subtitle.text += line + "\n" ;

        } while( line != "" && !f.eof() ) ;

		// remove last new line character
        while( subtitle.text.length() > 0 && subtitle.text[ subtitle.text.length() - 1 ] == '\n' ) {
            //subtitle.text.resize(subtitle.text.length() - 1);
            subtitle.text.erase(subtitle.text.length() - 1);
        }

        // add to container
        subtitles.push_back( subtitle );


        count ++ ;
    }

    return subtitles ;
}

void    MPlayer::Save(ostream& f, const SubtitleContainer& subtitles ) {

    int lastTime = 0;

    int count = 0;
    for(auto sbt : subtitles) {

        // timestamps
        f << (sbt.appearanceTime.miliseconds - lastTime) / 1000.0f ;
        f << " " ;
        f << (sbt.removalTime.miliseconds - sbt.appearanceTime.miliseconds) / 1000.0f ;
		f << endl;

        lastTime = sbt.removalTime.miliseconds ;

        // subtitle text
        f << sbt.text ;

        f << endl ;
        f << endl ;

		count++;
    }

}

string  MPlayer::GetExtensionForSaving() {

    return ".SUB" ;

}

string  MPlayer::GetFormatDescription() {
	
    return "MPlayer";
	
}
