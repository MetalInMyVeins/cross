#include <iostream>
#include <vector>
#include <string>

// FLTK headers
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

// Boost headers
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

// OpenGL and GLFW headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Bullet Physics headers
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

// Assimp headers
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Function to test FLTK
void testFLTK()
{
  Fl_Window window(300, 180, "FLTK Test");
  Fl_Box box(20, 40, 260, 100, "FLTK is working!");
  box.box(FL_UP_BOX);
  box.labelsize(24);
  box.align(FL_ALIGN_WRAP | FL_ALIGN_INSIDE);
  window.end();
  window.show();

  for (int i = 0; i < 10; ++i)
  {
    Fl::wait(0.1);
  }
}

// Function to test Boost
void testBoost()
{
  namespace fs = boost::filesystem;
  boost::system::error_code ec;

  fs::path currentDir = fs::current_path(ec);
  if (ec)
  {
    std::cerr << "Boost Filesystem error: " << ec.message() << std::endl;
    return;
  }

  std::cout << "Boost Filesystem is working. Current directory: "
            << currentDir.string() << std::endl;

  if (!ec)
  {
    std::cout << "Boost System is working." << std::endl;
  }
}

// Function to test OpenGL/GLFW/Glad
void testOpenGL()
{
  if (!glfwInit())
  {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(640, 480, "GLFW/Glad Test", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    std::cerr << "Failed to create GLFW window" << std::endl;
    return;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cerr << "Failed to initialize Glad" << std::endl;
    glfwTerminate();
    return;
  }

  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLFW and Glad are working." << std::endl;

  glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glfwSwapBuffers(window);

  glfwPollEvents();
  glfwWaitEventsTimeout(0.5);

  glfwTerminate();
}

// Function to test Bullet Physics
void testBullet()
{
  btBroadphaseInterface* broadphase = new btDbvtBroadphase();
  btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
  btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
  btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
  btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(
    dispatcher, broadphase, solver, collisionConfiguration);

  dynamicsWorld->setGravity(btVector3(0, -10, 0));

  btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
  btCollisionShape* fallShape = new btBoxShape(btVector3(1, 1, 1));

  btDefaultMotionState* groundMotionState = new btDefaultMotionState(
    btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
  btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(
    0, groundMotionState, groundShape, btVector3(0, 0, 0));
  btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);

  btDefaultMotionState* fallMotionState = new btDefaultMotionState(
    btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
  btScalar mass = 1;
  btVector3 fallInertia(0, 0, 0);
  fallShape->calculateLocalInertia(mass, fallInertia);
  btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(
    mass, fallMotionState, fallShape, fallInertia);
  btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);

  dynamicsWorld->addRigidBody(groundRigidBody);
  dynamicsWorld->addRigidBody(fallRigidBody);

  for (int i = 0; i < 100; ++i)
  {
    dynamicsWorld->stepSimulation(1 / 60.f, 10);

    btTransform trans;
    fallRigidBody->getMotionState()->getWorldTransform(trans);

    if (i % 20 == 0)
    {
      std::cout << "Bullet Physics: box height: " << trans.getOrigin().getY() << std::endl;
    }
  }

  dynamicsWorld->removeRigidBody(fallRigidBody);
  dynamicsWorld->removeRigidBody(groundRigidBody);
  delete fallRigidBody->getMotionState();
  delete fallRigidBody;
  delete groundRigidBody->getMotionState();
  delete groundRigidBody;
  delete fallShape;
  delete groundShape;
  delete dynamicsWorld;
  delete solver;
  delete dispatcher;
  delete collisionConfiguration;
  delete broadphase;

  std::cout << "Bullet Physics is working." << std::endl;
}

// Function to test Assimp
void testAssimp()
{
  Assimp::Importer importer;

  const aiScene* scene = importer.ReadFileFromMemory(
                           "ply\n"
                           "format ascii 1.0\n"
                           "element vertex 8\n"
                           "property float x\n"
                           "property float y\n"
                           "property float z\n"
                           "element face 12\n"
                           "property list uchar int vertex_indices\n"
                           "end_header\n"
                           "-1 -1 -1\n1 -1 -1\n1 1 -1\n-1 1 -1\n-1 -1 1\n1 -1 1\n1 1 1\n-1 1 1\n"
                           "3 0 1 2\n3 0 2 3\n3 1 5 6\n3 1 6 2\n3 5 4 7\n3 5 7 6\n3 4 0 3\n3 4 3 7\n"
                           "3 3 2 6\n3 3 6 7\n3 0 4 5\n3 0 5 1\n",
                           300,
                           aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
    return;
  }

  std::cout << "Assimp loaded test scene with " << scene->mNumMeshes << " meshes" << std::endl;
  if (scene->mNumMeshes > 0)
  {
    std::cout << "First mesh has " << scene->mMeshes[0]->mNumVertices << " vertices and "
              << scene->mMeshes[0]->mNumFaces << " faces" << std::endl;
  }

  std::cout << "Assimp is working." << std::endl;
}

int main()
{
  std::cout << "Starting library tests..." << std::endl;

  try
  {
    std::cout << "\nTesting Boost..." << std::endl;
    testBoost();

    std::cout << "\nTesting FLTK..." << std::endl;
    testFLTK();

    std::cout << "\nTesting OpenGL/GLFW/Glad..." << std::endl;
    testOpenGL();

    std::cout << "\nTesting Bullet Physics..." << std::endl;
    testBullet();

    std::cout << "\nTesting Assimp..." << std::endl;
    testAssimp();

    std::cout << "\nAll tests completed successfully!" << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << "Test failed: " << e.what() << std::endl;
    return 1;
  }
}


