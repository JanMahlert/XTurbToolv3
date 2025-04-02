#pragma once

#include <string>
#include <vector>

//This defines all the input data for XTurb. Sadly no way to make this look nicer.
//Vector is used for the arrays, so they can be changed in size dynamically.
struct InputData {
    // &BLADE section
    std::wstring name;
    int BN;
    double ROOT;
    int NTAPER;
    std::vector<double> RTAPER;
    std::vector<double> CTAPER;
    int NTWIST;
    std::vector<double> RTWIST;
    std::vector<double> DTWIST;
    int NAIRF;
    std::vector<double> RAIRF;
    std::vector<std::wstring> AIRFDATA;
    int BLENDAIRF;
    int PERCENTR;
    int STALLDELAY;
    int VITERNA;
    int NSWEEP;
    std::vector<double> RSWEEP;
    std::vector<double> LSWEEP;
    int NDIHED;
    std::vector<double> RDIHED;
    std::vector<double> LDIHED;
    int NTWAX;
    std::vector<double> RTWAX;
    std::vector<double> LTWAX;
    int NPIAX;
    std::vector<double> RPIAX;
    std::vector<double> LPIAX;

    // &OPERATION section
    int CHECK;
    int DESIGN;
    int NTSR;
    double BTSR;
    double ETSR;
    int NPITCH;
    double BPITCH;
    double EPITCH;
    int ANALYSIS;
    int NANA;
    std::vector<double> TSRANA;
    std::vector<double> PITCHANA;
    int PREDICTION;
    double BRADIUS;
    double RHOAIR;
    double MUAIR;
    int NPRE;
    std::vector<double> VWIND;
    std::vector<double> RPMPRE;
    std::vector<double> PITCHPRE;

    // &SOLVER section
    int METHOD;
    int JX;
    int COSDISTR;
    int GNUPLOT;

    // &HVM section
    double AVISC;

    // &BEMT section
    int RLOSS;
    int tipLoss;
    double AXRELAX;
    double ATRELAX;

    // &OPTI section
    int OPTIM;

	// Helper functions
    InputData();
    void writeToFile(const std::wstring& filename) const;
};