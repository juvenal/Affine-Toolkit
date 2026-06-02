#ifndef THREED_TO_RIB_HPP
#define THREED_TO_RIB_HPP

#include <vector>
#include <string>
#include <array>

namespace threedtorib {

    struct Vector3 {
            float x, y, z;
    };

    struct Face {
            std::vector<int> indices;
    };

    struct Mesh {
            std::vector<Vector3> points;
            std::vector<Face> faces;
    };

    enum class Orientation {
        RHS_Y_UP,
        LHS_Y_UP,
        RHS_Z_UP,
        LHS_Z_UP
    };

    // Transform mesh to LHS Y-up
    void transformToLHSYUp(Mesh &mesh, Orientation original);

    // Parse STL file (Binary or ASCII)
    Mesh parseSTL(const std::string &filename);

    // Parse OBJ file using tinyobjloader
    Mesh parseOBJ(const std::string &filename);

    // Generate RIB fragment
    std::string generateRIB(const Mesh &mesh, const std::string &identifier);

    // Detect orientation from file content/extension
    Orientation detectOrientation(const std::string &filename);

} // namespace threedtorib

#endif // THREED_TO_RIB_HPP
