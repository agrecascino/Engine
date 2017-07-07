
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <AL/al.h>
#include <AL/alut.h>
#include <map>
#include <memory>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <rapidxml.h>
#include <rapidxml_utils.h>
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/BroadphaseCollision/btAxisSweep3.h>
#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btEmptyShape.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h>
#include <LinearMath/btDefaultMotionState.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include <bmpread.h>

#include FT_FREETYPE_H

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"alut.lib")
#pragma comment(lib,"Bullet3Geometry_debug.lib")
#pragma comment(lib,"Bullet3Collision_debug.lib")
#pragma comment(lib,"BulletSoftBody_debug.lib")
#pragma comment(lib,"Bullet3Dynamics_debug.lib")
#pragma comment(lib,"Bullet3Common_debug.lib")
#pragma comment(lib,"BulletDynamics_debug.lib")
#pragma comment(lib,"BulletCollision_debug.lib")
#pragma comment(lib,"BulletInverseDynamics_debug.lib")
#pragma comment(lib,"LinearMath_debug.lib")
#pragma comment(lib,"freetype.lib")

class Engine;

class EntityManager;

FT_Library ft;

FT_Bitmap getBitmap(FT_Face face, char ch) {
    FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT);
    FT_Glyph glyph;
    FT_Get_Glyph(face->glyph, &glyph);
    FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
    FT_Bitmap& bitmap=bitmap_glyph->bitmap;

    return bitmap;
}

inline int next_p2 (int a )
{
    int rval=1;
    // rval<<=1 Is A Prettier Way Of Writing rval*=2;
    while(rval<a) rval<<=1;
    return rval;
}

class Entity {
public:
	virtual void onTick(double dt) {}
	~Entity() {}
};

float clip(float n, float lower, float upper) {
	return std::max(lower, std::min(n, upper));
}

class RenderedEntity : public Entity {
public:
	virtual void drawSelf(void) {}
	virtual glm::vec3 getPosition(void) { return xyz; }
	virtual void setPosition(glm::vec3 vec) { xyz = vec; }
protected:
	glm::vec3 xyz;
};

class CollidableEntity : public RenderedEntity {
public:
	CollidableEntity() {
		obj->setUserPointer(this);
	}
	virtual void setPosition(glm::vec3 vec) { xyz = vec; obj->getWorldTransform().setOrigin(btVector3(xyz.x,xyz.y,xyz.z)); }
	virtual void onCollision() {};
	virtual bool isFixed() { return fixed; }
	virtual btRigidBody* getRigidBody() { return obj; }
protected:
	bool fixed = true;
	btDefaultMotionState *defaultMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1)));
	btCollisionShape *shape = new btEmptyShape();
	btRigidBody *obj = new btRigidBody(0.0f,defaultMotionState,shape);
	
};

