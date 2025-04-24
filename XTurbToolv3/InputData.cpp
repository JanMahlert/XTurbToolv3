#include "InputData.h"
#include "HelperFunctions.h"
#include <fstream>

// Constructor: Set default values
InputData::InputData()
    : name(L"NREL-PhaseVI"), BN(2), ROOT(0.25), NTAPER(2), NAIRF(1), BLENDAIRF(0), PERCENTR(2),
    STALLDELAY(0), VITERNA(0), NSWEEP(2), NDIHED(2), NTWAX(2), NPIAX(2), CHECK(0), DESIGN(0),
    NTSR(10), BTSR(2), ETSR(20), NPITCH(2), BPITCH(1.8), EPITCH(3.0), ANALYSIS(0), NANA(2),
    PREDICTION(1), BRADIUS(5.03), RHOAIR(1.225), MUAIR(1.8E-05), NPRE(1), METHOD(1), JX(41),
    COSDISTR(1), GNUPLOT(2), AVISC(0.5), WAKEEXP(1), DX0(1.E-04), XSTR(1.0), XTREFFTZ(1.0),
    NSEC(20), IB(2), DIP(1), OMRELAX(0.2), NACMOD(0), LN(0.05), HN(0.025), XN(0.0),
    RLOSS(1), tipLoss(1), AXRELAX(0.125), ATRELAX(0.125), OPTIM(0) {
    RTAPER = { 0.25, 1.00 };
    CTAPER = { 0.1465, 0.0707 };
    NTWIST = 20;
    RTWIST = { 0.250, 0.267, 0.300, 0.328, 0.388, 0.449, 0.466, 0.509, 0.570, 0.631,
              0.633, 0.691, 0.752, 0.800, 0.812, 0.873, 0.934, 0.950, 0.994, 1.000 };
    DTWIST = { 20.040, 18.074, 14.292, 11.909, 7.979, 5.308, 4.715, 3.425, 2.083, 1.150,
              1.115, 0.494, -0.015, -0.381, -0.475, -0.920, -1.352, -1.469, -1.775, -1.816 };
    RAIRF = { 0.25 };
    AIRFDATA = { L"./s80905.polar" };
    RSWEEP = { 0.25, 1.00 };
    LSWEEP = { 0.00, 0.00 };
    RDIHED = { 0.25, 1.00 };
    LDIHED = { 0.00, 0.00 };
    RTWAX = { 0.25, 1.00 };
    LTWAX = { 0.30, 0.30 };
    RPIAX = { 0.25, 1.00 };
    LPIAX = { 0.30, 0.30 };
    TSRANA = { 7.585, 5.418, 6, 8, 10, 12, 14, 16, 18, 20 };
    PITCHANA = { 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0 };
    VWIND = { 7.0, 7.0, 9.0, 10.0, 11.0, 13.0 };
    RPMPRE = { 72.0, 72.0, 72.0, 72.0, 72.0, 72.0 };
    PITCHPRE = { 3.0, 3.0, 3.0, 3.0, 3.0, 3.0 };
}

