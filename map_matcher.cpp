// hmm_map_matcher.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <string>
#include <cmath>

namespace py = pybind11;

struct Edge {
    std::string id;
    double lon;
    double lat;
};

class HMMMapMatcher {
public:
    // TODO Load and parse actual JSON file.
    HMMMapMatcher(const std::string &edges_file) {
        edges = {
            {"c99288f9c46a0c1ace90f28b757d8637", -113.3304709, 37.1772849},
            {"c11d13831bc9af12a0f9ec8461da7247", -113.3302991, 37.1741368},
            {"5eada682750b86bacda8e9f27c7cd084", -113.331711, 37.181558},
            {"f5bf9e1c5930c8c939cc7cce86b5030c", -113.3264404, 37.1745837},
            {"4e73234ad9a6fa6ce59c4e99528bb999", -113.3225885, 37.1696072}
        };
    }

    // TODO meet requirements on this algo
    std::vector<std::string> match_trace(const std::vector<std::pair<double, double>> &gps_trace) {
        std::vector<std::string> matched_edges;
        for (auto &point : gps_trace) {
            double min_distance = 1e9;
            std::string best_edge;
            for (auto &edge : edges) {
                double d = euclidean_distance(point.first, point.second, edge.lat, edge.lon);
                if (d < min_distance) {
                    min_distance = d;
                    best_edge = edge.id;
                }
            }
            matched_edges.push_back(best_edge);
        }
        return matched_edges;
    }

private:
    std::vector<Edge> edges;

    double euclidean_distance(double lat1, double lon1, double lat2, double lon2) {
        return std::sqrt((lat1 - lat2) * (lat1 - lat2) + (lon1 - lon2) * (lon1 - lon2));
    }
};

PYBIND11_MODULE(map_matcher, m) {
    m.doc() = "A basic Map Matcher module";
    py::class_<HMMMapMatcher>(m, "HMMMapMatcher")
        .def(py::init<const std::string &>(), py::arg("edges_file"))
        .def("match_trace", &HMMMapMatcher::match_trace, py::arg("gps_trace"),
             "Match a list of GPS points (lat, lon) to edge IDs.");
}
