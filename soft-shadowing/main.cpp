#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include "engine_common.h"
#include "util.h"
#include "basic_glfw_camera.h"
#include "lighting.h"
#include "glfw.h"
#include "basic_mesh.h"
#include "world_transform.h"
#include "shadow_map_fbo.h"
#include "shadow_mapping_technique.h"

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

#define SHADOW_TEXTURE_WIDTH 4096
#define SHADOW_TEXTURE_HEIGHT 4096

#define INTENSITY_STEP 0.01f
#define ROTATION_STEP 1.0f

static void KeyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
static void MouseClickCallback(GLFWwindow* window, int button, int action, int mods);

class SoftShadows
{
public:

	SoftShadows()
	{
		m_directionalLight.AmbientIntensity = 0.5f;
		m_directionalLight.DiffuseIntensity = 0.9f;
		m_directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
		m_directionalLight.WorldDirection = Vector3f(1.0f, -0.75f, 0.0f);

		OrthoProjInfo lightProjection;
		lightProjection.l = -20.0f;
		lightProjection.r = 20.0f;
		lightProjection.t = 20.0f;
		lightProjection.b = -20.0f;
		lightProjection.n = -20.0f;
		lightProjection.f = 20.0f;

		m_lightProjectionMatrix.InitOrthoProjTransform(lightProjection);
	}

	virtual ~SoftShadows()
	{
		if (m_camera)
		{
			delete m_camera;
			m_camera = nullptr;
		}
		if (m_tree)
		{
			delete m_tree;
			m_tree = nullptr;
		}
	}

	void Initialize()
	{
		SetupWindow();
		SetupShadowMapping();
		RegisterCallbacks();
		SetupCamera();
		LoadModels();
		LoadShaders();
	}

