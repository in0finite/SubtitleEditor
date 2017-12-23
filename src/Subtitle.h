#ifndef SUBTITLE_H_INCLUDED
#define SUBTITLE_H_INCLUDED

#include <vector>
#include <string>
#include <regex>
#include <iostream>
using namespace std;


class TimeStamp {
public:
    int miliseconds = 0;

    /*
     *  Constructs from string in the following format: hh:mm:ss,mmm
     */
    bool tryParse(const string& s) {

        try {
            this->parse(s);
            return true ;
        } catch(...) {

        }

        return false ;
    }

    void parse(const string& s) {

        smatch results;
        regex_match(s, results, regex("(\\d\\d):(\\d\\d):(\\d\\d),(\\d\\d\\d)"));

        if(results.size() != 5) {
            throw runtime_error("Error reading timestamp from: " + s);
        }

        //this->format( stoi(results[0] + results[1]), stoi(results[2] + results[3]), stoi(results[4] + results[5]),
        //        stoi(results[6] + results[7] + results[8]));
        this->format( stoi(results[1]), stoi(results[2]), stoi(results[3]), stoi(results[4]) );

        /*
        for(int i=1; i < 5 ; i++) {
            cout << string(results[i]) + " " ;
        }
        cout << " -> " << this->miliseconds << endl ;
        */

    }

    static bool isValid(const string& s) {
        TimeStamp t;
        return t.tryParse(s);
    }

    void formatted( int& hours, int& minutes, int& seconds, int& miliseconds ) const {
		miliseconds = this->miliseconds % 1000 ;
		int totalSeconds = this->miliseconds / 1000 ;
		seconds = totalSeconds % 60 ;
		minutes = (totalSeconds / 60) % 60 ;
		hours = totalSeconds / 3600 ;
	}

    /*
     *  Returns string in the following format: hh:mm:ss,mmm
     */
    string formatted() const {
        int hours=0, minutes=0, seconds=0, miliseconds=0;
        formatted(hours, minutes, seconds, miliseconds);
        char buffer[20] = {0};
        sprintf_s( buffer, 20, "%02d:%02d:%02d,%03d", hours, minutes, seconds, miliseconds);
        return buffer ;
    }
	
	void format( int hours, int minutes, int seconds, int miliseconds ) {
		
		this->miliseconds = miliseconds + seconds * 1000 + minutes * 60 * 1000 + hours * 60 * 60 * 1000 ;
		
	}
	
};


class Subtitle {

public:

    Subtitle() { }

    string  GetText() { return text; }
    TimeStamp  GetTimeOfAppearance() { return appearanceTime; }
    TimeStamp  GetTimeOfRemoval() { return removalTime; }

public:

    string  text;
    TimeStamp   appearanceTime;
    TimeStamp   removalTime;


} ;


#define SubtitleContainer   vector<Subtitle>


#endif // SUBTITLE_H_INCLUDED