class Sky : public RenderedEntity {
public:
	void drawSelf(void) {
		glClearColor((98/255.0), (150/255.0), (234/255.0), 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
private:
};

class Camera : public CollidableEntity {
public:
	Camera(btDynamicsWorld *bt_world) {
		//camera_controller->setGravity(btVector3(0, -9, 0));
		camera_controller->setFriction(1);
		current_world = bt_world;
		fixed = false;
        FT_New_Face(ft, "FreeSans.ttf", 0, &face);
        FT_Set_Pixel_Sizes(face, 0, 16);
	}

	virtual void setPosition(glm::vec3 pos) {
		camera_controller->getWorldTransform().setOrigin(btVector3(pos.x, pos.y, pos.z));
	}

	btRigidBody *getCameraController(void) {
		return camera_controller;
	}

	void updateCamera(GLFWwindow *windowptr, double dt) {
		camera_controller->activate(true);
		btVector3 location = camera_controller->getWorldTransform().getOrigin();
		glEnable(GL_LIGHT0);
		float position[] = { location.getX(),location.getY(),location.getZ(),1.0f };
		float ambient[] = { 0.1f, (25.5 / 255.0),(25.5 / 255.0) };
		float light_direction[] = { 0.0f, -1.0f, 0.0f, 1.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);
        double xpos = 0, ypos = 0;
        if (!no) {
            glfwGetCursorPos(windowptr, &xpos, &ypos);
            std::cout << xpos << " " << ypos << std::endl;

		}
		else {
			xpos = 800;
			ypos = 450;
		}
		glfwSetCursorPos(windowptr, 800, 450);
        horizontal += dt * mspeed * (800 - xpos);
        vertical += dt * mspeed * (450 - ypos);

		if (vertical > 1.5f) {
			vertical = 1.5f;
		}
		else if (vertical < -1.5f) {
			vertical = -1.5f;
		}
		glm::vec3 direction(cos(vertical) * sin(horizontal), sin(vertical), cos(horizontal) * cos(vertical));
		glm::vec3 right = glm::vec3(sin(horizontal - 3.14f / 2.0f), 0, cos(horizontal - 3.14f / 2.0f));
		glm::vec3 up = glm::cross(right, direction);


		if (glfwGetKey(windowptr, GLFW_KEY_W) == GLFW_PRESS) {
			glm::vec3 temporary = direction * speed * (float)dt;
			temporary.y = 0;
			camera_controller->applyCentralImpulse(btVector3(temporary.x, temporary.y, temporary.z));
		}

		if (glfwGetKey(windowptr, GLFW_KEY_S) == GLFW_PRESS) {
			glm::vec3 temporary = -(direction * speed * (float)dt);
			temporary.y = 0;
			camera_controller->applyCentralImpulse(btVector3(temporary.x, temporary.y, temporary.z));
		}

		if (glfwGetKey(windowptr, GLFW_KEY_D) == GLFW_PRESS) {
			glm::vec3 temporary = right * speed * (float)dt;
			temporary.y = 0;
			camera_controller->applyCentralImpulse(btVector3(temporary.x, temporary.y, temporary.z));

		}

		if (glfwGetKey(windowptr, GLFW_KEY_A) == GLFW_PRESS) {
			glm::vec3 temporary = -(right * speed) * (float)dt;
			temporary.y = 0;
			camera_controller->applyCentralImpulse(btVector3(temporary.x, temporary.y, temporary.z));
		}

		if (glfwGetKey(windowptr, GLFW_KEY_N) == GLFW_PRESS) {
			no = true;
			if (current_world->getGravity().getY() == 0) {
				current_world->setGravity(btVector3(0, -9, 0));
				goto bail;
			}
			current_world->setGravity(btVector3(0, 0, 0));
		}

        if (glfwGetKey(windowptr, GLFW_KEY_SPACE) == GLFW_PRESS) {
            if(jumpEnable) {
                camera_controller->applyCentralImpulse(btVector3(0, 3, 0));
                jumpEnable = false;
            }
		}

        if(!jumpEnable && ((camera_controller->getLinearFactor().getY() > 0.0001) && camera_controller->getLinearFactor().getY() < 0.0001)) {
            framectr++;
        }
        if(framectr == 3) {
            jumpEnable = true;
            framectr = 0;
        }

	bail:
		btVector3 vel = camera_controller->getLinearVelocity();
		vel.setY(clip(vel.getY(),-32,32));
        vel.setX(clip(vel.getX(), -12, 12));
        vel.setZ(clip(vel.getZ(), -12, 12));
		camera_controller->setLinearVelocity(vel);
        viewmatrix = glm::lookAt(glm::vec3(location.getX(), location.getY(), location.getZ()), glm::vec3(location.getX(),location.getY(),location.getZ()) + direction, up) * glm::mat4(1.0);
		//std::cout << location.getY() << std::endl;
        camera_controller->setMotionState(groundMotionState);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&viewmatrix[0][0]);
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&projectionmatrix[0][0]);
        speed = 20.0f;
	}
private:
    int framectr = 0;
    bool jumpEnable = true;
	bool no = false;
	FT_Face face;
    float speed = 20.0f;
	float horizontal = 3.14f;
	float vertical = 0.0f;
    float mspeed = 0.05f;
	glm::mat4 viewmatrix;
	btDynamicsWorld *current_world;
	//btPairCachingGhostObject *ghost = new btPairCachingGhostObject();
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	btRigidBody *camera_controller = new btRigidBody(2, groundMotionState, new btCapsuleShape(1, 2));
	glm::mat4 projectionmatrix = glm::perspective(45.0f,16.0f/9.0f, 0.1f, 10000.0f);
};

