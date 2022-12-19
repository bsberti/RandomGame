#include "gdp/Engine.h"
#include "SimulationView.h"

#include "Time.h"
#include <map>
#include <vector>
#include <sstream>

int CalculateHash(const Vector3& p)
{
	// xxxyyyzzz
	return ((int)p.x / 10) * 1000000
		+ ((int)p.y / 10) * 1000
		+ ((int)p.z / 10);
}

std::map<int, std::vector<Triangle*>> g_AABB_Map;
void CreateAABBStructure(const Vector3& position, float scale) {

	unsigned int modelId = -1;
	unsigned int unused1, unused2;
	std::vector<glm::vec3> vertices;
	std::vector<int> triangles;
	GDP_GetModelData(modelId, vertices, triangles, unused1, unused2);
	for (int i = 0; i < triangles.size(); i += 3) {
		int indexA = i;
		int indexB = i + 1;
		int indexC = i + 2;

		Vector3 vertexA = Vector3(vertices[indexA]) * scale + position;
		Vector3 vertexB = Vector3(vertices[indexB]) * scale + position;
		Vector3 vertexC = Vector3(vertices[indexC]) * scale + position;
		Triangle* triangle = new Triangle(vertexA, vertexB, vertexC);

		int aabbA = CalculateHash(vertexA);
		int aabbB = CalculateHash(vertexB);
		int aabbC = CalculateHash(vertexC);

		g_AABB_Map[aabbA].push_back(triangle);

		if (aabbB != aabbA)
			g_AABB_Map[aabbB].push_back(triangle);

		if (aabbC != aabbA && aabbC != aabbB)
			g_AABB_Map[aabbC].push_back(triangle);

	}
}

using namespace gdp;
SimulationView g_View;

const char* WINDOW_TITLE = "PhysicsExample9";
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

#define SUCCESS 0

double g_LastCallOnUpdate;
double g_LastCallOnPhysicsUpdate;
double g_CurrentTime;

const int MILLISECONDS_IN_A_SECOND = 1000;
const int FRAMES_PER_SECOND = 60;
const double FRAME_RATE = (double)1 / FRAMES_PER_SECOND;

int WaitToUpdate = 0;

float renderCount = 0;
float renderTime = 0;
int numFramesPerSecond = 0;

float updateCount = 0;
float updateTime = 0;
int numUpdatesPerSecond = 0;

float physics_update_count = 0;
float physicsTime = 0;
int numPhysicsUpdatesPerSecond = 0;

float physics_update_timestep = 0.01f;

void Update() {
	Time::Update();
	double deltaTimeInSeconds = Time::GetUnscaledDeltaTimeSeconds();
	g_CurrentTime += deltaTimeInSeconds;

	std::stringstream ss;

	renderTime += deltaTimeInSeconds;
	updateTime += deltaTimeInSeconds;
	physicsTime += deltaTimeInSeconds;

	updateCount++;
	if (updateTime > 0.1f) {
		numUpdatesPerSecond = updateCount * 10;
		updateCount = 0;
		updateTime -= 0.1f;
	}

	if (physicsTime > 0.1f) {
		numPhysicsUpdatesPerSecond = physics_update_count * 10;
		physics_update_count = 0;
		physicsTime -= 0.1f;
	}

	if (renderTime > 0.1f) {
		numFramesPerSecond = renderCount * 10;
		renderCount = 0;
		renderTime -= 0.1f;
	}

	ss << WINDOW_TITLE << " FPS: " << numFramesPerSecond << " : Updates: " << numUpdatesPerSecond << " : Physics: " << numPhysicsUpdatesPerSecond;

	glutSetWindowTitle(ss.str().c_str());

	if (deltaTimeInSeconds > 0.1) {
		//printf("ElapsedUpdateTimeIsTooBig!!!\n");
		deltaTimeInSeconds = 0.1;
	}

	//printf("Update\n");
	g_View.Update(deltaTimeInSeconds);

	if (g_CurrentTime >= g_LastCallOnPhysicsUpdate + physics_update_timestep) {
		physics_update_count++;

		g_LastCallOnPhysicsUpdate = g_CurrentTime;
		//printf("PHYSICS\n");
		g_View.PhysicsUpdate(physics_update_timestep);
	}
}

void Render() {

	//printf("Render\n");
	g_View.Render();
	renderCount++;
}

void RunSimulation()
{
	srand(time(NULL));
	GDP_Initialize();
	GDP_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
	GDP_UpdateCallback(&Update);
	GDP_RenderCallback(&Render);

	Time::Update();
	g_View.Initialize(3);
	Time::Update();
	GDP_Run();

	g_View.Destroy();
	GDP_Destroy();

}

int main(int argc, char** argv) {
	RunSimulation();
	return SUCCESS;
}
