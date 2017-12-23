
#include "SubtitleAppCmdLine.h"
#include <iostream>
#include "SRT.h"
#include <windows.h>
#include <ctime>
using namespace std;


SubtitleAppCmdLine::SubtitleAppCmdLine() {
	
	m_ioHandlers.push_back(new SRT(this));
    m_processors.push_back(new SubtitleProcessor(this));

	this->Run();
	
}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
void SubtitleAppCmdLine::Run()
{


    while(1) {

        printUsage();

        try {
            string option = "";
            cin >> option ;

            processOption(option);

        } catch(std::exception& ex) {
            cout << "\n\nError:\n" << ex.what() << "\n\n" ;
        }

    }

}

/** @brief (one liner)
  *
  * (documentation goes here)
  */
void SubtitleAppCmdLine::printUsage()
{

    string str = "Choose one of the available options:\n" ;
    str += "exit\n";
    str += "load\n";
    str += "save\n";
	str += "play\n";
    for(auto prc : m_processors) {
        for(auto option : prc->GetOptions()) {
            str += option + "\n" ;
        }
    }

    str += "\n\n" ;

    cout << str ;
}


/** @brief (one liner)
  *
  * (documentation goes here)
  */
void SubtitleAppCmdLine::processOption(const string& option)
{

    if("exit" == option) {
        exit(0);
    } else if("load" == option || "save" == option) {

		if( m_ioHandlers.empty() ) {
			throw runtime_error("Sorry, but no IO handlers are loaded");
		}
		
		cout << "Choose one of the available formats:" << endl ;
		int i=0;
		for(auto io : m_ioHandlers) {
			cout << i << " - " << io->GetFormatDescription() << endl ;
			i++;
		}
		
		int choosenHandlerIndex = 0;
		cin >> choosenHandlerIndex ;
		if(choosenHandlerIndex < 0 || choosenHandlerIndex >= m_ioHandlers.size() ) {
			throw runtime_error("Invalid handler choosen");
		}
		
		auto choosenHandler = m_ioHandlers[choosenHandlerIndex] ;
	
        cout << "Enter file path:" << endl ;

        string path;
        cin >> path;

		/*
        auto pos = path.find(".");
        if( pos != string::npos ) {
            string ext = path.substr( pos + 1 );

            //find( m_ioHandlers.begin(), m_ioHandlers.end(), ext )
            auto foundIO = find_if(m_ioHandlers.begin(), m_ioHandlers.end(),
                                   [&ext] (SubtitleIO* io) { return io->GetExtension() == ext; } );
            if(foundIO != m_ioHandlers.end()) {
                // found io handler which can handle this type
                auto io = *foundIO ;
                if("load" == option)
                    m_subtitles = io->Load( path );
                else
                    io->Save( path, m_subtitles );
            } else {
                throw runtime_error("File format not supported");
            }

        } else {
            throw runtime_error("File has no extension");
        }
		*/
		
		if ("load" == option) {
			m_subtitles = choosenHandler->Load(path);
			cout << "\n\nSubtitles loaded - " << m_subtitles.size() << "\n\n";
		}
		else {
			choosenHandler->Save(path, m_subtitles);
			cout << "\n\nSubtitles saved to " << path << "\n\n";
		}

	}
	else if ("play" == option) {

		cout << "Enter num subtitles to show:\n";
		int n = 0;
		cin >> n;

		int i = 0;
		int t1 = clock();
		for (auto sub : m_subtitles) {

			if (i >= n)
				break;

			// how much to wait
			int deltaMs = sub.removalTime.miliseconds - sub.appearanceTime.miliseconds;

			// wait
			while ((clock() - t1) / (double) CLOCKS_PER_SEC < deltaMs / 1000) {
				Sleep(2);
			}

			// display text
			cout << sub.text;

			t1 = clock();
			i++;
		}

	}
	else {

        // check if any of the processors can process this option
        bool found = false;
        for(auto prc : m_processors) {
            auto options = prc->GetOptions();
            if( find( options.begin(), options.end(), option ) != options.end() ) {
                prc->ProcessOption(option);
                found = true ;
                break ;
            }
        }

        if(!found) {
            throw runtime_error("Unknown option");
        }
    }

}

