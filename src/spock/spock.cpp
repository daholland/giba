//
// Created by daholland on 3/23/21.
//

#include "spock.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <vk_types.h>
#include <vk_init.h>

#include "VkBootstrap.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include <glm/gtx/transform.hpp>

#include <iostream>
#include <fstream>



using namespace std;
using namespace spock;

#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::cout <<"Detected Vulkan error: " << err << std::endl; \
			abort();                                                \
		}                                                           \
	} while (0)



void Spock::init() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

    _window = SDL_CreateWindow(
                "spock gibalib",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                _windowExtent.width,
                _windowExtent.height,
                window_flags
            );

    init_vulkan();

    init_swapchain();

    init_commands();

    init_default_renderpass();

    init_framebuffers();
    init_sync_structures();

    init_pipelines();

    load_meshes();

    _isInitialized = true;
}

void Spock::init_vulkan() {
    vkb::InstanceBuilder builder;

    auto inst_ret = builder.set_app_name("GibaSpock")
            .request_validation_layers(true)
            .require_api_version(1,1,0)
            .use_default_debug_messenger()
            .build();

    vkb::Instance vkb_inst = inst_ret.value();

    _instance = vkb_inst.instance;

    _debug_messenger = vkb_inst.debug_messenger;

    SDL_Vulkan_CreateSurface(_window, _instance, &_surface);

    vkb::PhysicalDeviceSelector selector{vkb_inst};
    vkb::PhysicalDevice physicalDevice = selector
            .set_minimum_version(1,1)
            .set_surface(_surface)
            .select()
            .value();

    vkb::DeviceBuilder deviceBuilder{physicalDevice};

    vkb::Device vkbDevice = deviceBuilder.build().value();

    _device = vkbDevice.device;
    _chosenGPU = physicalDevice.physical_device;

    _graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    _graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = _chosenGPU;
    allocatorInfo.device = _device;
    allocatorInfo.instance = _instance;
    vmaCreateAllocator(&allocatorInfo, &_allocator);


}

void Spock::init_swapchain() {
    vkb::SwapchainBuilder swapchainBuilder{_chosenGPU,_device,_surface};

    vkb::Swapchain vkbSwapchain = swapchainBuilder
            .use_default_format_selection()
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(_windowExtent.width, _windowExtent.height)
            .build()
            .value();

    _swapchain = vkbSwapchain.swapchain;
    _swapchainImages = vkbSwapchain.get_images().value();
    _swapchainImageViews = vkbSwapchain.get_image_views().value();

    _swapchainImageFormat = vkbSwapchain.image_format;

    _mainDeletionQueue.push_function([=]() {
        vkDestroySwapchainKHR(_device, _swapchain, nullptr);
    });

    VkExtent3D depthImageExtent = {
            _windowExtent.width,
            _windowExtent.height,
            1
    };

    _depthFormat = VK_FORMAT_D32_SFLOAT;

    VkImageCreateInfo dimg_info = vkinit::image_create_info(_depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent);

    VmaAllocationCreateInfo dimg_allocinfo = {};
    dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vmaCreateImage(_allocator, &dimg_info, &dimg_allocinfo, &_depthImage._image, &_depthImage._allocation, nullptr);

    VkImageViewCreateInfo dview_info = vkinit::imageview_create_info(_depthFormat, _depthImage._image, VK_IMAGE_ASPECT_DEPTH_BIT);

    VK_CHECK(vkCreateImageView(_device, &dview_info, nullptr, &_depthImageView));

    _mainDeletionQueue.push_function([=]() {
       vkDestroyImageView(_device, _depthImageView, nullptr);
       vmaDestroyImage(_allocator, _depthImage._image, _depthImage._allocation);
    });

}

void Spock::init_commands() {

    VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_commandPool));

    VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_commandPool, 1);
    VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_mainCommandBuffer));

    _mainDeletionQueue.push_function([=]() {
        vkDestroyCommandPool(_device, _commandPool, nullptr);
    });

}

