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

namespace {
    typedef struct SDL_Window* SDL_Window_ptr;
}
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

    class Spock {
    public:

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
        VkSurfaceKHR _surface;

        DeletionQueue _mainDeletionQueue;

        VkSwapchainKHR _swapchain;
        VkFormat _swapchainImageFormat;
        std::vector<VkImage> _swapchainImages;
        std::vector<VkImageView> _swapchainImageViews;

        VkQueue _graphicsQueue;
        uint32_t _graphicsQueueFamily;

        VkCommandPool _commandPool;
        VkCommandBuffer _mainCommandBuffer;

        VkRenderPass _renderPass;
        std::vector<VkFramebuffer> _framebuffers;

        VkSemaphore _presentSemaphore, _renderSemaphore;
        VkFence _renderFence;

        VkPipelineLayout _trianglePipelineLayout;
        VkPipeline _trianglePipeline;
        VkPipeline _redTrianglePipeline;

        VkPipelineLayout _meshPipelineLayout;
        VkPipeline _meshPipeline;

        Mesh _triangleMesh;
        Mesh _monkeyMesh;

        VkImageView _depthImageView;
        AllocatedImage _depthImage;

        VkFormat _depthFormat;

        void init();

        void cleanup();

        void draw();

        void run();

    private:

        void init_vulkan();

        void init_swapchain();

        void init_commands();

        void init_default_renderpass();

        void init_framebuffers();

        void init_sync_structures();

        void init_pipelines();

        bool load_shader_module(const char *filePath, VkShaderModule *outShaderModule);

        void load_meshes();

        void upload_mesh(Mesh& mesh);

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
