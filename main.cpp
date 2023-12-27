#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#ifdef OPENMP
#include <omp.h>
#endif

const unsigned int PROGRAM_IDX = 0;
const unsigned int INPUT_IDX = 1;
const unsigned int OUTPUT_IDX = 2;
const unsigned int HEADER_POS = 0;
const unsigned int HEADER_LEN = 6;
const unsigned int CHAIN_ID_POS = 21;
const unsigned int CHAIN_ID_LEN = 1;
const std::string ATOM = "ATOM  ";
const std::string HETATM = "HETATM";
const std::string  TER = "TER   ";
const char SEP = '/';
const unsigned int SUCCESS = 0;
const unsigned int FAILED = 1;

void getFilteredLines(std::ifstream & iFileReader, std::string & filteredLines, std::string foo) {
    std::string line;
    std::string header;
    std::string chainId;
    std::string currChainId;
    bool egg = false;
    while(getline(iFileReader, line)) {
        header = line.substr(HEADER_POS, HEADER_LEN);
        currChainId = (header == ATOM || header == HETATM) ? line.substr(CHAIN_ID_POS, CHAIN_ID_LEN) : "";
        if (header == HETATM && !chainId.empty() && chainId != currChainId)
        {egg = true; continue;}

        if (header == ATOM && chainId.empty())
            chainId =  currChainId;

        if (header == TER)
            chainId.clear();

        filteredLines.append(line + '\n');
    }
    if (egg)
        std::cout << foo << std::endl;
    line.clear();
    header.clear();
    chainId.clear();
    currChainId.clear();
}

int main(int argc, char* argv[]) {
    std::string cPath = argv[PROGRAM_IDX];
    std::string iPath = argv[INPUT_IDX];
    std::string oPath = argv[OUTPUT_IDX];
    std::filesystem::create_directories(oPath);
    std::vector<std::string> iFiles;
    for (const auto& iFile : std::filesystem::directory_iterator(iPath))  {
        iFiles.emplace_back(iFile.path());
    }

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
                getFilteredLines(iFileReader, filteredLine, iFileName);
                iFileReader.close();
            } else {
                std::cout << "Unable to open file" << "\t" << iFileName << std::endl;
                return FAILED;
            }

            if (oFileWriter.is_open()) {
                oFileWriter << filteredLine + '\n';
                oFileWriter.close();
            } else {
                std::cout << "error" << "\t" << oFileName << std::endl;
                return FAILED;
            }
            filteredLine.clear();
        }
    }
    return SUCCESS;
}
