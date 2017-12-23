
#include "SubtitleIO.h"



class MPlayer : public SubtitleIO {


public:

    using SubtitleIO::SubtitleIO ;


    virtual	SubtitleContainer    Load(istream& f) override ;
    virtual	void    Save(ostream& f, const SubtitleContainer& subtitles ) override ;

    virtual	string  GetExtensionForSaving() override ;
	
	virtual	string  GetFormatDescription() override ;

private:


};