	void Execute()
	{
		while (!glfwWindowShouldClose(window)) {
			Render();
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	void Render()
	{
		static float cycle = 0.0f;
		RenderShadowMap();
		RenderLighting();
	}

	void RenderShadowMap()
	{
		m_shadowMapFBO.BindForWriting();
		glClear(GL_DEPTH_BUFFER_BIT);
		m_shadowMappingTechnique.Enable();

		Matrix4f ViewFromLight;
		Vector3f Up(0.0f, 1.0f, 0.0f);
		Vector3f Origin(0.0f, 0.0f, 0.0f);

		Matrix4f World = m_tree->GetWorldMatrix();
		ViewFromLight.InitCameraTransform(Origin, m_directionalLight.WorldDirection, Up);
		Matrix4f WVP = m_lightProjectionMatrix * ViewFromLight * World;
		m_shadowMappingTechnique.SetWVP(WVP);
		m_tree->Render();

		World = m_house->GetWorldMatrix();
		ViewFromLight.InitCameraTransform(Origin, m_directionalLight.WorldDirection, Up);
		WVP = m_lightProjectionMatrix * ViewFromLight * World;
		m_shadowMappingTechnique.SetWVP(WVP);
		m_house->Render();

		World = m_tree2->GetWorldMatrix();
		ViewFromLight.InitCameraTransform(Origin, m_directionalLight.WorldDirection, Up);
		WVP = m_lightProjectionMatrix * ViewFromLight * World;
		m_shadowMappingTechnique.SetWVP(WVP);
		m_tree2->Render();

		World = m_tree3->GetWorldMatrix();
		ViewFromLight.InitCameraTransform(Origin, m_directionalLight.WorldDirection, Up);
		WVP = m_lightProjectionMatrix * ViewFromLight * World;
		m_shadowMappingTechnique.SetWVP(WVP);
		m_tree3->Render();
	}

	void RenderLighting()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_lightingTechnique.Enable();

		m_shadowMapFBO.BindForReadingDSA(SHADOW_TEXTURE_UNIT);

		m_camera->OnRender();

		Matrix4f ViewFromCamera = m_camera->GetMatrix();
		Matrix4f ProjectionFromCamera;



		ProjectionFromCamera = m_camera->GetProjectionMat();


		Matrix4f ViewFromLight;
		Vector3f Up(0.0f, 1.0f, 0.0f);
		Vector3f Origin(0.0f, 0.0f, 0.0f);
		ViewFromLight.InitCameraTransform(Origin, m_directionalLight.WorldDirection, Up);

		m_lightingTechnique.SetMaterial(m_tree->GetMaterial());

		// Calculate World-View-Projection matrix from camera's perspective
		Matrix4f World = m_tree->GetWorldMatrix();
		Matrix4f WVP = ProjectionFromCamera * ViewFromCamera * World;
		m_lightingTechnique.SetWVP(WVP);

		// Calculate World-View-Projection matrix from light's perspective
		Matrix4f LightWVP = m_lightProjectionMatrix * ViewFromLight * World;
		m_lightingTechnique.SetLightWVP(LightWVP);

		Vector3f CameraLocalPos = m_tree->GetWorldTransform().WorldPosToLocalPos(m_camera->GetPos());
		m_lightingTechnique.SetCameraLocalPos(CameraLocalPos);
		m_directionalLight.CalcLocalDirection(m_tree->GetWorldTransform());
		m_lightingTechnique.SetDirectionalLight(m_directionalLight);

		m_tree->Render();

		// Set the WVP matrix from the camera point of view
		World = m_terrain->GetWorldMatrix();
		WVP = ProjectionFromCamera * ViewFromCamera * World;
		m_lightingTechnique.SetWVP(WVP);

		// Set the WVP matrix from the light point of view
		LightWVP = m_lightProjectionMatrix * ViewFromLight * World;
		m_lightingTechnique.SetLightWVP(LightWVP);

		// Update the shader with the local space pos/dir of the spot light
		m_directionalLight.CalcLocalDirection(m_terrain->GetWorldTransform());
		m_lightingTechnique.SetDirectionalLight(m_directionalLight);
		m_lightingTechnique.SetMaterial(m_terrain->GetMaterial());

		// Update the shader with the local space pos of the camera
		Vector3f CameraLocalPos3f = m_terrain->GetWorldTransform().WorldPosToLocalPos(m_camera->GetPos());
		m_lightingTechnique.SetCameraLocalPos(CameraLocalPos3f);

		m_terrain->Render();

		World = m_house->GetWorldMatrix();
		WVP = ProjectionFromCamera * ViewFromCamera * World;
		m_lightingTechnique.SetWVP(WVP);

		LightWVP = m_lightProjectionMatrix * ViewFromLight * World;
		m_lightingTechnique.SetLightWVP(LightWVP);

		m_directionalLight.CalcLocalDirection(m_house->GetWorldTransform());
		m_lightingTechnique.SetDirectionalLight(m_directionalLight);
		m_lightingTechnique.SetMaterial(m_house->GetMaterial());

		CameraLocalPos3f = m_house->GetWorldTransform().WorldPosToLocalPos(m_camera->GetPos());
		m_lightingTechnique.SetCameraLocalPos(CameraLocalPos3f);

		m_house->Render();

		World = m_tree2->GetWorldMatrix();
		WVP = ProjectionFromCamera * ViewFromCamera * World;
		m_lightingTechnique.SetWVP(WVP);

		LightWVP = m_lightProjectionMatrix * ViewFromLight * World;
		m_lightingTechnique.SetLightWVP(LightWVP);

		m_directionalLight.CalcLocalDirection(m_tree2->GetWorldTransform());
		m_lightingTechnique.SetDirectionalLight(m_directionalLight);
		m_lightingTechnique.SetMaterial(m_tree2->GetMaterial());

		CameraLocalPos3f = m_tree2->GetWorldTransform().WorldPosToLocalPos(m_camera->GetPos());
		m_lightingTechnique.SetCameraLocalPos(CameraLocalPos3f);

		m_tree2->Render();

		World = m_tree3->GetWorldMatrix();
		WVP = ProjectionFromCamera * ViewFromCamera * World;
		m_lightingTechnique.SetWVP(WVP);

		LightWVP = m_lightProjectionMatrix * ViewFromLight * World;
		m_lightingTechnique.SetLightWVP(LightWVP);

		m_directionalLight.CalcLocalDirection(m_tree3->GetWorldTransform());
		m_lightingTechnique.SetDirectionalLight(m_directionalLight);
		m_lightingTechnique.SetMaterial(m_tree3->GetMaterial());

		CameraLocalPos3f = m_tree3->GetWorldTransform().WorldPosToLocalPos(m_camera->GetPos());
		m_lightingTechnique.SetCameraLocalPos(CameraLocalPos3f);

		m_tree3->Render();
	}

	void MousePassiveMove(int x, int y)
	{
		m_camera->OnMouse(x, y);
	}

	void KeyInput(unsigned int key, int state)
	{
		if (state == GLFW_PRESS) {
			switch (key) {
			case GLFW_KEY_ESCAPE:
			case GLFW_KEY_Q:
				glfwDestroyWindow(window);
				glfwTerminate();
				exit(0);

			case GLFW_KEY_A:
				m_directionalLight.AmbientIntensity += INTENSITY_STEP;
				break;

			case GLFW_KEY_D:
				m_directionalLight.DiffuseIntensity -= INTENSITY_STEP;
				break;

			case GLFW_KEY_F:
				m_shadowMapFilterSize++;
				printf("Shadow map filter size increased to %d\n", m_shadowMapFilterSize);
				m_lightingTechnique.SetShadowMapFilterSize(m_shadowMapFilterSize);
				break;
			case GLFW_KEY_G:
				if (m_shadowMapFilterSize > 0) {
					m_shadowMapFilterSize--;
					printf("Shadow map filter size decreased to %d\n", m_shadowMapFilterSize);
					m_lightingTechnique.SetShadowMapFilterSize(m_shadowMapFilterSize);
				}
				break;
			}
		}
		m_camera->OnKeyboard(key);
	}

	void MouseButton(int button, int action, int x, int y) {}

private:

	void SetupWindow()
	{
		int major_ver = 0;
		int minor_ver = 0;
		bool is_full_screen = false;
		window = glfw_init(major_ver, minor_ver, SCREEN_WIDTH, SCREEN_HEIGHT, is_full_screen, "Artur_Adrian_GK");

		glfwSetCursorPos(window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	}


	void SetupShadowMapping()
	{
		bool ForPCF = true;

		if (!m_shadowMapFBO.Init(SHADOW_TEXTURE_WIDTH, SHADOW_TEXTURE_HEIGHT, ForPCF)) {
			exit(1);
		}
	}


	void RegisterCallbacks()
	{
		glfwSetKeyCallback(window, KeyPressCallback);
		glfwSetCursorPosCallback(window, MouseMoveCallback);
		glfwSetMouseButtonCallback(window, MouseClickCallback);
	}


	void SetupCamera()
	{
		m_cameraPosition = Vector3f(3.0f, 5.0f, -31.0f);
		m_cameraTarget = Vector3f(0.0f, -0.1f, 1.0f);

		Vector3f Up(0.0, 1.0f, 0.0f);

		float FieldOfView = 45.0f;
		float NearClip = 1.0f;
		float FarClip = 100.0f;
		PersProjInfo perspectiveInfo = { FieldOfView, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, NearClip, FarClip };

		m_camera = new BasicCamera(perspectiveInfo, m_cameraPosition, m_cameraTarget, Up);
	}


	void LoadShaders()
	{
		if (!m_lightingTechnique.Init()) {
			printf("Error initializing the lighting shaders\n");
			exit(1);
		}

		m_lightingTechnique.Enable();
		m_lightingTechnique.SetTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
		m_lightingTechnique.SetShadowMapTextureUnit(SHADOW_TEXTURE_UNIT_INDEX);
		m_lightingTechnique.SetShadowMapSize(SHADOW_TEXTURE_WIDTH, SHADOW_TEXTURE_HEIGHT);

		m_lightingTechnique.SetShadowMapFilterSize(m_shadowMapFilterSize);
		if (!m_shadowMappingTechnique.Init()) {
			printf("Error initializing the shadow mapping shaders\n");
			exit(1);
		}
	}


	void LoadModels()
	{
		m_house = new BasicMesh();
		if (!m_house->LoadMesh("../Content/house/house01.obj")) {
			printf("Error loading model");
			exit(0);
		}
		m_house->SetPosition(0.0f, 0.0f, 0.0f);

		m_tree = new BasicMesh();
		if (!m_tree->LoadMesh("../Content/MapleTree.obj")) {
			printf("Error loading model");
			exit(0);
		}
		m_tree->SetPosition(7.0f, 0.0f, -7.0f);
		m_tree->SetScale(0.1);

		m_tree2 = new BasicMesh();
		if (!m_tree2->LoadMesh("../Content/MapleTree.obj")) {
			printf("Error loading model");
			exit(0);
		}
		m_tree2->SetPosition(-5.0f, 0.0f, -9.0f);
		m_tree2->SetScale(0.1);

		m_tree3 = new BasicMesh();
		if (!m_tree3->LoadMesh("../Content/MapleTree.obj")) {
			printf("Error loading model");
			exit(0);
		}
		m_tree3->SetPosition(-10.0f, 0.0f, -4.0f);
		m_tree3->SetScale(0.1);

		m_terrain = new BasicMesh();
		if (!m_terrain->LoadMesh("../Content/terrain.obj")) {
			printf("Error loading model");
			exit(0);
		}
		m_terrain->SetPosition(0.0f, 0.0f, 0.0f);
	}

	GLFWwindow* window = NULL;
	BasicCamera* m_camera = NULL;
	LightingTechnique m_lightingTechnique;
	ShadowMappingTechnique m_shadowMappingTechnique;
	BasicMesh* m_tree = NULL;
	BasicMesh* m_tree2 = NULL;
	BasicMesh* m_tree3 = NULL;
	BasicMesh* m_terrain = NULL;
	BasicMesh* m_house = NULL;
	Matrix4f m_lightProjectionMatrix;
	Matrix4f m_cameraProjectionMatrix;
	DirectionalLight m_directionalLight;
	ShadowMapFBO m_shadowMapFBO;
	Vector3f m_cameraPosition;
	Vector3f m_cameraTarget;
	int m_shadowMapFilterSize = 4;
	float m_shadowMapSampleRadius = 0.0f;
	int m_shadowMapOffsetTextureSize = 16;
	int m_shadowMapOffsetFilterSize = 8;
};

SoftShadows* application = NULL;

static void KeyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	application->KeyInput(key, action);
}

static void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
	application->MousePassiveMove((int)xpos, (int)ypos);
}

static void MouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	application->MouseButton(button, action, (int)xpos, (int)ypos);
}

int main(int argc, char** argv)
{
	application = new SoftShadows();

	application->Initialize();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	application->Execute();

	delete application;

	return 0;
}
