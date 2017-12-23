
#include "SubtitleApp.h"
#include <exception>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include "SRT.h"
#include <windows.h>
using namespace std;


/** @brief (one liner)
  *
  * (documentation goes here)
  */
SubtitleApp::SubtitleApp()
{

	// load IO handlers from DLLs
	wchar_t buffer[1000] = { 0 };
	GetCurrentDirectory(1000, buffer);
    LoadPlugins(wstring(buffer) + L"\\plugins\\");

    //m_ioHandlers.push_back(new SRT());
    //m_processors.push_back(new SubtitleProcessor());

}

vector<wstring> SubtitleApp::FindAllPlugins( const wstring& path )
{
    vector<wstring> names;

    wstring search_path = path ;
    if(path[path.length() - 1] != L'\\' && path[path.length() - 1] != L'/')
        search_path += L"\\" ;
    search_path += L"*.dll";

    WIN32_FIND_DATA fd; 
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                names.push_back(fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    } 
    return names;
}

bool	SubtitleApp::LoadPlugin( const wstring& path ) {
	
	HINSTANCE hLib = LoadLibrary(path.c_str());
	if (!hLib) {
        wcout << "Could not load the plugin " << path.c_str() << endl;
        return false;
	}
  
    typedef SubtitleIO* (*pluginFunc)(SubtitleApp*);
	
	string funcName = "createSubtitleIO" ;
	pluginFunc f = (pluginFunc) GetProcAddress( hLib, funcName.c_str() );
	if (!f) {
        wcout << "Could not locate the function " << funcName.c_str() << " inside plugin " << path.c_str() << endl;
        return false;
	}
	
	// create new instance of a class, and add it to collection
    this->AddPlugin( f(this) );
  
    return true ;
}

void    SubtitleApp::SaveSubtitles( const string& path, SubtitleIO* ioHandler, const SubtitleContainer& subtitles ) {

    m_sizeInBytes = 0;

    ioHandler->Save(path, subtitles);

    m_sizeInBytes = ioHandler->GetSizeInBytes();

}

vector<SubtitleIO*> SubtitleApp::getIoHandlers() const
{
    return m_ioHandlers;
}

SubtitleContainer SubtitleApp::GetAllSubtitles() const
{
    SubtitleContainer subtitles(this->getSubtitlesCount());
    for(int i=0; i < this->getSubtitlesCount(); i++) {
        subtitles[i] = this->getSubtitleAt(i);
    }
    return subtitles;
}

bool SubtitleApp::AddPlugin(SubtitleIO *io)
{
    if(nullptr == io)
        return false;

    if(io->GetFormatDescription().length() < 1)
        return false;

    if( find_if(m_ioHandlers.begin(), m_ioHandlers.end(),
                [&io] (SubtitleIO* io_) { return io->GetFormatDescription() == io_->GetFormatDescription(); } ) != m_ioHandlers.end() ) {
        return false ;
    }

    m_ioHandlers.push_back(io);
    return true ;
}

SubtitleContainer    SubtitleApp::LoadSubtitles( const string& path, SubtitleIO* ioHandler ) {

    m_sizeInBytes = 0;

    auto subtitles = ioHandler->Load(path);

    m_sizeInBytes = ioHandler->GetSizeInBytes();

    return subtitles ;
}

int SubtitleApp::LoadPlugins(const wstring &path)
{
    int count = 0;
    auto plugins = FindAllPlugins( path );
    for( auto pluginName : plugins ) {
        auto fullPluginPath = path + L"\\" + pluginName;
        std::replace( fullPluginPath.begin(), fullPluginPath.end(), L'/', L'\\');
        if( LoadPlugin(fullPluginPath) )
            count++;
    }
    return count ;
}

SubtitleApp::~SubtitleApp() {

    for(auto io : m_ioHandlers) {
        delete io ;
    }

    //for(auto prc : m_processors) {
    //    delete prc ;
    //}

}

