//
// Created by daholland on 3/23/21.
//
// kudos to vkguide / https://github.com/vblanco20-1/vulkan-guide

#ifndef GIBA_SPOCK_H
#define GIBA_SPOCK_H
#include <vk_types.h>
#include <vector>
#include <deque>
#include <functional>
#include "vk_mesh.h"
#include <glm/glm.hpp>
#include <string>
#include <filesystem>


namespace tracy { class VkCtx; }

////
constexpr uint FRAME_OVERLAP = 2;

////


typedef struct SDL_Window* SDL_Window_ptr;

namespace spock {
    struct DeletionQueue {
        std::deque<std::function<void()>> deletors;

        void push_function(std::function<void()>&& function) {
            deletors.push_back(function);
        }

        void flush() {
            for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
                (*it)();
            }

            deletors.clear();
        }

    };

    struct MeshPushConstants {
        glm::vec4 data;
        glm::mat4 render_matrix;
    };

    struct Material {
        VkDescriptorSet textureSet{VK_NULL_HANDLE};
        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;
    };

    struct Texture {
        AllocatedImage image;
        VkImageView imageView;
    };

    struct RenderObject {
        Mesh* mesh;
        Material* material;
        glm::mat4 transformMatrix;
    };

    struct GPUCameraData {
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 viewproj;
    };

    struct GPUSceneData {
        glm::vec4 fogColor;
        glm::vec4 fogDistances;
        glm::vec4 ambientColor;
        glm::vec4 sunlightDirection;
        glm::vec4 sunlightColor;
    };

    struct GPUObjectData {
        glm::mat4 modelMatrix;
    };

    struct UploadContext {
        VkFence _uploadFence;
        VkCommandPool _commandPool;
    };

    struct AppState {
        glm::vec3 camPos;
    };

    struct FrameData {
        VkSemaphore _presentSemaphore, _renderSemaphore;
        VkFence _renderFence;

        VkCommandPool _commandPool;
        VkCommandBuffer _mainCommandBuffer;

        AllocatedBuffer cameraBuffer;
        VkDescriptorSet globalDescriptor;

        AllocatedBuffer objectBuffer;
        VkDescriptorSet objectDescriptor;

    };

    class Spock {
    public:
        AppState _appState;
        bool _isInitialized{false};
        int _frameNumber{0};
        int _selectedShader{0};
        VmaAllocator _allocator;

        VkExtent2D _windowExtent{1700, 900};

        SDL_Window_ptr _window{nullptr};

        VkInstance _instance;
        VkDebugUtilsMessengerEXT _debug_messenger;
        VkPhysicalDevice _chosenGPU;
        VkDevice _device;
        VkPhysicalDeviceProperties _gpuProperties;

        DeletionQueue _mainDeletionQueue;

        VkSurfaceKHR _surface;

        VkSwapchainKHR _swapchain;
        VkFormat _swapchainImageFormat;
        std::vector<VkImage> _swapchainImages;
        std::vector<VkImageView> _swapchainImageViews;

        VkQueue _graphicsQueue;
        uint32_t _graphicsQueueFamily;

        tracy::VkCtx* _graphicsQueueContext;

        VkRenderPass _renderPass;
        std::vector<VkFramebuffer> _framebuffers;
        FrameData _frames[FRAME_OVERLAP];
        UploadContext _uploadContext;

        VkDescriptorSetLayout _globalSetLayout;
        VkDescriptorSetLayout _objectSetLayout;
        VkDescriptorSetLayout _singleTextureSetLayout;

        VkDescriptorPool _descriptorPool;

        GPUSceneData _sceneParameters;
        AllocatedBuffer _sceneParametersBuffer;

        //VkPipeline _meshPipeline;

        VkImageView _depthImageView;
        AllocatedImage _depthImage;

        VkFormat _depthFormat;

        std::vector<RenderObject> _renderables;
        std::unordered_map<std::string, Material> _materials;
        std::unordered_map<std::string, Mesh> _meshes;
        std::unordered_map<std::string, Texture> _loadedTextures;


        Material* create_material(VkPipeline pipeline, VkPipelineLayout layout, const std::string& name);
        Material* get_material(const std::string& name);

        Mesh* get_mesh(const std::string& name);

        void load_images();

        FrameData& get_current_frame();
        AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memUsage);

        void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

        void init();

        void cleanup();

        void draw();
        void draw_objects(VkCommandBuffer cmd, RenderObject* first, int count);


    private:

        void init_vulkan();

        void init_swapchain();

        void init_commands();

        void init_default_renderpass();

        void init_framebuffers();

        void init_sync_structures();

        void init_descriptors();
        void init_pipelines();

        void init_scene();

        void init_imgui();

        bool load_shader_module(std::filesystem::path filePath, VkShaderModule *outShaderModule);
        void load_shader_module_from_path(std::filesystem::path filePath, VkShaderModule *outShaderModule);

        void load_meshes();

        void upload_mesh(Mesh& mesh);

        size_t pad_uniform_buffer_size(size_t originalSize);


    };

    class PipelineBuilder {
    public:

        std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
        VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
        VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
        VkViewport _viewport;
        VkRect2D _scissor;
        VkPipelineRasterizationStateCreateInfo _rasterizer;
        VkPipelineColorBlendAttachmentState _colorBlendAttachment;
        VkPipelineDepthStencilStateCreateInfo _depthStencil;
        VkPipelineMultisampleStateCreateInfo _multisampling;
        VkPipelineLayout _pipelineLayout;

        VkPipeline build_pipeline(VkDevice device, VkRenderPass pass);
    };




}

#endif //GIBA_SPOCK_H
