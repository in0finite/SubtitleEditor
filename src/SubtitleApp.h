#ifndef SUBTITLEAPP_H_INCLUDED
#define SUBTITLEAPP_H_INCLUDED

#include <vector>
#include "Subtitle.h"
#include "SubtitleProcessor.h"
#include "SubtitleIO.h"
using namespace std;

class SubtitleApp {

public:

    friend class SubtitleProcessor ;

    SubtitleApp();
    virtual ~SubtitleApp();


    // used by IO handlers to ask user for additional parameters
	virtual	int     askUserForInt( string description ) = 0;
    virtual	float   askUserForFloat( string description ) = 0;
    virtual	string  askUserForString( string description ) = 0;

    void    SaveSubtitles( const string& path, SubtitleIO* ioHandler, const SubtitleContainer& subtitles );
    SubtitleContainer    LoadSubtitles( const string& path, SubtitleIO* ioHandler );

    /// Loads plugins from specifed directory and returns number of plugins loaded.
    int    LoadPlugins( const wstring& path );

    vector<SubtitleIO*> getIoHandlers() const;

    bool    AddPlugin( SubtitleIO* io );

    // the following functions are provided for iteration and modifying of subtitles, so
    // that inherited classes can use their own storage for subtitles (e.g console app
    // can store it in internal container, or the gui app can store it in a gui element)
    //
    // by default, the class stores subtitles in internal container
    virtual Subtitle    getSubtitleAt(int index) const = 0 ;
    virtual void    setSubtitleAt(int index, const Subtitle& sub) = 0 ;
    virtual int    getSubtitlesCount() const = 0 ;

    // gets a copy of all subtitles
    SubtitleContainer   GetAllSubtitles() const ;

    int     GetSizeInBytes() const { return m_sizeInBytes ; }

protected:


    //SubtitleContainer   m_subtitles ;	// currently loaded subtitles

private:

    vector<SubtitleIO*>   m_ioHandlers ;
    //vector<SubtitleProcessor*>   m_processors ;

    vector<wstring>	FindAllPlugins( const wstring& path );
    bool	LoadPlugin(const wstring& path);

    int m_sizeInBytes = 0;

} ;


#endif // SUBTITLEAPP_H_INCLUDED
