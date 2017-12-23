
#include "SubtitleIO.h"
#include "SubtitleApp.h"
#include <fstream>


SubtitleContainer    SubtitleIO::Load(const string& path) {

    m_sizeInBytes = 0;

    fstream f;
	f.open(path);
    if(!f.is_open()) {
        throw runtime_error("Failed to open file for loading: " + path);
    }

    // get file size
    m_sizeInBytes = getFileSize(f);

    auto subtitles = this->Load(f);

	f.close();

	return subtitles;
}

void	SubtitleIO::Save(const string& path, const SubtitleContainer& subtitles) {

    m_sizeInBytes = 0;

    fstream f(path, ios_base::out);
    if(!f.is_open()) {
        throw runtime_error("Failed to open file for saving: " + path);
    }

	this->Save(f, subtitles);

    m_sizeInBytes = getFileSize(f);

	f.close();
}

int SubtitleIO::getFileSize(fstream &f) {

    auto original = f.tellp();

    f.seekg( 0, ios::beg);
    auto s1 = f.tellp();
    f.seekg(ios::end);
    int size = (int) (f.tellp() - s1);

    // restore original
    f.seekg( original, ios::beg);

    return size ;
}

