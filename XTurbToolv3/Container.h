#pragma once
#include "Window.h"
#include "Control.h"
#include "Logger.h"
#include "InputData.h"
#include "Graph.h"
#include "XTurbRunner.h"
#include "FileSelectorWindow.h"
#include "DataDisplayWindow.h"
#include <vector>

// This manages the main container window and its controls (Graph, Input fields, etc.)
class Container : public Window {
public:
    Container(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height, const std::wstring& xturbExeName = L"XTurb-g95-mingw32-cygwin-kirk.exe");
    ~Container() override;
    void create(HINSTANCE hInstance, int nCmdShow) override;
    LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;

private:
    int x;
    int y;
    int width;
    int height;
    std::vector<Control*> controls;
    HFONT hFontRegular;
    HFONT hFontBold;
    HMENU nextControlId;
    InputData inputData;
    std::wstring exeDir;

    // Output Display, using vector for better clean up; can be changed if necessary in the future, but will need work
    std::vector<DataDisplayWindow*> displayWindows; 
	std::vector<FileSelectorWindow*> fileSelectors;

    // Layout management
    static const int SPACING = 10;
    int currentY;
    int totalHeight;
    int scrollPos;

    // Input fields for data collection
	XTurbRunner* xturbRunner;
    InputField* nameInput;
    InputField* bnInput;
    InputField* rootInput;
    InputField* ntaperInput;
    InputField* rtaperInput;
    InputField* ctaperInput;
    InputField* ntwistInput;
    InputField* rtwistInput;
    InputField* dtwistInput;
    InputField* nairfInput;
    InputField* rairfInput;
    InputField* airfdataInput;
    InputField* blendairfInput;
    InputField* percentrInput;
    InputField* stalldelayInput;
    InputField* viternaInput;
    InputField* nsweepInput;
    InputField* rsweepInput;
    InputField* lsweepInput;
    InputField* ndihedInput;
    InputField* rdihedInput;
    InputField* ldihedInput;
    InputField* ntwaxInput;
    InputField* rtwaxInput;
    InputField* ltwaxInput;
    InputField* npiaxInput;
    InputField* rpiaxInput;
    InputField* lpiaxInput;
    InputField* checkInput;
    InputField* designInput;
    InputField* ntsrInput;
    InputField* btsrInput;
    InputField* etsrInput;
    InputField* npitchInput;
    InputField* bpitchInput;
    InputField* epitchInput;
    InputField* analysisInput;
    InputField* nanaInput;
    InputField* tsranaInput;
    InputField* pitchanaInput;
    InputField* predictionInput;
    InputField* bradiusInput;
    InputField* rhoairInput;
    InputField* muairInput;
    InputField* npreInput;
    InputField* vwindInput;
    InputField* rpmpresInput;
    InputField* pitchpreInput;
    InputField* methodInput;
    InputField* jxInput;
    InputField* cosdistrInput;
    InputField* gnuplotInput;
    InputField* wakeexpInput;
    InputField* DX0Input;
    InputField* XSTRInput;
    InputField* XTREFFTZInput;
    InputField* NSECInput;
    InputField* ibInput;
    InputField* DIPInput;
    InputField* OMRELAXInput;
    InputField* aviscInput;
    InputField* NACMODInput;
    InputField* LNInput;
    InputField* HNInput;
    InputField* XNInput;
    InputField* rlossInput;
    InputField* tiplossInput;
    InputField* axrelaxInput;
    InputField* atrelaxInput;
    InputField* optimInput;

    // Graph objects for Twist and Chord distributions
    TwistGraph* twistGraph;
    ChordGraph* chordGraph;

    // Helper methods
    HMENU generateControlId();
    void addControl(Control* control, int width, int height);
    void addLabeledInput(const std::wstring& labelText, InputField*& inputField, int labelWidth, int inputWidth, int height);
    void updateScrollRange();
    bool validateInputs(std::wstring& errorMessage);
    void updateGraphs();
};