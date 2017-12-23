
#include "SubtitleIO.h"



class SRT : public SubtitleIO {


public:

    using SubtitleIO::SubtitleIO ;


    virtual	SubtitleContainer    Load(istream& f) override ;
    virtual	void    Save(ostream& f, const SubtitleContainer& subtitles ) override ;

    virtual	string  GetExtensionForSaving() override ;
	
	virtual	string  GetFormatDescription() override ;

private:

    TimeStamp   ReadTimeStamp( const string& str );
    void   WriteTimeStamp( ostream& f, const TimeStamp& t );


};