// Write the data to a .inp file
void InputData::writeToFile(const std::wstring& filename) const {
    std::ofstream file(wstring_to_string(filename), std::ios::out | std::ios::trunc | std::ios::binary);
    if (!file.is_open()) {
        return;
    }

    // Write BOM for UTF-8
    //const unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
    //file.write(reinterpret_cast<const char*>(bom), sizeof(bom));

    // &BLADE section
    file << "&BLADE\r\n";
    file << "   Name       = '" << wstring_to_string(name) << "',\r\n";
    file << "\r\n";
    file << "   BN         = " << BN << ",\r\n";
    file << "\r\n";
    file << "   ROOT       = " << to_string(ROOT) << ",\r\n";
    file << "\r\n";
    file << "   NTAPER     = " << NTAPER << ",\r\n";
    file << "\r\n";
    file << "   RTAPER     = ";
    for (size_t i = 0; i < RTAPER.size(); ++i) {
        file << to_string(RTAPER[i]);
        if (i < RTAPER.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   CTAPER     = ";
    for (size_t i = 0; i < CTAPER.size(); ++i) {
        file << to_string(CTAPER[i]);
        if (i < CTAPER.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   NTWIST     = " << NTWIST << ",\r\n";
    file << "\r\n";
    file << "   RTWIST     = ";
    for (size_t i = 0; i < RTWIST.size(); ++i) {
        file << to_string(RTWIST[i]);
        if (i < RTWIST.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   DTWIST     = ";
    for (size_t i = 0; i < DTWIST.size(); ++i) {
        file << to_string(DTWIST[i]);
        if (i < DTWIST.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   NAIRF      = " << NAIRF << ",\r\n";
    file << "\r\n";
    file << "   RAIRF      = ";
    for (size_t i = 0; i < RAIRF.size(); ++i) {
        file << to_string(RAIRF[i]);
        if (i < RAIRF.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   AIRFDATA   = ";
    for (size_t i = 0; i < AIRFDATA.size(); ++i) {
        file << "'" << wstring_to_string(AIRFDATA[i]) << "'";
        if (i < AIRFDATA.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   BLENDAIRF  = " << BLENDAIRF << ",\r\n";
    file << "   PERCENTR   = " << PERCENTR << ",\r\n";
    file << "   STALLDELAY = " << STALLDELAY << ",\r\n";
    file << "   VITERNA    = " << VITERNA << ",\r\n";
    file << "\r\n";
    file << "   NSWEEP     = " << NSWEEP << ",\r\n";
    file << "\r\n";
    file << "   RSWEEP     = ";
    for (size_t i = 0; i < RSWEEP.size(); ++i) {
        file << to_string(RSWEEP[i]);
        if (i < RSWEEP.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   LSWEEP     = ";
    for (size_t i = 0; i < LSWEEP.size(); ++i) {
        file << to_string(LSWEEP[i]);
        if (i < LSWEEP.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   NDIHED     = " << NDIHED << ",\r\n";
    file << "\r\n";
    file << "   RDIHED     = ";
    for (size_t i = 0; i < RDIHED.size(); ++i) {
        file << to_string(RDIHED[i]);
        if (i < RDIHED.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   LDIHED     = ";
    for (size_t i = 0; i < LDIHED.size(); ++i) {
        file << to_string(LDIHED[i]);
        if (i < LDIHED.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   NTWAX      = " << NTWAX << ",\r\n";
    file << "\r\n";
    file << "   RTWAX      = ";
    for (size_t i = 0; i < RTWAX.size(); ++i) {
        file << to_string(RTWAX[i]);
        if (i < RTWAX.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   LTWAX      = ";
    for (size_t i = 0; i < LTWAX.size(); ++i) {
        file << to_string(LTWAX[i]);
        if (i < LTWAX.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   NPIAX      = " << NPIAX << ",\r\n";
    file << "\r\n";
    file << "   RPIAX      = ";
    for (size_t i = 0; i < RPIAX.size(); ++i) {
        file << to_string(RPIAX[i]);
        if (i < RPIAX.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   LPIAX      = ";
    for (size_t i = 0; i < LPIAX.size(); ++i) {
        file << to_string(LPIAX[i]);
        if (i < LPIAX.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "&END\r\n";

    // &OPERATION section
    file << "&OPERATION\r\n";
    file << "   CHECK      = " << CHECK << ",\r\n";
    file << "\r\n";
    file << "   DESIGN     = " << DESIGN << ",\r\n";
    file << "\r\n";
    file << "   NTSR       = " << NTSR << ",\r\n";
    file << "   BTSR       = " << to_string(BTSR) << ",\r\n";
    file << "   ETSR       = " << to_string(ETSR) << ",\r\n";
    file << "\r\n";
    file << "   NPITCH     = " << NPITCH << ",\r\n";
    file << "   BPITCH     = " << to_string(BPITCH) << ",\r\n";
    file << "   EPITCH     = " << to_string(EPITCH) << ",\r\n";
    file << "\r\n";
    file << "   ANALYSIS   = " << ANALYSIS << ",\r\n";
    file << "\r\n";
    file << "   NANA       = " << NANA << ",\r\n";
    file << "\r\n";
    file << "   TSRANA     = ";
    for (size_t i = 0; i < TSRANA.size(); ++i) {
        file << to_string(TSRANA[i]);
        if (i < TSRANA.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   PITCHANA   = ";
    for (size_t i = 0; i < PITCHANA.size(); ++i) {
        file << to_string(PITCHANA[i]);
        if (i < PITCHANA.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   PREDICTION = " << PREDICTION << ",\r\n";
    file << "\r\n";
    file << "   BRADIUS    = " << to_string(BRADIUS) << ",\r\n";
    file << "\r\n";
    file << "   RHOAIR     = " << to_string(RHOAIR) << ",\r\n";
    file << "\r\n";
    file << "   MUAIR      = " << to_string(MUAIR) << ",\r\n";
    file << "\r\n";
    file << "   NPRE       = " << NPRE << ",\r\n";
    file << "\r\n";
    file << "   VWIND      = ";
    for (size_t i = 0; i < VWIND.size(); ++i) {
        file << to_string(VWIND[i]);
        if (i < VWIND.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   RPMPRE     = ";
    for (size_t i = 0; i < RPMPRE.size(); ++i) {
        file << to_string(RPMPRE[i]);
        if (i < RPMPRE.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "\r\n";
    file << "   PITCHPRE   = ";
    for (size_t i = 0; i < PITCHPRE.size(); ++i) {
        file << to_string(PITCHPRE[i]);
        if (i < PITCHPRE.size() - 1) file << ",\r\n                ";
        else file << ",\r\n";
    }
    file << "&END\r\n";

    // &SOLVER section
    file << "&SOLVER\r\n";
    file << "  METHOD     = " << METHOD << ",\r\n";
    file << "  JX         = " << JX << ",\r\n";
    file << "  COSDISTR   = " << COSDISTR << ",\r\n";
    file << "  GNUPLOT    = " << GNUPLOT << ",\r\n";
    file << "&END\r\n";

    // &HVM section
    file << "&HVM\r\n";
    file << "  WAKEEXP    = " << WAKEEXP << ",\r\n";
    file << "  DX0        = " << to_string(DX0) << ",\r\n";
    file << "  XSTR       = " << to_string(XSTR) << ",\r\n";
    file << "  XTREFFTZ   = " << to_string(XTREFFTZ) << ",\r\n";
    file << "  NSEC       = " << NSEC << ",\r\n";
    file << "  IB         = " << IB << ",\r\n";
    file << "  DIP        = " << DIP << ",\r\n";
    file << "  OMRELAX    = " << to_string(OMRELAX) << ",\r\n";
    file << "  AVISC      = " << to_string(AVISC) << ",\r\n";
    file << "  NACMOD     = " << NACMOD << ",\r\n";
    file << "  LN         = " << to_string(LN) << ",\r\n";
    file << "  HN         = " << to_string(HN) << ",\r\n";
    file << "  XN         = " << to_string(XN) << ",\r\n";
    file << "&END\r\n";
    // &BEMT section
    file << "&BEMT\r\n";
    file << "  RLOSS      = " << RLOSS << ",\r\n";
    file << "  TLOSS      = " << tipLoss << ",\r\n";
    file << "  AXRELAX    = " << to_string(AXRELAX) << ",\r\n";
    file << "  ATRELAX    = " << to_string(ATRELAX) << ",\r\n";
    file << "&END\r\n";

    // &OPTI section
    file << "&OPTI\r\n";
    file << "  OPTIM      = " << OPTIM << ",\r\n";
    file << "&END\r\n";

    file.close();
}