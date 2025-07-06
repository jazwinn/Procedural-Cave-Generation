#include "CellularAutomata.h"
#include "Imgui.h"


void CellularAutomata::SetChunk(std::shared_ptr<Chunks> chunk, const glm::vec3& worldMin, const glm::vec3& worldSize) {
    m_chunk = chunk;
    m_worldMin = worldMin;
    m_worldSize = worldSize;
    m_width = chunk->GetWidth();
    m_height = chunk->GetHeight();
    m_depth = chunk->GetDepth();

    m_grid.assign(m_width * m_height * m_depth, 1);
    m_buffer = m_grid;
    m_currentIter = 0;
}

void CellularAutomata::SetSeeds(const std::vector<BinarySpacePartition::Room>& rooms) {
    std::fill(m_grid.begin(), m_grid.end(), 0); // EMPTY
    m_buffer = m_grid;
    m_currentIter = 0;
    m_completed = false;

    std::mt19937 rng(m_seed);

    auto worldToCell = [&](glm::vec3 ws) {
        glm::vec3 norm = (ws - m_worldMin) / m_worldSize;
        return glm::ivec3{
            glm::clamp(int(norm.x * m_width), 0, m_width - 1),
            glm::clamp(int(norm.y * m_height), 0, m_height - 1),
            glm::clamp(int(norm.z * m_depth), 0, m_depth - 1)
        };
        };

    for (const auto& room : rooms) {
        glm::vec3 roomMin = room.center;
        glm::vec3 roomMax = room.center + room.extent;

        if (m_params.spreadingMode) {
            std::uniform_real_distribution<float> xr(roomMin.x, roomMax.x);
            std::uniform_real_distribution<float> yr(roomMin.y, roomMax.y);
            std::uniform_real_distribution<float> zr(roomMin.z, roomMax.z);

            for (int i = 0; i < m_params.numSeeds; ++i) {
                glm::vec3 randPos = glm::vec3(xr(rng), yr(rng), zr(rng));
                glm::ivec3 cell = worldToCell(randPos);
                m_grid[cell.x + cell.y * m_width + cell.z * m_width * m_height] = 1;
            }
        }
        else {
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            glm::ivec3 c0 = worldToCell(roomMin);
            glm::ivec3 c1 = worldToCell(roomMax);

            for (int z = c0.z; z <= c1.z; ++z)
                for (int y = c0.y; y <= c1.y; ++y)
                    for (int x = c0.x; x <= c1.x; ++x) {
                        float p = dist(rng);
                        m_grid[x + y * m_width + z * m_width * m_height] = (p < m_params.wallProb) ? 1 : 0;
                    }
        }
    }

    for (int z = 0; z < m_depth; ++z)
        for (int y = 0; y < m_height; ++y)
            for (int x = 0; x < m_width; ++x)
                m_chunk->at(x, y, z) = m_grid[x + y * m_width + z * m_width * m_height] ? SOLID : EMPTY;
}

bool CellularAutomata::RunIteration() {
    bool changed = false;
    auto idx = [&](int x, int y, int z) { return x + y * m_width + z * m_width * m_height; };

    for (int z = 0; z < m_depth; ++z) {
        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                int cur = m_grid[idx(x, y, z)];
                int next = cur;

                int count = 0;
                for (int dz = -1; dz <= 1; ++dz)
                    for (int dy = -1; dy <= 1; ++dy)
                        for (int dx = -1; dx <= 1; ++dx) {
                            if (dx == 0 && dy == 0 && dz == 0) continue;
                            int nx = x + dx, ny = y + dy, nz = z + dz;
                            if (nx < 0 || ny < 0 || nz < 0 ||
                                nx >= m_width || ny >= m_height || nz >= m_depth)
                                continue;
                            if (m_grid[idx(nx, ny, nz)] == 1)
                                count++;
                        }

                if (m_params.spreadingMode) {
                    // Builder 1 rule: B: 2,6,9 / S: 4,6,8-9
                    if (cur == 1)
                        next = (count == 4 || count == 6 || count == 8 || count == 9) ? 1 : 0;
                    else
                        next = (count == 2 || count == 6 || count == 9) ? 1 : 0;
                }
                else {
                    // Classic CA rule
                    if (cur == 1 && count < m_params.deathLimit)
                        next = 0;
                    else if (cur == 0 && count > m_params.birthLimit)
                        next = 1;
                }

                m_buffer[idx(x, y, z)] = next;
                if (next != cur) changed = true;
            }
        }
    }

    std::swap(m_grid, m_buffer);

    // Update the voxel chunk
    for (int z = 0; z < m_depth; ++z)
        for (int y = 0; y < m_height; ++y)
            for (int x = 0; x < m_width; ++x)
                m_chunk->at(x, y, z) = m_grid[idx(x, y, z)] ? SOLID : EMPTY;

    return changed;
}

void CellularAutomata::Update() {
    if (m_completed) { return; }
    bool changed = RunIteration();
    ++m_currentIter;
    if (!changed || m_currentIter >= m_params.maxIterations) { m_completed = true; }
}

void CellularAutomata::Draw(Shapes& /*shapes*/, const glm::mat4& /*vp*/, const glm::vec4& /*color*/)
{
   
}

void CellularAutomata::DrawImgui() {
    if (ImGui::TreeNode("Cellular Automata Parameters")) {
        ImGui::Checkbox("Spreading Mode", &m_params.spreadingMode);
        if (m_params.spreadingMode) {
            ImGui::SliderInt("Seed Count", &m_params.numSeeds, 1, 10);
        } else {
            ImGui::SliderFloat("Wall Probability", &m_params.wallProb, 0.0f, 1.0f);
            ImGui::SliderInt("Birth Limit", &m_params.birthLimit, 1, 8);
            ImGui::SliderInt("Death Limit", &m_params.deathLimit, 1, 8);
        }
        ImGui::SliderInt("Max Iterations", &m_params.maxIterations, 1, 20);
        ImGui::TreePop();
    }
}