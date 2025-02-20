// map_matcher.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace py = pybind11;
using json = nlohmann::json;

struct Edge {
    std::string id;
    double lon;
    double lat;
};

class HMMMapMatcher {
public:
    HMMMapMatcher(const std::string &edges_file) {
        std::ifstream ifs(edges_file);
        if (!ifs) {
            throw std::runtime_error("Could not open edges file: " + edges_file);
        }
        json j;
        ifs >> j;
        // j is array of edge objects
        for (auto& edge_json : j) {
            std::string id = edge_json["id"].get<std::string>();
            auto coordinates = edge_json["gps"]["coordinates"];
            double sum_lat = 0.0, sum_lon = 0.0;
            int count = 0;
            for (auto& coord : coordinates) {
                double lon = coord[0].get<double>();
                double lat = coord[1].get<double>();
                sum_lat += lat;
                sum_lon += lon;
                ++count;
            }
            double avg_lat = (count > 0) ? sum_lat / count : 0.0;
            double avg_lon = (count > 0) ? sum_lon / count : 0.0;
            edges.push_back({id, avg_lon, avg_lat});
        }
    }

    // Basic HMM map matcher using the Viterbi algorithm.
    // The emission cost is the distance between the GPS point and the edges representative coordinate.
    // The transition cost is the distance between consecutive edges representative coordinates.
    std::vector<std::string> match_trace(const std::vector<std::pair<double, double>> &gps_trace) {
        int N = gps_trace.size();
        int M = edges.size();
        if (N == 0 || M == 0) {
            return {};
        }
        // dp[i][j] is minimum cost to reach GPS point i if we select edge j for that point.
        std::vector<std::vector<double>> dp(N, std::vector<double>(M, 1e12));
        // prev[i][j] stores the index of the best previous edge for backtracking.
        std::vector<std::vector<int>> prev(N, std::vector<int>(M, -1));

        // Initialization for the first GPS point
        for (int j = 0; j < M; j++) {
            double emission = euclidean_distance(gps_trace[0].first, gps_trace[0].second,
                                                 edges[j].lat, edges[j].lon);
            dp[0][j] = emission;
        }

        // Fill the dp table for subsequent GPS points
        for (int i = 1; i < N; i++) {
            for (int j = 0; j < M; j++) {
                double emission = euclidean_distance(gps_trace[i].first, gps_trace[i].second,
                                                     edges[j].lat, edges[j].lon);
                double best_cost = 1e12;
                int best_prev = -1;
                for (int k = 0; k < M; k++) {
                    // Transition cost: distance between the representative coordinates of edge k and edge j.
                    double transition = euclidean_distance(edges[k].lat, edges[k].lon,
                                                           edges[j].lat, edges[j].lon);
                    double cost = dp[i-1][k] + transition;
                    if (cost < best_cost) {
                        best_cost = cost;
                        best_prev = k;
                    }
                }
                dp[i][j] = emission + best_cost;
                prev[i][j] = best_prev;
            }
        }

        // Find the best last state
        double best_final = 1e12;
        int best_index = -1;
        for (int j = 0; j < M; j++) {
            if (dp[N-1][j] < best_final) {
                best_final = dp[N-1][j];
                best_index = j;
            }
        }

        // Backtrack to recover the best path
        std::vector<int> path(N, -1);
        int current = best_index;
        for (int i = N - 1; i >= 0; i--) {
            path[i] = current;
            current = prev[i][current];
        }

        // Create a vector of edge IDs corresponding to the best path.
        std::vector<std::string> matched_edges;
        for (int i = 0; i < N; i++) {
            matched_edges.push_back(edges[path[i]].id);
        }
        return matched_edges;
    }

private:
    std::vector<Edge> edges;

    // A simple Euclidean distance function.
    double euclidean_distance(double lat1, double lon1, double lat2, double lon2) {
        double dlat = lat1 - lat2;
        double dlon = lon1 - lon2;
        return std::sqrt(dlat * dlat + dlon * dlon);
    }
};

PYBIND11_MODULE(map_matcher, m) {
    m.doc() = "A basic HMM Map Matcher module";
    py::class_<HMMMapMatcher>(m, "HMMMapMatcher")
        .def(py::init<const std::string &>(), py::arg("edges_file"))
        .def("match_trace", &HMMMapMatcher::match_trace, py::arg("gps_trace"),
             "Match a list of GPS points (lat, lon) to a sequence of edge IDs.");
}
