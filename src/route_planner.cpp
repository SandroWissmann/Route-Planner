#include "route_planner.h"
#include <algorithm>

#include <iostream>

RoutePlanner::RoutePlanner(RouteModel& model, float start_x, float start_y,
                           float end_x, float end_y)
    : m_Model(model)
{
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    start_node = &m_Model.FindClosestNode(start_x, start_y);
    end_node = &m_Model.FindClosestNode(end_x, end_y);
}

float RoutePlanner::CalculateHValue(RouteModel::Node const* node)
{
    return node->distance(*end_node);
}

void RoutePlanner::AddNeighbors(RouteModel::Node* current_node)
{
    current_node->FindNeighbors();
    auto neighbors = current_node->neighbors;

    if (open_list.capacity() < open_list.size() + neighbors.size()) {
        open_list.reserve(open_list.size() + neighbors.size());
    }

    for (auto& neighbor : neighbors) {
        neighbor->parent = current_node;

        auto g_delta = current_node->distance(*neighbor);

        neighbor->g_value = current_node->g_value + g_delta;
        neighbor->h_value = CalculateHValue(neighbor);

        open_list.push_back(neighbor);
        neighbor->visited = true;
    }
}

RouteModel::Node* RoutePlanner::NextNode()
{
    std::sort(open_list.begin(), open_list.end(),
              [](const auto& a, const auto& b) {
                  return a->h_value + a->g_value > b->h_value + b->g_value;
              });
    auto nextNode = open_list.back();
    open_list.pop_back();
    return nextNode;
}

std::vector<RouteModel::Node>
RoutePlanner::ConstructFinalPath(RouteModel::Node* current_node)
{
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    while (current_node->parent != nullptr) {
        path_found.push_back(*current_node);
        distance += current_node->distance(*current_node->parent);
        current_node = current_node->parent;
    }

    path_found.push_back(*current_node);

    std::reverse(path_found.begin(), path_found.end());

    // Multiply the distance by the scale of the map to get meters.
    distance *= m_Model.MetricScale();
    return path_found;
}

void RoutePlanner::AStarSearch()
{
    start_node->visited = true;
    open_list.push_back(start_node);
    RouteModel::Node* current_node = nullptr;

    while (!open_list.empty()) {
        current_node = NextNode();

        if (current_node == end_node) {
            m_Model.path = ConstructFinalPath(current_node);
            return;
        }
        AddNeighbors(current_node);
    }
}