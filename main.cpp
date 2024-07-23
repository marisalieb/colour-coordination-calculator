#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

class Colour {
public:
    double h; // hue
    double s; // saturation
    double v; // value

    Colour(double h = 0, double s = 0, double v = 0) : h(h), s(s), v(v) {}

    // adjust the hue with wrapping around 360 degrees
    void adjustHue(double offset) {
        h = fmod(h + offset + 360, 360);
    }

    // converting hsv to rgb hex
    string toHex() const {
        double r, g, b;
        double sNormalised = s / 100;  // norm s
        double vNormalised = v / 100;  // norm v
        double c = vNormalised * sNormalised;
        double x = c * (1 - abs(fmod(h / 60.0, 2) - 1));
        double m = vNormalised - c;

        if (h >= 0 && h < 60) {
            r = c;
            g = x;
            b = 0;
        } else if (h >= 60 && h < 120) {
            r = x;
            g = c;
            b = 0;
        } else if (h >= 120 && h < 180) {
            r = 0;
            g = c;
            b = x;
        } else if (h >= 180 && h < 240) {
            r = 0;
            g = x;
            b = c;
        } else if (h >= 240 && h < 300) {
            r = x;
            g = 0;
            b = c;
        } else {
            r = c;
            g = 0;
            b = x;
        }

        r += m;
        g += m;
        b += m;

        // lambda function here converts each normalised 2 digit rgb value to 2 digit hexadecimal string
        // scale up to 255 range, convert double to int
        // make sure the hex code is always 2 digits long
        auto toHexComponent = [](double component) {
            int intComponent = static_cast<int>(component * 255+0.5);
            stringstream ss;
            ss << hex << setw(2) << setfill('0') << uppercase<< intComponent;
            return ss.str();
        };

        stringstream hexStream;
        // << used to insert data so rgb into the stream so that the format will be #RRGGBB
        hexStream << "#"
                  << toHexComponent(r)
                  << toHexComponent(g)
                  << toHexComponent(b);

        // converts all content of stream to single string
        return hexStream.str();
    }

};

// base class for colour harmonies
class ColourHarmony {
public:
    virtual vector<Colour> calculate(const Colour& baseColour) const = 0;
    virtual ~ColourHarmony() = default; // for cleanup
};

// specific harmony classes
class ComplementaryHarmony : public ColourHarmony {
public:
    vector<Colour> calculate(const Colour& baseColour) const override {
        Colour complementary = baseColour;
        complementary.adjustHue(180);
        return {complementary};
    }
};

class MonochromaticHarmony : public ColourHarmony {
public:
    vector<Colour> calculate(const Colour& baseColour) const override {
        Colour monochromatic = baseColour;
        // adjust saturation and value
        monochromatic.s = (monochromatic.s > 50) ? max(0.0, monochromatic.s - 20) : min(100.0, monochromatic.s + 20);
        monochromatic.v = (monochromatic.v > 50) ? max(0.0, monochromatic.v - 10) : min(100.0, monochromatic.v + 10);
        return {monochromatic};
    }
};

class AnalogousHarmony : public ColourHarmony {
public:
    vector<Colour> calculate(const Colour& baseColour) const override {
        vector<Colour> analogousColours(2, baseColour);
        double hueOffsets[2] = {25, -25};
        for (int i = 0; i < 2; ++i) {
            analogousColours[i].adjustHue(hueOffsets[i]);
        }
        return analogousColours;
    }
};

class SplitComplementaryHarmony : public ColourHarmony {
public:
    vector<Colour> calculate(const Colour& baseColour) const override {
        vector<Colour> splitComplementaryColours(2, baseColour);
        double hueOffsets[2] = {155, -155};
        for (int i = 0; i < 2; ++i) {
            splitComplementaryColours[i].adjustHue(hueOffsets[i]);
        }
        return splitComplementaryColours;
    }
};

class TriadicHarmony : public ColourHarmony {
public:
    vector<Colour> calculate(const Colour& baseColour) const override {
        vector<Colour> triadicColours(2, baseColour);
        double hueOffsets[2] = {-120, 120};
        for (int i = 0; i < 2; ++i) {
            triadicColours[i].adjustHue(hueOffsets[i]);
        }
        return triadicColours;
    }
};

class TetradicHarmony : public ColourHarmony {
public:
    vector<Colour> calculate(const Colour& baseColour) const override {
        vector<Colour> tetradicColours(3, baseColour);
        double hueOffsets[3] = {60, 120, -120};
        for (int i = 0; i < 3; ++i) {
            tetradicColours[i].adjustHue(hueOffsets[i]);
        }
        return tetradicColours;
    }
};


