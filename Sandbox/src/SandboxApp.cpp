#include "ExampleLayer.h"

#include <Marx.h>

#include <bitset>

struct WallVertex
{
	DX::XMFLOAT2 pos;
};
struct PlayerVertex
{
	DX::XMFLOAT4 pos;
};
struct WallIndices
{
	uint32_t p[2];
};
enum WallDir
{
	WallDir_Top = 0,
	WallDir_Right,
	WallDir_Bottom,
	WallDir_Left
};
enum MoveDir
{
	MoveDir_Up = 0,
	MoveDir_Down,
	MoveDir_Left,
	MoveDir_Right,
	MoveDir_None
};
struct Position
{
	uint32_t x, y;
};
struct LevelCell
{
	LevelCell() { walls.set(); }
public:
	std::bitset<4> walls;
	bool isGenVisited = false;
};

class GoalLine
{
public:
	GoalLine(Position goalPos)
		: m_goalPos(goalPos)
	{
		m_pShader.reset(
			Marx::Shader::create(
				Marx::Shader::loadShaderSrcFromFile("C:\\dev\\Marx\\Marx\\src\\shaders\\mazeLevelVS.hlsl"),
				Marx::Shader::loadShaderSrcFromFile("C:\\dev\\Marx\\Marx\\src\\shaders\\mazeLevelPS.hlsl")
			)
		);

		{
			float vertices[] = {
				(float)goalPos.x + 0.5f, (float)goalPos.y + 0.5f,
				1.0f, 1.0f
			};

			Marx::Reference<Marx::VertexBuffer> pVertexBuffer(
				Marx::VertexBuffer::create(vertices, 2 * sizeof(WallVertex))
			);
			pVertexBuffer->setLayout({
				{ Marx::ShaderDataType::Float2, "POSITION" }
				});

			uint32_t indices[] = { 0, 1 };

			Marx::Reference<Marx::IndexBuffer> pIndexBuffer(
				Marx::IndexBuffer::create(indices, 2, Marx::IndexBuffer::PrimitiveType::LineList)
			);

			m_pVertexArrayLine.reset(
				Marx::VertexArray::create()
			);

			m_pVertexArrayLine->setVertexBuffer(pVertexBuffer);
			m_pVertexArrayLine->setIndexBuffer(pIndexBuffer);
		}

		{
			WallVertex vertices[] = {
			   {{0.2f, 0.2f}},
			   {{0.2f, 0.8f}},
			   {{0.8f, 0.8f}},
			   {{0.8f, 0.2f}},
			};

			Marx::Reference<Marx::VertexBuffer> pVertexBuffer(
				Marx::VertexBuffer::create(vertices, 4 * sizeof(WallVertex))
			);
			pVertexBuffer->setLayout({
				{ Marx::ShaderDataType::Float2, "POSITION" }
				});

			uint32_t indices[] = {
				0, 1, 2,
				2, 3, 0
			};

			Marx::Reference<Marx::IndexBuffer> pIndexBuffer(
				Marx::IndexBuffer::create(indices, 6, Marx::IndexBuffer::PrimitiveType::TriangleList)
			);

			m_pVertexArrayGoal.reset(
				Marx::VertexArray::create()
			);
			m_pVertexArrayGoal->setVertexBuffer(pVertexBuffer);
			m_pVertexArrayGoal->setIndexBuffer(pIndexBuffer);
		}
	}
public:
	void onRender()
	{
		DX::XMMATRIX transform = DX::XMMatrixScaling(0.1f, 0.1f, 0.1f);
		Marx::Renderer::submit(m_pShader, m_pVertexArrayLine, DX::XMMatrixTranspose(transform), { 1.0f, 1.0f, 1.0f, 1.0f });

		transform = DX::XMMatrixTranslation((float)m_goalPos.x, (float)m_goalPos.y, 0.0f)
			* DX::XMMatrixScaling(0.1f, 0.1f, 0.1f);
		Marx::Renderer::submit(m_pShader, m_pVertexArrayGoal, DX::XMMatrixTranspose(transform), { 1.0f, 1.0f, 1.0f, 1.0f });
	}
	void setPlayerPos(Position pos)
	{
		WallVertex vertex{ { (float)pos.x + 0.5f, (float)pos.y + 0.5f } };
		m_pVertexArrayLine->getVertexBuffer()->updatePartial(&vertex.pos, 1, 1);
	}
private:
	Marx::Reference<Marx::Shader> m_pShader;
	Marx::Reference<Marx::VertexArray> m_pVertexArrayLine;
	Marx::Reference<Marx::VertexArray> m_pVertexArrayGoal;
	Position m_goalPos;
};

