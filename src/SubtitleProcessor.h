#ifndef SUBTITLEPROCESSOR_INCLUDED
#define SUBTITLEPROCESSOR_INCLUDED

#include <string>
#include <vector>
using namespace std;

class SubtitleApp;


class SubtitleProcessor {

public:

    SubtitleProcessor( SubtitleApp* app ) : m_app(app) { }

    virtual ~SubtitleProcessor() { }

	virtual vector<string>   GetOptions() { return vector<string>(); }
	virtual int    ProcessOption(const string& command) { return 0; }

    virtual void    RemoveAllSubtitles() { }

protected:

    SubtitleApp*    m_app ;


} ;


#endif // SUBTITLEPROCESSOR_INCLUDED