btVector3 convert(glm::vec3 v) {
	return btVector3(v.x, v.y, v.z);
}

void test(float x, float y) {
	glColor3f(x, y, 0);
}

class PolyObject : public CollidableEntity {
public:
	PolyObject(glm::vec3 pos) { xyz = pos;  obj->getWorldTransform().setOrigin(btVector3(xyz.x, xyz.y, xyz.z));	}
	void setGeometry(std::vector<glm::vec3> polygons, bool isquad = false) {
		if (isquad) {
			for (size_t i = 0; i < polygons.size(); i += 4) {
				vertices.push_back(polygons[i]);
				vertices.push_back(polygons[i + 1]);
				vertices.push_back(polygons[i + 2]);
				vertices.push_back(polygons[i + 1]);
				vertices.push_back(polygons[i + 2]);
				vertices.push_back(polygons[i + 3]);
			}
		}
		else {
			vertices = polygons;
		}
		quad = isquad;
		btTriangleMesh *mesh = new btTriangleMesh;
		if (!isquad) {
			assert((polygons.size() % 3) == 0);
		}
		else {
			assert((polygons.size() % 4) == 0);
		}
		if (!isquad) {
			for (size_t i = 0; i < polygons.size(); i += 3) {
				mesh->addTriangle(btVector3(polygons[i].x, polygons[i].y, polygons[i].z), btVector3(polygons[i + 1].x, polygons[i + 1].y, polygons[i + 1].z), btVector3(polygons[i + 2].x, polygons[i + 2].y, polygons[i + 2].z));
			}
		}
		else {
			for (size_t i = 0; i < polygons.size(); i += 4) {
				mesh->addTriangle(convert(polygons[i]), convert(polygons[i + 1]), convert(polygons[i + 2]));
				mesh->addTriangle(convert(polygons[i + 1]), convert(polygons[i + 2]), convert(polygons[i + 3]));
			}
		}
		obj->setCollisionShape(new btBvhTriangleMeshShape(mesh, false));
	}
	void setTextures(std::vector<GLuint> textures) {
		if (quad) {
			for (size_t i = 0; i < textures.size(); i++) {
				gltex.push_back(textures[i]);
				gltex.push_back(textures[i]);
				gltex.push_back(textures[i]);
				gltex.push_back(textures[i]);
				gltex.push_back(textures[i]);
				gltex.push_back(textures[i]);

			}
			//assert(((textures.size() * (2 / 3)) / 4) >= C);
			textured = true;
			return;
		}
		else {
			assert((textures.size() / 3) >= textures.size());
		}
		gltex = textures;
		textured = true;

	}
	void setColor(std::vector<glm::vec3> c, bool pv) {
		if (!quad) {
			colors = c;
		}
		else {
			for (size_t i = 0; i < c.size(); i += 4) {
				colors.push_back(c[i]);
				colors.push_back(c[i + 1]);
				colors.push_back(c[i + 2]);
				colors.push_back(c[i + 1]);
				colors.push_back(c[i + 2]);
				colors.push_back(c[i + 3]);
			}
		}
		pervertexcoloring = pv;
	}

	glm::vec3 computeNormal(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
		return glm::normalize(glm::cross(b - a, c - a));
	}