template<uint32_t width, uint32_t height>
class Level
{
public:
	Level()
	{
		m_pShader.reset(
			Marx::Shader::create(
				Marx::Shader::loadShaderSrcFromFile("C:\\dev\\Marx\\Marx\\src\\shaders\\mazeLevelVS.hlsl"),
				Marx::Shader::loadShaderSrcFromFile("C:\\dev\\Marx\\Marx\\src\\shaders\\mazeLevelPS.hlsl")
			)
		);

		constexpr uint32_t nVertices = (width + 1) * (height + 1);

		std::shared_ptr<WallVertex*> pVertices = std::make_shared<WallVertex*>(new WallVertex[nVertices]);
		uint32_t i = 0;
		for (int y = 0; y <= height; ++y)
		{
			for (int x = 0; x <= width; ++x)
			{
				(*pVertices.get())[i].pos = { (float)x, (float)y };
				++i;
			}
		}

		Marx::Reference<Marx::VertexBuffer> pVertexBuffer(
			Marx::VertexBuffer::create(*pVertices.get(), nVertices * sizeof(WallVertex))
		);
		pVertexBuffer->setLayout({
			{Marx::ShaderDataType::Float2, "POSITION"}
			});


		constexpr uint32_t nIndices = (width * (height + 1) + height * (width + 1)) * 2;

		Marx::Reference<Marx::IndexBuffer> pIndexBuffer(
			Marx::IndexBuffer::create(nullptr, nIndices, Marx::IndexBuffer::PrimitiveType::LineList)
		);

		m_pVertexArray.reset(
			Marx::VertexArray::create()
		);
		m_pVertexArray->setVertexBuffer(pVertexBuffer);
		m_pVertexArray->setIndexBuffer(pIndexBuffer);

		std::srand((unsigned int)std::time(NULL));
		generateLevel();
	}
public:
	void onRender()
	{
		DX::XMMATRIX transform = DX::XMMatrixScaling(0.1f, 0.1f, 0.1f);
		Marx::Renderer::submit(m_pShader, m_pVertexArray, transform, { 1.0f, 1.0f, 1.0f, 1.0f });
	}
	WallIndices genWallIndices(uint32_t x, uint32_t y, WallDir w)
	{
		uint32_t worldPos = getWorldPos(x, y);
		switch (w)
		{
		case WallDir_Top:
			return { worldPos + y + width + 1, worldPos + y + width + 2 };
		case WallDir_Right:
			return { worldPos + y + 1, worldPos + y + width + 2 };
		case WallDir_Bottom:
			return { worldPos + y, worldPos + y + 1 };
		case WallDir_Left:
			return { worldPos + y, worldPos + y + width + 1 };
		default:
			return WallIndices();
		}
	}
	void addWall(uint32_t x, uint32_t y, WallDir w)
	{
		WallIndices p = genWallIndices(x, y, w);
		auto iBuf = m_pVertexArray->getIndexBuffer();
		iBuf->updatePartial(p.p, iBuf->getCount(), 2);
		iBuf->setCount(iBuf->getCount() + 2);
	}
	void setVisited(uint32_t x, uint32_t y)
	{
		if (isVisited(x, y))
			return;

		m_visited.set(getWorldPos(x, y));
		if (m_cells[getWorldPos(x, y)].walls[WallDir_Left] && ((x == 0) || ((x > 0) && !isVisited(x - 1, y))))
			addWall(x, y, WallDir_Left);
		if (m_cells[getWorldPos(x, y)].walls[WallDir_Bottom] && ((y == 0) || ((y > 0) && !isVisited(x, y - 1))))
			addWall(x, y, WallDir_Bottom);
		if (m_cells[getWorldPos(x, y)].walls[WallDir_Right] && ((x == width - 1) || ((x < width - 1) && !isVisited(x + 1, y))))
			addWall(x, y, WallDir_Right);
		if (m_cells[getWorldPos(x, y)].walls[WallDir_Top] && ((y == height - 1) || ((y < height - 1) && !isVisited(x, y + 1))))
			addWall(x, y, WallDir_Top);
	}
	bool isVisited(uint32_t x, uint32_t y) { return m_visited.test(getWorldPos(x, y)); }
	uint32_t getWorldPos(uint32_t x, uint32_t y) { return y * width + x; }
	MoveDir randomPossibleMoveDir(const Position& currPos)
	{
		std::vector<MoveDir> possibleDirs;
		if ((currPos.y > 0) && !m_cells[getWorldPos(currPos.x, currPos.y - 1)].isGenVisited)
			possibleDirs.push_back(MoveDir_Down);
		if ((currPos.x > 0) && !m_cells[getWorldPos(currPos.x - 1, currPos.y)].isGenVisited)
			possibleDirs.push_back(MoveDir_Left);
		if ((currPos.x < width - 1) && !m_cells[getWorldPos(currPos.x + 1, currPos.y)].isGenVisited)
			possibleDirs.push_back(MoveDir_Right);
		if ((currPos.y < height - 1) && !m_cells[getWorldPos(currPos.x, currPos.y + 1)].isGenVisited)
			possibleDirs.push_back(MoveDir_Up);

		if (possibleDirs.size() == 0)
			return MoveDir_None;
		return possibleDirs[std::rand() % possibleDirs.size()];
	}
	bool isPossibleMoveDir(const Position& pos, MoveDir dir)
	{
		switch (dir)
		{
		case MoveDir_Up:
			return (pos.y < height - 1) && !m_cells[getWorldPos(pos.x, pos.y)].walls[WallDir_Top];
		case MoveDir_Down:
			return (pos.y > 0) && !m_cells[getWorldPos(pos.x, pos.y)].walls[WallDir_Bottom];
		case MoveDir_Left:
			return (pos.x > 0) && !m_cells[getWorldPos(pos.x, pos.y)].walls[WallDir_Left];
		case MoveDir_Right:
			return (pos.x < width - 1) && !m_cells[getWorldPos(pos.x, pos.y)].walls[WallDir_Right];
		}
		return false;
	}
	Position getGoalPos() { return m_goalPos; };
private:
	void generateLevel()
	{
		Position currPos = { 0, 0 };
		uint32_t deepCellPos = 0;
		uint32_t deepCellDepth = 1;
		std::vector<uint32_t> genTree;
		genTree.push_back(getWorldPos(currPos.x, currPos.y));
		m_cells[getWorldPos(currPos.x, currPos.y)].isGenVisited = true;

		while (genTree.size() > 0)
		{
			switch (randomPossibleMoveDir(currPos))
			{
			case MoveDir_Up:
				m_cells[getWorldPos(currPos.x, currPos.y)].walls.reset(WallDir_Top);
				++currPos.y;
				m_cells[getWorldPos(currPos.x, currPos.y)].walls.reset(WallDir_Bottom);
				m_cells[getWorldPos(currPos.x, currPos.y)].isGenVisited = true;
				break;
			case MoveDir_Down:
				m_cells[getWorldPos(currPos.x, currPos.y)].walls.reset(WallDir_Bottom);
				--currPos.y;
				m_cells[getWorldPos(currPos.x, currPos.y)].walls.reset(WallDir_Top);
				m_cells[getWorldPos(currPos.x, currPos.y)].isGenVisited = true;
				break;
			case MoveDir_Left:
				m_cells[getWorldPos(currPos.x, currPos.y)].walls.reset(WallDir_Left);
				--currPos.x;
				m_cells[getWorldPos(currPos.x, currPos.y)].walls.reset(WallDir_Right);
				m_cells[getWorldPos(currPos.x, currPos.y)].isGenVisited = true;
				break;
			case MoveDir_Right:
				m_cells[getWorldPos(currPos.x, currPos.y)].walls.reset(WallDir_Right);
				++currPos.x;
				m_cells[getWorldPos(currPos.x, currPos.y)].walls.reset(WallDir_Left);
				m_cells[getWorldPos(currPos.x, currPos.y)].isGenVisited = true;
				break;
			default:
				if (genTree.size() > deepCellDepth)
				{
					deepCellDepth = (uint32_t)genTree.size();
					deepCellPos = genTree.back();
				}
				genTree.pop_back();
				if (genTree.size() > 0)
				{
					currPos.x = genTree.back() % width;
					currPos.y = genTree.back() / width;
				}
				continue;
			}
			genTree.push_back(getWorldPos(currPos.x, currPos.y));
		}

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				setVisited(x, y);
			}
		}

		m_goalPos.x = deepCellPos % width;
		m_goalPos.y = deepCellPos / width;
	}
