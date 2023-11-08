// Nick Russert      826301366
// Molly Compton     825530141

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

struct SymbolInfo
{
    string symbol;
    string address;
};

struct LitInfo
{
    string name;
    string litConst;
    string length;
    string address;
};

/*
// this is test code for seeing what the tables look like
void printTables(const map<int, string> &symtab, const map<int, littab_row> &littab)
{
    // Print symtab
    std::cout << "Symbol  Address Flags:" << std::endl;
    std::cout << "-----------------------" << std::endl;
    for (const auto &pair : symtab)
    {
        std::cout << std::setw(8) << std::left << pair.second                                       // symbol name
                  << std::setw(6) << std::right << std::hex << std::uppercase << pair.first << " R" // address and  "R"
                  << std::endl;
    }
    std::cout << std::endl; // Add an empty line for clarity

    // Print littab
    std::cout << "Name    Lit_Const  Length Address:" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    for (const auto &pair : littab)
    {
        std::cout << std::setw(8) << std::left << (pair.second.name.empty() ? "" : pair.second.name) // Name
                  << std::setw(10) << std::left << pair.second.lit_const                             // Lit_Const
                  << std::setw(6) << std::right << std::dec << pair.second.length                    // Length
                  << std::setw(6) << std::right << std::hex << std::uppercase << pair.first          // Address
                  << std::endl;
    }
}
*/

// takes a string and returns a vector with all of the tokens
vector<string> getTokens(string str, char delimiter)
{
    vector<string> words;
    stringstream strStream(str);
    string word;
    while (getline(strStream >> std::ws, word, delimiter))
    {
        words.push_back(word);
    }
    return words;
}

// convert an int to a hex string with the specified width using leading zeros
string intToHex(int value, int width)
{
    stringstream ss;
    ss << setfill('0') << setw(width) << uppercase << hex << value;
    return ss.str();
}

// print one line to the output table, using either a file or cout as the first parameter
void printFormatted(ostream &out, const string &locCtrStr, const string &label, const string &instr, const string &arg, const string &obj)
{
    out << setw(4) << locCtrStr
        << setfill(' ') << setw(8) << ""
        << left << setfill(' ') << setw(12) << label
        << left << setfill(' ') << setw(12) << instr
        << left << setfill(' ') << setw(12) << arg
        << left << setfill(' ') << setw(12) << obj << endl;
}

// based on the the file name, read in symtab and littab
void parseSym(const string &symFileName, vector<SymbolInfo> &symbols, vector<LitInfo> &litConst)
{
    ifstream symFile(symFileName);
    if (!symFile.is_open())
    {
        cerr << "Error opening .sym file." << endl;
        return;
    }

    string line;
    bool readingSymbols = false;
    bool readingLitConst = false;

    while (getline(symFile, line))
    {
        if (line == "Symbol  Address Flags:" || line == "Symbol  Value   Flags:")
        {
            readingSymbols = true;
            readingLitConst = false;
            continue;
        }
        if (line == "Name    Lit_Const  Length Address:")
        {
            readingSymbols = false;
            readingLitConst = true;
            continue;
        }

        if (readingSymbols)
        {
            // Process lines in the symbol table
            if (line.find_first_not_of('-') != string::npos)
            {
                istringstream iss(line);
                SymbolInfo symbolInfo;
                iss >> symbolInfo.symbol >> symbolInfo.address;
                symbolInfo.address = symbolInfo.address.substr(symbolInfo.address.length() - 4, 4);
                symbols.push_back(symbolInfo);
            }
        }
        else if (readingLitConst)
        {
            // Process lines in the literal table
            if (!line.empty() && line.find_first_not_of('-') != string::npos)
            {
                istringstream iss(line);
                LitInfo litInfo;
                iss >> litInfo.name >> litInfo.litConst >> litInfo.length >> litInfo.address;
                litInfo.address = litInfo.address.substr(litInfo.address.length() - 4, 4);
                litConst.push_back(litInfo);
            }
        }
    }
}

