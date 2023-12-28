#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#ifdef OPENMP
#include <omp.h>
#endif

const char EMPTY_CHAIN_ID = ' ';
const char SEP = '/';
const char NL = '\n';
const unsigned int INPUT_IDX = 1;
const unsigned int OUTPUT_IDX = 2;
const unsigned int HEADER_POS = 0;
const unsigned int HEADER_LEN = 6;
const unsigned int CHAIN_ID_POS = 21;
const unsigned int SUCCESS = 0;
const unsigned int FAILED = 1;
const unsigned int BAR_WIDTH = 20;
const double DONE_INIT = 0.0;
const std::string ATM = "ATOM  ";
const std::string HTM = "HETATM";
const std::string TER = "TER   ";

void getFilteredLines(std::ifstream & iFileReader, std::string & filteredLines) {
    std::string line;
    std::string header;
    char chainId;
    char currChainId;

    while(getline(iFileReader, line)) {
        header = line.substr(HEADER_POS, HEADER_LEN);
        currChainId = (header == ATM || header == HTM) ? line[CHAIN_ID_POS] : EMPTY_CHAIN_ID;

        if (header == HTM && chainId != EMPTY_CHAIN_ID && chainId != currChainId)
            line[CHAIN_ID_POS] = chainId;

        if (header == ATM && chainId == EMPTY_CHAIN_ID)
            chainId =  currChainId;

        if (header == TER)
            chainId = EMPTY_CHAIN_ID;

        filteredLines.append(line + NL);
    }

    line.clear();
    header.clear();
}

void updateProgressBar(double progress) {
    std::cout << "[";
    auto pos = (int) (BAR_WIDTH * progress);
    for (int i = 0; i < BAR_WIDTH; ++i) {
        if (i < pos)
            std::cout << "=";
        else if (i == pos)
            std::cout << ">";
        else
            std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
}

int main(int argc, char* argv[]) {
    std::string iPath = argv[INPUT_IDX];
    std::string oPath = argv[OUTPUT_IDX];
    std::filesystem::create_directories(oPath);
    std::vector<std::string> iFiles;
    for (const auto& iFile : std::filesystem::directory_iterator(iPath))  {
        iFiles.emplace_back(iFile.path());
    }
    double done = DONE_INIT;
#pragma omp parallel
    {
        std::string iFileName;
        std::string oFileName;
        std::string filteredLine;
        std::ifstream iFileReader;
        std::ofstream oFileWriter;
#pragma omp for
        for (size_t iFileIdx = 0; iFileIdx < iFiles.size(); iFileIdx++) {
            iFileName = std::string(iFiles[iFileIdx]);
            iFileReader = std::ifstream (iFiles[iFileIdx]);
            int fileNamePos = iFileName.rfind(SEP) + 1;
            oFileName = oPath + SEP + iFileName.substr(fileNamePos, iFileName.size() - fileNamePos);
            oFileWriter = std::ofstream (oFileName);

            if (iFileReader.is_open()) {
                getFilteredLines(iFileReader, filteredLine);
                iFileReader.close();
            } else {
                std::cout << "Unable to open file" << "\t" << iFileName << std::endl;
                return FAILED;
            }

            if (oFileWriter.is_open()) {
                oFileWriter << filteredLine;
                oFileWriter.close();
            } else {
                std::cout << "error" << "\t" << oFileName << std::endl;
                return FAILED;
            }
//            updateProgressBar(++done / (double) iFiles.size());
            iFileName.clear();
            oFileName.clear();
            filteredLine.clear();
        }
    }
    iPath.clear();
    oPath.clear();
    iFiles.clear();
    std::cout << "process succeed"  << std::endl;
    return SUCCESS;
}