	void drawSelf(void) {
		//std::cout << obj->getWorldTransform().getOrigin().getY() << std::endl;
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_NORMALIZE);
		if (colors.size()) {
			if (!pervertexcoloring) {
				assert((colors.size() % 3) == 0);
				assert(colors.size() >= (vertices.size() / 3));
			}
			else {
				assert(colors.size() >= vertices.size());
			}
		}
		for (size_t i = 0; i+2 < vertices.size(); i+= 3) {
			glBindTexture(GL_TEXTURE_2D, gltex[i]);
			glBegin(GL_TRIANGLES);
			if (!textured) {
				if (colors.size()) {
					if (pervertexcoloring) {
						glColor3f(colors[i].x, colors[i].y, colors[i].z);
					}
					else {
						if (((i + 1) % 3) == 0) {
							glColor3f(colors[i / 3].x, colors[i / 3].y, colors[i / 3].z);
						}
					}
				}
				else {
					glColor3f(0.8, 0.8, 0.8);
				}
			}
			//std::cout << i << std:: endl;
			//std::cout << vertices[i].x << vertices[i].y << vertices[i].z << std::endl;
			glm::vec3 normal = computeNormal(vertices[i], vertices[i + 1], vertices[i + 2]);
            //glNormal3f(normal.x, normal.y, normal.z);
            //std::cout << normal.x << " " << normal.y << " " << normal.z << std::endl;
			setQuadUV(i);
			glVertex3f(vertices[i].x + xyz.x, vertices[i].y + xyz.y, vertices[i].z + xyz.z);
			setQuadUV(i+1);
			glVertex3f(vertices[i+1].x + xyz.x, vertices[i+1].y + xyz.y, vertices[i+1].z + xyz.z);
			setQuadUV(i+2);
			glVertex3f(vertices[i+2].x + xyz.x, vertices[i+2].y + xyz.y, vertices[i+2].z + xyz.z);
			glEnd();
		}
		if (textured) {
			glDisable(GL_TEXTURE_2D);
		}
		glDisable(GL_NORMALIZE);
		glDisable(GL_COLOR_MATERIAL);
	}
private:
	void setQuadUV(int i) {
		size_t k = i - (floor(i / 6.0)) * 6;

		if (k == 5) {
			glTexCoord2f(1.0f, 1.0f);
		}
		else if (k == 4) {
			glTexCoord2f(1.0f, 0.0f);
		}
		else if (k == 3) {
			glTexCoord2f(0.0f, 1.0f);
		}
		else if (k == 2) {
			glTexCoord2f(1.0f, 0.0f);
		}
		else if (k == 1) {
			glTexCoord2f(0.0f, 1.0f);
		}
		else if (k == 0) {
			glTexCoord2f(0.0f, 0.0f);
		}
	}
	bool textured = false;
	bool quad = true;
	bool pervertexcoloring = true;
	std::vector<GLuint> gltex;
	std::vector<float[2]> uvs;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec3> vertices;
};

class MediaFile {
public:

private:
};

class MediaStreamer {
public:
	MediaStreamer() {
		//alutInit(NULL, NULL);
	}
private:
	ALuint sources[32];
};

class CollisionManager {
	friend class EntityManager;
	friend class glRenderer;
public:
	CollisionManager() {
		bt_collision_config = new btDefaultCollisionConfiguration();
	    bt_dispatcher = new btCollisionDispatcher(bt_collision_config);
		bt_broadphase = new btDbvtBroadphase();
		bt_broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
		bt_solver = new btSequentialImpulseConstraintSolver;
		bt_world = new btDiscreteDynamicsWorld(bt_dispatcher, bt_broadphase, bt_solver, bt_collision_config);
		bt_world->setGravity(btVector3(0, -29, 0));
		btGImpactCollisionAlgorithm::registerAlgorithm(bt_dispatcher);
		bt_world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	}

private:
	btCollisionConfiguration *bt_collision_config;
	btCollisionDispatcher *bt_dispatcher;
	btBroadphaseInterface *bt_broadphase;
	btSequentialImpulseConstraintSolver* bt_solver;
	btDiscreteDynamicsWorld *bt_world;
	std::map<unsigned long, std::unique_ptr<CollidableEntity>> collidables;
};

class EntityManager {
	friend class CollisionManager;
	friend class glRenderer;
public:
	unsigned long addEntity(std::unique_ptr<CollidableEntity> &entity) {
		collidables[ids[2]] = std::move(entity);
		collision.bt_world->addRigidBody(collidables[ids[2]]->getRigidBody());
		ids[2]++;
		return ids[2] - 1;

	}
	unsigned long addEntity(std::unique_ptr<RenderedEntity> &entity) {
		renderobjects[ids[1]] = std::move(entity);
		ids[1]++;
		return ids[1] - 1;
	}

