#include "Container.h"
#include "Button.h"
#include "Label.h"
#include "InputField.h"
#include "Logger.h"
#include "HelperFunctions.h"
#include "BEMTOutputParser.h"
#include <algorithm>
#include <thread>
#include <fstream>

// Constructor: Initialize with parent window and position/size
Container::Container(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height, const std::wstring& xturbExeName)
    : Window(), x(x), y(y), width(width), height(height),
    hFontRegular(nullptr), hFontBold(nullptr), nextControlId((HMENU)1001), currentY(0), totalHeight(0), scrollPos(0),
    nameInput(nullptr), bnInput(nullptr), rootInput(nullptr), ntaperInput(nullptr), rtaperInput(nullptr),
    ctaperInput(nullptr), ntwistInput(nullptr), rtwistInput(nullptr), dtwistInput(nullptr), nairfInput(nullptr),
    rairfInput(nullptr), airfdataInput(nullptr), blendairfInput(nullptr), percentrInput(nullptr),
    stalldelayInput(nullptr), viternaInput(nullptr), nsweepInput(nullptr), rsweepInput(nullptr),
    lsweepInput(nullptr), ndihedInput(nullptr), rdihedInput(nullptr), ldihedInput(nullptr),
    ntwaxInput(nullptr), rtwaxInput(nullptr), ltwaxInput(nullptr), npiaxInput(nullptr),
    rpiaxInput(nullptr), lpiaxInput(nullptr), checkInput(nullptr), designInput(nullptr),
    ntsrInput(nullptr), btsrInput(nullptr), etsrInput(nullptr), npitchInput(nullptr),
    bpitchInput(nullptr), epitchInput(nullptr), analysisInput(nullptr), nanaInput(nullptr),
    tsranaInput(nullptr), pitchanaInput(nullptr), predictionInput(nullptr), bradiusInput(nullptr),
    rhoairInput(nullptr), muairInput(nullptr), npreInput(nullptr), vwindInput(nullptr),
    rpmpresInput(nullptr), pitchpreInput(nullptr), methodInput(nullptr), jxInput(nullptr),
    cosdistrInput(nullptr), gnuplotInput(nullptr), aviscInput(nullptr), rlossInput(nullptr),
    tiplossInput(nullptr), axrelaxInput(nullptr), atrelaxInput(nullptr), optimInput(nullptr),
    twistGraph(nullptr), chordGraph(nullptr), xturbRunner(nullptr), exeDir(L"")
{
    this->hInstance = hInstance;
    this->hwnd = parent;

    wchar_t exePath[MAX_PATH];
    if (GetModuleFileNameW(nullptr, exePath, MAX_PATH) > 0) {
        std::wstring fullPath = exePath;
        size_t lastSlash = fullPath.find_last_of(L"\\");
        if (lastSlash != std::wstring::npos) {
            exeDir = fullPath.substr(0, lastSlash + 1);
            Logger::logError(L"exeDir set to: " + exeDir);
        }
    }
    else {
        Logger::logError(L"Failed to get executable directory!");
        exeDir = L".";
    }

    // Initialize XTurbRunner with the provided executable name
    xturbRunner = new XTurbRunner(exeDir + xturbExeName);
}

// Destructor: Clean up resources
Container::~Container() {
    if (hFontRegular) {
        DeleteObject(hFontRegular);
        hFontRegular = nullptr;
    }
    if (hFontBold) {
        DeleteObject(hFontBold);
        hFontBold = nullptr;
    }
    for (Control* control : controls) {
        delete control;
    }
    controls.clear();
    delete twistGraph;
    delete chordGraph;
    delete xturbRunner;
    for (auto window : displayWindows) delete window; // Clean up all display windows
    for (auto* selector : fileSelectors) delete selector;
    xturbRunner = nullptr;
    displayWindows.clear();
    fileSelectors.clear();
}

// Generate a unique control ID
HMENU Container::generateControlId() {
    HMENU id = nextControlId;
    nextControlId = (HMENU)((UINT_PTR)nextControlId + 1);
    return id;
}

// Add a control with layout management
void Container::addControl(Control* control, int width, int height) {
    control->create();
    if (control->getHandle()) {
        controls.push_back(control);
        currentY += height + SPACING;
        totalHeight = currentY;
    }
    else {
        Logger::logError(L"Failed to add control to container!");
        delete control;
    }
}

// Add a labeled input field pair
void Container::addLabeledInput(const std::wstring& labelText, InputField*& inputField, int labelWidth, int inputWidth, int height) {
    int labelY = currentY;
    Label* label = new Label(hwnd, hInstance, 0, labelY - scrollPos, labelWidth, height, labelText);
    label->create();
    if (label->getHandle()) {
        controls.push_back(label);
        if (hFontRegular) {
            label->setFont(hFontRegular);
        }
    }
    else {
        Logger::logError(L"Failed to create label: " + labelText);
        delete label;
    }

    inputField = new InputField(hwnd, hInstance, labelWidth + 10, labelY - scrollPos, inputWidth, height, generateControlId());
    inputField->create();
    if (inputField->getHandle()) {
        controls.push_back(inputField);
        if (hFontRegular) {
            inputField->setFont(hFontRegular);
        }
    }
    else {
        Logger::logError(L"Failed to create input field for: " + labelText);
        delete inputField;
        inputField = nullptr;
    }
    currentY += height + SPACING;
    totalHeight = currentY;
}