private:
	Marx::Reference<Marx::Shader> m_pShader;
	Marx::Reference<Marx::VertexArray> m_pVertexArray;
	std::bitset<width * height> m_visited;
	LevelCell m_cells[width * height];
	Position m_goalPos;
};

class Player
{
public:
	Player()
	{
		m_pShader.reset(
			Marx::Shader::create(
				Marx::Shader::loadShaderSrcFromFile("C:\\dev\\Marx\\Marx\\src\\shaders\\flatColorVS.hlsl"),
				Marx::Shader::loadShaderSrcFromFile("C:\\dev\\Marx\\Marx\\src\\shaders\\flatColorPS.hlsl")
			)
		);

		PlayerVertex vertices[] = {
			{{0.2f, 0.2f, 0.0f, 1.0f}},
			{{0.2f, 0.8f, 0.0f, 1.0f}},
			{{0.8f, 0.8f, 0.0f, 1.0f}},
			{{0.8f, 0.2f, 0.0f, 1.0f}},
		};

		Marx::Reference<Marx::VertexBuffer> pVertexBuffer(
			Marx::VertexBuffer::create(vertices, 4 * sizeof(PlayerVertex))
		);
		pVertexBuffer->setLayout({
			{ Marx::ShaderDataType::Float4, "POSITION" }
			});

		uint32_t indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		Marx::Reference<Marx::IndexBuffer> pIndexBuffer(
			Marx::IndexBuffer::create(indices, 6, Marx::IndexBuffer::PrimitiveType::TriangleList)
		);

		m_pVertexArray.reset(
			Marx::VertexArray::create()
		);
		m_pVertexArray->setVertexBuffer(pVertexBuffer);
		m_pVertexArray->setIndexBuffer(pIndexBuffer);
	}
public:
	void onRender()
	{
		DX::XMMATRIX transform = DX::XMMatrixTranslation((float)m_pos.x, (float)m_pos.y, 0.0f) * DX::XMMatrixScaling(0.1f, 0.1f, 0.1f);
		Marx::Renderer::submit(m_pShader, m_pVertexArray, DX::XMMatrixTranspose(transform), { 0.5f, 0.5f, 0.5f, 1.0f });
	}
	bool onKeyPress(Marx::KeyPressEvent& e)
	{
		switch (e.getKeyCode())
		{
		case MX_KEY_A: if (isPossibleMoveDirCallback(m_pos, MoveDir_Left)) --m_pos.x; break;
		case MX_KEY_S: if (isPossibleMoveDirCallback(m_pos, MoveDir_Down)) --m_pos.y; break;
		case MX_KEY_D: if (isPossibleMoveDirCallback(m_pos, MoveDir_Right)) ++m_pos.x; break;
		case MX_KEY_W: if (isPossibleMoveDirCallback(m_pos, MoveDir_Up)) ++m_pos.y; break;
		default: return false;
		}
		return true;
	}
	uint32_t getX() { return m_pos.x; }
	uint32_t getY() { return m_pos.y; }
public:
	std::function<bool(const Position&, MoveDir)> isPossibleMoveDirCallback;
private:
	Marx::Reference<Marx::Shader> m_pShader;
	Marx::Reference<Marx::VertexArray> m_pVertexArray;
	Position m_pos = { 0, 0 };
	uint32_t m_worldWidth = 10;
	uint32_t m_worldHeight = 10;
};

