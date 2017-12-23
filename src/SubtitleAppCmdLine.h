#ifndef SUBTITLEAPPCMDLINE_H_INCLUDED
#define SUBTITLEAPPCMDLINE_H_INCLUDED

#include "SubtitleApp.h"
using namespace std;

class SubtitleAppCmdLine : public SubtitleApp {

public:

    SubtitleAppCmdLine();
    virtual ~SubtitleAppCmdLine() { }


	virtual	int     askUserForInt( string description ) override {
		return askUserFor<int>(description);
	}
    virtual	float   askUserForFloat( string description ) override {
		return askUserFor<float>(description);
	}
    virtual	string  askUserForString( string description ) override {
		return askUserFor<string>(description);
	}
	
	template <typename T>
	T	askUserFor( string description )  {
		cout << description << endl ;
		T t;
		cin >> t ;
		return t;
	}

private:

	void    printUsage();
    void    processOption( const string& option );

    void    Run();

} ;


#endif // SUBTITLEAPPCMDLINE_H_INCLUDED
