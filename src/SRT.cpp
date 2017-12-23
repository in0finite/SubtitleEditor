

#include "SRT.h"
#include <iostream>
#include <SerbianSubtitleReplacements.h>



SubtitleContainer    SRT::Load(istream& f) {

    SubtitleContainer subtitles ;

    int count = 0;
	int lineCount = 0;

    while(!f.eof()) {

        Subtitle subtitle ;

		string line = "";

        // subtitle count
		getline(f, line);
		lineCount++;

        // timestamps
		getline(f, line);
		lineCount++;


        smatch results;
        regex_match(line, results, regex("(\\d\\d):(\\d\\d):(\\d\\d),(\\d\\d\\d) --> (\\d\\d):(\\d\\d):(\\d\\d),(\\d\\d\\d)"));

        if(results.size() != 9) {
            throw runtime_error("Error reading timestamps at line " + to_string(lineCount) );
        }

        subtitle.appearanceTime.format( stoi(results[1]), stoi(results[2]), stoi(results[3]), stoi(results[4]) );
        subtitle.removalTime.format( stoi(results[5]), stoi(results[6]), stoi(results[7]), stoi(results[8]) );


        /*
        string magicStr = " --> " ;
        auto pos = line.find(magicStr);
        if(pos == string::npos) {
            throw runtime_error("Error reading subtitles at line " + lineCount);
        }

        subtitle.appearanceTime = ReadTimeStamp(line.substr(0, pos));
        subtitle.removalTime = ReadTimeStamp(line.substr(pos + 5));
        */

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

        poop2017cppmodification::replace( subtitle.text );

        // add to container
        subtitles.push_back( subtitle );


        count ++ ;
    }

    return subtitles ;
}

void    SRT::Save(ostream& f, const SubtitleContainer& subtitles ) {

    int count = 0;
    for(auto sbt : subtitles) {

        // subtitle number
        f << count << endl ;

        // timestamps
        this->WriteTimeStamp( f, sbt.appearanceTime );
        f << " --> " ;
        this->WriteTimeStamp( f, sbt.removalTime );
		f << endl;

        // subtitle text
        f << sbt.text ;

        f << endl ;

        // empty line
		if(count != subtitles.size() - 1)
			f << endl ;

		count++;
    }

}

string  SRT::GetExtensionForSaving() {

    return ".SRT" ;

}

string  SRT::GetFormatDescription() {
	
	return "SRT";
	
}

TimeStamp   SRT::ReadTimeStamp( const string& str ) {

	if(str.length() != 12) {
        throw runtime_error("Invalid time stamp");
	}

	TimeStamp t;
	char num[4] = {0};
	
	num[0] = str[0];
	num[1] = str[1];
	t.miliseconds += atoi(num) * 60 * 60 * 1000 ;
	
	num[0] = str[3];
	num[1] = str[4];
	t.miliseconds += atoi(num) * 60 * 1000 ;
	
	num[0] = str[6];
	num[1] = str[7];
	t.miliseconds += atoi(num) * 1000 ;
	
	num[0] = str[9];
	num[1] = str[10];
	num[2] = str[11];
	t.miliseconds += atoi(num) ;
	
	return t ;
}

void   SRT::WriteTimeStamp( ostream& f, const TimeStamp& t ) {

	int hours=0, minutes=0, seconds=0, miliseconds=0;
	t.formatted( hours, minutes, seconds, miliseconds );
	
	char buffer[15] = { 0 };
    sprintf_s( buffer, 15, "%02d%s%02d%s%02d%s%03d", hours, ":", minutes, ":", seconds,
        ",", miliseconds );

	f << buffer;

}