void Spock::init_default_renderpass(){
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = _swapchainImageFormat;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depth_attachment = {};
    depth_attachment.flags = 0;
    depth_attachment.format = _depthFormat;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref = {};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    VkAttachmentDescription attachments[2] = { color_attachment, depth_attachment };

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    render_pass_info.attachmentCount = 2;
    render_pass_info.pAttachments = &attachments[0];

    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;

    VK_CHECK(vkCreateRenderPass(_device, &render_pass_info, nullptr, &_renderPass));
    _mainDeletionQueue.push_function([=]() {
        vkDestroyRenderPass(_device, _renderPass, nullptr);
    });

}

void Spock::init_framebuffers() {
    VkFramebufferCreateInfo fb_info = vkinit::framebuffer_create_info(_renderPass, _windowExtent);
//    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//    fb_info.pNext = nullptr;
//
//    fb_info.renderPass = _renderPass;
//    fb_info.attachmentCount = 1;
//    fb_info.width = _windowExtent.width;
//    fb_info.height = _windowExtent.height;
//    fb_info.layers = 1;

    const uint32_t swapchain_imagecount = _swapchainImages.size();
    _framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

    for (int i = 0; i < swapchain_imagecount; i++) {
        VkImageView attachments[2];
        attachments[0] = _swapchainImageViews[i];
        attachments[1] = _depthImageView;

        fb_info.pAttachments = attachments;
        fb_info.attachmentCount = 2;

        VK_CHECK(vkCreateFramebuffer(_device, &fb_info, nullptr, &_framebuffers[i]));

        _mainDeletionQueue.push_function([=]() {
            vkDestroyFramebuffer(_device, _framebuffers[i], nullptr);
            vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
        });
    }
}

void Spock::init_sync_structures() {
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;

    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_renderFence));

    _mainDeletionQueue.push_function([=]() {
        vkDestroyFence(_device, _renderFence, nullptr);
    });

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.flags = 0;

    VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_presentSemaphore));
    VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_renderSemaphore));
    _mainDeletionQueue.push_function([=]() {
        vkDestroySemaphore(_device, _presentSemaphore, nullptr);
        vkDestroySemaphore(_device, _renderSemaphore, nullptr);
    });
}

