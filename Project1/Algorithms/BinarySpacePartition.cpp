#include "BinarySpacePartition.h"
#include "imgui.h"

void BinarySpacePartition::Update() {
	m_lines.clear();
    m_rooms.clear();
    Partition(m_origin, m_size, 0);
}

void BinarySpacePartition::Draw(Shapes& shapes, const glm::mat4& vp, const glm::vec4& color) {
    for (auto& ln : m_lines) {
        shapes.Draw_Line(vp, ln.start, ln.end, color);
    }

    shapes.Draw_Rectangle(vp, m_origin, m_size, glm::vec4{ 1.f,1.f,1.f,1.f }, WIREFRAME);
}

void BinarySpacePartition::DrawImgui() {
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("BSP Parameters")) {
        ImGui::SliderInt("Min Room Size", &params.minSize, 1, m_size.x/2);
        ImGui::SliderInt("Max Depth", &params.maxDepth, 1, 10);
		ImGui::SliderInt("Buffer", &params.buffer, 0, 20);
        ImGui::SliderFloat("Balance", &params.balance, 0.01f, 0.49f);
        ImGui::SliderFloat3("Origin", &m_origin.x, -100, 100);
        ImGui::SliderFloat3("Size", &m_size.x, 0.1, 100);
		ImGui::Checkbox("Random Rooms", &params.randomRooms);
		if (params.randomRooms) {
			ImGui::SliderInt("Max Room", &params.maxRoomCount, 1, 100);
		}


        ImGui::TreePop();
    }
}

void BinarySpacePartition::Partition(const glm::vec3& origin, const glm::vec3& size, int depth) {

    srand(m_seed);

    glm::vec3 offSet = size * 0.5f;

    if (depth >= params.maxDepth || glm::min(glm::min(size.x, size.y), size.z) < params.minSize * 2) {
        m_rooms.push_back({ origin, size });
        return;
    }

    int axis = (size.x > size.y && size.x > size.z) ? 0 : (size.y > size.z ? 1 : 2);


    float axisLength = size[axis];

    //int minB = int(len * params.balance);
    //int maxB = int(len - minB);
    ////if (maxB - minB < params.minSize) {
    ////    m_rooms.push_back({ origin, size });
    ////    return;
    ////}

	float minBound = origin[axis] - offSet[axis];
	float maxBound = origin[axis] + offSet[axis];

	float validMinBound = minBound + params.balance * axisLength;
	float validMaxBound = maxBound - params.balance * axisLength;

	float splitRange = validMaxBound - validMinBound;
	if (splitRange < 1.0f) {
		// Not enough space to split meaningfully
		m_rooms.push_back({ origin, size });
		return;
	}

    float split = 0;


	int count = 0;
    while (true) {
		split = validMinBound + static_cast<float>(rand()) / RAND_MAX * splitRange;

		float leftSize = split - minBound;
		float rightSize = maxBound - split;

		if (leftSize < params.minSize || rightSize < params.minSize) {
			count++;


			if (count >= 10) {
				m_rooms.push_back({ origin, size });
				return;
			}


			continue; // too small on one side
		}



        break;
    }
    

	glm::vec3 p0 = origin;
	glm::vec3 p1 = origin;
	glm::vec3 p2 = origin;
	glm::vec3 p3 = origin;

	switch (axis) {
	case 0: // Split along X
		p0 = origin + glm::vec3{ split - origin.x, -offSet.y, -offSet.z };
		p1 = origin + glm::vec3{ split - origin.x,  offSet.y, -offSet.z };
		p2 = origin + glm::vec3{ split - origin.x, -offSet.y,  offSet.z };
		p3 = origin + glm::vec3{ split - origin.x,  offSet.y,  offSet.z };
		break;
	case 1: // Split along Y
		p0 = origin + glm::vec3{ -offSet.x, split - origin.y, -offSet.z };
		p1 = origin + glm::vec3{ offSet.x, split - origin.y, -offSet.z };
		p2 = origin + glm::vec3{ -offSet.x, split - origin.y,  offSet.z };
		p3 = origin + glm::vec3{ offSet.x, split - origin.y,  offSet.z };
		break;
	case 2: // Split along Z
		p0 = origin + glm::vec3{ -offSet.x, -offSet.y, split - origin.z };
		p1 = origin + glm::vec3{ offSet.x, -offSet.y, split - origin.z };
		p2 = origin + glm::vec3{ -offSet.x,  offSet.y, split - origin.z };
		p3 = origin + glm::vec3{ offSet.x,  offSet.y, split - origin.z };
		break;
	}
    m_lines.push_back({ p0, p1 });
    m_lines.push_back({ p0, p2 });
    m_lines.push_back({ p3, p1 });
    m_lines.push_back({ p3, p2 });

	float leftSize = split - minBound;
	float rightSize = maxBound - split;

	glm::vec3 sizeA = size;
	glm::vec3 sizeB = size;
	sizeA[axis] = leftSize;
	sizeB[axis] = rightSize;

	glm::vec3 originA = origin;
	glm::vec3 originB = origin;
	originA[axis] = minBound + leftSize * 0.5f;
	originB[axis] = split + rightSize * 0.5f;


	Partition(originA, sizeA, depth + 1);
	Partition(originB, sizeB, depth + 1);
}