// Update the scroll range based on content height
void Container::updateScrollRange() {
    SCROLLINFO si = { 0 };
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = totalHeight;
    si.nPage = height;
    si.nPos = (std::min)(scrollPos, (std::max)(0, totalHeight - height));
    SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
    scrollPos = si.nPos;
}
// Update both graphs
void Container::updateGraphs() {
    if (twistGraph) {
        InvalidateRect(twistGraph->getHandle(), nullptr, TRUE);
        UpdateWindow(twistGraph->getHandle());
    }
    if (chordGraph) {
        InvalidateRect(chordGraph->getHandle(), nullptr, TRUE);
        UpdateWindow(chordGraph->getHandle());
    }
}
/*
// Validate all input fields
bool Container::validateInputs(std::wstring& errorMessage) {
    errorMessage.clear();

    Logger::logError(L"Validating inputs...");
    if (nameInput) Logger::logError(L"Name: " + nameInput->getText());
    if (bnInput) Logger::logError(L"BN: " + bnInput->getText());
    if (rootInput) Logger::logError(L"Root: " + rootInput->getText());
    if (ntaperInput) Logger::logError(L"NTaper: " + ntaperInput->getText());
    if (rtaperInput) Logger::logError(L"RTaper: " + rtaperInput->getText());

    // Helper lambda to check if a string is a valid number
   // Helper lambda to check if a string is a valid number
    auto isValidNumber = [](const std::wstring& str) -> bool {
        if (str.empty()) return false;
        try {
            size_t pos;
            std::string strConverted = wstring_to_string(str); // Convert wstring to string
            double value = std::stod(strConverted, &pos); // Use the return value
            bool valid = pos == strConverted.length();
            if (!valid) {
                Logger::logError(L"Invalid number format: " + str);
            }
            return valid;
        }
        catch (...) {
            Logger::logError(L"Exception parsing number: " + str);
            return false;
        }
        };

    // Helper lambda to check if a string is a valid integer
    auto isValidInteger = [](const std::wstring& str) -> bool {
        if (str.empty()) return false;
        try {
            size_t pos;
            std::string strConverted = wstring_to_string(str); // Convert wstring to string
            int value = std::stoi(strConverted, &pos); // Use the return value
            bool valid = pos == strConverted.length();
            if (!valid) {
                Logger::logError(L"Invalid integer format: " + str);
            }
            return valid;
        }
        catch (...) {
            Logger::logError(L"Exception parsing integer: " + str);
            return false;
        }
        };

    // &BLADE section validation
    if (nameInput) {
        std::wstring name = nameInput->getText();
        if (name.empty()) {
            errorMessage += L"Blade Name cannot be empty.\n";
        }
    }

    if (bnInput) {
        std::wstring bnText = bnInput->getText();
        if (!isValidInteger(bnText)) {
            errorMessage += L"Number of Blades must be a valid integer.\n";
        }
        else {
            int bn = _wtoi(bnText.c_str());
            if (bn <= 0) {
                errorMessage += L"Number of Blades must be greater than 0.\n";
            }
        }
    }

    if (rootInput) {
        std::wstring rootText = rootInput->getText();
        if (!isValidNumber(rootText)) {
            errorMessage += L"Root Radius must be a valid number.\n";
        }
        else {
            double root = _wtof(rootText.c_str());
            if (root <= 0) {
                errorMessage += L"Root Radius must be greater than 0.\n";
            }
        }
    }

    if (ntaperInput && rtaperInput && ctaperInput) {
        std::wstring ntaperText = ntaperInput->getText();
        if (!isValidInteger(ntaperText)) {
            errorMessage += L"Number of Taper Points must be a valid integer.\n";
        }
        else {
            int ntaper = _wtoi(ntaperText.c_str());
            if (ntaper <= 0) {
                errorMessage += L"Number of Taper Points must be greater than 0.\n";
            }
            else {
                auto rtaperValues = parseCommaSeparatedDoubles(rtaperInput->getText());
                auto ctaperValues = parseCommaSeparatedDoubles(ctaperInput->getText());
                if (rtaperValues.size() != static_cast<size_t>(ntaper)) {
                    errorMessage += L"Number of Radial Taper Points (" + std::to_wstring(rtaperValues.size()) +
                        L") does not match NTAPER (" + ntaperText + L").\n";
                }
                if (ctaperValues.size() != static_cast<size_t>(ntaper)) {
                    errorMessage += L"Number of Chord Taper Points (" + std::to_wstring(ctaperValues.size()) +
                        L") does not match NTAPER (" + ntaperText + L").\n";
                }
                for (double r : rtaperValues) {
                    if (r < 0 || r > 1) {
                        errorMessage += L"Radial Taper Points must be between 0 and 1.\n";
                        break;
                    }
                }
                for (double c : ctaperValues) {
                    if (c <= 0) {
                        errorMessage += L"Chord Taper Points must be greater than 0.\n";
                        break;
                    }
                }
            }
        }
    }

    if (ntwistInput && rtwistInput && dtwistInput) {
        std::wstring ntwistText = ntwistInput->getText();
        if (!isValidInteger(ntwistText)) {
            errorMessage += L"Number of Twist Points must be a valid integer.\n";
        }
        else {
            int ntwist = _wtoi(ntwistText.c_str());
            if (ntwist <= 0) {
                errorMessage += L"Number of Twist Points must be greater than 0.\n";
            }
            else {
                auto rtwistValues = parseCommaSeparatedDoubles(rtwistInput->getText());
                auto dtwistValues = parseCommaSeparatedDoubles(dtwistInput->getText());
                if (rtwistValues.size() != static_cast<size_t>(ntwist)) {
                    errorMessage += L"Number of Radial Twist Points (" + std::to_wstring(rtwistValues.size()) +
                        L") does not match NTWIST (" + ntwistText + L").\n";
                }
                if (dtwistValues.size() != static_cast<size_t>(ntwist)) {
                    errorMessage += L"Number of Twist Angles (" + std::to_wstring(dtwistValues.size()) +
                        L") does not match NTWIST (" + ntwistText + L").\n";
                }
                for (double r : rtwistValues) {
                    if (r < 0 || r > 1) {
                        errorMessage += L"Radial Twist Points must be between 0 and 1.\n";
                        break;
                    }
                }
            }
        }
    }

    if (nairfInput && rairfInput && airfdataInput) {
        std::wstring nairfText = nairfInput->getText();
        if (!isValidInteger(nairfText)) {
            errorMessage += L"Number of Airfoil Data Sets must be a valid integer.\n";
        }
        else {
            int nairf = _wtoi(nairfText.c_str());
            if (nairf <= 0) {
                errorMessage += L"Number of Airfoil Data Sets must be greater than 0.\n";
            }
            else {
                auto rairfValues = parseCommaSeparatedDoubles(rairfInput->getText());
                auto airfdataValues = parseCommaSeparatedWStrings(airfdataInput->getText());
                if (rairfValues.size() != static_cast<size_t>(nairf)) {
                    errorMessage += L"Number of Radial Airfoil Points (" + std::to_wstring(rairfValues.size()) +
                        L") does not match NAIRF (" + nairfText + L").\n";
                }
                if (airfdataValues.size() != static_cast<size_t>(nairf)) {
                    errorMessage += L"Number of Airfoil Data Files (" + std::to_wstring(airfdataValues.size()) +
                        L") does not match NAIRF (" + nairfText + L").\n";
                }
                for (double r : rairfValues) {
                    if (r < 0 || r > 1) {
                        errorMessage += L"Radial Airfoil Points must be between 0 and 1.\n";
                        break;
                    }
                }
                for (const auto& path : airfdataValues) {
                    if (path.empty()) {
                        errorMessage += L"Airfoil Data File paths cannot be empty.\n";
                        break;
                    }
                }
            }
        }
    }

    if (blendairfInput) {
        std::wstring blendairfText = blendairfInput->getText();
        if (!isValidInteger(blendairfText)) {
            errorMessage += L"Blend Airfoil Data must be a valid integer (0 or 1).\n";
        }
        else {
            int blendairf = _wtoi(blendairfText.c_str());
            if (blendairf != 0 && blendairf != 1) {
                errorMessage += L"Blend Airfoil Data must be 0 (No) or 1 (Yes).\n";
            }
        }
    }

    if (percentrInput) {
        std::wstring percentrText = percentrInput->getText();
        if (!isValidInteger(percentrText)) {
            errorMessage += L"Blending Percentage must be a valid integer.\n";
        }
        else {
            int percentr = _wtoi(percentrText.c_str());
            if (percentr < 0 || percentr > 100) {
                errorMessage += L"Blending Percentage must be between 0 and 100.\n";
            }
        }
    }

    if (stalldelayInput) {
        std::wstring stalldelayText = stalldelayInput->getText();
        if (!isValidInteger(stalldelayText)) {
            errorMessage += L"Stall Delay Model must be a valid integer (0, 1, or 2).\n";
        }
        else {
            int stalldelay = _wtoi(stalldelayText.c_str());
            if (stalldelay < 0 || stalldelay > 2) {
                errorMessage += L"Stall Delay Model must be 0 (No), 1 (Selig & Du), or 2 (Solution-Based).\n";
            }
        }
    }

    if (viternaInput) {
        std::wstring viternaText = viternaInput->getText();
        if (!isValidInteger(viternaText)) {
            errorMessage += L"Viterna Correction must be a valid integer (0 or 1).\n";
        }
        else {
            int viterna = _wtoi(viternaText.c_str());
            if (viterna != 0 && viterna != 1) {
                errorMessage += L"Viterna Correction must be 0 (No) or 1 (Yes).\n";
            }
        }
    }

    if (nsweepInput && rsweepInput && lsweepInput) {
        std::wstring nsweepText = nsweepInput->getText();
        if (!isValidInteger(nsweepText)) {
            errorMessage += L"Number of Sweep Points must be a valid integer.\n";
        }
        else {
            int nsweep = _wtoi(nsweepText.c_str());
            if (nsweep <= 0) {
                errorMessage += L"Number of Sweep Points must be greater than 0.\n";
            }
            else {
                auto rsweepValues = parseCommaSeparatedDoubles(rsweepInput->getText());
                auto lsweepValues = parseCommaSeparatedDoubles(lsweepInput->getText());
                if (rsweepValues.size() != static_cast<size_t>(nsweep)) {
                    errorMessage += L"Number of Radial Sweep Points (" + std::to_wstring(rsweepValues.size()) +
                        L") does not match NSWEEP (" + nsweepText + L").\n";
                }
                if (lsweepValues.size() != static_cast<size_t>(nsweep)) {
                    errorMessage += L"Number of Sweep Lengths (" + std::to_wstring(lsweepValues.size()) +
                        L") does not match NSWEEP (" + nsweepText + L").\n";
                }
                for (double r : rsweepValues) {
                    if (r < 0 || r > 1) {
                        errorMessage += L"Radial Sweep Points must be between 0 and 1.\n";
                        break;
                    }
                }
            }
        }
    }

    if (ndihedInput && rdihedInput && ldihedInput) {
        std::wstring ndihedText = ndihedInput->getText();
        if (!isValidInteger(ndihedText)) {
            errorMessage += L"Number of Dihedral Points must be a valid integer.\n";
        }
        else {
            int ndihed = _wtoi(ndihedText.c_str());
            if (ndihed <= 0) {
                errorMessage += L"Number of Dihedral Points must be greater than 0.\n";
            }
            else {
                auto rdihedValues = parseCommaSeparatedDoubles(rdihedInput->getText());
                auto ldihedValues = parseCommaSeparatedDoubles(ldihedInput->getText());
                if (rdihedValues.size() != static_cast<size_t>(ndihed)) {
                    errorMessage += L"Number of Radial Dihedral Points (" + std::to_wstring(rdihedValues.size()) +
                        L") does not match NDIHED (" + ndihedText + L").\n";
                }
                if (ldihedValues.size() != static_cast<size_t>(ndihed)) {
                    errorMessage += L"Number of Dihedral Lengths (" + std::to_wstring(ldihedValues.size()) +
                        L") does not match NDIHED (" + ndihedText + L").\n";
                }
                for (double r : rdihedValues) {
                    if (r < 0 || r > 1) {
                        errorMessage += L"Radial Dihedral Points must be between 0 and 1.\n";
                        break;
                    }
                }
            }
        }
    }

    if (ntwaxInput && rtwaxInput && ltwaxInput) {
        std::wstring ntwaxText = ntwaxInput->getText();
        if (!isValidInteger(ntwaxText)) {
            errorMessage += L"Number of Twist Axis Points must be a valid integer.\n";
        }
        else {
            int ntwax = _wtoi(ntwaxText.c_str());
            if (ntwax <= 0) {
                errorMessage += L"Number of Twist Axis Points must be greater than 0.\n";
            }
            else {
                auto rtwaxValues = parseCommaSeparatedDoubles(rtwaxInput->getText());
                auto ltwaxValues = parseCommaSeparatedDoubles(ltwaxInput->getText());
                if (rtwaxValues.size() != static_cast<size_t>(ntwax)) {
                    errorMessage += L"Number of Radial Twist Axis Points (" + std::to_wstring(rtwaxValues.size()) +
                        L") does not match NTWAX (" + ntwaxText + L").\n";
                }
                if (ltwaxValues.size() != static_cast<size_t>(ntwax)) {
                    errorMessage += L"Number of Twist Axis Locations (" + std::to_wstring(ltwaxValues.size()) +
                        L") does not match NTWAX (" + ntwaxText + L").\n";
                }
                for (double r : rtwaxValues) {
                    if (r < 0 || r > 1) {
                        errorMessage += L"Radial Twist Axis Points must be between 0 and 1.\n";
                        break;
                    }
                }
            }
        }
    }

    if (npiaxInput && rpiaxInput && lpiaxInput) {
        std::wstring npiaxText = npiaxInput->getText();
        if (!isValidInteger(npiaxText)) {
            errorMessage += L"Number of Pitch Axis Points must be a valid integer.\n";
        }
        else {
            int npiax = _wtoi(npiaxText.c_str());
            if (npiax <= 0) {
                errorMessage += L"Number of Pitch Axis Points must be greater than 0.\n";
            }
            else {
                auto rpiaxValues = parseCommaSeparatedDoubles(rpiaxInput->getText());
                auto lpiaxValues = parseCommaSeparatedDoubles(lpiaxInput->getText());
                if (rpiaxValues.size() != static_cast<size_t>(npiax)) {
                    errorMessage += L"Number of Radial Pitch Axis Points (" + std::to_wstring(rpiaxValues.size()) +
                        L") does not match NPIAX (" + npiaxText + L").\n";
                }
                if (lpiaxValues.size() != static_cast<size_t>(npiax)) {
                    errorMessage += L"Number of Pitch Axis Locations (" + std::to_wstring(lpiaxValues.size()) +
                        L") does not match NPIAX (" + npiaxText + L").\n";
                }
                for (double r : rpiaxValues) {
                    if (r < 0 || r > 1) {
                        errorMessage += L"Radial Pitch Axis Points must be between 0 and 1.\n";
                        break;
                    }
                }
            }
        }
    }

    // &OPERATION section validation
    if (checkInput) {
        std::wstring checkText = checkInput->getText();
        if (!isValidInteger(checkText)) {
            errorMessage += L"Check Mode must be a valid integer (0 or 1).\n";
        }
        else {
            int check = _wtoi(checkText.c_str());
            if (check != 0 && check != 1) {
                errorMessage += L"Check Mode must be 0 (Proceed) or 1 (Review).\n";
            }
        }
    }

    if (designInput) {
        std::wstring designText = designInput->getText();
        if (!isValidInteger(designText)) {
            errorMessage += L"Design Mode must be a valid integer (0 or 1).\n";
        }
        else {
            int design = _wtoi(designText.c_str());
            if (design != 0 && design != 1) {
                errorMessage += L"Design Mode must be 0 (No) or 1 (Yes).\n";
            }
        }
    }

    if (ntsrInput) {
        std::wstring ntsrText = ntsrInput->getText();
        if (!isValidInteger(ntsrText)) {
            errorMessage += L"Number of TSR for Design must be a valid integer.\n";
        }
        else {
            int ntsr = _wtoi(ntsrText.c_str());
            if (ntsr <= 0) {
                errorMessage += L"Number of TSR for Design must be greater than 0.\n";
            }
        }
    }

    if (btsrInput) {
        std::wstring btsrText = btsrInput->getText();
        if (!isValidNumber(btsrText)) {
            errorMessage += L"Beginning TSR must be a valid number.\n";
        }
    }

    if (etsrInput) {
        std::wstring etsrText = etsrInput->getText();
        if (!isValidNumber(etsrText)) {
            errorMessage += L"Ending TSR must be a valid number.\n";
        }
    }

    if (npitchInput) {
        std::wstring npitchText = npitchInput->getText();
        if (!isValidInteger(npitchText)) {
            errorMessage += L"Number of Pitch Angles for Design must be a valid integer.\n";
        }
        else {
            int npitch = _wtoi(npitchText.c_str());
            if (npitch <= 0) {
                errorMessage += L"Number of Pitch Angles for Design must be greater than 0.\n";
            }
        }
    }

    if (bpitchInput) {
        std::wstring bpitchText = bpitchInput->getText();
        if (!isValidNumber(bpitchText)) {
            errorMessage += L"Beginning Pitch Angle must be a valid number.\n";
        }
    }

    if (epitchInput) {
        std::wstring epitchText = epitchInput->getText();
        if (!isValidNumber(epitchText)) {
            errorMessage += L"Ending Pitch Angle must be a valid number.\n";
        }
    }

    if (analysisInput) {
        std::wstring analysisText = analysisInput->getText();
        if (!isValidInteger(analysisText)) {
            errorMessage += L"Analysis Mode must be a valid integer (0 or 1).\n";
        }
        else {
            int analysis = _wtoi(analysisText.c_str());
            if (analysis != 0 && analysis != 1) {
                errorMessage += L"Analysis Mode must be 0 (No) or 1 (Yes).\n";
            }
        }
    }

    if (nanaInput && tsranaInput && pitchanaInput) {
        std::wstring nanaText = nanaInput->getText();
        if (!isValidInteger(nanaText)) {
            errorMessage += L"Number of Analysis Cases must be a valid integer.\n";
        }
        else {
            int nana = _wtoi(nanaText.c_str());
            if (nana <= 0) {
                errorMessage += L"Number of Analysis Cases must be greater than 0.\n";
            }
            else {
                auto tsranaValues = parseCommaSeparatedDoubles(tsranaInput->getText());
                auto pitchanaValues = parseCommaSeparatedDoubles(pitchanaInput->getText());
                if (tsranaValues.size() != static_cast<size_t>(nana)) {
                    errorMessage += L"Number of TSR Analysis Values (" + std::to_wstring(tsranaValues.size()) +
                        L") does not match NANA (" + nanaText + L").\n";
                }
                if (pitchanaValues.size() != static_cast<size_t>(nana)) {
                    errorMessage += L"Number of Pitch Analysis Values (" + std::to_wstring(pitchanaValues.size()) +
                        L") does not match NANA (" + nanaText + L").\n";
                }
                for (double tsr : tsranaValues) {
                    if (tsr <= 0) {
                        errorMessage += L"TSR Analysis Values must be greater than 0.\n";
                        break;
                    }
                }
            }
        }
    }

    if (predictionInput) {
        std::wstring predictionText = predictionInput->getText();
        if (!isValidInteger(predictionText)) {
            errorMessage += L"Prediction Mode must be a valid integer (0 or 1).\n";
        }
        else {
            int prediction = _wtoi(predictionText.c_str());
            if (prediction != 0 && prediction != 1) {
                errorMessage += L"Prediction Mode must be 0 (No) or 1 (Yes).\n";
            }
        }
    }

    if (bradiusInput) {
        std::wstring bradiusText = bradiusInput->getText();
        if (!isValidNumber(bradiusText)) {
            errorMessage += L"Blade Radius must be a valid number.\n";
        }
        else {
            double bradius = _wtof(bradiusText.c_str());
            if (bradius <= 0) {
                errorMessage += L"Blade Radius must be greater than 0.\n";
            }
        }
    }

    if (rhoairInput) {
        std::wstring rhoairText = rhoairInput->getText();
        if (!isValidNumber(rhoairText)) {
            errorMessage += L"Air Density must be a valid number.\n";
        }
        else {
            double rhoair = _wtof(rhoairText.c_str());
            if (rhoair <= 0) {
                errorMessage += L"Air Density must be greater than 0.\n";
            }
        }
    }

    if (muairInput) {
        std::wstring muairText = muairInput->getText();
        if (!isValidNumber(muairText)) {
            errorMessage += L"Air Viscosity must be a valid number.\n";
        }
        else {
            double muair = _wtof(muairText.c_str());
            if (muair <= 0) {
                errorMessage += L"Air Viscosity must be greater than 0.\n";
            }
        }
    }

    if (npreInput && vwindInput && rpmpresInput && pitchpreInput) {
        std::wstring npreText = npreInput->getText();
        if (!isValidInteger(npreText)) {
            errorMessage += L"Number of Prediction Cases must be a valid integer.\n";
        }
        else {
            int npre = _wtoi(npreText.c_str());
            if (npre <= 0) {
                errorMessage += L"Number of Prediction Cases must be greater than 0.\n";
            }
            else {
                auto vwindValues = parseCommaSeparatedDoubles(vwindInput->getText());
                auto rpmpresValues = parseCommaSeparatedDoubles(rpmpresInput->getText());
                auto pitchpreValues = parseCommaSeparatedDoubles(pitchpreInput->getText());
                if (vwindValues.size() != static_cast<size_t>(npre)) {
                    errorMessage += L"Number of Wind Speeds (" + std::to_wstring(vwindValues.size()) +
                        L") does not match NPRE (" + npreText + L").\n";
                }
                if (rpmpresValues.size() != static_cast<size_t>(npre)) {
                    errorMessage += L"Number of RPM Values (" + std::to_wstring(rpmpresValues.size()) +
                        L") does not match NPRE (" + npreText + L").\n";
                }
                if (pitchpreValues.size() != static_cast<size_t>(npre)) {
                    errorMessage += L"Number of Pitch Prediction Values (" + std::to_wstring(pitchpreValues.size()) +
                        L") does not match NPRE (" + npreText + L").\n";
                }
                for (double v : vwindValues) {
                    if (v <= 0) {
                        errorMessage += L"Wind Speeds must be greater than 0.\n";
                        break;
                    }
                }
                for (double rpm : rpmpresValues) {
                    if (rpm <= 0) {
                        errorMessage += L"RPM Values must be greater than 0.\n";
                        break;
                    }
                }
            }
        }
    }

    // &SOLVER section validation
    if (methodInput) {
        std::wstring methodText = methodInput->getText();
        if (!isValidInteger(methodText)) {
            errorMessage += L"Solver Method must be a valid integer (1 or 2).\n";
        }
        else {
            int method = _wtoi(methodText.c_str());
            if (method != 1 && method != 2) {
                errorMessage += L"Solver Method must be 1 (BEMT) or 2 (HVM).\n";
            }
        }
    }

    if (jxInput) {
        std::wstring jxText = jxInput->getText();
        if (!isValidInteger(jxText)) {
            errorMessage += L"Number of Radial Stations must be a valid integer.\n";
        }
        else {
            int jx = _wtoi(jxText.c_str());
            if (jx <= 0) {
                errorMessage += L"Number of Radial Stations must be greater than 0.\n";
            }
            if (jx % 2 == 0) {
                errorMessage += L"Number of Radial Stations must be an odd number.\n";
            }
        }
    }

    if (cosdistrInput) {
        std::wstring cosdistrText = cosdistrInput->getText();
        if (!isValidInteger(cosdistrText)) {
            errorMessage += L"Cosine Distribution must be a valid integer (0 or 1).\n";
        }
        else {
            int cosdistr = _wtoi(cosdistrText.c_str());
            if (cosdistr != 0 && cosdistr != 1) {
                errorMessage += L"Cosine Distribution must be 0 (No) or 1 (Yes).\n";
            }
        }
    }

    if (gnuplotInput) {
        std::wstring gnuplotText = gnuplotInput->getText();
        if (!isValidInteger(gnuplotText)) {
            errorMessage += L"Gnuplot Output must be a valid integer (0, 1, or 2).\n";
        }
        else {
            int gnuplot = _wtoi(gnuplotText.c_str());
            if (gnuplot < 0 || gnuplot > 2) {
                errorMessage += L"Gnuplot Output must be 0 (No), 1 (Yes), or 2 (Global).\n";
            }
        }
    }

    // &HVM section validation
    if (aviscInput) {
        std::wstring aviscText = aviscInput->getText();
        if (!isValidNumber(aviscText)) {
            errorMessage += L"Artificial Viscosity must be a valid number.\n";
        }
        else {
            double avisc = _wtof(aviscText.c_str());
            if (avisc < 0) {
                errorMessage += L"Artificial Viscosity must be non-negative.\n";
            }
        }
    }

    // &BEMT section validation
    if (rlossInput) {
        std::wstring rlossText = rlossInput->getText();
        if (!isValidInteger(rlossText)) {
            errorMessage += L"Root Loss Factor must be a valid integer (0 or 1).\n";
        }
        else {
            int rloss = _wtoi(rlossText.c_str());
            if (rloss != 0 && rloss != 1) {
                errorMessage += L"Root Loss Factor must be 0 (No) or 1 (Yes).\n";
            }
        }
    }

    if (tiplossInput) {
        std::wstring tiplossText = tiplossInput->getText();
        if (!isValidInteger(tiplossText)) {
            errorMessage += L"Tip Loss Factor must be a valid integer (0, 1, or 2).\n";
        }
        else {
            int tiploss = _wtoi(tiplossText.c_str());
            if (tiploss < 0 || tiploss > 2) {
                errorMessage += L"Tip Loss Factor must be 0 (No), 1 (AeroDyn), or 2 (Advanced).\n";
            }
        }
    }

    if (axrelaxInput) {
        std::wstring axrelaxText = axrelaxInput->getText();
        if (!isValidNumber(axrelaxText)) {
            errorMessage += L"Axial Relaxation Factor must be a valid number.\n";
        }
        else {
            double axrelax = _wtof(axrelaxText.c_str());
            if (axrelax <= 0 || axrelax > 1) {
                errorMessage += L"Axial Relaxation Factor must be between 0 and 1.\n";
            }
        }
    }

    if (atrelaxInput) {
        std::wstring atrelaxText = atrelaxInput->getText();
        if (!isValidNumber(atrelaxText)) {
            errorMessage += L"Tangential Relaxation Factor must be a valid number.\n";
        }
        else {
            double atrelax = _wtof(atrelaxText.c_str());
            if (atrelax <= 0 || atrelax > 1) {
                errorMessage += L"Tangential Relaxation Factor must be between 0 and 1.\n";
            }
        }
    }

    // &OPTI section validation
    if (optimInput) {
        std::wstring optimText = optimInput->getText();
        if (!isValidInteger(optimText)) {
            errorMessage += L"Inverse Design Mode must be a valid integer (0, 1, 2, or 3).\n";
        }
        else {
            int optim = _wtoi(optimText.c_str());
            if (optim < 0 || optim > 3) {
                errorMessage += L"Inverse Design Mode must be 0 (No), 1 (Chord & Twist), 2 (Twist & CL), or 3 (Chord & CL).\n";
            }
        }
    }

    return errorMessage.empty();
}
*/
// Validate all input fields
bool Container::validateInputs(std::wstring& errorMessage) {
    errorMessage.clear();

    // Debug: Log all input values
    Logger::logError(L"Validating inputs...");
    if (nameInput) Logger::logError(L"Name: " + nameInput->getText());
    if (bnInput) Logger::logError(L"BN: " + bnInput->getText());
    if (rootInput) Logger::logError(L"Root: " + rootInput->getText());
    if (ntaperInput) Logger::logError(L"NTaper: " + ntaperInput->getText());
    if (rtaperInput) Logger::logError(L"RTaper: " + rtaperInput->getText());
    // Add similar logs for other fields as needed

    // Temporarily disable all validation checks
    Logger::logError(L"Validation disabled - assuming all inputs are valid.");
    return true;
}

