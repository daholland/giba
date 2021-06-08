//
// Created by daholland on 3/23/21.
//
#include "spock.h"

#include <SDL.h>

#include <SDL_vulkan.h>
#include "VkBootstrap.h"

#define VMA_IMPLEMENTATION

#include <vk_mem_alloc.h>
#include <glm/gtx/transform.hpp>

#include <imgui.h>

#include <imgui_impl_sdl.h>
#include <imgui_impl_vulkan.h>
#include <vk_types.h>

#include <vk_init.h>
#include <vk_textures.h>

#include "Tracy.hpp"
#include "TracyVulkan.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

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
    ZoneScopedN("Engine Init");
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

    init_descriptors();

    init_pipelines();

    //load_meshes();
    //load_images();

    //init_scene();

    init_imgui();

    //init_state();
    _appState = AppState {};
    _appState.camPos = { 0.f, -6.f, -10.f };

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

    vkGetPhysicalDeviceProperties(_chosenGPU, &_gpuProperties);
    std::cout << "The GPU has a minimum buffer alignment of " << _gpuProperties.limits.minUniformBufferOffsetAlignment << std::endl;

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
    VkCommandPoolCreateInfo uploadCommandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily);

    VK_CHECK(vkCreateCommandPool(_device, &uploadCommandPoolInfo, nullptr, &_uploadContext._commandPool));
    _mainDeletionQueue.push_function([=]() {
       vkDestroyCommandPool(_device, _uploadContext._commandPool, nullptr);
    });


    for (int i = 0; i < FRAME_OVERLAP; i++) {
        VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_frames[i]._commandPool));

        VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_frames[i]._commandPool, 1);
        VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_frames[i]._mainCommandBuffer));

        _mainDeletionQueue.push_function([=]() {
            vkDestroyCommandPool(_device, _frames[i]._commandPool, nullptr);
        });
    }
    _graphicsQueueContext = TracyVkContext(_chosenGPU, _device, _graphicsQueue, _frames[0]._mainCommandBuffer);

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
    VkFenceCreateInfo fenceCreateInfo = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
    VkFenceCreateInfo uploadFenceCreateInfo = vkinit::fence_create_info();

    VK_CHECK(vkCreateFence(_device, &uploadFenceCreateInfo, nullptr, &_uploadContext._uploadFence));
    _mainDeletionQueue.push_function([=](){
       vkDestroyFence(_device, _uploadContext._uploadFence, nullptr);
    });

    VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphore_create_info();


    for (int i = 0; i < FRAME_OVERLAP; i++) {
        VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_frames[i]._renderFence));

        _mainDeletionQueue.push_function([=]() {
            vkDestroyFence(_device, _frames[i]._renderFence, nullptr);
        });


        VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._presentSemaphore));
        VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._renderSemaphore));
        _mainDeletionQueue.push_function([=]() {
            vkDestroySemaphore(_device, _frames[i]._presentSemaphore, nullptr);
            vkDestroySemaphore(_device, _frames[i]._renderSemaphore, nullptr);
        });
    }
}