// from code, figure out the opcode and the format (format will be set to 2 for Format 2, and 3 for Format 3 or Format 4)
bool lookupCode(int code, string &instr, int &format)
{
    int codes[] = {
        0x18, 0x58, 0x90, 0x40, 0xB4, 0x28, 0x88, 0xA0, 0x24, 0x64,
        0x9C, 0xC4, 0xC0, 0xF4, 0x3C, 0x30, 0x34, 0x38, 0x48, 0x00,
        0x68, 0x50, 0x70, 0x08, 0x6C, 0x74, 0x04, 0xD0, 0x20, 0x60,
        0x98, 0xC8, 0x44, 0xD8, 0xAC, 0x4C, 0xA4, 0xA8, 0xF0, 0xEC,
        0x0C, 0x78, 0x54, 0x80, 0xD4, 0x14, 0x7C, 0xE8, 0x84, 0x10,
        0x1C, 0x5C, 0x94, 0xB0, 0xE0, 0xF8, 0x2C, 0xB8, 0xDC};
    string instrs[] = {
        "ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP", "COMPF", "COMPR", "DIV", "DIVF",
        "DIVR", "FIX", "FLOAT", "HIO", "J", "JEQ", "JGT", "JLT", "JSUB", "LDA",
        "LDB", "LDCH", "LDF", "LDL", "LDS", "LDT", "LDX", "LPS", "MUL", "MULF",
        "MULR", "NORM", "OR", "RD", "RMO", "RSUB", "SHIFTL", "SHIFTR", "SIO", "SSK",
        "STA", "STB", "STCH", "STF", "STI", "STL", "STS", "STSW", "STT", "STX",
        "SUB", "SUBF", "SUBR", "SVC", "TD", "TIO", "TIX", "TIXR", "WD"};
    // 2 means Format 2, and 3 means Format 3 or Format 4
    int formats[] = {
        3, 3, 2, 3, 2, 3, 3, 2, 3, 3,
        2, 1, 1, 1, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        2, 1, 3, 3, 2, 3, 2, 2, 1, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 2, 2, 3, 1, 3, 2, 3};

    code = code & 0b11111100; // opcodes are based on the first six bits only, so clear the last two bits with a bit mask

    // search for the code and if it's found, update the name and format
    instr = "";
    format = -1;
    for (int i = 0; i < sizeof(formats) / sizeof(int); i++)
    {
        if (code == codes[i])
        {
            instr = instrs[i];
            format = formats[i];
            return true;
        }
    }
    return false;
}

// from a hex character in ascii format, get its value as an int or if it's not found return -1
int oneHexCharToInt(char c)
{
    if (c >= 48 && c <= 57)
        return c - 48; // if digit
    if (c >= 65 && c <= 70)
        return c - 55; // if uppercase
    if (c >= 97 && c <= 102)
        return c - 87; // if lowercase
    return -1;
}

// combine two hex chars in ascii format into a single int
int twoHexCharsToInt(char c1, char c2)
{
    return (oneHexCharToInt(c1) * 16) + oneHexCharToInt(c2);
}

int hexStringToInt(const std::string &hexNum)
{
    int result = 0;

    // Check if msb is 1, if so it is negative
    bool isNegative = !hexNum.empty() && (hexNum[0] >= '8' && hexNum[0] <= 'F');

    for (size_t i = (isNegative ? 1 : 0); i < hexNum.length(); ++i)
    {
        int val = oneHexCharToInt(hexNum[i]);
        if (val == -1)
        {
            return -1; // throw error, invalid hex
        }
        result = (result * 16) + val;
    }

    // if negative do 2s comp
    if (isNegative)
    {
        result -= (1 << (4 * (hexNum.length() - (isNegative ? 1 : 0))));
    }

    return result;
}

string getDisplacement(const string &obj, int format)
{
    string displacement;

    if (format == 4)
    {
        // For format 4, take the entire displacement.
        displacement = obj.substr(4, 4);
    }
    else if (format == 3)
    {
        displacement = obj.substr(3, 4);
    }

    return displacement;
}