// Create the container as a child window
void Container::create(HINSTANCE hInstance, int nCmdShow) {
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = Window::WndProc;
    wcex.hInstance = hInstance;
    wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wcex.lpszClassName = L"ContainerClass";

    if (!RegisterClassExW(&wcex)) {
        Logger::logError(L"Failed to register container class!");
        return;
    }

    hwnd = CreateWindowW(L"ContainerClass", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL,
        x, y, width, height, hwnd, nullptr, hInstance, this);

    if (!hwnd) {
        Logger::logError(L"Failed to create container!");
        return;
    }

    // Create fonts
    hFontRegular = CreateFontW(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    hFontBold = CreateFontW(-12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    if (!hFontRegular) {
        Logger::logError(L"Failed to create regular font!");
    }
    if (!hFontBold) {
        Logger::logError(L"Failed to create bold font!");
    }

    // Add controls with layout
    currentY = 0;

    // Define standard dimensions for labels and input fields
    const int standardLabelWidth = 175;
    const int standardLabelHeight = 20;
    const int standardInputWidth = 150;
    const int standardInputHeight = 20;

    // Header label
    Label* headerLabel = new Label(hwnd, hInstance, 0, currentY - scrollPos, 300, 20, L"Blade Parameters (Geometric Definitions)");
    addControl(headerLabel, 150, 20);
    if (hFontBold) {
        headerLabel->setFont(hFontBold);
    }

    // &BLADE section fields (using standard dimensions)
    addLabeledInput(L"Design Name:", nameInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    nameInput->setDefaultText(L"NREL-PhaseVI");
    addLabeledInput(L"Number of Blades:", bnInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    bnInput->setDefaultText(L"2");
    addLabeledInput(L"Root Radius:", rootInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    rootInput->setDefaultText(L"0.25");
    addLabeledInput(L"Number of Chord Definitions:", ntaperInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    ntaperInput->setDefaultText(L"2");
    addLabeledInput(L"Radial Location / Radius:", rtaperInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    rtaperInput->setDefaultText(L"0.25,1.00");
    addLabeledInput(L"Blade Chord / Radius:", ctaperInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    ctaperInput->setDefaultText(L"0.1465,0.0707");
    addLabeledInput(L"Number of Twist Definitions:", ntwistInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    ntwistInput->setDefaultText(L"20");
    addLabeledInput(L"Radial Location / Radius:", rtwistInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    rtwistInput->setDefaultText(L"0.250,0.267,0.300,0.328,0.388,0.449,0.466,0.509,0.570,0.631,0.633,0.691,0.752,0.800,0.812,0.873,0.934,0.950,0.994,1.000");
    addLabeledInput(L"Twist Angles (deg):", dtwistInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    dtwistInput->setDefaultText(L"20.040,18.074,14.292,11.909,7.979,5.308,4.715,3.425,2.083,1.150,1.115,0.494,-0.015,-0.381,-0.475,-0.920,-1.352,-1.469,-1.775,-1.816");
    addLabeledInput(L"Number of Airfoil Data Sets:", nairfInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    nairfInput->setDefaultText(L"1");
    addLabeledInput(L"Radial Starting Loc. of Airfoil(s):", rairfInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    rairfInput->setDefaultText(L"0.25");
    addLabeledInput(L"Path to XFOIL Polar of Airfoil(s):", airfdataInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    airfdataInput->setDefaultText(L"./s80905.polar");
    addLabeledInput(L"Blend Airfoil Data (0/1):", blendairfInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    blendairfInput->setDefaultText(L"0");
    addLabeledInput(L"Blending Percentage (%):", percentrInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    percentrInput->setDefaultText(L"2");
    addLabeledInput(L"Stall Delay Model (0-2):", stalldelayInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    stalldelayInput->setDefaultText(L"0");
    addLabeledInput(L"Viterna Extrapolation (0/1):", viternaInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    viternaInput->setDefaultText(L"0");
    addLabeledInput(L"Number of Sweep Definitions:", nsweepInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    nsweepInput->setDefaultText(L"2");
    addLabeledInput(L"Radial Location / Radius:", rsweepInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    rsweepInput->setDefaultText(L"0.25,1.00");
    addLabeledInput(L"Blade Sweep / Radius:", lsweepInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    lsweepInput->setDefaultText(L"0.00,0.00");
    addLabeledInput(L"Number of Dihedral Definitions:", ndihedInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    ndihedInput->setDefaultText(L"2");
    addLabeledInput(L"Radial Location / Radius:", rdihedInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    rdihedInput->setDefaultText(L"0.25,1.00");
    addLabeledInput(L"Blade Dihedral / Radius:", ldihedInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    ldihedInput->setDefaultText(L"0.00,0.00");
    addLabeledInput(L"Number of Twist Axis Definitions:", ntwaxInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    ntwaxInput->setDefaultText(L"2");
    addLabeledInput(L"Radial Location / Radius:", rtwaxInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    rtwaxInput->setDefaultText(L"0.25,1.00");
    addLabeledInput(L"Twist Axis / Chord:", ltwaxInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    ltwaxInput->setDefaultText(L"0.30,0.30");
    addLabeledInput(L"Number of Pitch Axis Definitions:", npiaxInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    npiaxInput->setDefaultText(L"2");
    addLabeledInput(L"Radial Location / Radius:", rpiaxInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    rpiaxInput->setDefaultText(L"0.25,1.00");
    addLabeledInput(L"Pitch Axis / Chord:", lpiaxInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    lpiaxInput->setDefaultText(L"0.30,0.30");

    // Add graphs for Twist and Chord distributions
    Label* twistGraphLabel = new Label(hwnd, hInstance, standardInputWidth + standardLabelWidth + 50, 10 - scrollPos, 150, 20, L"Twist Distribution");
    addControl(twistGraphLabel, 150, 20);
    if (hFontBold) {
        twistGraphLabel->setFont(hFontBold);
    }

    twistGraph = new TwistGraph(hwnd, hInstance, standardInputWidth+standardLabelWidth+50, 30 - scrollPos, 350, 250, rtwistInput, dtwistInput);
    twistGraph->create();
    currentY += 200 + SPACING;
    totalHeight = currentY;

    Label* chordGraphLabel = new Label(hwnd, hInstance, standardInputWidth + standardLabelWidth + 50, 300 - scrollPos, 150, 20, L"Chord Distribution");
    addControl(chordGraphLabel, 150, 20);
    if (hFontBold) {
        chordGraphLabel->setFont(hFontBold);
    }

    chordGraph = new ChordGraph(hwnd, hInstance, standardInputWidth + standardLabelWidth + 50, 320 - scrollPos, 350, 250, rtaperInput, ctaperInput);
    chordGraph->create();
    currentY += 200 + SPACING;
    totalHeight = currentY;

    currentY += -450;
    // &OPERATION section header
    Label* operationHeader = new Label(hwnd, hInstance, 0, currentY - scrollPos, 300, 20, L"Operation Parameters (Design, Analysis, Prediction)");
    addControl(operationHeader, 150, 20);
    if (hFontBold) {
        operationHeader->setFont(hFontBold);
    }
    // &OPERATION section fields (using standard dimensions)
    addLabeledInput(L"Check Mode (0/1):", checkInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    checkInput->setDefaultText(L"0");
    addLabeledInput(L"Design Mode (0/1):", designInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    designInput->setDefaultText(L"0");
    addLabeledInput(L"Number of Tip Speed Ratios:", ntsrInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    ntsrInput->setDefaultText(L"10");
    addLabeledInput(L"Minimum Tip Speed Ratio:", btsrInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    btsrInput->setDefaultText(L"2");
    addLabeledInput(L"Maximum Tip Speed Ratio:", etsrInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    etsrInput->setDefaultText(L"20");
    addLabeledInput(L"Number of Blade Pitch Angles:", npitchInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    npitchInput->setDefaultText(L"2");
    addLabeledInput(L"Minimum Pitch Angle:", bpitchInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    bpitchInput->setDefaultText(L"1.8");
    addLabeledInput(L"Maximum Pitch Angle:", epitchInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    epitchInput->setDefaultText(L"3.0");
    addLabeledInput(L"Analysis Mode (0/1):", analysisInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    analysisInput->setDefaultText(L"0");
    addLabeledInput(L"Number of Analysis Cases:", nanaInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    nanaInput->setDefaultText(L"2");
    addLabeledInput(L"TSR Analysis Values:", tsranaInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    tsranaInput->setDefaultText(L"7.585,5.418,6,8,10,12,14,16,18,20");
    addLabeledInput(L"Pitch Analysis Values:", pitchanaInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    pitchanaInput->setDefaultText(L"3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0,3.0");
    addLabeledInput(L"Prediction Mode (0/1):", predictionInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    predictionInput->setDefaultText(L"1");
    addLabeledInput(L"Blade Radius (m):", bradiusInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    bradiusInput->setDefaultText(L"5.03");
    addLabeledInput(L"Air Density (kg/m^3):", rhoairInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    rhoairInput->setDefaultText(L"1.225");
    addLabeledInput(L"Air Dynamic Viscosity [kg/(ms)]:", muairInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    muairInput->setDefaultText(L"1.8E-05");
    addLabeledInput(L"Number of Prediction Cases:", npreInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    npreInput->setDefaultText(L"1");
    addLabeledInput(L"Wind Speeds:", vwindInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    vwindInput->setDefaultText(L"7.0,7.0,9.0,10.0,11.0,13.0");
    addLabeledInput(L"RPM Values:", rpmpresInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    rpmpresInput->setDefaultText(L"72.0,72.0,72.0,72.0,72.0,72.0");
    addLabeledInput(L"Blade Pitch Angles (deg):", pitchpreInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    pitchpreInput->setDefaultText(L"3.0,3.0,3.0,3.0,3.0,3.0");

    // &SOLVER section header
    Label* solverHeader = new Label(hwnd, hInstance, 0, currentY - scrollPos, 300, 20, L"Solver Parameters (Solver and Grid Selection)");
    addControl(solverHeader, 150, 20);
    if (hFontBold) {
        solverHeader->setFont(hFontBold);
    }

    // &SOLVER section fields (using standard dimensions)
    addLabeledInput(L"Solver Method (1/2):", methodInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    methodInput->setDefaultText(L"1");
    addLabeledInput(L"Number of Radial Stations:", jxInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    jxInput->setDefaultText(L"41");
    addLabeledInput(L"Cosine Distribution (0/1):", cosdistrInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    cosdistrInput->setDefaultText(L"1");
    addLabeledInput(L"Gnuplot Output Files (0-2):", gnuplotInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    gnuplotInput->setDefaultText(L"2");

    // &HVM section header
    Label* hvmHeader = new Label(hwnd, hInstance, 0, currentY - scrollPos, 300, 20, L"Helicoidal Vortex Method (NO NEED TO CHANGE)");
    addControl(hvmHeader, 150, 20);
    if (hFontBold) {
        hvmHeader->setFont(hFontBold);
    }

    // &HVM section fields (using standard dimensions)
    addLabeledInput(L"Wake Expansion (0/1):", wakeexpInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    wakeexpInput->setDefaultText(L"1");
    addLabeledInput(L"Initial Mesh Step dx0:", DX0Input, standardLabelWidth, standardInputWidth, standardInputHeight);
    DX0Input->setDefaultText(L"1.E-04");
    addLabeledInput(L"End Mesh Stretching:", XSTRInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    XSTRInput->setDefaultText(L"1.0");
    addLabeledInput(L"Location of Trefftz Plane:", XTREFFTZInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    XTREFFTZInput->setDefaultText(L"1.0");
    addLabeledInput(L"Number of Sectors:", NSECInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    NSECInput->setDefaultText(L"20");
    addLabeledInput(L"Number of Segments:", ibInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    ibInput->setDefaultText(L"2");
    addLabeledInput(L"Interval for Boundary Nodes:", DIPInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    DIPInput->setDefaultText(L"1");
    addLabeledInput(L"Omega Relaxation Factor:", OMRELAXInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    OMRELAXInput->setDefaultText(L"0.2");
    addLabeledInput(L"Artificial Viscosity:", aviscInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    aviscInput->setDefaultText(L"0.5");
    addLabeledInput(L"Nacelle Mode (0/1):", NACMODInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    NACMODInput->setDefaultText(L"0");
    addLabeledInput(L"Nacelle 1/2 Length:", LNInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    LNInput->setDefaultText(L"0.05");
    addLabeledInput(L"Nacelle 1/2 Height:", HNInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    HNInput->setDefaultText(L"0.025");
    addLabeledInput(L"Axial Location of NAC Midpoint:", XNInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    XNInput->setDefaultText(L"0.0");

    // &BEMT section header
    Label* bemtHeader = new Label(hwnd, hInstance, 0, currentY - scrollPos, 350, 20, L"BEMT Parameters (Blade Element Momentum Theory)");
    addControl(bemtHeader, 150, 20);
    if (hFontBold) {
        bemtHeader->setFont(hFontBold);
    }

    // &BEMT section fields (using standard dimensions)
    addLabeledInput(L"Root Loss Factor (0/1):", rlossInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    rlossInput->setDefaultText(L"1");
    addLabeledInput(L"Tip Loss Factor (0-2):", tiplossInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    tiplossInput->setDefaultText(L"1");
    addLabeledInput(L"Axial Relaxation Factor:", axrelaxInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    axrelaxInput->setDefaultText(L"0.125");
    addLabeledInput(L"Tangential Relaxation Factor:", atrelaxInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    atrelaxInput->setDefaultText(L"0.125");

    // &OPTI section header
    Label* optiHeader = new Label(hwnd, hInstance, 0, currentY - scrollPos, 350, 20, L"Inverse Design and Optimization Parameters");
    addControl(optiHeader, 150, 20);
    if (hFontBold) {
        optiHeader->setFont(hFontBold);
    }

    // &OPTI section fields (using standard dimensions)
    addLabeledInput(L"Inverse Design Mode (0-3):", optimInput, standardLabelWidth, standardInputWidth, standardInputHeight);
    optimInput->setDefaultText(L"0");

    // Save Button
    Button* button = new Button(hwnd, hInstance, 0, currentY - scrollPos, 100, 30, L"Save", generateControlId());
    addControl(button, 100, 30);
    if (hFontRegular) {
        button->setFont(hFontRegular);
    }

    // Run XTurb Button
    Button* runButton = new Button(hwnd, hInstance, 235, currentY - 40 - scrollPos, 100, 30, L"Run XTurb", generateControlId());
    addControl(runButton, 100, 30);
    if (hFontRegular) {
        runButton->setFont(hFontRegular);
    }

    // Update scroll range
    updateScrollRange();

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
}
// Handle messages specific to the container
LRESULT Container::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    if (!isValid) {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    static int runNumber = 0;

    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_BTNFACE + 1));
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_VSCROLL: {
        SCROLLINFO si = { 0 };
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);
        int oldPos = si.nPos;

        switch (LOWORD(wParam)) {
        case SB_LINEUP:
            si.nPos = (std::max)(si.nMin, si.nPos - 10);
            break;
        case SB_LINEDOWN:
            si.nPos = (std::min)(si.nMax - (int)si.nPage + 1, si.nPos + 10);
            break;
        case SB_PAGEUP:
            si.nPos = (std::max)(si.nMin, si.nPos - (int)si.nPage);
            break;
        case SB_PAGEDOWN:
            si.nPos = (std::min)(si.nMax - (int)si.nPage + 1, si.nPos + (int)si.nPage);
            break;
        case SB_THUMBTRACK:
            si.nPos = HIWORD(wParam);
            break;
        }

        if (si.nPos != oldPos) {
            si.fMask = SIF_POS;
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
            scrollPos = si.nPos;

            // Move all controls based on scroll position
            for (Control* control : controls) {
                HWND hControl = control->getHandle();
                RECT rc;
                GetWindowRect(hControl, &rc);
                MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
                SetWindowPos(hControl, nullptr, rc.left, rc.top - (si.nPos - oldPos), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            }

            // Move the graph windows
            if (twistGraph) {
                HWND hGraph = twistGraph->getHandle();
                RECT rc;
                GetWindowRect(hGraph, &rc);
                MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
                SetWindowPos(hGraph, nullptr, rc.left, rc.top - (si.nPos - oldPos), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            }
            if (chordGraph) {
                HWND hGraph = chordGraph->getHandle();
                RECT rc;
                GetWindowRect(hGraph, &rc);
                MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
                SetWindowPos(hGraph, nullptr, rc.left, rc.top - (si.nPos - oldPos), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            }

            InvalidateRect(hwnd, nullptr, TRUE);
            UpdateWindow(hwnd);
        }
        return 0;
    }
    case WM_SIZE: {
        height = HIWORD(lParam);
        updateScrollRange();
        return 0;
    }
    case WM_USER + 101: {
        Logger::logError(L"WM_USER + 101 received with wParam=" + std::to_wstring(wParam));
        HWND runButton = (HWND)lParam;
        EnableWindow(runButton, TRUE);
        if (wParam == 1) {
            MessageBoxW(hwnd, L"XTurb executed successfully.", L"Success", MB_OK | MB_ICONINFORMATION);
            Logger::logError(L"Showing FileSelectorWindow");
            runNumber++;
            std::wstring title = L"File Selector XTurb Run " + std::to_wstring(runNumber);
            FileSelectorWindow* newSelector = new FileSelectorWindow(hInstance, hwnd, exeDir);
            newSelector->create(hInstance, SW_SHOW);
            if (!newSelector->getHwnd()) {
                Logger::logError(L"FileSelectorWindow creation failed");
                delete newSelector;
            }
            else {
                SetWindowTextW(newSelector->getHwnd(), title.c_str());
                Logger::logError(L"FileSelectorWindow created with hwnd " + std::to_wstring(reinterpret_cast<LONG_PTR>(newSelector->getHwnd())));
                fileSelectors.push_back(newSelector);
            }
        }
        else {
            MessageBoxW(hwnd, L"XTurb execution failed.", L"Error", MB_OK | MB_ICONERROR);
        }
        return 0;
    }
    case WM_USER + 102: { // File selected
        FileSelectorWindow* selector = reinterpret_cast<FileSelectorWindow*>(lParam);
        std::wstring filePath = selector->getSelectedFile();
        Logger::logError(L"Opening file: " + filePath);
        OutputData outputData;
        BEMTOutputParser parser(filePath);
        if (parser.parse(outputData)) {
            std::wstring fileName = filePath.substr(filePath.find_last_of(L"\\") + 1);
            DataDisplayWindow* displayWindow = new DataDisplayWindow(hInstance, hwnd, outputData, fileName);
            displayWindow->create(hInstance, SW_SHOW);
            displayWindows.push_back(displayWindow);
            ShowWindow(displayWindow->getHwnd(), SW_SHOW);
        }
        return 0;
    }
    case WM_COMMAND: {
        HMENU controlId = (HMENU)LOWORD(wParam);
        WORD notificationCode = HIWORD(wParam);

        // Find the control that sent the message
        Control* sourceControl = nullptr;
        for (Control* control : controls) {
            if (control->getId() == controlId) {
                sourceControl = control;
                break;
            }
        }

        if (!sourceControl) break;

        // Handle Save button click (second-to-last control)
        if (dynamic_cast<Button*>(sourceControl) &&
            sourceControl->getHandle() == controls[controls.size() - 2]->getHandle()) {
            // Validate inputs before saving
            std::wstring errorMessage;
            if (!validateInputs(errorMessage)) {
                MessageBoxW(hwnd, errorMessage.c_str(), L"Validation Error", MB_OK | MB_ICONERROR);
                return 0;
            }

            // Collect data from all input fields
            if (nameInput) inputData.name = nameInput->getText();
            if (bnInput) inputData.BN = _wtoi(bnInput->getText().c_str());
            if (rootInput) inputData.ROOT = _wtof(rootInput->getText().c_str());
            if (ntaperInput) inputData.NTAPER = _wtoi(ntaperInput->getText().c_str());
            if (rtaperInput) inputData.RTAPER = parseCommaSeparatedDoubles(rtaperInput->getText());
            if (ctaperInput) inputData.CTAPER = parseCommaSeparatedDoubles(ctaperInput->getText());
            if (ntwistInput) inputData.NTWIST = _wtoi(ntwistInput->getText().c_str());
            if (rtwistInput) inputData.RTWIST = parseCommaSeparatedDoubles(rtwistInput->getText());
            if (dtwistInput) inputData.DTWIST = parseCommaSeparatedDoubles(dtwistInput->getText());
            if (nairfInput) inputData.NAIRF = _wtoi(nairfInput->getText().c_str());
            if (rairfInput) inputData.RAIRF = parseCommaSeparatedDoubles(rairfInput->getText());
            if (airfdataInput) inputData.AIRFDATA = parseCommaSeparatedWStrings(airfdataInput->getText());
            if (blendairfInput) inputData.BLENDAIRF = _wtoi(blendairfInput->getText().c_str());
            if (percentrInput) inputData.PERCENTR = _wtoi(percentrInput->getText().c_str());
            if (stalldelayInput) inputData.STALLDELAY = _wtoi(stalldelayInput->getText().c_str());
            if (viternaInput) inputData.VITERNA = _wtoi(viternaInput->getText().c_str());
            if (nsweepInput) inputData.NSWEEP = _wtoi(nsweepInput->getText().c_str());
            if (rsweepInput) inputData.RSWEEP = parseCommaSeparatedDoubles(rsweepInput->getText());
            if (lsweepInput) inputData.LSWEEP = parseCommaSeparatedDoubles(lsweepInput->getText());
            if (ndihedInput) inputData.NDIHED = _wtoi(ndihedInput->getText().c_str());
            if (rdihedInput) inputData.RDIHED = parseCommaSeparatedDoubles(rdihedInput->getText());
            if (ldihedInput) inputData.LDIHED = parseCommaSeparatedDoubles(ldihedInput->getText());
            if (ntwaxInput) inputData.NTWAX = _wtoi(ntwaxInput->getText().c_str());
            if (rtwaxInput) inputData.RTWAX = parseCommaSeparatedDoubles(rtwaxInput->getText());
            if (ltwaxInput) inputData.LTWAX = parseCommaSeparatedDoubles(ltwaxInput->getText());
            if (npiaxInput) inputData.NPIAX = _wtoi(npiaxInput->getText().c_str());
            if (rpiaxInput) inputData.RPIAX = parseCommaSeparatedDoubles(rpiaxInput->getText());
            if (lpiaxInput) inputData.LPIAX = parseCommaSeparatedDoubles(lpiaxInput->getText());
            if (checkInput) inputData.CHECK = _wtoi(checkInput->getText().c_str());
            if (designInput) inputData.DESIGN = _wtoi(designInput->getText().c_str());
            if (ntsrInput) inputData.NTSR = _wtoi(ntsrInput->getText().c_str());
            if (btsrInput) inputData.BTSR = _wtof(btsrInput->getText().c_str());
            if (etsrInput) inputData.ETSR = _wtof(etsrInput->getText().c_str());
            if (npitchInput) inputData.NPITCH = _wtoi(npitchInput->getText().c_str());
            if (bpitchInput) inputData.BPITCH = _wtof(bpitchInput->getText().c_str());
            if (epitchInput) inputData.EPITCH = _wtof(epitchInput->getText().c_str());
            if (analysisInput) inputData.ANALYSIS = _wtoi(analysisInput->getText().c_str());
            if (nanaInput) inputData.NANA = _wtoi(nanaInput->getText().c_str());
            if (tsranaInput) inputData.TSRANA = parseCommaSeparatedDoubles(tsranaInput->getText());
            if (pitchanaInput) inputData.PITCHANA = parseCommaSeparatedDoubles(pitchanaInput->getText());
            if (predictionInput) inputData.PREDICTION = _wtoi(predictionInput->getText().c_str());
            if (bradiusInput) inputData.BRADIUS = _wtof(bradiusInput->getText().c_str());
            if (rhoairInput) inputData.RHOAIR = _wtof(rhoairInput->getText().c_str());
            if (muairInput) inputData.MUAIR = _wtof(muairInput->getText().c_str());
            if (npreInput) inputData.NPRE = _wtoi(npreInput->getText().c_str());
            if (vwindInput) inputData.VWIND = parseCommaSeparatedDoubles(vwindInput->getText());
            if (rpmpresInput) inputData.RPMPRE = parseCommaSeparatedDoubles(rpmpresInput->getText());
            if (pitchpreInput) inputData.PITCHPRE = parseCommaSeparatedDoubles(pitchpreInput->getText());
            if (methodInput) inputData.METHOD = _wtoi(methodInput->getText().c_str());
            if (jxInput) inputData.JX = _wtoi(jxInput->getText().c_str());
            if (cosdistrInput) inputData.COSDISTR = _wtoi(cosdistrInput->getText().c_str());
            if (gnuplotInput) inputData.GNUPLOT = _wtoi(gnuplotInput->getText().c_str());
            if (aviscInput) inputData.AVISC = _wtof(aviscInput->getText().c_str());
            if (wakeexpInput) inputData.WAKEEXP = _wtoi(wakeexpInput->getText().c_str());      
            if (DX0Input) inputData.DX0 = _wtof(DX0Input->getText().c_str());                   
            if (XSTRInput) inputData.XSTR = _wtof(XSTRInput->getText().c_str());              
            if (XTREFFTZInput) inputData.XTREFFTZ = _wtof(XTREFFTZInput->getText().c_str());   
            if (NSECInput) inputData.NSEC = _wtoi(NSECInput->getText().c_str());               
            if (ibInput) inputData.IB = _wtoi(ibInput->getText().c_str());                   
            if (DIPInput) inputData.DIP = _wtoi(DIPInput->getText().c_str());                 
            if (OMRELAXInput) inputData.OMRELAX = _wtof(OMRELAXInput->getText().c_str());     
            if (NACMODInput) inputData.NACMOD = _wtoi(NACMODInput->getText().c_str());        
            if (LNInput) inputData.LN = _wtof(LNInput->getText().c_str());               
            if (HNInput) inputData.HN = _wtof(HNInput->getText().c_str());                  
            if (XNInput) inputData.XN = _wtof(XNInput->getText().c_str());                     
            if (rlossInput) inputData.RLOSS = _wtoi(rlossInput->getText().c_str());
            if (tiplossInput) inputData.tipLoss = _wtoi(tiplossInput->getText().c_str());
            if (axrelaxInput) inputData.AXRELAX = _wtof(axrelaxInput->getText().c_str());
            if (atrelaxInput) inputData.ATRELAX = _wtof(atrelaxInput->getText().c_str());
            if (optimInput) inputData.OPTIM = _wtoi(optimInput->getText().c_str());
            // Write to file in the same directory as XTurbTool.exe
            std::wstring inputFilePath = exeDir + L"output.inp";
            Logger::logError(L"Saving to: " + inputFilePath); // Add logging for debugging
            inputData.writeToFile(inputFilePath);
            std::ifstream checkFile(wstring_to_string(inputFilePath));
            if (!checkFile.is_open()) {
                Logger::logError(L"Failed to write to " + inputFilePath);
                MessageBoxW(hwnd, (L"Failed to write to " + inputFilePath).c_str(), L"Error", MB_OK | MB_ICONERROR);
            }
            else {
                checkFile.close();
                std::wstring message = L"Data saved to " + inputFilePath;
                MessageBoxW(hwnd, message.c_str(), L"Info", MB_OK | MB_ICONINFORMATION);
            }
        }
        // Handle Run XTurb button click (last control)
        else if (dynamic_cast<Button*>(sourceControl) &&
            sourceControl->getHandle() == controls[controls.size() - 1]->getHandle()) {
            Logger::logError(L"Run XTurb button clicked, ID: " + std::to_wstring((LONG_PTR)controlId));
            std::wstring inputFilePath = exeDir + L"output.inp";
            std::ifstream checkFile(wstring_to_string(inputFilePath));
            if (!checkFile.is_open()) {
                Logger::logError(L"Input file not found: " + inputFilePath);
                MessageBoxW(hwnd, (L"Input file not found: " + inputFilePath).c_str(), L"Error", MB_OK | MB_ICONERROR);
                return 0;
            }
            checkFile.close();

            HWND runButtonHandle = sourceControl->getHandle();
            EnableWindow(runButtonHandle, FALSE);
            Logger::logError(L"Starting XTurbRunner with exe: " + xturbRunner->getExePath());
            std::thread([this, inputFilePath, runButtonHandle]() {
                bool success = xturbRunner->run(inputFilePath);
                Logger::logError(L"XTurbRunner finished with success: " + std::to_wstring(success));
                PostMessage(hwnd, WM_USER + 101, success ? 1 : 0, (LPARAM)runButtonHandle);
                }).detach();
        }

        // Handle input field changes
        if (dynamic_cast<InputField*>(sourceControl) && notificationCode == EN_CHANGE) {
            std::wstring text = dynamic_cast<InputField*>(sourceControl)->getText();
            Logger::logError(L"Input changed to: " + text);

            // Update graphs if relevant fields change
            if (sourceControl == rtaperInput || sourceControl == ctaperInput ||
                sourceControl == rtwistInput || sourceControl == dtwistInput) {
                updateGraphs();
            }
        }
        break;
    }
    default:
        return Window::handleMessage(msg, wParam, lParam);
    }

    return 0;
}