// convert rgb hex to hsv, so if input value is in hex format
Colour hexToHSV(const string& hex) {
    // remove # if there
    string hexCode = (hex[0] == '#') ? hex.substr(1) : hex;

    // convert the hex to rgb components os from RRGGBB split up
    // stoi string to int (string to convert, pointer to int which isnt needed so nullptr, base number system 16 because hexadecimal)
    int r = stoi(hexCode.substr(0, 2), nullptr, 16);
    int g = stoi(hexCode.substr(2, 2), nullptr, 16);
    int b = stoi(hexCode.substr(4, 2), nullptr, 16);

    // normalise rgb values so no longer 255 range
    double rNorm = r / 255.0;
    double gNorm = g / 255.0;
    double bNorm = b / 255.0;

    // calculate chroma, max value among the rgb vals
    double cMax = max({rNorm, gNorm, bNorm});
    double cMin = min({rNorm, gNorm, bNorm});
    double chroma = cMax - cMin;

    // calculate hue
    // if chroma is 0 the colour will grey shade and hue set to 0
    double h = 0;
    if (chroma > 0) {
        if (cMax == rNorm) {
            h = fmod((60 * ((gNorm - bNorm) / chroma) + 360), 360);
        } else if (cMax == gNorm) {
            h = fmod((60 * ((bNorm - rNorm) / chroma) + 120), 360);
        } else if (cMax == bNorm) {
            h = fmod((60 * ((rNorm - gNorm) / chroma) + 240), 360);
        }
    }
    // scale to percentage, s is ratio of chroma to cmax, if 0 also grey shade
    double s = (cMax > 0) ? (chroma / cMax) * 100 : 0;

    // scale v to percentage
    double v = cMax * 100;


    return Colour(h, s, v);
}


void printHarmony(const ColourHarmony& harmony, const Colour& colour) {
    vector<Colour> colours = harmony.calculate(colour);
    for (const Colour& c : colours) {
        cout << "\nHSV: (" << c.h << ", " << c.s << ", " << c.v << ")" << endl
             << "Hex: " << c.toHex() << endl;
    }
}

// handle user input and harmony choices
void colourCoordinationTool() {
    Colour inputColour;
    string harmonyChoice, inputMode;

    // ask for colour input mode
    cout << "\nDo you want to input the colour in HSV or RGB hex? (Enter 'hsv' or 'hex'):";
    cin >> inputMode;

    if (inputMode == "hsv") {
        cout << "Enter HSV values (H S V) - separate with space";
        cin >> inputColour.h >> inputColour.s >> inputColour.v;

    } else if (inputMode == "hex") {
        string hexCode;
        cout << "Enter RGB hex colour (e.g. #992e99):";
        cin >> hexCode;
        inputColour = hexToHSV(hexCode);

    } else {
        cout << "Invalid input mode!" << endl;
        return;
    }

    // input harmony choice
    cout << "1 - complementary - input 'c'" << endl
         << "2 - monochromatic - input 'm'" << endl
         << "3 - analogous - input 'a'" << endl
         << "4 - split-complementary - input 'sp'" << endl
         << "5 - triadic - input 'tri'" << endl
         << "6 - tetradic - input 'tet'" << endl
         << "Choose a harmony:";
    cin >> harmonyChoice;

    // create appropriate harmony object based on user input
    unique_ptr<ColourHarmony> harmony;

    if (harmonyChoice == "c") {
        harmony = make_unique<ComplementaryHarmony>();
    } else if (harmonyChoice == "m") {
        harmony = make_unique<MonochromaticHarmony>();
    } else if (harmonyChoice == "a") {
        harmony = make_unique<AnalogousHarmony>();
    } else if (harmonyChoice == "sp") {
        harmony = make_unique<SplitComplementaryHarmony>();
    } else if (harmonyChoice == "tri") {
        harmony = make_unique<TriadicHarmony>();
    } else if (harmonyChoice == "tet") {
        harmony = make_unique<TetradicHarmony>();
    } else {
        cout << "Invalid harmony choice!\n";
        return;
    }

    // print result
    printHarmony(*harmony, inputColour);
}

int main() {
    colourCoordinationTool();
    /*
    Colour testColour(60, 70, 60);
    cout << "HSV: (" << testColour.h << ", " << testColour.s << ", " << testColour.v << ")" << endl
        << "Hex: " << testColour.toHex() << endl;
        */
    return 0;
}