void Spock::init_pipelines() {


    VkPipelineLayoutCreateInfo mesh_pipeline_layout_info = vkinit::pipeline_layout_create_info();

    VkPushConstantRange push_constant;
    push_constant.offset = 0;
    push_constant.size = sizeof(MeshPushConstants);
    push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    mesh_pipeline_layout_info.pushConstantRangeCount = 1;
    mesh_pipeline_layout_info.pPushConstantRanges = &push_constant;

    VkDescriptorSetLayout setLayouts[] = { _globalSetLayout, _objectSetLayout };

    mesh_pipeline_layout_info.setLayoutCount = std::size(setLayouts);
    mesh_pipeline_layout_info.pSetLayouts = setLayouts;

    VkPipelineLayout meshPipelineLayout;
    VK_CHECK(vkCreatePipelineLayout(_device, &mesh_pipeline_layout_info, nullptr, &meshPipelineLayout));

    VkPipelineLayoutCreateInfo textured_pipeline_layout_info = mesh_pipeline_layout_info;

    VkDescriptorSetLayout texturedSetLayouts[] = { _globalSetLayout,
                                                   _objectSetLayout,
                                                   _singleTextureSetLayout };

    textured_pipeline_layout_info.setLayoutCount = std::size(texturedSetLayouts);
    textured_pipeline_layout_info.pSetLayouts = texturedSetLayouts;

    VkPipelineLayout texturedPipeLayout;
    VK_CHECK(vkCreatePipelineLayout(_device, &textured_pipeline_layout_info, nullptr, &texturedPipeLayout));


    PipelineBuilder pipelineBuilder;

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

    VkShaderModule meshVertShader;
    load_shader_module_from_path("src/shaders/tri_mesh.vert.spv", &meshVertShader);

    VkShaderModule colorMeshShader;
    load_shader_module_from_path("src/shaders/default_lit.frag.spv", &colorMeshShader);

    VkShaderModule texturedMeshShader;
    load_shader_module_from_path("src/shaders/textured_lit.frag.spv", &texturedMeshShader);

    VertexInputDescription vertexDescription = Vertex::get_vertex_description();

    pipelineBuilder._vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription.attributes.size();
    pipelineBuilder._vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();

    pipelineBuilder._vertexInputInfo.vertexBindingDescriptionCount = vertexDescription.bindings.size();
    pipelineBuilder._vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();

    pipelineBuilder._shaderStages.clear();

    pipelineBuilder._shaderStages.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, meshVertShader));
    pipelineBuilder._shaderStages.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, colorMeshShader));

    pipelineBuilder._pipelineLayout = meshPipelineLayout;
    VkPipeline meshPipeline = pipelineBuilder.build_pipeline(_device, _renderPass);

    create_material(meshPipeline, meshPipelineLayout, "defaultmesh");

    pipelineBuilder._shaderStages.clear();
    pipelineBuilder._shaderStages.push_back(
            vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, meshVertShader));
    pipelineBuilder._shaderStages.push_back(
            vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, texturedMeshShader));

    pipelineBuilder._pipelineLayout = texturedPipeLayout;
    VkPipeline texPipeline = pipelineBuilder.build_pipeline(_device, _renderPass);
    create_material(texPipeline, texturedPipeLayout, "texturedmesh");



    //clean
    vkDestroyShaderModule(_device, meshVertShader, nullptr);
    vkDestroyShaderModule(_device, colorMeshShader, nullptr);
    vkDestroyShaderModule(_device, texturedMeshShader, nullptr);

    _mainDeletionQueue.push_function([=](){

       vkDestroyPipeline(_device, meshPipeline, nullptr);
       vkDestroyPipeline(_device, texPipeline, nullptr);

       vkDestroyPipelineLayout(_device, meshPipelineLayout, nullptr);
        vkDestroyPipelineLayout(_device, texturedPipeLayout, nullptr);

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
        vkDeviceWaitIdle(_device);

        _mainDeletionQueue.flush();
        TracyVkDestroy(_graphicsQueueContext);

        vmaDestroyAllocator(_allocator);

        vkDestroySurfaceKHR(_instance, _surface, nullptr);

        vkDestroyDevice(_device, nullptr);
        vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
        vkDestroyInstance(_instance, nullptr);

        SDL_DestroyWindow(_window);
    }
}