// open the input file, look for lines that start with 'T', and parse them, write out results
int main(int argc, char **argv)
{

    if (argc < 3)
    {
        std::cout << "usage: disassm filename.obj filename.sym" << endl;
        exit(EXIT_FAILURE);
    }

    fstream infile;
    string line;
    string base;

    //  load symtab and littab from the .sym file
    const string symFileName = argv[2];
    vector<SymbolInfo> symbols;
    vector<LitInfo> litConst;
    parseSym(symFileName, symbols, litConst);

    // printTables(symtab, littab);
    // cout << endl;

    // read the .obj file
    infile.open(argv[1], ios::in);

    ofstream outfile("out.lst");

    string programName;
    string locCtr = "0";

    // go through the file one line at a time
    while (getline(infile, line))
    {
        if (line.size() == 0)
            continue;

        string label = "";
        string instr = "";
        string operand = "";

        if (locCtr == "0" && line[0] == 'H')
        {
            programName = line.substr(1, 6);
            label = programName;
            locCtr = line.substr(7, 4), 0, 16;
            instr = "START";
            operand = "0";

            printFormatted(outfile, locCtr, label, instr, operand, "");

            continue;
        }

        if (line[0] != 'T')
            continue; // we want text records only, so skip this line if it doesn't start with 'T'

        locCtr = line.substr(3, 4);

        int charCounter = 9; // skip to the first opcode in the line

        // this loop should run once for each opcode in the line
        while (charCounter < line.size())
        {
            // formats 2, 3, and 4 all have at least 2 bytes, so read in the next 3 nibbles
            char firstNibble = line[charCounter];
            char secondNibble = line[charCounter + 1];
            char thirdNibble = line[charCounter + 2];

            // look up the opcode based on the first two nibbles
            int firstByte = twoHexCharsToInt(firstNibble, secondNibble);
            int format;
            bool result = lookupCode(firstByte, instr, format); // returns format=3 for both Format3 and Format 4

            // these flags are used in Format 3 and Format 4
            // where they are in the file: ||op op op op op op n i || x b p e data data data data || other data...
            bool n, i, x, b, p, e;

            // if format is now 3, get n i x b p e, and use x to figure out if it's Format 3 or Format 4
            // then figure out OAM and TAAM
            // use a different bitmap to read each of the flags
            if (format == 3)
            {
                int secondNibbleInt = oneHexCharToInt(secondNibble);
                int thirdNibbleInt = oneHexCharToInt(thirdNibble);
                n = (secondNibbleInt & 0b0010) > 0;
                i = (secondNibbleInt & 0b0001) > 0;
                x = (thirdNibbleInt & 0b1000) > 0;
                b = (thirdNibbleInt & 0b0100) > 0;
                p = (thirdNibbleInt & 0b0010) > 0;
                e = (thirdNibbleInt & 0b0001) > 0;
                if (e)
                    format = 4;
            }

            string obj = "";
            int constLength;

            // this is needed at the top. If found in litTable, update charcounter accordingly
            for (const LitInfo &lit : litConst)
            {
                if (locCtr == lit.address)
                {
                    instr = "BYTE";
                    obj = lit.litConst.substr(2, lit.litConst.length() - 3);
                    constLength = lit.litConst.substr(2, lit.litConst.length() - 3).length();
                    format = 0;
                    break;
                }
            }

            int increment;

            // if byte, charcounter is updated to length of the constant
            if (instr == "BYTE")
            {
                charCounter += constLength;
            }
            // else char counter is +3
            else
            {
                // this will be output in the last column
                obj = obj + firstNibble + secondNibble + thirdNibble;
                int nibblesToAdd = 1; // default: format is 2
                if (format == 3)
                    nibblesToAdd = 3; // if format is 3, get 3 more nibbles
                if (format == 4)
                    nibblesToAdd = 5; // if format is 4, get 5 more nibbles
                charCounter += 3;

                for (int j = 0; j < nibblesToAdd; j++)
                {
                    obj += (line[charCounter]);
                    charCounter++;
                }
            }

            // based on flags i and n determine the OAM (operand addressing mode)
            string oam = "";
            if (format == 3 || format == 4)
            {
                oam = "simple";
                if (i && !n)
                    oam = "immediate";
                if (!i && n)
                    oam = "indirect";
            }

            // based on flags b, p, and x determine TAAM (target address mode)
            string taam = "";
            if (format == 3 || format == 4)
            {
                if (b && !p)
                    taam = "base";
                if (!b && p)
                    taam = "pc";
                if (!b && !p)
                    taam = "absolute";
                // if (x) taam += "_indexed";    // we don't need to deal with X for this version of the disassembler
            }

            // for the instruction, add +if it's Format 4
            if (format == 4)
            {
                instr = "+" + instr;
            }

            // update the operand according to the target addressing mode
            // if it's base relative add the stored base value
            // if it PC-relative add the current locCtr + format (because wthePC always points to the next instruction
            // if it's absolute don't change anything
            string disp = getDisplacement(obj, format);
            int dispInt = hexStringToInt(disp);
            string fourDigitDisp = intToHex(dispInt, 4);
            operand = fourDigitDisp;

            if (taam == "base" || taam == "pc")
            {
                int operandVal = hexStringToInt(operand);
                if (taam == "base") // TA = (base) + disp
                {
                    int baseInt = hexStringToInt(base);
                    int dispIntBase = hexStringToInt(disp);
                    baseInt = baseInt + dispIntBase;
                    string newVal = intToHex(baseInt, 4);
                    operand = newVal;
                }
                else
                { // TA = (PC) + disp
                    // this is disp value
                    disp = getDisplacement(obj, format);
                    int dispIntPc = hexStringToInt(disp);

                    // this is content of PC register
                    int locCtrInt = hexStringToInt(locCtr);
                    locCtrInt = locCtrInt + format;

                    // save current position in the obj file
                    streampos fileCurrLine = infile.tellg();

                    // if at end of text record, read the location from the next text record
                    // needs to be fixed, messes up last operand record
                    string nextLine;
                    string isNextText;
                    getline(infile, isNextText);
                    infile.seekg(fileCurrLine);

                    if (isNextText[0] == 'T')
                    {
                        if (charCounter == line.length())
                        {
                            getline(infile, nextLine);
                            string newLocCtr = nextLine.substr(3, 4);
                            locCtrInt = hexStringToInt(newLocCtr);
                            infile.seekg(fileCurrLine);
                        }
                    }

                    // target address
                    int TA = locCtrInt + dispIntPc;
                    string newVal = intToHex(TA, 4);
                    operand = newVal;
                }
            }

            // finally, for the operand, add # for immediate and @ for indirect
            string targetAddressPrefix = "";
            if (oam == "immediate")
            {
                targetAddressPrefix = "#";
            }
            if (oam == "indirect")
            {
                targetAddressPrefix = "@";
            }

            // read from symbol table
            for (const SymbolInfo &symbol : symbols)
            {
                if (locCtr == symbol.address)
                {
                    label = symbol.symbol;
                    break;
                }
            }

            // read from lit table
            for (const LitInfo &lit : litConst)
            {
                if (locCtr == lit.address)
                {
                    label = lit.name;
                    operand = lit.litConst;
                    obj = lit.litConst.substr(2, lit.litConst.length() - 3);
                    break;
                }
            }

            // logic for clear
            if (instr == "CLEAR")
            {
                if (obj.substr(2, 2) == "00")
                {
                    operand = 'A';
                }
                if (obj.substr(2, 2) == "10")
                {
                    operand = 'X';
                }
                if (obj.substr(2, 2) == "20")
                {
                    operand = 'L';
                }
                if (obj.substr(2, 2) == "30")
                {
                    operand = 'B';
                }
                if (obj.substr(2, 2) == "40")
                {
                    operand = 'S';
                }
                if (obj.substr(2, 2) == "50")
                {
                    operand = 'T';
                }
                if (obj.substr(2, 2) == "60")
                {
                    operand = 'F';
                }
                if (obj.substr(2, 2) == "80")
                {
                    operand = "PC";
                }
                if (obj.substr(2, 2) == "90")
                {
                    operand = '9';
                }
            }

            // output the line
            printFormatted(outfile, locCtr, label, instr, (targetAddressPrefix + operand), obj);

            // handle possible BASE assembler directive
            if (instr == "LDB" || instr == "+LDB")
            {
                printFormatted(outfile, "", "", "BASE", operand, "");
                // store the value of the base so it can be used to update the operand
                // when the target addressing mode is base-relative
                base = operand;
            }

            if (instr == "BYTE")
            {
                increment = constLength / 2;
            }
            else
                increment = format;

            // update locCtr based on whether the instruction is Format 2, Format 3, or Format 4
            int locCtrInt = hexStringToInt(locCtr);
            locCtrInt += increment;
            locCtr = intToHex(locCtrInt, 4);
            label = "";
        }
    }

    // add the END assembler directive
    printFormatted(outfile, "", "", "END", programName, "");

    infile.close();
    outfile.close();
    std::cout << "written to out.lst" << endl;
}
