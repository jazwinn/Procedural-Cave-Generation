#pragma once

#ifndef BINARYSPACEPARTITION_H
#define BINARYSPACEPARTITION_H

#include "Generator.h"
#include "DrawShapes.h"

struct BSPParams {
	int minSize   = 25;
	int maxDepth  = 6;
	float balance = 0.3f;

};

class BinarySpacePartition : public Generator {
public:
	BinarySpacePartition(unsigned int& seed) :m_seed(seed) {}

	struct Line {
		glm::vec3 start;
		glm::vec3 end;
	};

	struct Room {
		glm::vec3 StartPoint;
		glm::vec3 extent;
	};

	std::vector<Line> m_lines;
	std::vector<Room> m_rooms;
	
	void SetBounds(const glm::vec3& origin, const glm::vec3& size) {
		m_origin = origin;
		m_size = size;
	}

	void Clear() {
		m_lines.clear();
		m_rooms.clear();
	}
	
	std::vector<Room> GetRooms() { return m_rooms; }
	std::vector<Line> GetLines() { return m_lines; }
	glm::vec3 GetOrigin() const { return m_origin; }
	glm::vec3 GetSize() const { return m_size; }

	virtual void Update() override;
	virtual void Draw(Shapes&, const glm::mat4&, const glm::vec4&) override;
	virtual void DrawImgui() override;

public:
	BSPParams params;
	glm::vec3 m_origin{ 0.0f };
	glm::vec3 m_size{ 1.0f };

private:
	void Partition(const glm::ivec3&, const glm::ivec3&, int depth);
	unsigned int& m_seed;
};

#endif