void Spock::draw() {
    ZoneScopedN("Engine Draw");

    ImGui::Render();
    {
        ZoneScopedN("Fence Wait");
        VK_CHECK(vkWaitForFences(_device, 1, &get_current_frame()._renderFence, true, 1000000000));
        //vkDeviceWaitIdle(_device);
        VK_CHECK(vkResetFences(_device, 1, &get_current_frame()._renderFence));

        VK_CHECK(vkResetCommandBuffer(get_current_frame()._mainCommandBuffer, 0));
    }

    uint32_t swapchainImageIndex;
    {
        ZoneScopedNC("Acquire Next Image", tracy::Color::White);
        VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain, 1000000000, get_current_frame()._presentSemaphore, nullptr,
                                       &swapchainImageIndex));
    }
    VkCommandBuffer cmd = get_current_frame()._mainCommandBuffer;

    VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    VkClearValue clearValue;
    float flash = abs(sin((float) _frameNumber / 120.f));
    clearValue.color = {{0.0f, 0.0f, flash, 1.0f}};
    {
        TracyVkZone(_graphicsQueueContext, get_current_frame()._mainCommandBuffer, "All Frame");
        ZoneScopedNC("Render Frame", tracy::Color::White);

        VkClearValue depthClear;
        depthClear.depthStencil.depth = 1.f;

        VkRenderPassBeginInfo rpInfo = vkinit::renderpass_begin_info(_renderPass, _windowExtent,
                                                                     _framebuffers[swapchainImageIndex]);
    //    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    //    rpInfo.pNext = nullptr;
    //
    //    rpInfo.renderPass = _renderPass;
    //    rpInfo.renderArea.offset.x = 0;
    //    rpInfo.renderArea.offset.y = 0;
    //    rpInfo.renderArea.extent = _windowExtent;
    //    rpInfo.framebuffer = _framebuffers[swapchainImageIndex];

        VkClearValue clearValues[] = {clearValue, depthClear};
        rpInfo.clearValueCount = 2;
        rpInfo.pClearValues = &clearValues[0];

        //////////
        vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

        draw_objects(cmd, _renderables.data(), _renderables.size());

        {
            TracyVkZone(_graphicsQueueContext, get_current_frame()._mainCommandBuffer, "Imgui Draw");
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
        }
        vkCmdEndRenderPass(cmd);
        /////////////
}

    TracyVkCollect(_graphicsQueueContext, get_current_frame()._mainCommandBuffer);

    VK_CHECK(vkEndCommandBuffer(cmd));

    VkSubmitInfo submit = vkinit::submit_info(&cmd);
    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    submit.pWaitDstStageMask = &waitStage;
    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = &get_current_frame()._presentSemaphore;

    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &get_current_frame()._renderSemaphore;

    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &cmd;
    {
        ZoneScopedN("Queue Submit");
        VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submit, get_current_frame()._renderFence));
    }
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &_swapchain;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &get_current_frame()._renderSemaphore;

    presentInfo.pImageIndices = &swapchainImageIndex;
    {
        ZoneScopedN("Queue Present");
        VK_CHECK(vkQueuePresentKHR(_graphicsQueue, &presentInfo));
    }
    FrameMark;
    _frameNumber++;
}