class MazeLayer : public Marx::Layer
{
public:
	MazeLayer()
		: m_camera(-1.6f, 1.6f, -0.9f, 0.9f), m_goalLine({ 0, 0 })
	{
		Marx::RenderCommand::setClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		m_player.isPossibleMoveDirCallback = std::bind(&Level<10, 10>::isPossibleMoveDir, &m_level, std::placeholders::_1, std::placeholders::_2);

		m_goalLine = GoalLine(m_level.getGoalPos());
	}
public:
	virtual void onUpdate(Marx::Timestep ts) override
	{
		m_camPos.x -= (m_camPos.x - m_player.getX()) * 0.001f;
		m_camPos.y -= (m_camPos.y - m_player.getY()) * 0.001f;

		m_camera.setPosition({ m_camPos.x * 0.1f, m_camPos.y * 0.1f, 0.0f });
		m_level.setVisited(m_player.getX(), m_player.getY());
		m_goalLine.setPlayerPos({ m_player.getX(), m_player.getY() });

		Marx::RenderCommand::clear();
		Marx::Renderer::beginScene(m_camera);
		m_level.onRender();
		m_player.onRender();
		m_goalLine.onRender();
		Marx::Renderer::endScene();
	}
	virtual void onImGuiRender() override {};
	virtual void onEvent(Marx::Event& event) override {
		Marx::EventDispatcher dispatcher(event);
		dispatcher.dispatch<Marx::KeyPressEvent>(std::bind(&Player::onKeyPress, &m_player, std::placeholders::_1));
	};
private:
	Level<10, 10> m_level;
	Player m_player;
	GoalLine m_goalLine;
	Marx::OrthographicCamera m_camera;
	DX::XMFLOAT3 m_camPos = { 0.0f, 0.0f, 0.0f };
};

class Sandbox : public Marx::Application
{
public:
	Sandbox()
	{
		//pushLayer(new ExampleLayer());
		pushLayer(new MazeLayer());
	}
	~Sandbox()
	{}
};

Marx::Application* Marx::createApplication()
{
	return new Sandbox();
}