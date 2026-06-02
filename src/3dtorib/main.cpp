#include "3dtorib.hpp"
#include "config.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

using namespace threedtorib;

void printHelp(const char *toolname) {
    std::cout << toolname << "\n"
              << RAT_COPYRIGHT_STATEMENT
              << RENDERMAN_COPYRIGHT_STATEMENT
              << "Usage: " << toolname << " [options] <input_file> <output_rib_file>\n"
              << "Options:\n"
              << "  -v, --version    Show version information\n"
              << "  -h, --help       Show this help message\n";
}

void printVersion(const char *toolname) {
    std::cout << toolname << " version " << AFFINE_VERSION << "\n"
              << RAT_COPYRIGHT_STATEMENT
              << RENDERMAN_COPYRIGHT_STATEMENT;
}

int main(int argc, char *argv[]) {
    const char *toolname = "3dtorib";
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--version") {
            printVersion(toolname);
            return 0;
        }
        else if (arg == "-h" || arg == "--help") {
            printHelp(toolname);
            return 0;
        }
        else {
            args.push_back(arg);
        }
    }

    if (args.size() < 2) {
        printHelp(toolname);
        return 1;
    }

    std::string input_file = args[0];
    std::string output_file = args[1];

    try {
        std::string ext = std::filesystem::path(input_file).extension().string();
        for (auto &c : ext)
            c = std::tolower(c);

        Mesh mesh;
        if (ext == ".obj") {
            mesh = parseOBJ(input_file);
        }
        else if (ext == ".stl") {
            mesh = parseSTL(input_file);
        }
        else {
            // Try to detect by header if extension is unknown
            std::ifstream ifs(input_file, std::ios::binary);
            char header[6] = {0};
            ifs.read(header, 5);
            if (std::string(header) == "solid") {
                mesh = parseSTL(input_file);
            }
            else {
                // Check if it's Binary STL by size
                ifs.seekg(0, std::ios::end);
                size_t size = ifs.tellg();
                if (size >= 84) {
                    mesh = parseSTL(input_file);
                }
                else {
                    throw std::runtime_error("Unknown file format for: " + input_file);
                }
            }
        }

        Orientation orientation = detectOrientation(input_file);
        transformToLHSYUp(mesh, orientation);

        std::string rib = generateRIB(mesh, std::filesystem::path(input_file).filename().string());

        std::ofstream ofs(output_file);
        if (!ofs.is_open()) {
            throw std::runtime_error("Could not open output file: " + output_file);
        }
        ofs << rib;
        ofs.close();

        std::cout << "Successfully converted " << input_file << " to " << output_file << "\n";
        std::cout << "Detected orientation: ";
        switch (orientation) {
            case Orientation::RHS_Y_UP:
                std::cout << "RHS Y-up";
                break;
            case Orientation::LHS_Y_UP:
                std::cout << "LHS Y-up";
                break;
            case Orientation::RHS_Z_UP:
                std::cout << "RHS Z-up";
                break;
            case Orientation::LHS_Z_UP:
                std::cout << "LHS Z-up";
                break;
        }
        std::cout << "\n";
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