bool Spock::load_shader_module(filesystem::path filePath, VkShaderModule *outShaderModule) {
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

void Spock::load_shader_module_from_path(filesystem::path filePath, VkShaderModule *outShaderModule) {
    auto fn = filePath.filename();

    if (!load_shader_module(filePath, outShaderModule))
    {
        std::cout << "Error when building " << fn  << " " << std::endl;
    }
    else {
        std::cout << fn << " shader loaded" << std::endl;
    }
}

void Spock::load_meshes() {
    Mesh triMesh;
    triMesh._vertices.resize(3);

    triMesh._vertices[0].position = {1.f, 1.f, 0.f};
    triMesh._vertices[1].position = {-1.f, 1.f, 0.f};
    triMesh._vertices[2].position = {0.f, -1.f, 0.f};

    triMesh._vertices[0].color = {0.f, 1.f, 0.f};
    triMesh._vertices[1].color = {0.f, 1.f, 0.f};
    triMesh._vertices[2].color = {0.f, 1.f, 0.f};

    Mesh monkMesh;
    monkMesh.load_from_obj("../assets/monkey_smooth.obj");

    Mesh lostEmpire{};
    lostEmpire.load_from_obj("../assets/lost_empire.obj");

    upload_mesh(triMesh);
    upload_mesh(monkMesh);
    upload_mesh(lostEmpire);

    _meshes["monkey"] = monkMesh;
    _meshes["triangle"] = triMesh;
    _meshes["empire"] = lostEmpire;

}

void Spock::upload_mesh(Mesh &mesh) {
    ZoneScopedNC("Upload Mesh", tracy::Color::Orange);
    const size_t bufferSize = mesh._vertices.size() * sizeof(Vertex);

    VkBufferCreateInfo stagingBufferInfo = {};
    stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferInfo.pNext = nullptr;

    stagingBufferInfo.size = bufferSize;
    stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VmaAllocationCreateInfo vmaAllocInfo = {};
    vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

    AllocatedBuffer stagingBuffer;

    VK_CHECK(vmaCreateBuffer(_allocator, &stagingBufferInfo, &vmaAllocInfo,
                             &stagingBuffer._buffer,
                             &stagingBuffer._allocation,
                             nullptr));

    void* data;
    vmaMapMemory(_allocator, stagingBuffer._allocation, &data);
    memcpy(data, mesh._vertices.data(), mesh._vertices.size() * sizeof(Vertex));
    vmaUnmapMemory(_allocator, stagingBuffer._allocation);

    VkBufferCreateInfo vertexBufferInfo = {};
    vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexBufferInfo.pNext = nullptr;

    vertexBufferInfo.size = bufferSize;
    vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    vmaAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VK_CHECK(vmaCreateBuffer(_allocator, &vertexBufferInfo, &vmaAllocInfo,
                             &mesh._vertexBuffer._buffer,
                             &mesh._vertexBuffer._allocation,
                             nullptr));

    immediate_submit([=](VkCommandBuffer cmd) {
        VkBufferCopy copy;
        copy.dstOffset = 0;
        copy.srcOffset = 0;
        copy.size = bufferSize;
        vkCmdCopyBuffer(cmd, stagingBuffer._buffer, mesh._vertexBuffer._buffer, 1, &copy);
    });



    _mainDeletionQueue.push_function([=]() {
       vmaDestroyBuffer(_allocator, mesh._vertexBuffer._buffer, mesh._vertexBuffer._allocation);
    });
    vmaDestroyBuffer(_allocator, stagingBuffer._buffer, stagingBuffer._allocation);

}

Material *Spock::create_material(VkPipeline pipeline, VkPipelineLayout layout, const string &name) {
    Material mat{};
    mat.pipeline = pipeline;
    mat.pipelineLayout = layout;
    _materials[name] = mat;
    return &_materials[name];
}

Material *Spock::get_material(const string &name) {
    auto it = _materials.find(name);
    if (it == _materials.end()) {
        return nullptr;
    } else {
        return &(*it).second;
    }
}

Mesh *Spock::get_mesh(const string &name) {
    auto it = _meshes.find(name);
    if (it == _meshes.end()) {
        return nullptr;
    } else {
        return &(*it).second;
    }
}

void Spock::load_images() {
    ZoneScopedNC("Load Texture", tracy::Color::Yellow);

    Texture lostEmpire;
    vkutil::load_image_from_file(*this, "../assets/lost_empire-RGBA.png", lostEmpire.image);

    VkImageViewCreateInfo imageinfo = vkinit::imageview_create_info(VK_FORMAT_R8G8B8A8_SRGB, lostEmpire.image._image, VK_IMAGE_ASPECT_COLOR_BIT);
    vkCreateImageView(_device, &imageinfo, nullptr, &lostEmpire.imageView);

    _loadedTextures["empire_diffuse"] = lostEmpire;

    _mainDeletionQueue.push_function([=](){
       vkDestroyImageView(_device, lostEmpire.imageView , nullptr);
    });

}

void Spock::draw_objects(VkCommandBuffer cmd, RenderObject *first, int count) {
    glm::vec3 camPos = { 0.f, -6.f, -10.f };

    glm::mat4 view = glm::translate(glm::mat4(1.f), _appState.camPos);
    glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
    projection[1][1] *= -1;

    GPUCameraData camData;
    camData.projection = projection;
    camData.view = view;
    camData.viewproj = projection * view;

    {
        ZoneScopedNC("CamBuffer Allocate", tracy::Color::Red);
        void *data;
        vmaMapMemory(_allocator, get_current_frame().cameraBuffer._allocation, &data);
        memcpy(data, &camData, sizeof(GPUCameraData));
        vmaUnmapMemory(_allocator, get_current_frame().cameraBuffer._allocation);
    }
    float framed = (_frameNumber / 120.f);

    _sceneParameters.ambientColor = { sin(framed), 0, cos(framed), 1 };

    int frameIndex = _frameNumber % FRAME_OVERLAP;
    {
        ZoneScopedNC("SceneBuffer Allocate", tracy::Color::Red);
        char *sceneData;
        vmaMapMemory(_allocator, _sceneParametersBuffer._allocation, (void **) &sceneData);


        sceneData += pad_uniform_buffer_size(sizeof(GPUSceneData)) * frameIndex;

        memcpy(sceneData, &_sceneParameters, sizeof(GPUSceneData));

        vmaUnmapMemory(_allocator, _sceneParametersBuffer._allocation);
    }

    {
        ZoneScopedNC("ObjectBuffer Allocate", tracy::Color::Red);
        void *objectData;
        vmaMapMemory(_allocator, get_current_frame().objectBuffer._allocation, &objectData);

        GPUObjectData *objectSSBO = (GPUObjectData *) objectData;

        for (int i = 0; i < count; i++) {
            RenderObject &object = first[i];
            objectSSBO[i].modelMatrix = object.transformMatrix;
        }

        vmaUnmapMemory(_allocator, get_current_frame().objectBuffer._allocation);
    }
    Mesh* lastMesh = nullptr;
    Material* lastMaterial = nullptr;
    for (int i = 0; i < count; i++) {
        RenderObject& object = first[i];

        if (object.material != lastMaterial) {
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipeline);
            lastMaterial = object.material;

            uint32_t uniform_offset = pad_uniform_buffer_size(sizeof(GPUSceneData)) * frameIndex;

            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    object.material->pipelineLayout, 0, 1,
                                    &get_current_frame().globalDescriptor, 1, &uniform_offset);

            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    object.material->pipelineLayout, 1, 1,
                                    &get_current_frame().objectDescriptor, 0, nullptr);
            if (object.material->textureSet != VK_NULL_HANDLE) {
                vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, object.material->pipelineLayout,
                                        2, 1, &object.material->textureSet, 0, nullptr);
            }
        }

        MeshPushConstants constants{};
        constants.render_matrix = object.transformMatrix;

        vkCmdPushConstants(cmd, object.material->pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT,
                           0, sizeof(MeshPushConstants), &constants);

        if (object.mesh != lastMesh) {
            VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers(cmd, 0, 1, &object.mesh->_vertexBuffer._buffer, &offset);
            lastMesh = object.mesh;
        }

        vkCmdDraw(cmd, object.mesh->_vertices.size(), 1, 0, i);
    }
}

