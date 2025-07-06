#pragma once

#ifndef BINARYSPACEPARTITION_H
#define BINARYSPACEPARTITION_H

#include "Generator.h"
#include "Shapes.h"


struct BSPParams {
	int minSize   = 25;
	int maxDepth  = 6;
	float balance = 0.3f;
	int buffer = 10; // Buffer space around rooms
	bool randomRooms = false;
	int randomRoomCount = 10; // Number of random rooms to generate
};

class BinarySpacePartition : public Generator {
public:
	BinarySpacePartition(unsigned int& seed) :m_seed(seed) {
		
	}
	BSPParams params;
	glm::vec3 m_origin{ 0.0f };
	glm::vec3 m_size{ 1.0f };

	struct Line {
		glm::vec3 start;
		glm::vec3 end;
	};
	std::vector<Line> m_lines;

	struct Room {
		glm::vec3 center;
		glm::vec3 extent;
	};

	std::vector<Room> m_rooms;

	virtual void Update() override;
	virtual void Draw(Shapes&, const glm::mat4&, const glm::vec4&) override;
	virtual void DrawImgui() override;
	
	void SetBounds(const glm::vec3& origin, const glm::vec3& size) {
		m_origin = origin;
		m_size = size;
	}
	
	std::vector<Room> GetRooms() { return m_rooms; }
private:
	void Partition(const glm::vec3&, const glm::vec3&, int);
	unsigned int& m_seed;
};

#endif