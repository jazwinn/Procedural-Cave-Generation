#include "BinarySpacePartition.h"

void BinarySpacePartition::Update() {
	m_lines.clear();
    m_rooms.clear();
    Partition(m_origin, m_size, 0);
}

void BinarySpacePartition::Draw(Shapes& shapes, const glm::mat4& vp, const glm::vec4& color) {
    for (auto& ln : m_lines) {
        shapes.Draw_Line(vp, ln.start, ln.end, color);
    }
}

void BinarySpacePartition::DrawImgui() {
    if (ImGui::TreeNode("BSP Parameters")) {
        ImGui::SliderInt("Min Room Size", &params.minSize, 1, 3);
        ImGui::SliderInt("Max Depth", &params.maxDepth, 1, 10);
        ImGui::SliderFloat("Balance", &params.balance, 0.1f, 0.3f);
        ImGui::InputFloat3("Origin", &m_origin.x);
        ImGui::InputFloat3("Size", &m_size.x);
        ImGui::TreePop();
    }
}

void BinarySpacePartition::Partition(const glm::vec3& origin, const glm::vec3& size, int depth) {
    if (depth >= params.maxDepth || glm::min(glm::min(size.x, size.y), size.z) < params.minSize * 2) {
        m_rooms.push_back({ origin, size });
        return;
    }
    int axis = (size.x > size.y && size.x > size.z) ? 0 : (size.y > size.z ? 1 : 2);
    float len = size[axis];
    int minB = int(len * params.balance);
    int maxB = int(len - minB);
    if (maxB - minB < params.minSize) {
        m_rooms.push_back({ origin, size });
        return;
    }
    float split = float(minB + rand() % (maxB - minB));

    glm::vec3 p0 = origin;
    glm::vec3 p1 = origin;
    glm::vec3 p2 = origin;
    glm::vec3 p3 = origin;
    switch (axis) {
    case 0:
        p0 = origin + glm::vec3{ split, 0, 0 };
        p1 = origin + glm::vec3{ split, size.y, 0 };
        p2 = origin + glm::vec3{ split, 0, size.z };
        p3 = origin + glm::vec3{ split, size.y, size.z };
        break;
    case 1:
        p0 = origin + glm::vec3{ 0, split, 0 };
        p1 = origin + glm::vec3{ size.x, split, 0 };
        p2 = origin + glm::vec3{ 0, split, size.z };
        p3 = origin + glm::vec3{ size.x, split, size.z };
        break;
    case 2:
        p0 = origin + glm::vec3{ 0, 0, split };
        p1 = origin + glm::vec3{ size.x, 0, split };
        p2 = origin + glm::vec3{ 0, size.y, split };
        p3 = origin + glm::vec3{ size.x, size.y, split };
        break;
    }
    m_lines.push_back({ p0, p1 });
    m_lines.push_back({ p0, p2 });
    m_lines.push_back({ p3, p1 });
    m_lines.push_back({ p3, p2 });

    glm::vec3 sizeA = size;
    glm::vec3 sizeB = size;
    glm::vec3 originB = origin;
    sizeA[axis] = split;
    sizeB[axis] = size[axis] - split;
    originB[axis] += split;
    Partition(origin, sizeA, depth + 1);
    Partition(originB, sizeB, depth + 1);
}