void Spock::init_scene() {
    RenderObject monkey{};
    monkey.mesh = get_mesh("monkey");
    monkey.material = get_material("defaultmesh");
    monkey.transformMatrix = glm::mat4 {1.f};

    _renderables.push_back(monkey);

    for (int x = -20; x <= 20; x++) {
        for (int y = -20; y <= 20; y++) {
            RenderObject tri{};
            tri.mesh = get_mesh("triangle");
            tri.material = get_material("defaultmesh");
            glm::mat4 translation = glm::translate(glm::mat4{1.0}, glm::vec3(x, 0, y));
            glm::mat4 scale = glm::scale(glm::mat4{1.0}, glm::vec3(0.2, 0.2, 0.2));
            tri.transformMatrix = translation * scale;

            _renderables.push_back(tri);
        }
    }

    RenderObject map;
    map.mesh = get_mesh("empire");
    map.material = get_material("texturedmesh");
    map.transformMatrix = glm::translate(glm::vec3{ 5, -10, 0 });

    _renderables.push_back(map);

    VkSamplerCreateInfo samplerInfo = vkinit::sampler_create_info(VK_FILTER_NEAREST);

    VkSampler blockySampler;
    vkCreateSampler(_device, &samplerInfo, nullptr, &blockySampler);

    Material* texturedMat = get_material("texturedmesh");

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = _descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &_singleTextureSetLayout;

    vkAllocateDescriptorSets(_device, &allocInfo, &texturedMat->textureSet);

    VkDescriptorImageInfo imageBufferInfo;
    imageBufferInfo.sampler = blockySampler;
    imageBufferInfo.imageView = _loadedTextures["empire_diffuse"].imageView;
    imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkWriteDescriptorSet texture1 = vkinit::write_descriptor_image(
                                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                         texturedMat->textureSet, &imageBufferInfo, 0);

    vkUpdateDescriptorSets(_device, 1, &texture1, 0, nullptr);

    _mainDeletionQueue.push_function([=](){
        vkDestroySampler(_device, blockySampler, nullptr);
    });

}