	unsigned long addEntity(std::unique_ptr<Entity> &entity) {
		gameobjects[ids[0]] = std::move(entity);
		ids[0]++;
		return ids[0] - 1;
	}

	void removeEntity(unsigned long id) {
		gameobjects.erase(id);
	}

	void removeRenderableEntity(unsigned long id) {
		renderobjects.erase(id);
	}

	void removeCollidableEntity(unsigned long id) {
		collision.bt_world->removeRigidBody(collidables[ids[2]]->getRigidBody());
		collidables.erase(id);
	}

	void drawAllObjects() {
		for (auto& kv : renderobjects) {
			kv.second->drawSelf();
		}

		for (auto& kv : collidables) {
			kv.second->drawSelf();
		}
	}

	void tickAllObjects(float dt) {
		for (auto &kv : gameobjects) {
			kv.second->onTick(dt);
		}
		
		for (auto &kv : renderobjects) {
		    kv.second->onTick(dt);
		}

		for (auto &kv : collidables) {
			kv.second->onTick(dt);
		}
	}

	void clearAllObjects() {
		for (auto &kv : collidables) {
			collision.bt_world->removeRigidBody(kv.second->getRigidBody());
		}
		gameobjects.clear();
		renderobjects.clear();
		collidables.clear();
	}
private:
	CollisionManager collision;
	unsigned long ids[3] = { 0, 0, 0 };
	std::map<unsigned long, std::unique_ptr<Entity>> gameobjects;
	std::map<unsigned long, std::unique_ptr<RenderedEntity>> renderobjects;
	std::map<unsigned long, std::unique_ptr<CollidableEntity>> &collidables = collision.collidables;
};

class glRenderer {
	friend class EntityManager;
public:
	glRenderer(EntityManager &manager) : manager(manager), camera(manager.collision.bt_world) {
        int t = 0;
		glfwInit();
		window = glfwCreateWindow(1600, 900, "Cultural Enrichment", NULL, NULL);
		glfwMakeContextCurrent(window);
		glewInit();
        //glfwSetInputMode(window, GLFW_CURSOR, 1);
        glfwSetInputMode(window, GLFW_CURSOR,GLFW_CURSOR_NORMAL);
		glfwSetCursorPos(window, 800, 450);
		glfwSwapInterval(1);
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		manager.collision.bt_world->addRigidBody(camera.getCameraController());
		camera.setPosition(glm::vec3(3,20,3));
		oldtime = glfwGetTime();
		newtime = glfwGetTime();
		lastphysrun = glfwGetTime();
	}
	void drawScreen() {
		manager.drawAllObjects();
		glFlush();
		glfwSwapBuffers(window);
	}
	int handleInput() {
		newtime = glfwGetTime();
		//std::cout << (newtime - oldtime) << std::endl;
		double dt = (newtime - oldtime)* 62.5;
		double phystime = newtime - lastphysrun;
		//std::cout << (phystime * 1000) << std::endl;
		if ((phystime * 1000) > 0) {
			manager.collision.bt_world->stepSimulation(5);
			lastphysrun = glfwGetTime();
		}
		//std::cout << dt << std::endl;
		oldtime = glfwGetTime();
		glfwPollEvents();
		camera.updateCamera(window, dt);
		if (glfwWindowShouldClose(window) || glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			return -1;
		}
		return 0;
		
	}

	~glRenderer() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}
private:
	double lastphysrun = 0.0;
	double oldtime = 0.0;
	double newtime = 0.0;
	Camera camera;

	EntityManager &manager;
	GLFWwindow *window = NULL;
};

