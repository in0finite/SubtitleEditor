
#ifndef SUBTITLEIO_H_INCLUDED
#define SUBTITLEIO_H_INCLUDED

#include <vector>
#include <string>
#include "Subtitle.h"
//#include <SubtitleApp.h>
using namespace std;


class SubtitleApp;


class SubtitleIO {

public:

    SubtitleIO( SubtitleApp* app ) : m_app(app) { }

    virtual ~SubtitleIO() { }


    virtual SubtitleContainer    Load(istream& f) = 0;
    virtual void    Save(ostream& f, const SubtitleContainer& subtitles ) = 0;

    SubtitleContainer    Load(const string& path);
    void    Save(const string& path, const SubtitleContainer& subtitles );


    virtual string  GetExtensionForSaving() = 0 ;
	
    /// String that describes this file format. It is used to uniqely identify SubtitleIO.
	virtual string  GetFormatDescription() = 0 ;

    int     GetSizeInBytes() { return m_sizeInBytes ; }

    static int  getFileSize( fstream& f );

protected:

    SubtitleApp*    m_app;
    int m_sizeInBytes = 0;

};

#endif
