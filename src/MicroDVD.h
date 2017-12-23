
#include "SubtitleIO.h"



class MicroDVD : public SubtitleIO {


public:

    using SubtitleIO::SubtitleIO ;


    virtual SubtitleContainer    Load(istream& f) override ;
    virtual void    Save(ostream& f, const SubtitleContainer& subtitles ) override ;

    virtual	string  GetExtensionForSaving() override ;
	
	virtual	string  GetFormatDescription() override ;


    static void replaceAll(std::string& str, const std::string& from, const std::string& to);

private:

    //TimeStamp   ReadTimeStamp( const string& str );
    void   WriteTimeStamp( const fstream& f, const TimeStamp& t );


};