class Engine {
	friend class glRenderer;
public:
    Engine() : video(manager) {
		FT_Init_FreeType(&ft);
		rapidxml::file<> xmlfile("startup.xml");
		rapidxml::xml_document<> document;
		document.parse<0>(xmlfile.data());
		rapidxml::xml_node<> *currentnode = (document.first_node());
		while (currentnode != NULL) {
			std::cout << currentnode->name() << std::endl;
			std::cout << strlen(currentnode->name()) << std::endl;
			if (strcmp(currentnode->name(),"loadmap") == 0) {
				loadMapFromXML(currentnode->value());
			}
			currentnode = currentnode->next_sibling();
		}
	}
	void gameLoop() {
		while (true) {
			if (video.handleInput() == -1) {
				break;
			}
			video.drawScreen();
		}
	}

	void loadEngineInline(const char* name) {
		if (strcmp(name,"sky") == 0) {
			std::unique_ptr<RenderedEntity> sky;
			RenderedEntity *skyptr = new Sky();
			sky.reset(skyptr);
			manager.addEntity(sky);
		}
	}

	void loadMapFromXML(const char* filename) {
		rapidxml::file<> xmlfile(filename);
		rapidxml::xml_document<> document;
		document.parse<0>(xmlfile.data());
		rapidxml::xml_node<> *currentnode = (document.first_node());
		while (currentnode != NULL) {
			if (strcmp(currentnode->name(),"engineinline") == 0) {
				loadEngineInline(currentnode->value());
			}

			if (strcmp(currentnode->name(), "mapgeometry") == 0) {
				rapidxml::xml_node<> *verticenode = currentnode->first_node();
				std::vector<glm::vec3> vertices;
				std::vector<glm::vec3> colors;
				std::vector<GLuint> textures;
				bool textured = false;
				bool quad = false;
				while (verticenode != NULL) {
					if (strcmp(verticenode->name(), "quad") == 0) {
						quad = true;
					}
					if (strcmp(verticenode->name(), "texture") == 0) {
						bmpread_t bitmap;
						GLuint loaded_texture;
						if (!bmpread(verticenode->value(), 0, &bitmap)) {
							std::cout << "failed to load texture: " << verticenode->value() << std::endl;
							assert(false);
						}
						glGenTextures(1, &loaded_texture);
						glBindTexture(GL_TEXTURE_2D, loaded_texture);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

						glTexImage2D(GL_TEXTURE_2D, 0, 3, bitmap.width, bitmap.height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data);
						bmpread_free(&bitmap);
						textures.push_back(loaded_texture);
						textured = true;
					}
					if (strcmp(verticenode->name(), "vertice") == 0) {
						char *vcopy = new char[verticenode->value_size() + 1];
						memset(vcopy, 0, verticenode->value_size() + 1);
						strncpy(vcopy,verticenode->value(), verticenode->value_size() + 1);
						char *ptr = strtok(vcopy, ",");
						std::vector<float> values;
						while (ptr != NULL) {
							values.push_back(strtof(ptr,NULL));
							ptr = strtok(NULL, ",");
						}
						assert(values.size() == 3);
						vertices.push_back(glm::vec3(values[0],values[1],values[2]));
					}
					else if (strcmp(verticenode->name(), "color") == 0) {
						char *vcopy = new char[verticenode->value_size() + 1];
						memset(vcopy, 0, verticenode->value_size() + 1);
						strncpy(vcopy, verticenode->value(), strlen(verticenode->value()));
						char *ptr = strtok(vcopy, ",");
						std::vector<float> values;
						while (ptr != NULL) {
							values.push_back(strtof(ptr, NULL));
							ptr = strtok(NULL, ",");
						}
						assert(values.size() == 3);
						colors.push_back(glm::vec3(values[0], values[1], values[2]));
					}
					verticenode = verticenode->next_sibling();
				}
				PolyObject *p = new PolyObject(glm::vec3(0.0f, 0.0f, 0.0f));
				p->setGeometry(vertices, quad);
				p->setColor(colors, true);
				if (textured) {
					p->setTextures(textures);
				}
				std::unique_ptr<CollidableEntity> ptr;
				ptr.reset(p);
                manager.addEntity(ptr);
			}
			currentnode = currentnode->next_sibling();
		}
	}
private:
	EntityManager manager;
	glRenderer video;
	MediaStreamer audio;
};

int main()
{
	Engine game;
	game.gameLoop();
    return 0;
}
