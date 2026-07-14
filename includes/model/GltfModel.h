#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glad/glad.h>
#include <tiny_gltf.h>
#include "opengl/Texture.h"
#include "opengl/OGLRenderData.h"
#include "opengl/Shader.h"
#include "model/GltfNode.h"
#include "model/ModelLoader.h"
#include "model/GltfAnimationClip.h"
#include "IKSolver.h"
#include <opengl/Mesh.h>

struct GltfNodeData
{
	std::shared_ptr<GltfNode> rootNode;
	std::vector<std::shared_ptr<GltfNode>> nodeList;
};

struct GltfSkin
{
    std::string name;
    std::vector<int> joints;
    std::vector<glm::mat4> jointMatrices;
    std::vector<glm::mat4> inverseBindMatrices;
    std::vector<int> nodeToJoint;
};

class GltfModel {
public:
    bool loadModel(OGLRenderData& renderData,
        std::string modelFilename,
        std::string textureFilename,
        bool useMeshPrimitiveApproach,
        bool isInstanced);
    void draw();
    void drawInstanced(int instanceCount);
    void drawNodeApproach(Shader& s, bool log);
    void drawNode(std::shared_ptr<GltfNode> node, Shader& s, bool log);
   
    std::shared_ptr<GltfNode> mDebugRootNode;
    std::vector<std::shared_ptr<GltfNode>> mDebugNodeList{};
    void cleanup();

    std::string getModelFilename();
    int getNodeCount();
    GltfNodeData getGltfNodes();
    int getTriangleCount();

    void uploadVertexBuffers();
    void uploadIndexBuffer();

    std::vector<glm::mat4> getInverseBindMatrices();
    std::vector<int> getNodeToJoint();

    std::vector<std::shared_ptr<GltfAnimationClip>> getAnimClips();

    void resetNodeData(std::shared_ptr<GltfNode> treeNode);

private:
    void createVertexBuffers();
    void createIndexBuffer();

    void getJointData();
    void getWeightData();
    void getInvBindMatrices();
    void getAnimations();
    void getNodes(std::shared_ptr<GltfNode> treeNode);
    void getNodeData(std::shared_ptr<GltfNode> treeNode);
    std::vector<std::shared_ptr<GltfNode>> getNodeList(std::vector<std::shared_ptr<GltfNode>>
        & nodeList, int nodeNum);

    std::string mModelFilename;
    int mNodeCount = 0;

    std::shared_ptr<tinygltf::Model> mModel = nullptr;

    std::vector<glm::tvec4<uint16_t>> mJointVec{};
    std::vector<glm::vec4> mWeightVec{};
    std::vector<glm::mat4> mInverseBindMatrices{};

    std::vector<int> mAttribAccessors{};
    std::vector<int> mNodeToJoint{};

    std::vector<std::shared_ptr<GltfAnimationClip>> mAnimClips{};

    std::vector<GltfMesh> mMeshes{};
    std::vector<GltfSkin> mSkins{};

    GLenum getGLComponentType(const tinygltf::Accessor& accessor, int accessorNum);
    int getComponentCount(const tinygltf::Accessor& accessor, int accessorNum);
    void createPrimitive(const tinygltf::Primitive& gltfPrimitive, GltfPrimitive& primitive);
    void uploadPrimitiveBuffers(GltfPrimitive& primitive);
    void createIndexBuffer(const tinygltf::Primitive& tinyPrimitive, GltfPrimitive& primitive);
    void createMeshes();
    void calculateBindPose();
    void loadSkins();

    GLuint mVAO = 0;
    std::vector<GLuint> mVertexVBO{};
    GLuint mIndexVBO = 0;
    std::map<std::string, GLint> attributes =
    { {"POSITION", 0}, {"NORMAL", 1}, {"TEXCOORD_0", 2}, {"JOINTS_0", 3}, {"WEIGHTS_0", 4} };

    Texture mTex{};

    ModelLoader mModelLoader{};
};