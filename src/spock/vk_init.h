//
// Created by daholland on 3/23/21.
//

#ifndef GIBA_VK_INIT_H
#define GIBA_VK_INIT_H

#include <vk_types.h>

namespace vkinit {
    VkCommandPoolCreateInfo command_pool_create_info(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);
    VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool, uint32_t  count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY );
    VkFramebufferCreateInfo framebuffer_create_info(VkRenderPass render, VkExtent2D extent);
    VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule shaderModule);
    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info();
    VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info(VkPrimitiveTopology topology);
    VkPipelineRasterizationStateCreateInfo rasterization_state_create_info(VkPolygonMode polygonMode);
    VkPipelineMultisampleStateCreateInfo multisampling_state_create_info();
    VkPipelineColorBlendAttachmentState color_blend_attachment_state();
    VkPipelineLayoutCreateInfo pipeline_layout_create_info();
    VkImageCreateInfo image_create_info(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
    VkImageViewCreateInfo imageview_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);
    VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp);
    VkRenderPassBeginInfo renderpass_begin_info(VkRenderPass render, VkExtent2D extent, VkFramebuffer framebuffer);
}

#endif //GIBA_VK_INIT_H
