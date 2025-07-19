#include "BinarySpacePartition.h"
#include "imgui.h"

void BinarySpacePartition::Update() {
	m_lines.clear();
    m_rooms.clear();
    Partition(glm::ivec3(0), m_size, 0);
}

void BinarySpacePartition::Draw(Shapes& shapes, const glm::mat4& vp, const glm::vec4& color) {
    for (auto& ln : m_lines) {
        shapes.Draw_Line(vp, ln.start, ln.end, color);
    }

    shapes.Draw_Rectangle(vp, m_origin, m_size, glm::vec4{ 1.f, 1.f, 1.f, 1.f }, WIREFRAME);
}

void BinarySpacePartition::DrawImgui() {
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("BSP Parameters")) {
        ImGui::SliderInt("Min Room Size", &params.minSize, 1, m_size.x/2);
        ImGui::SliderInt("Max Depth", &params.maxDepth, 1, 10);
        ImGui::SliderFloat("Balance", &params.balance, 0.01f, 0.49f);
        ImGui::SliderFloat3("Origin", &m_origin.x, -100, 100);
        ImGui::SliderFloat3("Size", &m_size.x, 0.1, 100);
        ImGui::TreePop();
    }
}

void BinarySpacePartition::Partition(const glm::ivec3& origin, const glm::ivec3& cubeSize, int depth) {
	srand(m_seed);

	if (depth >= params.maxDepth || glm::min(glm::min(cubeSize.x, cubeSize.y), cubeSize.z) < static_cast<int>(params.minSize) * 2) {
		m_rooms.push_back({ origin, cubeSize });
		return;
	}

	// Choose axis to split
	int axis = (cubeSize.x > cubeSize.y && cubeSize.x > cubeSize.z) ? 0 : (cubeSize.y > cubeSize.z ? 1 : 2);
	int axisLength = cubeSize[axis];

	int minBound = origin[axis];
	int maxBound = origin[axis] + axisLength;

	int validMin = minBound + static_cast<int>(params.balance * axisLength);
	int validMax = maxBound - static_cast<int>(params.balance * axisLength);

	if (validMax <= validMin) {
		m_rooms.push_back({ origin, cubeSize });
		return;
	}

	int split = validMin + rand() % (validMax - validMin + 1);

	int leftSize = split - minBound;
	int rightSize = maxBound - split;

	if (leftSize < params.minSize || rightSize < params.minSize) {
		m_rooms.push_back({ origin, cubeSize });
		return;
	}

	glm::vec3 p0 = glm::vec3(origin);
	glm::vec3 p1 = p0, p2 = p0, p3 = p0;

	switch (axis) {
	case 0:
		p0.x = split;
		p1 = p0 + glm::vec3(0, cubeSize.y, 0);
		p2 = p0 + glm::vec3(0, 0, cubeSize.z);
		p3 = p0 + glm::vec3(0, cubeSize.y, cubeSize.z);
		break;
	case 1:
		p0.y = split;
		p1 = p0 + glm::vec3(cubeSize.x, 0, 0);
		p2 = p0 + glm::vec3(0, 0, cubeSize.z);
		p3 = p0 + glm::vec3(cubeSize.x, 0, cubeSize.z);
		break;
	case 2:
		p0.z = split;
		p1 = p0 + glm::vec3(cubeSize.x, 0, 0);
		p2 = p0 + glm::vec3(0, cubeSize.y, 0);
		p3 = p0 + glm::vec3(cubeSize.x, cubeSize.y, 0);
		break;
	}

	glm::vec3 gridCenter = m_origin;

	p0 -= m_size * 0.5f;
	p1 -= m_size * 0.5f;
	p2 -= m_size * 0.5f;
	p3 -= m_size * 0.5f;

	m_lines.push_back({ p0, p1 });
	m_lines.push_back({ p0, p2 });
	m_lines.push_back({ p3, p1 });
	m_lines.push_back({ p3, p2 });

	// Recursive subdivision
	glm::ivec3 sizeA = cubeSize;
	glm::ivec3 sizeB = cubeSize;
	sizeA[axis] = leftSize;
	sizeB[axis] = rightSize;

	glm::vec3 originA = glm::vec3(origin);
	glm::vec3 originB = glm::vec3(origin);
	originB[axis] += leftSize;

	Partition(originA, glm::vec3(sizeA), depth + 1);
	Partition(originB, glm::vec3(sizeB), depth + 1);
}