void Spock::init_pipelines() {
    VkShaderModule triangleFragShader;
    if (!load_shader_module("../../src/shaders/colored_triangle.frag.spv", &triangleFragShader))
    {
        std::cout << "Error when building the tri frag!!!" << std::endl;
    }
    else {
        std::cout << "Triangle frag shader loaded" << std::endl;
    }

    VkShaderModule triangleVertexShader;
    if (!load_shader_module("../../src/shaders/colored_triangle.vert.spv", &triangleVertexShader))
    {
        std::cout << "Error when building the tri vert!!!" << std::endl;
    }
    else {
        std::cout << "Triangle vert shader loaded" << std::endl;
    }

    VkShaderModule redTriangleFragShader;
    if (!load_shader_module("../../src/shaders/triangle.frag.spv", &redTriangleFragShader))
    {
        std::cout << "Error when building the redtri frag!!!" << std::endl;
    }
    else {
        std::cout << "red Triangle frag shader loaded" << std::endl;
    }

    VkShaderModule redTriangleVertexShader;
    if (!load_shader_module("../../src/shaders/triangle.vert.spv", &redTriangleVertexShader))
    {
        std::cout << "Error when building the red tri vert!!!" << std::endl;
    }
    else {
        std::cout << "red Triangle vert shader loaded" << std::endl;
    }



    VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::pipeline_layout_create_info();

    VK_CHECK(vkCreatePipelineLayout(_device, &pipeline_layout_info, nullptr, &_trianglePipelineLayout));

    VkPipelineLayoutCreateInfo mesh_pipeline_layout_info = vkinit::pipeline_layout_create_info();

    VkPushConstantRange push_constant;
    push_constant.offset = 0;
    push_constant.size = sizeof(MeshPushConstants);
    push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    mesh_pipeline_layout_info.pushConstantRangeCount = 1;
    mesh_pipeline_layout_info.pPushConstantRanges = &push_constant;

    VK_CHECK(vkCreatePipelineLayout(_device, &mesh_pipeline_layout_info, nullptr, &_meshPipelineLayout));


    PipelineBuilder pipelineBuilder;
    pipelineBuilder._shaderStages.push_back(
            vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, triangleVertexShader)
            );
    pipelineBuilder._shaderStages.push_back(
            vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader)
            );

    pipelineBuilder._vertexInputInfo = vkinit::vertex_input_state_create_info();
    pipelineBuilder._inputAssembly = vkinit::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    pipelineBuilder._viewport.x = 0.0f;
    pipelineBuilder._viewport.y = 0.0f;
    pipelineBuilder._viewport.width = (float)_windowExtent.width;
    pipelineBuilder._viewport.height = (float)_windowExtent.height;
    pipelineBuilder._viewport.minDepth = 0.0f;
    pipelineBuilder._viewport.maxDepth = 1.0f;

    pipelineBuilder._scissor.offset = {0,0};
    pipelineBuilder._scissor.extent = _windowExtent;

    pipelineBuilder._rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL);

    pipelineBuilder._multisampling = vkinit::multisampling_state_create_info();

    pipelineBuilder._colorBlendAttachment = vkinit::color_blend_attachment_state();

    pipelineBuilder._depthStencil = vkinit::depth_stencil_create_info(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);

    pipelineBuilder._pipelineLayout = _trianglePipelineLayout;

    _trianglePipeline = pipelineBuilder.build_pipeline(_device, _renderPass);

    pipelineBuilder._shaderStages.clear();

    pipelineBuilder._shaderStages.push_back(
            vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, redTriangleVertexShader)
    );
    pipelineBuilder._shaderStages.push_back(
            vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, redTriangleFragShader)
    );

    _redTrianglePipeline = pipelineBuilder.build_pipeline(_device, _renderPass);

    VkShaderModule meshVertShader;
    if (!load_shader_module("../../src/shaders/tri_mesh.vert.spv", &meshVertShader)) {
        std::cout << "Error when building the trimesh shader module!" << std::endl;
    } else {
        std::cout << "TriMesh vertex shader successfully loaded" << std::endl;
    }
    VertexInputDescription vertexDescription = Vertex::get_vertex_description();

    pipelineBuilder._vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription.attributes.size();
    pipelineBuilder._vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();

    pipelineBuilder._vertexInputInfo.vertexBindingDescriptionCount = vertexDescription.bindings.size();
    pipelineBuilder._vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();

    pipelineBuilder._shaderStages.clear();

    pipelineBuilder._shaderStages.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, meshVertShader));
    pipelineBuilder._shaderStages.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader));

    pipelineBuilder._pipelineLayout = _meshPipelineLayout;
    _meshPipeline = pipelineBuilder.build_pipeline(_device, _renderPass);

    //clean
    vkDestroyShaderModule(_device, meshVertShader, nullptr);
    vkDestroyShaderModule(_device, redTriangleVertexShader, nullptr);
    vkDestroyShaderModule(_device, redTriangleFragShader, nullptr);
    vkDestroyShaderModule(_device, triangleVertexShader, nullptr);
    vkDestroyShaderModule(_device, triangleFragShader, nullptr);

    _mainDeletionQueue.push_function([=](){
       vkDestroyPipeline(_device, _redTrianglePipeline, nullptr);
       vkDestroyPipeline(_device, _trianglePipeline, nullptr);
       vkDestroyPipeline(_device, _meshPipeline, nullptr);

       vkDestroyPipelineLayout(_device, _meshPipelineLayout, nullptr);
       vkDestroyPipelineLayout(_device, _trianglePipelineLayout, nullptr);
    });

}