FrameData &Spock::get_current_frame() {
    return _frames[_frameNumber % FRAME_OVERLAP];
}

AllocatedBuffer Spock::create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memUsage) {
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;

    bufferCreateInfo.size = allocSize;
    bufferCreateInfo.usage = usage;

    VmaAllocationCreateInfo vmaallocInfo = {};
    vmaallocInfo.usage = memUsage;

    AllocatedBuffer newBuffer;

    VK_CHECK(vmaCreateBuffer(_allocator, &bufferCreateInfo,
                                &vmaallocInfo, &newBuffer._buffer, &newBuffer._allocation,
                                nullptr));



    return newBuffer;
}

void Spock::init_descriptors() {
    const size_t sceneParamBufferSize = FRAME_OVERLAP * pad_uniform_buffer_size(sizeof(GPUSceneData));
    _sceneParametersBuffer = create_buffer(sceneParamBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

    std::vector<VkDescriptorPoolSize> sizes = {
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10 },
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10 }
    };

    VkDescriptorPoolCreateInfo  pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.pNext = nullptr;

    pool_info.flags = 0;
    pool_info.maxSets = 10;
    pool_info.poolSizeCount = (uint32_t)sizes.size();
    pool_info.pPoolSizes = sizes.data();

    vkCreateDescriptorPool(_device, &pool_info, nullptr, &_descriptorPool);

    VkDescriptorSetLayoutBinding cameraBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT,0);

    VkDescriptorSetLayoutBinding sceneBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1);


    VkDescriptorSetLayoutBinding bindings[] = { cameraBind, sceneBind };

    VkDescriptorSetLayoutCreateInfo setInfo = {};
    setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    setInfo.pNext = nullptr;

    setInfo.bindingCount = 2;
    setInfo.flags = 0;
    setInfo.pBindings = bindings;

    vkCreateDescriptorSetLayout(_device, &setInfo, nullptr, &_globalSetLayout);

    VkDescriptorSetLayoutBinding objectBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);

    VkDescriptorSetLayoutCreateInfo set2info = {};
    set2info.bindingCount = 1;
    set2info.flags = 0;
    set2info.pNext = nullptr;
    set2info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    set2info.pBindings = &objectBind;

    vkCreateDescriptorSetLayout(_device, &set2info, nullptr, &_objectSetLayout);

    VkDescriptorSetLayoutBinding textureBind = vkinit::descriptorset_layout_binding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0);

    VkDescriptorSetLayoutCreateInfo set3info = {};
    set3info.bindingCount = 1;
    set3info.flags = 0;
    set3info.pNext = nullptr;
    set3info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    set3info.pBindings = &textureBind;

    vkCreateDescriptorSetLayout(_device, &set3info, nullptr, &_singleTextureSetLayout);

    for (int i = 0; i < FRAME_OVERLAP; i++) {
        const int MAX_OBJECTS = 10000;
        _frames[i].objectBuffer = create_buffer(sizeof(GPUObjectData) * MAX_OBJECTS, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

        _frames[i].cameraBuffer = create_buffer(sizeof(GPUCameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.pNext = nullptr;

        allocInfo.descriptorSetCount = 1;
        allocInfo.descriptorPool = _descriptorPool;
        allocInfo.pSetLayouts = &_globalSetLayout;

        vkAllocateDescriptorSets(_device, &allocInfo, &_frames[i].globalDescriptor);

        VkDescriptorSetAllocateInfo objSetAllocInfo = {};
        objSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        objSetAllocInfo.pNext = nullptr;

        objSetAllocInfo.descriptorSetCount = 1;
        objSetAllocInfo.descriptorPool = _descriptorPool;
        objSetAllocInfo.pSetLayouts = &_objectSetLayout;

        vkAllocateDescriptorSets(_device, &objSetAllocInfo, &_frames[i].objectDescriptor);

        VkDescriptorBufferInfo cameraInfo = {};
        cameraInfo.buffer = _frames[i].cameraBuffer._buffer;
        cameraInfo.offset = 0;
        cameraInfo.range = sizeof(GPUCameraData);

        VkDescriptorBufferInfo sceneInfo = {};
        sceneInfo.buffer = _sceneParametersBuffer._buffer;
        sceneInfo.offset = 0;
        sceneInfo.range = sizeof(GPUSceneData);

        VkDescriptorBufferInfo objectBufferInfo = {};
        objectBufferInfo.buffer = _frames[i].objectBuffer._buffer;
        objectBufferInfo.offset = 0;
        objectBufferInfo.range = sizeof(GPUObjectData) * MAX_OBJECTS;


        VkWriteDescriptorSet cameraWrite = vkinit::write_decriptor_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _frames[i].globalDescriptor, &cameraInfo, 0);
        VkWriteDescriptorSet sceneWrite = vkinit::write_decriptor_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, _frames[i].globalDescriptor, &sceneInfo, 1);
        VkWriteDescriptorSet objectWrite = vkinit::write_decriptor_buffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, _frames[i].objectDescriptor, &objectBufferInfo, 0);

        VkWriteDescriptorSet setWrites[] = { cameraWrite, sceneWrite, objectWrite };

        uint setWritesCount = std::size(setWrites);
        vkUpdateDescriptorSets(_device, setWritesCount, setWrites, 0, nullptr);

    }

    _mainDeletionQueue.push_function([=](){

        vkDestroyDescriptorSetLayout(_device, _globalSetLayout, nullptr);
        vkDestroyDescriptorSetLayout(_device, _objectSetLayout, nullptr);
        vkDestroyDescriptorSetLayout(_device, _singleTextureSetLayout, nullptr);

        vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
        vmaDestroyBuffer(_allocator, _sceneParametersBuffer._buffer, _sceneParametersBuffer._allocation);


        for (int i = 0; i < FRAME_OVERLAP; i++) {
            vmaDestroyBuffer(_allocator, _frames[i].cameraBuffer._buffer, _frames[i].cameraBuffer._allocation);
            vmaDestroyBuffer(_allocator, _frames[i].objectBuffer._buffer, _frames[i].objectBuffer._allocation);
        }
    });

}

