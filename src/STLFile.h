#ifndef IMG2STL_STLFILE_H_
#define IMG2STL_STLFILE_H_

#include <cstdint>
#include <string>
#include <vector>
#include "common.h"

struct __attribute__((packed)) Triangle
{
    Eigen::Vector3f norm;
    Eigen::Vector3f points[3];
    uint16_t attribute_byte_count;

    Triangle(Eigen::Vector3f _points[], Eigen::Vector3f _norm);
    Triangle(Eigen::Vector3f _points[], bool _norm_dir);
    Triangle(Triangle & tri);

    void Print();
};

class STLFile
{
public:
    enum file_type
    {
        ASCII = 0,
        BIN
    };

    explicit STLFile(const char *t_name = "");
    ~STLFile() { m_triangle_list.clear(); };

    void add_triangle(const Triangle &t_polygon);
    void add_stl(const STLFile &t_stl);

    STLFile &operator+=(const Triangle &t_polygon)
    {
        add_triangle(t_polygon);
        return *this;
    }

    STLFile &operator+=(const STLFile &t_stl)
    {
        add_stl(t_stl);
        return *this;
    }

    void clear() { m_triangle_list.clear(); }

    void to_ascii(string &result);
    void to_bin(uint8_t **t_result, uint32_t &t_size);

    uint32_t count() const { return m_triangle_list.size(); }

    void set_header(const string &str);

    void save_file(file_type f_type, const string &filename);

private:
    std::string m_name; // name (for ASCII; without whitespaces) or header (for BIN; <= 80 symbols)
    std::vector<Triangle> m_triangle_list;
};

#endif //   IMG2STL_STLFILE_H_