void Spock::cleanup() {
    if (_isInitialized) {
//        vkDeviceWaitIdle(_device);
//
//        vkDestroyCommandPool(_device,_commandPool, nullptr);
//
//        vkDestroySemaphore(_device,_renderSemaphore, nullptr);
//        vkDestroySemaphore(_device,_presentSemaphore, nullptr);
//        vkDestroyFence(_device, _renderFence, nullptr);
//
//        vkDestroySwapchainKHR(_device, _swapchain, nullptr);
//
//        vkDestroyRenderPass(_device, _renderPass, nullptr);
//
//        for (int i = 0; i < _swapchainImageViews.size(); i++) {
//            vkDestroyFramebuffer(_device,_framebuffers[i],nullptr);
//            vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
//        }

        vkWaitForFences(_device, 1, &_renderFence, true, 1000000000);

        _mainDeletionQueue.flush();

        vmaDestroyAllocator(_allocator);

        vkDestroySurfaceKHR(_instance, _surface, nullptr);

        vkDestroyDevice(_device, nullptr);
        vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
        vkDestroyInstance(_instance, nullptr);

        SDL_DestroyWindow(_window);
    }
}

void Spock::draw() {
    VK_CHECK(vkWaitForFences(_device, 1, &_renderFence, true, 1000000000));
    VK_CHECK(vkResetFences(_device, 1, &_renderFence));

    uint32_t swapchainImageIndex;
    VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain,1000000000, _presentSemaphore, nullptr, &swapchainImageIndex));

    VK_CHECK(vkResetCommandBuffer(_mainCommandBuffer, 0));

    VkCommandBuffer cmd = _mainCommandBuffer;

    VkCommandBufferBeginInfo cmdBeginInfo = {};
    cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBeginInfo.pNext = nullptr;
    cmdBeginInfo.pInheritanceInfo = nullptr;
    cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    VkClearValue clearValue;
    float flash = abs(sin(_frameNumber / 120.f));
    clearValue.color = { { 0.0f, 0.0f, flash, 1.0f}};

    VkClearValue depthClear;
    depthClear.depthStencil.depth = 1.f;

    VkRenderPassBeginInfo rpInfo = vkinit::renderpass_begin_info(_renderPass, _windowExtent, _framebuffers[swapchainImageIndex]);
//    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//    rpInfo.pNext = nullptr;
//
//    rpInfo.renderPass = _renderPass;
//    rpInfo.renderArea.offset.x = 0;
//    rpInfo.renderArea.offset.y = 0;
//    rpInfo.renderArea.extent = _windowExtent;
//    rpInfo.framebuffer = _framebuffers[swapchainImageIndex];

    VkClearValue clearValues[] = { clearValue, depthClear };
    rpInfo.clearValueCount = 2;
    rpInfo.pClearValues = &clearValues[0];

    //////////
    vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

    if (_selectedShader == 0) {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _trianglePipeline);
        vkCmdDraw(cmd,3,1,0,0);
    } else if (_selectedShader == 1) {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _redTrianglePipeline);
        vkCmdDraw(cmd,3,1,0,0);
    } else {
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _meshPipeline);

        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(cmd, 0, 1, &_monkeyMesh._vertexBuffer._buffer, &offset);

        glm::vec3 camPos = { 0.f, 0.f, -2.f};

        glm::mat4 view = glm::translate(glm::mat4 { 1.f}, camPos);
        glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
        projection[1][1] *= -1;
        glm::mat4 model = glm::rotate(glm::mat4{1.f}, glm::radians(_frameNumber * 0.4f), glm::vec3(0,1,0));

        glm::mat4 mesh_matrix = projection * view * model;

        MeshPushConstants constants;
        constants.render_matrix = mesh_matrix;

        vkCmdPushConstants(cmd, _meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);

        vkCmdDraw(cmd,_monkeyMesh._vertices.size(),1,0,0);
    }



    vkCmdEndRenderPass(cmd);
    /////////////

    VK_CHECK(vkEndCommandBuffer(cmd));

    VkSubmitInfo submit = {};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.pNext = nullptr;

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    submit.pWaitDstStageMask = &waitStage;
    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = &_presentSemaphore;

    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &_renderSemaphore;

    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &cmd;

    VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submit, _renderFence));

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &_swapchain;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &_renderSemaphore;

    presentInfo.pImageIndices = &swapchainImageIndex;

    VK_CHECK(vkQueuePresentKHR(_graphicsQueue, &presentInfo));

    _frameNumber++;
}