size_t Spock::pad_uniform_buffer_size(size_t originalSize) {
    size_t minUboAlignment = _gpuProperties.limits.minUniformBufferOffsetAlignment;
    size_t alignedSize = originalSize;
    if (minUboAlignment > 0) {
        alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
    }
    return alignedSize;
}

void Spock::immediate_submit(function<void(VkCommandBuffer)> &&function) {
    ZoneScopedNC("Immediate Submit", tracy::Color::White);

    VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_uploadContext._commandPool, 1);
    VkCommandBuffer cmd;
    VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &cmd));

    VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

    function(cmd);

    VK_CHECK(vkEndCommandBuffer(cmd));

    VkSubmitInfo submit = vkinit::submit_info(&cmd);

    VK_CHECK(vkQueueSubmit(_graphicsQueue, 1, &submit, _uploadContext._uploadFence));

    vkWaitForFences(_device, 1, &_uploadContext._uploadFence, true, 999999999);
    vkResetFences(_device, 1, &_uploadContext._uploadFence);

    vkResetCommandPool(_device, _uploadContext._commandPool, 0);
}

void Spock::init_imgui() {
    VkDescriptorPoolSize pool_sizes[] =
            {
                    { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
            };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VkDescriptorPool imguiPool;
    VK_CHECK(vkCreateDescriptorPool(_device, &pool_info, nullptr, &imguiPool));

    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = NULL;

    ImGui_ImplSDL2_InitForVulkan(_window);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = _instance;
    init_info.PhysicalDevice = _chosenGPU;
    init_info.Device = _device;
    init_info.Queue = _graphicsQueue;
    init_info.DescriptorPool = imguiPool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;

    ImGui_ImplVulkan_Init(&init_info, _renderPass);

    immediate_submit([&](VkCommandBuffer cmd) {
       ImGui_ImplVulkan_CreateFontsTexture(cmd);
    });

    ImGui_ImplVulkan_DestroyFontUploadObjects();

    _mainDeletionQueue.push_function([=]() {
       vkDestroyDescriptorPool(_device, imguiPool, nullptr);
       ImGui_ImplVulkan_Shutdown();
    });
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



