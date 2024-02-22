#include "STLFile.h"
#include <iostream>
#include <fstream>
#include <sstream>

Triangle::Triangle(Eigen::Vector3f _points[], Eigen::Vector3f _norm)
{
    points[0] = _points[0];
    points[1] = _points[1];
    points[2] = _points[2];
    norm = _norm;
}

Triangle::Triangle(Eigen::Vector3f _points[], bool _norm_dir)
{
    using Eigen::Vector3f;
    points[0] = _points[0];
    points[1] = _points[1];
    points[2] = _points[2];
    Vector3f a = points[2] - points[1];
    Vector3f b = points[0] - points[1];
    if (_norm_dir)
    {
        norm = a.cross(b);
    }
    else
    {
        norm = b.cross(a);
    }
    norm.normalize();
}

Triangle::Triangle(Triangle & tri)
{
    points[0] = tri.points[0];
    points[1] = tri.points[1];
    points[2] = tri.points[2];
    norm = tri.norm;
}

void Triangle::Print()
{
    std::cout << "Tri:\n"
              << std::endl;
    std::cout << norm << std::endl;
    std::cout << points[0] << std::endl;
    std::cout << points[1] << std::endl;
    std::cout << points[2] << std::endl;
}

STLFile::STLFile(const char *t_name) : m_name(t_name)
{
    if (m_name.length() > 80)
        m_name.resize(80);
}

void STLFile::add_triangle(const Triangle &t_polygon)
{
    m_triangle_list.emplace_back(t_polygon);
}

void STLFile::add_stl(const STLFile &t_stl)
{
    std::size_t start_sz = count();
    std::size_t sz = t_stl.count();
    m_triangle_list.resize(start_sz + sz);
    for (std::size_t i = 0; i < sz; ++i)
        m_triangle_list.at(start_sz + i) = t_stl.m_triangle_list.at(i);
}

void STLFile::to_ascii(string &result)
{
    std::stringstream ss;
    for (auto &c : m_name)
        if (c == ' ' || c == '\n' || c == '\t')
            c = '_';

    ss << "solid " << m_name << "\n";

    for (auto &edr34                                              : m_triangle_list)
    {
        ss << " facet normal " << tri.norm[0] << " " << tri.norm[1] << " " << tri.norm[2] << "\n";
        ss << "  outer loop\n";
        ss << "   vertex " << tri.points[0][0] << " " << tri.points[0][1] << " " << tri.points[0][2] << "\n";
        ss << "   vertex " << tri.points[1][0] << " " << tri.points[1][1] << " " << tri.points[1][2] << "\n";
        ss << "   vertex " << tri.points[2][0] << " " << tri.points[2][1] << " " << tri.points[2][2] << "\n";
        ss << "  endloop\n endfacet\n";
    }

    ss << "endsolid " << m_name;

    result = ss.str();
}

void STLFile::set_header(const string &str)
{
    m_name = str;
    m_name.resize(80, '\0');
}

void STLFile::save_file(STLFile::file_type f_type, const string &filename)
{
    using namespace std;

    if (f_type == STLFile::ASCII)
    {
        ofstream file(filename.c_str());
        string result;
        to_ascii(result);
        file.write(result.c_str(), result.size());
        file.close();
    }
    else if (f_type == STLFile::BIN)
    {
        ofstream file(filename.c_str(), ios::binary);

        file.write(m_name.c_str(), 80);
        uint32_t size = m_triangle_list.size();
        file.write((char *)(&size), 4);
        for (uint32_t i = 0; i < size; i++)
        {
            /* code */
        }
        

        file.close();
    }
}