void Spock::run() {
    SDL_Event e;
    bool bQuit = false;

    while (!bQuit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT
                //|| (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q)
                ) {
                bQuit = true;
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_q) {
                    SDL_Event ev;
                    ev.type = SDL_QUIT;

                    SDL_PushEvent(&ev);
                }

                if (e.key.keysym.sym == SDLK_SPACE) {
                    _selectedShader += 1;
                    if(_selectedShader > 2) {
                        _selectedShader = 0;
                    }
                }
            }
        }

        draw();
    }
}

bool Spock::load_shader_module(const char *filePath, VkShaderModule *outShaderModule) {
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        return false;
    }

    size_t fileSize = (size_t)file.tellg();

    std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

    file.seekg(0);

    file.read((char*)buffer.data(), fileSize);

    file.close();

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;

    createInfo.codeSize = buffer.size() * sizeof(uint32_t);
    createInfo.pCode = buffer.data();

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        return false;
    }
    *outShaderModule = shaderModule;
    return true;


}

void Spock::load_meshes() {
    _triangleMesh._vertices.resize(3);

    _triangleMesh._vertices[0].position = {1.f, 1.f, 0.f};
    _triangleMesh._vertices[1].position = {-1.f, 1.f, 0.f};
    _triangleMesh._vertices[2].position = {0.f, -1.f, 0.f};

    _triangleMesh._vertices[0].color = {0.f, 1.f, 0.f};
    _triangleMesh._vertices[1].color = {0.f, 1.f, 0.f};
    _triangleMesh._vertices[2].color = {0.f, 1.f, 0.f};

    _monkeyMesh.load_from_obj("../../assets/monkey_smooth.obj");

    upload_mesh(_triangleMesh);
    upload_mesh(_monkeyMesh);


}

void Spock::upload_mesh(Mesh &mesh) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = mesh._vertices.size() * sizeof(Vertex);
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    VmaAllocationCreateInfo vmaAllocInfo = {};
    vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    VK_CHECK(vmaCreateBuffer(_allocator, &bufferInfo, &vmaAllocInfo,
                             &mesh._vertexBuffer._buffer,
                             &mesh._vertexBuffer._allocation,
                             nullptr));

    _mainDeletionQueue.push_function([=]() {
       vmaDestroyBuffer(_allocator, mesh._vertexBuffer._buffer, mesh._vertexBuffer._allocation);
    });

    void* data;
    vmaMapMemory(_allocator, mesh._vertexBuffer._allocation, &data);
    memcpy(data, mesh._vertices.data(), mesh._vertices.size() * sizeof(Vertex));
    vmaUnmapMemory(_allocator, mesh._vertexBuffer._allocation);

}

VkPipeline PipelineBuilder::build_pipeline(VkDevice device, VkRenderPass pass) {
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;

    viewportState.viewportCount = 1;
    viewportState.pViewports = &_viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &_scissor;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.pNext = nullptr;

    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &_colorBlendAttachment;

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;

    pipelineInfo.stageCount = _shaderStages.size();
    pipelineInfo.pStages = _shaderStages.data();
    pipelineInfo.pVertexInputState = &_vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &_inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &_rasterizer;
    pipelineInfo.pMultisampleState = &_multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &_depthStencil;
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = pass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VkPipeline newPipeline;
    if (vkCreateGraphicsPipelines(
            device, VK_NULL_HANDLE,1,
            &pipelineInfo,nullptr, &newPipeline) != VK_SUCCESS) {
        std::cout << "failed to create pipeline!!" << std::endl;
        return VK_NULL_HANDLE;

    } else {
        return newPipeline;
    }

}


