#include "3dtorib.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <stdexcept>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace threedtorib {

    void transformToLHSYUp(Mesh &mesh, Orientation original) {
        for (auto &p : mesh.points) {
            float x = p.x;
            float y = p.y;
            float z = p.z;

            switch (original) {
                case Orientation::RHS_Y_UP:
                    p.z = -z;
                    break;
                case Orientation::LHS_Y_UP:
                    break;
                case Orientation::RHS_Z_UP:
                    p.y = z;
                    p.z = y;
                    break;
                case Orientation::LHS_Z_UP:
                    p.y = z;
                    p.z = y;
                    break;
            }
        }

        if (original == Orientation::RHS_Y_UP || original == Orientation::RHS_Z_UP) {
            for (auto &face : mesh.faces) {
                if (face.indices.size() > 2) {
                    std::reverse(face.indices.begin() + 1, face.indices.end());
                }
            }
        }
    }

    Mesh parseSTL(const std::string &filename) {
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        char header[6] = {0};
        ifs.read(header, 5);
        ifs.seekg(0);

        Mesh mesh;
        if (std::string(header) == "solid") {
            // ASCII STL
            std::string line;
            while (std::getline(ifs, line)) {
                std::stringstream ss(line);
                std::string word;
                ss >> word;
                if (word == "vertex") {
                    float x, y, z;
                    ss >> x >> y >> z;
                    mesh.points.push_back({x, y, z});
                    if (mesh.points.size() % 3 == 0) {
                        int last = mesh.points.size() - 1;
                        mesh.faces.push_back({{last - 2, last - 1, last}});
                    }
                }
            }
        }
        else {
            // Binary STL
            char dummy[80];
            ifs.read(dummy, 80);

            uint32_t num_facets;
            ifs.read(reinterpret_cast<char *>(&num_facets), 4);

            mesh.points.reserve(num_facets * 3);
            mesh.faces.reserve(num_facets);

            for (uint32_t i = 0; i < num_facets; ++i) {
                float normal[3];
                float v[3][3];
                uint16_t attr_count;

                ifs.read(reinterpret_cast<char *>(normal), 12);
                for (int j = 0; j < 3; ++j) {
                    ifs.read(reinterpret_cast<char *>(v[j]), 12);
                    mesh.points.push_back({v[j][0], v[j][1], v[j][2]});
                }
                ifs.read(reinterpret_cast<char *>(&attr_count), 2);

                int last = mesh.points.size() - 1;
                mesh.faces.push_back({{last - 2, last - 1, last}});
            }
        }

        return mesh;
    }

    Mesh parseOBJ(const std::string &filename) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str())) {
            throw std::runtime_error("Failed to load OBJ: " + err);
        }

        Mesh mesh;
        // Extract vertices
        for (size_t v = 0; v < attrib.vertices.size(); v += 3) {
            mesh.points.push_back({attrib.vertices[v], attrib.vertices[v + 1], attrib.vertices[v + 2]});
        }

        // Extract faces
        for (const auto &shape : shapes) {
            size_t index_offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                int fv = shape.mesh.num_face_vertices[f];
                Face face;
                for (size_t v = 0; v < (size_t)fv; v++) {
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                    face.indices.push_back(idx.vertex_index);
                }
                mesh.faces.push_back(face);
                index_offset += fv;
            }
        }

        return mesh;
    }

    std::string generateRIB(const Mesh &mesh, const std::string &identifier) {
        std::stringstream ss;
        ss << "AttributeBegin\n";
        ss << "  Attribute \"identifier\" \"string name\" [\"" << identifier << "\"]\n";
        ss << "  PointsPolygons\n";

        // nvertices
        ss << "    [";
        for (size_t i = 0; i < mesh.faces.size(); ++i) {
            ss << mesh.faces[i].indices.size() << (i == mesh.faces.size() - 1 ? "" : " ");
        }
        ss << "]\n";

        // vertices
        ss << "    [";
        for (size_t i = 0; i < mesh.faces.size(); ++i) {
            for (size_t j = 0; j < mesh.faces[i].indices.size(); ++j) {
                ss << mesh.faces[i].indices[j] << (i == mesh.faces.size() - 1 && j == mesh.faces[i].indices.size() - 1 ? "" : " ");
            }
        }
        ss << "]\n";

        // "P"
        ss << "    \"P\" [";
        for (size_t i = 0; i < mesh.points.size(); ++i) {
            ss << mesh.points[i].x << " " << mesh.points[i].y << " " << mesh.points[i].z << (i == mesh.points.size() - 1 ? "" : " ");
        }
        ss << "]\n";

        ss << "AttributeEnd\n";
        return ss.str();
    }

    Orientation detectOrientation(const std::string &filename) {
        // Default to RHS Y-up for both OBJ and STL as it's most common
        Orientation detected = Orientation::RHS_Y_UP;

        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs.is_open())
            return detected;

        char buffer[1024] = {0};
        ifs.read(buffer, 1024);
        std::string content(buffer, ifs.gcount());

        // Simple heuristic: look for hints in header/comments
        if (content.find("Z-up") != std::string::npos || content.find("Z_UP") != std::string::npos) {
            detected = Orientation::RHS_Z_UP;
        }
        if (content.find("LHS") != std::string::npos) {
            if (detected == Orientation::RHS_Z_UP)
                detected = Orientation::LHS_Z_UP;
            else
                detected = Orientation::LHS_Y_UP;
        }

        return detected;
    }

} // namespace threedtorib
