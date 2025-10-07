#include "header.hpp"

#include <iostream>
#include <chrono>
#include <sys/sdt.h>
#include "throttling_frame_joined.cpp"

static typeof(vkCreateInstance) *original_vkCreateInstance;
static typeof(vkDestroyInstance) *original_vkDestroyInstance;
static typeof(vkEnumeratePhysicalDevices) *original_vkEnumeratePhysicalDevices;
static typeof(vkGetPhysicalDeviceProperties) *original_vkGetPhysicalDeviceProperties;
static typeof(vkGetPhysicalDeviceQueueFamilyProperties) *original_vkGetPhysicalDeviceQueueFamilyProperties;
static typeof(vkGetPhysicalDeviceMemoryProperties) *original_vkGetPhysicalDeviceMemoryProperties;
static typeof(vkGetPhysicalDeviceFeatures) *original_vkGetPhysicalDeviceFeatures;
static typeof(vkGetPhysicalDeviceFormatProperties) *original_vkGetPhysicalDeviceFormatProperties;
static typeof(vkGetPhysicalDeviceImageFormatProperties) *original_vkGetPhysicalDeviceImageFormatProperties;
static typeof(vkCreateDevice) *original_vkCreateDevice;
static typeof(vkDestroyDevice) *original_vkDestroyDevice;
static typeof(vkEnumerateInstanceVersion) *original_vkEnumerateInstanceVersion;
static typeof(vkEnumerateInstanceLayerProperties) *original_vkEnumerateInstanceLayerProperties;
static typeof(vkEnumerateInstanceExtensionProperties) *original_vkEnumerateInstanceExtensionProperties;
static typeof(vkEnumerateDeviceLayerProperties) *original_vkEnumerateDeviceLayerProperties;
static typeof(vkEnumerateDeviceExtensionProperties) *original_vkEnumerateDeviceExtensionProperties;
static typeof(vkGetDeviceQueue) *original_vkGetDeviceQueue;
static typeof(vkQueueSubmit) *original_vkQueueSubmit;
static typeof(vkQueueWaitIdle) *original_vkQueueWaitIdle;
static typeof(vkDeviceWaitIdle) *original_vkDeviceWaitIdle;
static typeof(vkAllocateMemory) *original_vkAllocateMemory;
static typeof(vkFreeMemory) *original_vkFreeMemory;
static typeof(vkMapMemory) *original_vkMapMemory;
static typeof(vkUnmapMemory) *original_vkUnmapMemory;
static typeof(vkFlushMappedMemoryRanges) *original_vkFlushMappedMemoryRanges;
static typeof(vkInvalidateMappedMemoryRanges) *original_vkInvalidateMappedMemoryRanges;
static typeof(vkGetDeviceMemoryCommitment) *original_vkGetDeviceMemoryCommitment;
static typeof(vkGetBufferMemoryRequirements) *original_vkGetBufferMemoryRequirements;
static typeof(vkBindBufferMemory) *original_vkBindBufferMemory;
static typeof(vkGetImageMemoryRequirements) *original_vkGetImageMemoryRequirements;
static typeof(vkBindImageMemory) *original_vkBindImageMemory;
static typeof(vkGetImageSparseMemoryRequirements) *original_vkGetImageSparseMemoryRequirements;
static typeof(vkGetPhysicalDeviceSparseImageFormatProperties) *original_vkGetPhysicalDeviceSparseImageFormatProperties;
static typeof(vkQueueBindSparse) *original_vkQueueBindSparse;
static typeof(vkCreateFence) *original_vkCreateFence;
static typeof(vkDestroyFence) *original_vkDestroyFence;
static typeof(vkResetFences) *original_vkResetFences;
static typeof(vkGetFenceStatus) *original_vkGetFenceStatus;
static typeof(vkWaitForFences) *original_vkWaitForFences;
static typeof(vkCreateSemaphore) *original_vkCreateSemaphore;
static typeof(vkDestroySemaphore) *original_vkDestroySemaphore;
static typeof(vkCreateEvent) *original_vkCreateEvent;
static typeof(vkDestroyEvent) *original_vkDestroyEvent;
static typeof(vkGetEventStatus) *original_vkGetEventStatus;
static typeof(vkSetEvent) *original_vkSetEvent;
static typeof(vkResetEvent) *original_vkResetEvent;
static typeof(vkCreateQueryPool) *original_vkCreateQueryPool;
static typeof(vkDestroyQueryPool) *original_vkDestroyQueryPool;
static typeof(vkGetQueryPoolResults) *original_vkGetQueryPoolResults;
static typeof(vkCreateBuffer) *original_vkCreateBuffer;
static typeof(vkDestroyBuffer) *original_vkDestroyBuffer;
static typeof(vkCreateBufferView) *original_vkCreateBufferView;
static typeof(vkDestroyBufferView) *original_vkDestroyBufferView;
static typeof(vkCreateImage) *original_vkCreateImage;
static typeof(vkDestroyImage) *original_vkDestroyImage;
static typeof(vkGetImageSubresourceLayout) *original_vkGetImageSubresourceLayout;
static typeof(vkCreateImageView) *original_vkCreateImageView;
static typeof(vkDestroyImageView) *original_vkDestroyImageView;
static typeof(vkCreateShaderModule) *original_vkCreateShaderModule;
static typeof(vkDestroyShaderModule) *original_vkDestroyShaderModule;
static typeof(vkCreatePipelineCache) *original_vkCreatePipelineCache;
static typeof(vkDestroyPipelineCache) *original_vkDestroyPipelineCache;
static typeof(vkGetPipelineCacheData) *original_vkGetPipelineCacheData;
static typeof(vkMergePipelineCaches) *original_vkMergePipelineCaches;
static typeof(vkCreateGraphicsPipelines) *original_vkCreateGraphicsPipelines;
static typeof(vkCreateComputePipelines) *original_vkCreateComputePipelines;
static typeof(vkDestroyPipeline) *original_vkDestroyPipeline;
static typeof(vkCreatePipelineLayout) *original_vkCreatePipelineLayout;
static typeof(vkDestroyPipelineLayout) *original_vkDestroyPipelineLayout;
static typeof(vkCreateSampler) *original_vkCreateSampler;
static typeof(vkDestroySampler) *original_vkDestroySampler;
static typeof(vkCreateDescriptorSetLayout) *original_vkCreateDescriptorSetLayout;
static typeof(vkDestroyDescriptorSetLayout) *original_vkDestroyDescriptorSetLayout;
static typeof(vkCreateDescriptorPool) *original_vkCreateDescriptorPool;
static typeof(vkDestroyDescriptorPool) *original_vkDestroyDescriptorPool;
static typeof(vkResetDescriptorPool) *original_vkResetDescriptorPool;
static typeof(vkAllocateDescriptorSets) *original_vkAllocateDescriptorSets;
static typeof(vkFreeDescriptorSets) *original_vkFreeDescriptorSets;
static typeof(vkUpdateDescriptorSets) *original_vkUpdateDescriptorSets;
static typeof(vkCreateFramebuffer) *original_vkCreateFramebuffer;
static typeof(vkDestroyFramebuffer) *original_vkDestroyFramebuffer;
static typeof(vkCreateRenderPass) *original_vkCreateRenderPass;
static typeof(vkDestroyRenderPass) *original_vkDestroyRenderPass;
static typeof(vkGetRenderAreaGranularity) *original_vkGetRenderAreaGranularity;
static typeof(vkCreateCommandPool) *original_vkCreateCommandPool;
static typeof(vkDestroyCommandPool) *original_vkDestroyCommandPool;
static typeof(vkResetCommandPool) *original_vkResetCommandPool;
static typeof(vkAllocateCommandBuffers) *original_vkAllocateCommandBuffers;
static typeof(vkFreeCommandBuffers) *original_vkFreeCommandBuffers;
static typeof(vkBeginCommandBuffer) *original_vkBeginCommandBuffer;
static typeof(vkEndCommandBuffer) *original_vkEndCommandBuffer;
static typeof(vkResetCommandBuffer) *original_vkResetCommandBuffer;
static typeof(vkCmdBindPipeline) *original_vkCmdBindPipeline;
static typeof(vkCmdSetViewport) *original_vkCmdSetViewport;
static typeof(vkCmdSetScissor) *original_vkCmdSetScissor;
static typeof(vkCmdSetLineWidth) *original_vkCmdSetLineWidth;
static typeof(vkCmdSetDepthBias) *original_vkCmdSetDepthBias;
static typeof(vkCmdSetBlendConstants) *original_vkCmdSetBlendConstants;
static typeof(vkCmdSetDepthBounds) *original_vkCmdSetDepthBounds;
static typeof(vkCmdSetStencilCompareMask) *original_vkCmdSetStencilCompareMask;
static typeof(vkCmdSetStencilWriteMask) *original_vkCmdSetStencilWriteMask;
static typeof(vkCmdSetStencilReference) *original_vkCmdSetStencilReference;
static typeof(vkCmdBindDescriptorSets) *original_vkCmdBindDescriptorSets;
static typeof(vkCmdBindIndexBuffer) *original_vkCmdBindIndexBuffer;
static typeof(vkCmdBindVertexBuffers) *original_vkCmdBindVertexBuffers;
static typeof(vkCmdDraw) *original_vkCmdDraw;
static typeof(vkCmdDrawIndexed) *original_vkCmdDrawIndexed;
static typeof(vkCmdDrawIndirect) *original_vkCmdDrawIndirect;
static typeof(vkCmdDrawIndexedIndirect) *original_vkCmdDrawIndexedIndirect;
static typeof(vkCmdDispatch) *original_vkCmdDispatch;
static typeof(vkCmdDispatchIndirect) *original_vkCmdDispatchIndirect;
static typeof(vkCmdCopyBuffer) *original_vkCmdCopyBuffer;
static typeof(vkCmdCopyImage) *original_vkCmdCopyImage;
static typeof(vkCmdBlitImage) *original_vkCmdBlitImage;
static typeof(vkCmdCopyBufferToImage) *original_vkCmdCopyBufferToImage;
static typeof(vkCmdCopyImageToBuffer) *original_vkCmdCopyImageToBuffer;
static typeof(vkCmdUpdateBuffer) *original_vkCmdUpdateBuffer;
static typeof(vkCmdFillBuffer) *original_vkCmdFillBuffer;
static typeof(vkCmdClearColorImage) *original_vkCmdClearColorImage;
static typeof(vkCmdClearDepthStencilImage) *original_vkCmdClearDepthStencilImage;
static typeof(vkCmdClearAttachments) *original_vkCmdClearAttachments;
static typeof(vkCmdResolveImage) *original_vkCmdResolveImage;
static typeof(vkCmdSetEvent) *original_vkCmdSetEvent;
static typeof(vkCmdResetEvent) *original_vkCmdResetEvent;
static typeof(vkCmdWaitEvents) *original_vkCmdWaitEvents;
static typeof(vkCmdPipelineBarrier) *original_vkCmdPipelineBarrier;
static typeof(vkCmdBeginQuery) *original_vkCmdBeginQuery;
static typeof(vkCmdEndQuery) *original_vkCmdEndQuery;
static typeof(vkCmdResetQueryPool) *original_vkCmdResetQueryPool;
static typeof(vkCmdWriteTimestamp) *original_vkCmdWriteTimestamp;
static typeof(vkCmdCopyQueryPoolResults) *original_vkCmdCopyQueryPoolResults;
static typeof(vkCmdPushConstants) *original_vkCmdPushConstants;
static typeof(vkCmdBeginRenderPass) *original_vkCmdBeginRenderPass;
static typeof(vkCmdNextSubpass) *original_vkCmdNextSubpass;
static typeof(vkCmdEndRenderPass) *original_vkCmdEndRenderPass;
static typeof(vkCmdExecuteCommands) *original_vkCmdExecuteCommands;
static typeof(vkQueuePresentKHR) *original_vkQueuePresentKHR;
static typeof(vkGetDeviceQueue2) *original_vkGetDeviceQueue2;

PRIVATE INITIALIZER
void load_original_functions()
{
    auto flag = RTLD_NEXT;
    original_vkCreateInstance = reinterpret_cast<decltype(original_vkCreateInstance)>(original_dlsym(flag, "vkCreateInstance"));
    original_vkDestroyInstance = reinterpret_cast<decltype(original_vkDestroyInstance)>(original_dlsym(flag, "vkDestroyInstance"));
    original_vkEnumeratePhysicalDevices = reinterpret_cast<decltype(original_vkEnumeratePhysicalDevices)>(original_dlsym(flag, "vkEnumeratePhysicalDevices"));
    original_vkGetPhysicalDeviceProperties = reinterpret_cast<decltype(original_vkGetPhysicalDeviceProperties)>(original_dlsym(flag, "vkGetPhysicalDeviceProperties"));
    original_vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<decltype(original_vkGetPhysicalDeviceQueueFamilyProperties)>(original_dlsym(flag, "vkGetPhysicalDeviceQueueFamilyProperties"));
    original_vkGetPhysicalDeviceMemoryProperties = reinterpret_cast<decltype(original_vkGetPhysicalDeviceMemoryProperties)>(original_dlsym(flag, "vkGetPhysicalDeviceMemoryProperties"));
    original_vkGetPhysicalDeviceFeatures = reinterpret_cast<decltype(original_vkGetPhysicalDeviceFeatures)>(original_dlsym(flag, "vkGetPhysicalDeviceFeatures"));
    original_vkGetPhysicalDeviceFormatProperties = reinterpret_cast<decltype(original_vkGetPhysicalDeviceFormatProperties)>(original_dlsym(flag, "vkGetPhysicalDeviceFormatProperties"));
    original_vkGetPhysicalDeviceImageFormatProperties = reinterpret_cast<decltype(original_vkGetPhysicalDeviceImageFormatProperties)>(original_dlsym(flag, "vkGetPhysicalDeviceImageFormatProperties"));
    original_vkCreateDevice = reinterpret_cast<decltype(original_vkCreateDevice)>(original_dlsym(flag, "vkCreateDevice"));
    original_vkDestroyDevice = reinterpret_cast<decltype(original_vkDestroyDevice)>(original_dlsym(flag, "vkDestroyDevice"));
    original_vkEnumerateInstanceVersion = reinterpret_cast<decltype(original_vkEnumerateInstanceVersion)>(original_dlsym(flag, "vkEnumerateInstanceVersion"));
    original_vkEnumerateInstanceLayerProperties = reinterpret_cast<decltype(original_vkEnumerateInstanceLayerProperties)>(original_dlsym(flag, "vkEnumerateInstanceLayerProperties"));
    original_vkEnumerateInstanceExtensionProperties = reinterpret_cast<decltype(original_vkEnumerateInstanceExtensionProperties)>(original_dlsym(flag, "vkEnumerateInstanceExtensionProperties"));
    original_vkEnumerateDeviceLayerProperties = reinterpret_cast<decltype(original_vkEnumerateDeviceLayerProperties)>(original_dlsym(flag, "vkEnumerateDeviceLayerProperties"));
    original_vkEnumerateDeviceExtensionProperties = reinterpret_cast<decltype(original_vkEnumerateDeviceExtensionProperties)>(original_dlsym(flag, "vkEnumerateDeviceExtensionProperties"));
    original_vkGetDeviceQueue = reinterpret_cast<decltype(original_vkGetDeviceQueue)>(original_dlsym(flag, "vkGetDeviceQueue"));
    original_vkQueueSubmit = reinterpret_cast<decltype(original_vkQueueSubmit)>(original_dlsym(flag, "vkQueueSubmit"));
    original_vkQueueWaitIdle = reinterpret_cast<decltype(original_vkQueueWaitIdle)>(original_dlsym(flag, "vkQueueWaitIdle"));
    original_vkDeviceWaitIdle = reinterpret_cast<decltype(original_vkDeviceWaitIdle)>(original_dlsym(flag, "vkDeviceWaitIdle"));
    original_vkAllocateMemory = reinterpret_cast<decltype(original_vkAllocateMemory)>(original_dlsym(flag, "vkAllocateMemory"));
    original_vkFreeMemory = reinterpret_cast<decltype(original_vkFreeMemory)>(original_dlsym(flag, "vkFreeMemory"));
    original_vkMapMemory = reinterpret_cast<decltype(original_vkMapMemory)>(original_dlsym(flag, "vkMapMemory"));
    original_vkUnmapMemory = reinterpret_cast<decltype(original_vkUnmapMemory)>(original_dlsym(flag, "vkUnmapMemory"));
    original_vkFlushMappedMemoryRanges = reinterpret_cast<decltype(original_vkFlushMappedMemoryRanges)>(original_dlsym(flag, "vkFlushMappedMemoryRanges"));
    original_vkInvalidateMappedMemoryRanges = reinterpret_cast<decltype(original_vkInvalidateMappedMemoryRanges)>(original_dlsym(flag, "vkInvalidateMappedMemoryRanges"));
    original_vkGetDeviceMemoryCommitment = reinterpret_cast<decltype(original_vkGetDeviceMemoryCommitment)>(original_dlsym(flag, "vkGetDeviceMemoryCommitment"));
    original_vkGetBufferMemoryRequirements = reinterpret_cast<decltype(original_vkGetBufferMemoryRequirements)>(original_dlsym(flag, "vkGetBufferMemoryRequirements"));
    original_vkBindBufferMemory = reinterpret_cast<decltype(original_vkBindBufferMemory)>(original_dlsym(flag, "vkBindBufferMemory"));
    original_vkGetImageMemoryRequirements = reinterpret_cast<decltype(original_vkGetImageMemoryRequirements)>(original_dlsym(flag, "vkGetImageMemoryRequirements"));
    original_vkBindImageMemory = reinterpret_cast<decltype(original_vkBindImageMemory)>(original_dlsym(flag, "vkBindImageMemory"));
    original_vkGetImageSparseMemoryRequirements = reinterpret_cast<decltype(original_vkGetImageSparseMemoryRequirements)>(original_dlsym(flag, "vkGetImageSparseMemoryRequirements"));
    original_vkGetPhysicalDeviceSparseImageFormatProperties = reinterpret_cast<decltype(original_vkGetPhysicalDeviceSparseImageFormatProperties)>(original_dlsym(flag, "vkGetPhysicalDeviceSparseImageFormatProperties"));
    original_vkQueueBindSparse = reinterpret_cast<decltype(original_vkQueueBindSparse)>(original_dlsym(flag, "vkQueueBindSparse"));
    original_vkCreateFence = reinterpret_cast<decltype(original_vkCreateFence)>(original_dlsym(flag, "vkCreateFence"));
    original_vkDestroyFence = reinterpret_cast<decltype(original_vkDestroyFence)>(original_dlsym(flag, "vkDestroyFence"));
    original_vkResetFences = reinterpret_cast<decltype(original_vkResetFences)>(original_dlsym(flag, "vkResetFences"));
    original_vkGetFenceStatus = reinterpret_cast<decltype(original_vkGetFenceStatus)>(original_dlsym(flag, "vkGetFenceStatus"));
    original_vkWaitForFences = reinterpret_cast<decltype(original_vkWaitForFences)>(original_dlsym(flag, "vkWaitForFences"));
    original_vkCreateSemaphore = reinterpret_cast<decltype(original_vkCreateSemaphore)>(original_dlsym(flag, "vkCreateSemaphore"));
    original_vkDestroySemaphore = reinterpret_cast<decltype(original_vkDestroySemaphore)>(original_dlsym(flag, "vkDestroySemaphore"));
    original_vkCreateEvent = reinterpret_cast<decltype(original_vkCreateEvent)>(original_dlsym(flag, "vkCreateEvent"));
    original_vkDestroyEvent = reinterpret_cast<decltype(original_vkDestroyEvent)>(original_dlsym(flag, "vkDestroyEvent"));
    original_vkGetEventStatus = reinterpret_cast<decltype(original_vkGetEventStatus)>(original_dlsym(flag, "vkGetEventStatus"));
    original_vkSetEvent = reinterpret_cast<decltype(original_vkSetEvent)>(original_dlsym(flag, "vkSetEvent"));
    original_vkResetEvent = reinterpret_cast<decltype(original_vkResetEvent)>(original_dlsym(flag, "vkResetEvent"));
    original_vkCreateQueryPool = reinterpret_cast<decltype(original_vkCreateQueryPool)>(original_dlsym(flag, "vkCreateQueryPool"));
    original_vkDestroyQueryPool = reinterpret_cast<decltype(original_vkDestroyQueryPool)>(original_dlsym(flag, "vkDestroyQueryPool"));
    original_vkGetQueryPoolResults = reinterpret_cast<decltype(original_vkGetQueryPoolResults)>(original_dlsym(flag, "vkGetQueryPoolResults"));
    original_vkCreateBuffer = reinterpret_cast<decltype(original_vkCreateBuffer)>(original_dlsym(flag, "vkCreateBuffer"));
    original_vkDestroyBuffer = reinterpret_cast<decltype(original_vkDestroyBuffer)>(original_dlsym(flag, "vkDestroyBuffer"));
    original_vkCreateBufferView = reinterpret_cast<decltype(original_vkCreateBufferView)>(original_dlsym(flag, "vkCreateBufferView"));
    original_vkDestroyBufferView = reinterpret_cast<decltype(original_vkDestroyBufferView)>(original_dlsym(flag, "vkDestroyBufferView"));
    original_vkCreateImage = reinterpret_cast<decltype(original_vkCreateImage)>(original_dlsym(flag, "vkCreateImage"));
    original_vkDestroyImage = reinterpret_cast<decltype(original_vkDestroyImage)>(original_dlsym(flag, "vkDestroyImage"));
    original_vkGetImageSubresourceLayout = reinterpret_cast<decltype(original_vkGetImageSubresourceLayout)>(original_dlsym(flag, "vkGetImageSubresourceLayout"));
    original_vkCreateImageView = reinterpret_cast<decltype(original_vkCreateImageView)>(original_dlsym(flag, "vkCreateImageView"));
    original_vkDestroyImageView = reinterpret_cast<decltype(original_vkDestroyImageView)>(original_dlsym(flag, "vkDestroyImageView"));
    original_vkCreateShaderModule = reinterpret_cast<decltype(original_vkCreateShaderModule)>(original_dlsym(flag, "vkCreateShaderModule"));
    original_vkDestroyShaderModule = reinterpret_cast<decltype(original_vkDestroyShaderModule)>(original_dlsym(flag, "vkDestroyShaderModule"));
    original_vkCreatePipelineCache = reinterpret_cast<decltype(original_vkCreatePipelineCache)>(original_dlsym(flag, "vkCreatePipelineCache"));
    original_vkDestroyPipelineCache = reinterpret_cast<decltype(original_vkDestroyPipelineCache)>(original_dlsym(flag, "vkDestroyPipelineCache"));
    original_vkGetPipelineCacheData = reinterpret_cast<decltype(original_vkGetPipelineCacheData)>(original_dlsym(flag, "vkGetPipelineCacheData"));
    original_vkMergePipelineCaches = reinterpret_cast<decltype(original_vkMergePipelineCaches)>(original_dlsym(flag, "vkMergePipelineCaches"));
    original_vkCreateGraphicsPipelines = reinterpret_cast<decltype(original_vkCreateGraphicsPipelines)>(original_dlsym(flag, "vkCreateGraphicsPipelines"));
    original_vkCreateComputePipelines = reinterpret_cast<decltype(original_vkCreateComputePipelines)>(original_dlsym(flag, "vkCreateComputePipelines"));
    original_vkDestroyPipeline = reinterpret_cast<decltype(original_vkDestroyPipeline)>(original_dlsym(flag, "vkDestroyPipeline"));
    original_vkCreatePipelineLayout = reinterpret_cast<decltype(original_vkCreatePipelineLayout)>(original_dlsym(flag, "vkCreatePipelineLayout"));
    original_vkDestroyPipelineLayout = reinterpret_cast<decltype(original_vkDestroyPipelineLayout)>(original_dlsym(flag, "vkDestroyPipelineLayout"));
    original_vkCreateSampler = reinterpret_cast<decltype(original_vkCreateSampler)>(original_dlsym(flag, "vkCreateSampler"));
    original_vkDestroySampler = reinterpret_cast<decltype(original_vkDestroySampler)>(original_dlsym(flag, "vkDestroySampler"));
    original_vkCreateDescriptorSetLayout = reinterpret_cast<decltype(original_vkCreateDescriptorSetLayout)>(original_dlsym(flag, "vkCreateDescriptorSetLayout"));
    original_vkDestroyDescriptorSetLayout = reinterpret_cast<decltype(original_vkDestroyDescriptorSetLayout)>(original_dlsym(flag, "vkDestroyDescriptorSetLayout"));
    original_vkCreateDescriptorPool = reinterpret_cast<decltype(original_vkCreateDescriptorPool)>(original_dlsym(flag, "vkCreateDescriptorPool"));
    original_vkDestroyDescriptorPool = reinterpret_cast<decltype(original_vkDestroyDescriptorPool)>(original_dlsym(flag, "vkDestroyDescriptorPool"));
    original_vkResetDescriptorPool = reinterpret_cast<decltype(original_vkResetDescriptorPool)>(original_dlsym(flag, "vkResetDescriptorPool"));
    original_vkAllocateDescriptorSets = reinterpret_cast<decltype(original_vkAllocateDescriptorSets)>(original_dlsym(flag, "vkAllocateDescriptorSets"));
    original_vkFreeDescriptorSets = reinterpret_cast<decltype(original_vkFreeDescriptorSets)>(original_dlsym(flag, "vkFreeDescriptorSets"));
    original_vkUpdateDescriptorSets = reinterpret_cast<decltype(original_vkUpdateDescriptorSets)>(original_dlsym(flag, "vkUpdateDescriptorSets"));
    original_vkCreateFramebuffer = reinterpret_cast<decltype(original_vkCreateFramebuffer)>(original_dlsym(flag, "vkCreateFramebuffer"));
    original_vkDestroyFramebuffer = reinterpret_cast<decltype(original_vkDestroyFramebuffer)>(original_dlsym(flag, "vkDestroyFramebuffer"));
    original_vkCreateRenderPass = reinterpret_cast<decltype(original_vkCreateRenderPass)>(original_dlsym(flag, "vkCreateRenderPass"));
    original_vkDestroyRenderPass = reinterpret_cast<decltype(original_vkDestroyRenderPass)>(original_dlsym(flag, "vkDestroyRenderPass"));
    original_vkGetRenderAreaGranularity = reinterpret_cast<decltype(original_vkGetRenderAreaGranularity)>(original_dlsym(flag, "vkGetRenderAreaGranularity"));
    original_vkCreateCommandPool = reinterpret_cast<decltype(original_vkCreateCommandPool)>(original_dlsym(flag, "vkCreateCommandPool"));
    original_vkDestroyCommandPool = reinterpret_cast<decltype(original_vkDestroyCommandPool)>(original_dlsym(flag, "vkDestroyCommandPool"));
    original_vkResetCommandPool = reinterpret_cast<decltype(original_vkResetCommandPool)>(original_dlsym(flag, "vkResetCommandPool"));
    original_vkAllocateCommandBuffers = reinterpret_cast<decltype(original_vkAllocateCommandBuffers)>(original_dlsym(flag, "vkAllocateCommandBuffers"));
    original_vkFreeCommandBuffers = reinterpret_cast<decltype(original_vkFreeCommandBuffers)>(original_dlsym(flag, "vkFreeCommandBuffers"));
    original_vkBeginCommandBuffer = reinterpret_cast<decltype(original_vkBeginCommandBuffer)>(original_dlsym(flag, "vkBeginCommandBuffer"));
    original_vkEndCommandBuffer = reinterpret_cast<decltype(original_vkEndCommandBuffer)>(original_dlsym(flag, "vkEndCommandBuffer"));
    original_vkResetCommandBuffer = reinterpret_cast<decltype(original_vkResetCommandBuffer)>(original_dlsym(flag, "vkResetCommandBuffer"));
    original_vkCmdBindPipeline = reinterpret_cast<decltype(original_vkCmdBindPipeline)>(original_dlsym(flag, "vkCmdBindPipeline"));
    original_vkCmdSetViewport = reinterpret_cast<decltype(original_vkCmdSetViewport)>(original_dlsym(flag, "vkCmdSetViewport"));
    original_vkCmdSetScissor = reinterpret_cast<decltype(original_vkCmdSetScissor)>(original_dlsym(flag, "vkCmdSetScissor"));
    original_vkCmdSetLineWidth = reinterpret_cast<decltype(original_vkCmdSetLineWidth)>(original_dlsym(flag, "vkCmdSetLineWidth"));
    original_vkCmdSetDepthBias = reinterpret_cast<decltype(original_vkCmdSetDepthBias)>(original_dlsym(flag, "vkCmdSetDepthBias"));
    original_vkCmdSetBlendConstants = reinterpret_cast<decltype(original_vkCmdSetBlendConstants)>(original_dlsym(flag, "vkCmdSetBlendConstants"));
    original_vkCmdSetDepthBounds = reinterpret_cast<decltype(original_vkCmdSetDepthBounds)>(original_dlsym(flag, "vkCmdSetDepthBounds"));
    original_vkCmdSetStencilCompareMask = reinterpret_cast<decltype(original_vkCmdSetStencilCompareMask)>(original_dlsym(flag, "vkCmdSetStencilCompareMask"));
    original_vkCmdSetStencilWriteMask = reinterpret_cast<decltype(original_vkCmdSetStencilWriteMask)>(original_dlsym(flag, "vkCmdSetStencilWriteMask"));
    original_vkCmdSetStencilReference = reinterpret_cast<decltype(original_vkCmdSetStencilReference)>(original_dlsym(flag, "vkCmdSetStencilReference"));
    original_vkCmdBindDescriptorSets = reinterpret_cast<decltype(original_vkCmdBindDescriptorSets)>(original_dlsym(flag, "vkCmdBindDescriptorSets"));
    original_vkCmdBindIndexBuffer = reinterpret_cast<decltype(original_vkCmdBindIndexBuffer)>(original_dlsym(flag, "vkCmdBindIndexBuffer"));
    original_vkCmdBindVertexBuffers = reinterpret_cast<decltype(original_vkCmdBindVertexBuffers)>(original_dlsym(flag, "vkCmdBindVertexBuffers"));
    original_vkCmdDraw = reinterpret_cast<decltype(original_vkCmdDraw)>(original_dlsym(flag, "vkCmdDraw"));
    original_vkCmdDrawIndexed = reinterpret_cast<decltype(original_vkCmdDrawIndexed)>(original_dlsym(flag, "vkCmdDrawIndexed"));
    original_vkCmdDrawIndirect = reinterpret_cast<decltype(original_vkCmdDrawIndirect)>(original_dlsym(flag, "vkCmdDrawIndirect"));
    original_vkCmdDrawIndexedIndirect = reinterpret_cast<decltype(original_vkCmdDrawIndexedIndirect)>(original_dlsym(flag, "vkCmdDrawIndexedIndirect"));
    original_vkCmdDispatch = reinterpret_cast<decltype(original_vkCmdDispatch)>(original_dlsym(flag, "vkCmdDispatch"));
    original_vkCmdDispatchIndirect = reinterpret_cast<decltype(original_vkCmdDispatchIndirect)>(original_dlsym(flag, "vkCmdDispatchIndirect"));
    original_vkCmdCopyBuffer = reinterpret_cast<decltype(original_vkCmdCopyBuffer)>(original_dlsym(flag, "vkCmdCopyBuffer"));
    original_vkCmdCopyImage = reinterpret_cast<decltype(original_vkCmdCopyImage)>(original_dlsym(flag, "vkCmdCopyImage"));
    original_vkCmdBlitImage = reinterpret_cast<decltype(original_vkCmdBlitImage)>(original_dlsym(flag, "vkCmdBlitImage"));
    original_vkCmdCopyBufferToImage = reinterpret_cast<decltype(original_vkCmdCopyBufferToImage)>(original_dlsym(flag, "vkCmdCopyBufferToImage"));
    original_vkCmdCopyImageToBuffer = reinterpret_cast<decltype(original_vkCmdCopyImageToBuffer)>(original_dlsym(flag, "vkCmdCopyImageToBuffer"));
    original_vkCmdUpdateBuffer = reinterpret_cast<decltype(original_vkCmdUpdateBuffer)>(original_dlsym(flag, "vkCmdUpdateBuffer"));
    original_vkCmdFillBuffer = reinterpret_cast<decltype(original_vkCmdFillBuffer)>(original_dlsym(flag, "vkCmdFillBuffer"));
    original_vkCmdClearColorImage = reinterpret_cast<decltype(original_vkCmdClearColorImage)>(original_dlsym(flag, "vkCmdClearColorImage"));
    original_vkCmdClearDepthStencilImage = reinterpret_cast<decltype(original_vkCmdClearDepthStencilImage)>(original_dlsym(flag, "vkCmdClearDepthStencilImage"));
    original_vkCmdClearAttachments = reinterpret_cast<decltype(original_vkCmdClearAttachments)>(original_dlsym(flag, "vkCmdClearAttachments"));
    original_vkCmdResolveImage = reinterpret_cast<decltype(original_vkCmdResolveImage)>(original_dlsym(flag, "vkCmdResolveImage"));
    original_vkCmdSetEvent = reinterpret_cast<decltype(original_vkCmdSetEvent)>(original_dlsym(flag, "vkCmdSetEvent"));
    original_vkCmdResetEvent = reinterpret_cast<decltype(original_vkCmdResetEvent)>(original_dlsym(flag, "vkCmdResetEvent"));
    original_vkCmdWaitEvents = reinterpret_cast<decltype(original_vkCmdWaitEvents)>(original_dlsym(flag, "vkCmdWaitEvents"));
    original_vkCmdPipelineBarrier = reinterpret_cast<decltype(original_vkCmdPipelineBarrier)>(original_dlsym(flag, "vkCmdPipelineBarrier"));
    original_vkCmdBeginQuery = reinterpret_cast<decltype(original_vkCmdBeginQuery)>(original_dlsym(flag, "vkCmdBeginQuery"));
    original_vkCmdEndQuery = reinterpret_cast<decltype(original_vkCmdEndQuery)>(original_dlsym(flag, "vkCmdEndQuery"));
    original_vkCmdResetQueryPool = reinterpret_cast<decltype(original_vkCmdResetQueryPool)>(original_dlsym(flag, "vkCmdResetQueryPool"));
    original_vkCmdWriteTimestamp = reinterpret_cast<decltype(original_vkCmdWriteTimestamp)>(original_dlsym(flag, "vkCmdWriteTimestamp"));
    original_vkCmdCopyQueryPoolResults = reinterpret_cast<decltype(original_vkCmdCopyQueryPoolResults)>(original_dlsym(flag, "vkCmdCopyQueryPoolResults"));
    original_vkCmdPushConstants = reinterpret_cast<decltype(original_vkCmdPushConstants)>(original_dlsym(flag, "vkCmdPushConstants"));
    original_vkCmdBeginRenderPass = reinterpret_cast<decltype(original_vkCmdBeginRenderPass)>(original_dlsym(flag, "vkCmdBeginRenderPass"));
    original_vkCmdNextSubpass = reinterpret_cast<decltype(original_vkCmdNextSubpass)>(original_dlsym(flag, "vkCmdNextSubpass"));
    original_vkCmdEndRenderPass = reinterpret_cast<decltype(original_vkCmdEndRenderPass)>(original_dlsym(flag, "vkCmdEndRenderPass"));
    original_vkCmdExecuteCommands = reinterpret_cast<decltype(original_vkCmdExecuteCommands)>(original_dlsym(flag, "vkCmdExecuteCommands"));
    original_vkQueuePresentKHR = reinterpret_cast<decltype(original_vkQueuePresentKHR)>(original_dlsym(flag, "vkQueuePresentKHR"));
    original_vkGetDeviceQueue2 = reinterpret_cast<decltype(original_vkGetDeviceQueue2)>(original_dlsym(flag, "vkGetDeviceQueue2"));
}

extern "C" PUBLIC
VkResult vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance) {
    DTRACE_PROBE(vkapi, vkCreateInstance_entry);
    auto _result = (*original_vkCreateInstance)(pCreateInfo, pAllocator, pInstance);
    DTRACE_PROBE(vkapi, vkCreateInstance_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyInstance_entry);
    (*original_vkDestroyInstance)(instance, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyInstance_exit);
}

extern "C" PUBLIC
VkResult vkEnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices) {
    DTRACE_PROBE(vkapi, vkEnumeratePhysicalDevices_entry);
    auto _result = (*original_vkEnumeratePhysicalDevices)(instance, pPhysicalDeviceCount, pPhysicalDevices);
    DTRACE_PROBE(vkapi, vkEnumeratePhysicalDevices_exit);
    return _result;
}

extern "C" PUBLIC
void vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties) {
    DTRACE_PROBE(vkapi, vkGetPhysicalDeviceProperties_entry);
    (*original_vkGetPhysicalDeviceProperties)(physicalDevice, pProperties);
    DTRACE_PROBE(vkapi, vkGetPhysicalDeviceProperties_exit);
}

extern "C" PUBLIC
void vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties) {
    DTRACE_PROBE(vkapi, vkGetPhysicalDeviceQueueFamilyProperties_entry);
    (*original_vkGetPhysicalDeviceQueueFamilyProperties)(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    DTRACE_PROBE(vkapi, vkGetPhysicalDeviceQueueFamilyProperties_exit);
}

extern "C" PUBLIC
void vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties) {
    DTRACE_PROBE(vkapi, vkGetPhysicalDeviceMemoryProperties_entry);
    (*original_vkGetPhysicalDeviceMemoryProperties)(physicalDevice, pMemoryProperties);
    DTRACE_PROBE(vkapi, vkGetPhysicalDeviceMemoryProperties_exit);
}

extern "C" PUBLIC
void vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures) {
    DTRACE_PROBE(vkapi, vkGetPhysicalDeviceFeatures_entry);
    (*original_vkGetPhysicalDeviceFeatures)(physicalDevice, pFeatures);
    DTRACE_PROBE(vkapi, vkGetPhysicalDeviceFeatures_exit);
}

extern "C" PUBLIC
void vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties* pFormatProperties) {
    DTRACE_PROBE(vkapi, vkGetPhysicalDeviceFormatProperties_entry);
    (*original_vkGetPhysicalDeviceFormatProperties)(physicalDevice, format, pFormatProperties);
    DTRACE_PROBE(vkapi, vkGetPhysicalDeviceFormatProperties_exit);
}

extern "C" PUBLIC
VkResult vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties) {
    DTRACE_PROBE(vkapi, vkGetPhysicalDeviceImageFormatProperties_entry);
    auto _result = (*original_vkGetPhysicalDeviceImageFormatProperties)(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
    DTRACE_PROBE(vkapi, vkGetPhysicalDeviceImageFormatProperties_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice) {
    DTRACE_PROBE(vkapi, vkCreateDevice_entry);
    auto _result = (*original_vkCreateDevice)(physicalDevice, pCreateInfo, pAllocator, pDevice);
    DTRACE_PROBE(vkapi, vkCreateDevice_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyDevice_entry);
    (*original_vkDestroyDevice)(device, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyDevice_exit);
}

extern "C" PUBLIC
VkResult vkEnumerateInstanceVersion(uint32_t* pApiVersion) {
    DTRACE_PROBE(vkapi, vkEnumerateInstanceVersion_entry);
    auto _result = (*original_vkEnumerateInstanceVersion)(pApiVersion);
    DTRACE_PROBE(vkapi, vkEnumerateInstanceVersion_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties) {
    DTRACE_PROBE(vkapi, vkEnumerateInstanceLayerProperties_entry);
    auto _result = (*original_vkEnumerateInstanceLayerProperties)(pPropertyCount, pProperties);
    DTRACE_PROBE(vkapi, vkEnumerateInstanceLayerProperties_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkEnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties) {
    DTRACE_PROBE(vkapi, vkEnumerateInstanceExtensionProperties_entry);
    auto _result = (*original_vkEnumerateInstanceExtensionProperties)(pLayerName, pPropertyCount, pProperties);
    DTRACE_PROBE(vkapi, vkEnumerateInstanceExtensionProperties_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkLayerProperties* pProperties) {
    DTRACE_PROBE(vkapi, vkEnumerateDeviceLayerProperties_entry);
    auto _result = (*original_vkEnumerateDeviceLayerProperties)(physicalDevice, pPropertyCount, pProperties);
    DTRACE_PROBE(vkapi, vkEnumerateDeviceLayerProperties_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties) {
    DTRACE_PROBE(vkapi, vkEnumerateDeviceExtensionProperties_entry);
    auto _result = (*original_vkEnumerateDeviceExtensionProperties)(physicalDevice, pLayerName, pPropertyCount, pProperties);
    DTRACE_PROBE(vkapi, vkEnumerateDeviceExtensionProperties_exit);
    return _result;
}

extern "C" PUBLIC
void vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue) {
    DTRACE_PROBE(vkapi, vkGetDeviceQueue_entry);
    (*original_vkGetDeviceQueue)(device, queueFamilyIndex, queueIndex, pQueue);
    DTRACE_PROBE(vkapi, vkGetDeviceQueue_exit);
}

extern "C" PUBLIC
VkResult vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) {
    DTRACE_PROBE(vkapi, vkQueueSubmit_entry);
    auto _result = (*original_vkQueueSubmit)(queue, submitCount, pSubmits, fence);
    DTRACE_PROBE(vkapi, vkQueueSubmit_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkQueueWaitIdle(VkQueue queue) {
    DTRACE_PROBE(vkapi, vkQueueWaitIdle_entry);
    auto _result = (*original_vkQueueWaitIdle)(queue);
    DTRACE_PROBE(vkapi, vkQueueWaitIdle_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkDeviceWaitIdle(VkDevice device) {
    DTRACE_PROBE(vkapi, vkDeviceWaitIdle_entry);
    auto _result = (*original_vkDeviceWaitIdle)(device);
    DTRACE_PROBE(vkapi, vkDeviceWaitIdle_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkAllocateMemory(VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory) {
    DTRACE_PROBE(vkapi, vkAllocateMemory_entry);
    auto _result = (*original_vkAllocateMemory)(device, pAllocateInfo, pAllocator, pMemory);
    DTRACE_PROBE(vkapi, vkAllocateMemory_exit);
    return _result;
}

extern "C" PUBLIC
void vkFreeMemory(VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkFreeMemory_entry);
    (*original_vkFreeMemory)(device, memory, pAllocator);
    DTRACE_PROBE(vkapi, vkFreeMemory_exit);
}

extern "C" PUBLIC
VkResult vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData) {
    DTRACE_PROBE(vkapi, vkMapMemory_entry);
    auto _result = (*original_vkMapMemory)(device, memory, offset, size, flags, ppData);
    DTRACE_PROBE(vkapi, vkMapMemory_exit);
    return _result;
}

extern "C" PUBLIC
void vkUnmapMemory(VkDevice device, VkDeviceMemory memory) {
    DTRACE_PROBE(vkapi, vkUnmapMemory_entry);
    (*original_vkUnmapMemory)(device, memory);
    DTRACE_PROBE(vkapi, vkUnmapMemory_exit);
}

extern "C" PUBLIC
VkResult vkFlushMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges) {
    DTRACE_PROBE(vkapi, vkFlushMappedMemoryRanges_entry);
    auto _result = (*original_vkFlushMappedMemoryRanges)(device, memoryRangeCount, pMemoryRanges);
    DTRACE_PROBE(vkapi, vkFlushMappedMemoryRanges_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkInvalidateMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges) {
    DTRACE_PROBE(vkapi, vkInvalidateMappedMemoryRanges_entry);
    auto _result = (*original_vkInvalidateMappedMemoryRanges)(device, memoryRangeCount, pMemoryRanges);
    DTRACE_PROBE(vkapi, vkInvalidateMappedMemoryRanges_exit);
    return _result;
}

extern "C" PUBLIC
void vkGetDeviceMemoryCommitment(VkDevice device, VkDeviceMemory memory, VkDeviceSize* pCommittedMemoryInBytes) {
    DTRACE_PROBE(vkapi, vkGetDeviceMemoryCommitment_entry);
    (*original_vkGetDeviceMemoryCommitment)(device, memory, pCommittedMemoryInBytes);
    DTRACE_PROBE(vkapi, vkGetDeviceMemoryCommitment_exit);
}

extern "C" PUBLIC
void vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer, VkMemoryRequirements* pMemoryRequirements) {
    DTRACE_PROBE(vkapi, vkGetBufferMemoryRequirements_entry);
    (*original_vkGetBufferMemoryRequirements)(device, buffer, pMemoryRequirements);
    DTRACE_PROBE(vkapi, vkGetBufferMemoryRequirements_exit);
}

extern "C" PUBLIC
VkResult vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset) {
    DTRACE_PROBE(vkapi, vkBindBufferMemory_entry);
    auto _result = (*original_vkBindBufferMemory)(device, buffer, memory, memoryOffset);
    DTRACE_PROBE(vkapi, vkBindBufferMemory_exit);
    return _result;
}

extern "C" PUBLIC
void vkGetImageMemoryRequirements(VkDevice device, VkImage image, VkMemoryRequirements* pMemoryRequirements) {
    DTRACE_PROBE(vkapi, vkGetImageMemoryRequirements_entry);
    (*original_vkGetImageMemoryRequirements)(device, image, pMemoryRequirements);
    DTRACE_PROBE(vkapi, vkGetImageMemoryRequirements_exit);
}

extern "C" PUBLIC
VkResult vkBindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset) {
    DTRACE_PROBE(vkapi, vkBindImageMemory_entry);
    auto _result = (*original_vkBindImageMemory)(device, image, memory, memoryOffset);
    DTRACE_PROBE(vkapi, vkBindImageMemory_exit);
    return _result;
}

extern "C" PUBLIC
void vkGetImageSparseMemoryRequirements(VkDevice device, VkImage image, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements* pSparseMemoryRequirements) {
    DTRACE_PROBE(vkapi, vkGetImageSparseMemoryRequirements_entry);
    (*original_vkGetImageSparseMemoryRequirements)(device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
    DTRACE_PROBE(vkapi, vkGetImageSparseMemoryRequirements_exit);
}

extern "C" PUBLIC
void vkGetPhysicalDeviceSparseImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling, uint32_t* pPropertyCount, VkSparseImageFormatProperties* pProperties) {
    DTRACE_PROBE(vkapi, vkGetPhysicalDeviceSparseImageFormatProperties_entry);
    (*original_vkGetPhysicalDeviceSparseImageFormatProperties)(physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
    DTRACE_PROBE(vkapi, vkGetPhysicalDeviceSparseImageFormatProperties_exit);
}

extern "C" PUBLIC
VkResult vkQueueBindSparse(VkQueue queue, uint32_t bindInfoCount, const VkBindSparseInfo* pBindInfo, VkFence fence) {
    DTRACE_PROBE(vkapi, vkQueueBindSparse_entry);
    auto _result = (*original_vkQueueBindSparse)(queue, bindInfoCount, pBindInfo, fence);
    DTRACE_PROBE(vkapi, vkQueueBindSparse_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkCreateFence(VkDevice device, const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence) {
    DTRACE_PROBE(vkapi, vkCreateFence_entry);
    auto _result = (*original_vkCreateFence)(device, pCreateInfo, pAllocator, pFence);
    DTRACE_PROBE(vkapi, vkCreateFence_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyFence(VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyFence_entry);
    (*original_vkDestroyFence)(device, fence, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyFence_exit);
}

extern "C" PUBLIC
VkResult vkResetFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences) {
    DTRACE_PROBE(vkapi, vkResetFences_entry);
    auto _result = (*original_vkResetFences)(device, fenceCount, pFences);
    DTRACE_PROBE(vkapi, vkResetFences_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkGetFenceStatus(VkDevice device, VkFence fence) {
    DTRACE_PROBE(vkapi, vkGetFenceStatus_entry);
    auto _result = (*original_vkGetFenceStatus)(device, fence);
    DTRACE_PROBE(vkapi, vkGetFenceStatus_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkWaitForFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout) {
    DTRACE_PROBE(vkapi, vkWaitForFences_entry);
    auto _result = (*original_vkWaitForFences)(device, fenceCount, pFences, waitAll, timeout);
    DTRACE_PROBE(vkapi, vkWaitForFences_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkCreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore) {
    DTRACE_PROBE(vkapi, vkCreateSemaphore_entry);
    auto _result = (*original_vkCreateSemaphore)(device, pCreateInfo, pAllocator, pSemaphore);
    DTRACE_PROBE(vkapi, vkCreateSemaphore_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroySemaphore(VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroySemaphore_entry);
    (*original_vkDestroySemaphore)(device, semaphore, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroySemaphore_exit);
}

extern "C" PUBLIC
VkResult vkCreateEvent(VkDevice device, const VkEventCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkEvent* pEvent) {
    DTRACE_PROBE(vkapi, vkCreateEvent_entry);
    auto _result = (*original_vkCreateEvent)(device, pCreateInfo, pAllocator, pEvent);
    DTRACE_PROBE(vkapi, vkCreateEvent_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyEvent(VkDevice device, VkEvent event, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyEvent_entry);
    (*original_vkDestroyEvent)(device, event, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyEvent_exit);
}

extern "C" PUBLIC
VkResult vkGetEventStatus(VkDevice device, VkEvent event) {
    DTRACE_PROBE(vkapi, vkGetEventStatus_entry);
    auto _result = (*original_vkGetEventStatus)(device, event);
    DTRACE_PROBE(vkapi, vkGetEventStatus_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkSetEvent(VkDevice device, VkEvent event) {
    DTRACE_PROBE(vkapi, vkSetEvent_entry);
    auto _result = (*original_vkSetEvent)(device, event);
    DTRACE_PROBE(vkapi, vkSetEvent_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkResetEvent(VkDevice device, VkEvent event) {
    DTRACE_PROBE(vkapi, vkResetEvent_entry);
    auto _result = (*original_vkResetEvent)(device, event);
    DTRACE_PROBE(vkapi, vkResetEvent_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkCreateQueryPool(VkDevice device, const VkQueryPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkQueryPool* pQueryPool) {
    DTRACE_PROBE(vkapi, vkCreateQueryPool_entry);
    auto _result = (*original_vkCreateQueryPool)(device, pCreateInfo, pAllocator, pQueryPool);
    DTRACE_PROBE(vkapi, vkCreateQueryPool_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyQueryPool(VkDevice device, VkQueryPool queryPool, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyQueryPool_entry);
    (*original_vkDestroyQueryPool)(device, queryPool, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyQueryPool_exit);
}

extern "C" PUBLIC
VkResult vkGetQueryPoolResults(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, size_t dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags) {
    DTRACE_PROBE(vkapi, vkGetQueryPoolResults_entry);
    auto _result = (*original_vkGetQueryPoolResults)(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
    DTRACE_PROBE(vkapi, vkGetQueryPoolResults_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkCreateBuffer(VkDevice device, const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer) {
    DTRACE_PROBE(vkapi, vkCreateBuffer_entry);
    auto _result = (*original_vkCreateBuffer)(device, pCreateInfo, pAllocator, pBuffer);
    DTRACE_PROBE(vkapi, vkCreateBuffer_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyBuffer(VkDevice device, VkBuffer buffer, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyBuffer_entry);
    (*original_vkDestroyBuffer)(device, buffer, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyBuffer_exit);
}

extern "C" PUBLIC
VkResult vkCreateBufferView(VkDevice device, const VkBufferViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBufferView* pView) {
    DTRACE_PROBE(vkapi, vkCreateBufferView_entry);
    auto _result = (*original_vkCreateBufferView)(device, pCreateInfo, pAllocator, pView);
    DTRACE_PROBE(vkapi, vkCreateBufferView_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyBufferView(VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyBufferView_entry);
    (*original_vkDestroyBufferView)(device, bufferView, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyBufferView_exit);
}

extern "C" PUBLIC
VkResult vkCreateImage(VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage) {
    DTRACE_PROBE(vkapi, vkCreateImage_entry);
    auto _result = (*original_vkCreateImage)(device, pCreateInfo, pAllocator, pImage);
    DTRACE_PROBE(vkapi, vkCreateImage_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyImage_entry);
    (*original_vkDestroyImage)(device, image, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyImage_exit);
}

extern "C" PUBLIC
void vkGetImageSubresourceLayout(VkDevice device, VkImage image, const VkImageSubresource* pSubresource, VkSubresourceLayout* pLayout) {
    DTRACE_PROBE(vkapi, vkGetImageSubresourceLayout_entry);
    (*original_vkGetImageSubresourceLayout)(device, image, pSubresource, pLayout);
    DTRACE_PROBE(vkapi, vkGetImageSubresourceLayout_exit);
}

extern "C" PUBLIC
VkResult vkCreateImageView(VkDevice device, const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pView) {
    DTRACE_PROBE(vkapi, vkCreateImageView_entry);
    auto _result = (*original_vkCreateImageView)(device, pCreateInfo, pAllocator, pView);
    DTRACE_PROBE(vkapi, vkCreateImageView_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyImageView(VkDevice device, VkImageView imageView, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyImageView_entry);
    (*original_vkDestroyImageView)(device, imageView, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyImageView_exit);
}

extern "C" PUBLIC
VkResult vkCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule) {
    DTRACE_PROBE(vkapi, vkCreateShaderModule_entry);
    auto _result = (*original_vkCreateShaderModule)(device, pCreateInfo, pAllocator, pShaderModule);
    DTRACE_PROBE(vkapi, vkCreateShaderModule_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyShaderModule_entry);
    (*original_vkDestroyShaderModule)(device, shaderModule, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyShaderModule_exit);
}

extern "C" PUBLIC
VkResult vkCreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineCache* pPipelineCache) {
    DTRACE_PROBE(vkapi, vkCreatePipelineCache_entry);
    auto _result = (*original_vkCreatePipelineCache)(device, pCreateInfo, pAllocator, pPipelineCache);
    DTRACE_PROBE(vkapi, vkCreatePipelineCache_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyPipelineCache_entry);
    (*original_vkDestroyPipelineCache)(device, pipelineCache, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyPipelineCache_exit);
}

extern "C" PUBLIC
VkResult vkGetPipelineCacheData(VkDevice device, VkPipelineCache pipelineCache, size_t* pDataSize, void* pData) {
    DTRACE_PROBE(vkapi, vkGetPipelineCacheData_entry);
    auto _result = (*original_vkGetPipelineCacheData)(device, pipelineCache, pDataSize, pData);
    DTRACE_PROBE(vkapi, vkGetPipelineCacheData_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkMergePipelineCaches(VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount, const VkPipelineCache* pSrcCaches) {
    DTRACE_PROBE(vkapi, vkMergePipelineCaches_entry);
    auto _result = (*original_vkMergePipelineCaches)(device, dstCache, srcCacheCount, pSrcCaches);
    DTRACE_PROBE(vkapi, vkMergePipelineCaches_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkCreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) {
    DTRACE_PROBE(vkapi, vkCreateGraphicsPipelines_entry);
    auto _result = (*original_vkCreateGraphicsPipelines)(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    DTRACE_PROBE(vkapi, vkCreateGraphicsPipelines_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) {
    DTRACE_PROBE(vkapi, vkCreateComputePipelines_entry);
    auto _result = (*original_vkCreateComputePipelines)(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    DTRACE_PROBE(vkapi, vkCreateComputePipelines_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyPipeline(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyPipeline_entry);
    (*original_vkDestroyPipeline)(device, pipeline, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyPipeline_exit);
}

extern "C" PUBLIC
VkResult vkCreatePipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineLayout* pPipelineLayout) {
    DTRACE_PROBE(vkapi, vkCreatePipelineLayout_entry);
    auto _result = (*original_vkCreatePipelineLayout)(device, pCreateInfo, pAllocator, pPipelineLayout);
    DTRACE_PROBE(vkapi, vkCreatePipelineLayout_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyPipelineLayout_entry);
    (*original_vkDestroyPipelineLayout)(device, pipelineLayout, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyPipelineLayout_exit);
}

extern "C" PUBLIC
VkResult vkCreateSampler(VkDevice device, const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSampler* pSampler) {
    DTRACE_PROBE(vkapi, vkCreateSampler_entry);
    auto _result = (*original_vkCreateSampler)(device, pCreateInfo, pAllocator, pSampler);
    DTRACE_PROBE(vkapi, vkCreateSampler_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroySampler(VkDevice device, VkSampler sampler, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroySampler_entry);
    (*original_vkDestroySampler)(device, sampler, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroySampler_exit);
}

extern "C" PUBLIC
VkResult vkCreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout) {
    DTRACE_PROBE(vkapi, vkCreateDescriptorSetLayout_entry);
    auto _result = (*original_vkCreateDescriptorSetLayout)(device, pCreateInfo, pAllocator, pSetLayout);
    DTRACE_PROBE(vkapi, vkCreateDescriptorSetLayout_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyDescriptorSetLayout_entry);
    (*original_vkDestroyDescriptorSetLayout)(device, descriptorSetLayout, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyDescriptorSetLayout_exit);
}

extern "C" PUBLIC
VkResult vkCreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorPool* pDescriptorPool) {
    DTRACE_PROBE(vkapi, vkCreateDescriptorPool_entry);
    auto _result = (*original_vkCreateDescriptorPool)(device, pCreateInfo, pAllocator, pDescriptorPool);
    DTRACE_PROBE(vkapi, vkCreateDescriptorPool_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyDescriptorPool_entry);
    (*original_vkDestroyDescriptorPool)(device, descriptorPool, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyDescriptorPool_exit);
}

extern "C" PUBLIC
VkResult vkResetDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags) {
    DTRACE_PROBE(vkapi, vkResetDescriptorPool_entry);
    auto _result = (*original_vkResetDescriptorPool)(device, descriptorPool, flags);
    DTRACE_PROBE(vkapi, vkResetDescriptorPool_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkAllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets) {
    DTRACE_PROBE(vkapi, vkAllocateDescriptorSets_entry);
    auto _result = (*original_vkAllocateDescriptorSets)(device, pAllocateInfo, pDescriptorSets);
    DTRACE_PROBE(vkapi, vkAllocateDescriptorSets_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets) {
    DTRACE_PROBE(vkapi, vkFreeDescriptorSets_entry);
    auto _result = (*original_vkFreeDescriptorSets)(device, descriptorPool, descriptorSetCount, pDescriptorSets);
    DTRACE_PROBE(vkapi, vkFreeDescriptorSets_exit);
    return _result;
}

extern "C" PUBLIC
void vkUpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies) {
    DTRACE_PROBE(vkapi, vkUpdateDescriptorSets_entry);
    (*original_vkUpdateDescriptorSets)(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
    DTRACE_PROBE(vkapi, vkUpdateDescriptorSets_exit);
}

extern "C" PUBLIC
VkResult vkCreateFramebuffer(VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer) {
    DTRACE_PROBE(vkapi, vkCreateFramebuffer_entry);
    auto _result = (*original_vkCreateFramebuffer)(device, pCreateInfo, pAllocator, pFramebuffer);
    DTRACE_PROBE(vkapi, vkCreateFramebuffer_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyFramebuffer_entry);
    (*original_vkDestroyFramebuffer)(device, framebuffer, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyFramebuffer_exit);
}

extern "C" PUBLIC
VkResult vkCreateRenderPass(VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass) {
    DTRACE_PROBE(vkapi, vkCreateRenderPass_entry);
    auto _result = (*original_vkCreateRenderPass)(device, pCreateInfo, pAllocator, pRenderPass);
    DTRACE_PROBE(vkapi, vkCreateRenderPass_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyRenderPass_entry);
    (*original_vkDestroyRenderPass)(device, renderPass, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyRenderPass_exit);
}

extern "C" PUBLIC
void vkGetRenderAreaGranularity(VkDevice device, VkRenderPass renderPass, VkExtent2D* pGranularity) {
    DTRACE_PROBE(vkapi, vkGetRenderAreaGranularity_entry);
    (*original_vkGetRenderAreaGranularity)(device, renderPass, pGranularity);
    DTRACE_PROBE(vkapi, vkGetRenderAreaGranularity_exit);
}

extern "C" PUBLIC
VkResult vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool) {
    DTRACE_PROBE(vkapi, vkCreateCommandPool_entry);
    auto _result = (*original_vkCreateCommandPool)(device, pCreateInfo, pAllocator, pCommandPool);
    DTRACE_PROBE(vkapi, vkCreateCommandPool_exit);
    return _result;
}

extern "C" PUBLIC
void vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator) {
    DTRACE_PROBE(vkapi, vkDestroyCommandPool_entry);
    (*original_vkDestroyCommandPool)(device, commandPool, pAllocator);
    DTRACE_PROBE(vkapi, vkDestroyCommandPool_exit);
}

extern "C" PUBLIC
VkResult vkResetCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags) {
    DTRACE_PROBE(vkapi, vkResetCommandPool_entry);
    auto _result = (*original_vkResetCommandPool)(device, commandPool, flags);
    DTRACE_PROBE(vkapi, vkResetCommandPool_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers) {
    DTRACE_PROBE(vkapi, vkAllocateCommandBuffers_entry);
    auto _result = (*original_vkAllocateCommandBuffers)(device, pAllocateInfo, pCommandBuffers);
    DTRACE_PROBE(vkapi, vkAllocateCommandBuffers_exit);
    return _result;
}

extern "C" PUBLIC
void vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) {
    DTRACE_PROBE(vkapi, vkFreeCommandBuffers_entry);
    (*original_vkFreeCommandBuffers)(device, commandPool, commandBufferCount, pCommandBuffers);
    DTRACE_PROBE(vkapi, vkFreeCommandBuffers_exit);
}

extern "C" PUBLIC
VkResult vkBeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) {
    DTRACE_PROBE(vkapi, vkBeginCommandBuffer_entry);
    auto _result = (*original_vkBeginCommandBuffer)(commandBuffer, pBeginInfo);
    DTRACE_PROBE(vkapi, vkBeginCommandBuffer_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkEndCommandBuffer(VkCommandBuffer commandBuffer) {
    DTRACE_PROBE(vkapi, vkEndCommandBuffer_entry);
    auto _result = (*original_vkEndCommandBuffer)(commandBuffer);
    DTRACE_PROBE(vkapi, vkEndCommandBuffer_exit);
    return _result;
}

extern "C" PUBLIC
VkResult vkResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags) {
    DTRACE_PROBE(vkapi, vkResetCommandBuffer_entry);
    auto _result = (*original_vkResetCommandBuffer)(commandBuffer, flags);
    DTRACE_PROBE(vkapi, vkResetCommandBuffer_exit);
    return _result;
}

extern "C" PUBLIC
void vkCmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) {
    DTRACE_PROBE(vkapi, vkCmdBindPipeline_entry);
    (*original_vkCmdBindPipeline)(commandBuffer, pipelineBindPoint, pipeline);
    DTRACE_PROBE(vkapi, vkCmdBindPipeline_exit);
}

extern "C" PUBLIC
void vkCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports) {
    DTRACE_PROBE(vkapi, vkCmdSetViewport_entry);
    (*original_vkCmdSetViewport)(commandBuffer, firstViewport, viewportCount, pViewports);
    DTRACE_PROBE(vkapi, vkCmdSetViewport_exit);
}

extern "C" PUBLIC
void vkCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors) {
    DTRACE_PROBE(vkapi, vkCmdSetScissor_entry);
    (*original_vkCmdSetScissor)(commandBuffer, firstScissor, scissorCount, pScissors);
    DTRACE_PROBE(vkapi, vkCmdSetScissor_exit);
}

extern "C" PUBLIC
void vkCmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth) {
    DTRACE_PROBE(vkapi, vkCmdSetLineWidth_entry);
    (*original_vkCmdSetLineWidth)(commandBuffer, lineWidth);
    DTRACE_PROBE(vkapi, vkCmdSetLineWidth_exit);
}

extern "C" PUBLIC
void vkCmdSetDepthBias(VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) {
    DTRACE_PROBE(vkapi, vkCmdSetDepthBias_entry);
    (*original_vkCmdSetDepthBias)(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
    DTRACE_PROBE(vkapi, vkCmdSetDepthBias_exit);
}

extern "C" PUBLIC
void vkCmdSetBlendConstants(VkCommandBuffer commandBuffer, const float blendConstants[4]) {
    DTRACE_PROBE(vkapi, vkCmdSetBlendConstants_entry);
    (*original_vkCmdSetBlendConstants)(commandBuffer, blendConstants);
    DTRACE_PROBE(vkapi, vkCmdSetBlendConstants_exit);
}

extern "C" PUBLIC
void vkCmdSetDepthBounds(VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds) {
    DTRACE_PROBE(vkapi, vkCmdSetDepthBounds_entry);
    (*original_vkCmdSetDepthBounds)(commandBuffer, minDepthBounds, maxDepthBounds);
    DTRACE_PROBE(vkapi, vkCmdSetDepthBounds_exit);
}

extern "C" PUBLIC
void vkCmdSetStencilCompareMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t compareMask) {
    DTRACE_PROBE(vkapi, vkCmdSetStencilCompareMask_entry);
    (*original_vkCmdSetStencilCompareMask)(commandBuffer, faceMask, compareMask);
    DTRACE_PROBE(vkapi, vkCmdSetStencilCompareMask_exit);
}

extern "C" PUBLIC
void vkCmdSetStencilWriteMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t writeMask) {
    DTRACE_PROBE(vkapi, vkCmdSetStencilWriteMask_entry);
    (*original_vkCmdSetStencilWriteMask)(commandBuffer, faceMask, writeMask);
    DTRACE_PROBE(vkapi, vkCmdSetStencilWriteMask_exit);
}

extern "C" PUBLIC
void vkCmdSetStencilReference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t reference) {
    DTRACE_PROBE(vkapi, vkCmdSetStencilReference_entry);
    (*original_vkCmdSetStencilReference)(commandBuffer, faceMask, reference);
    DTRACE_PROBE(vkapi, vkCmdSetStencilReference_exit);
}

extern "C" PUBLIC
void vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) {
    DTRACE_PROBE(vkapi, vkCmdBindDescriptorSets_entry);
    (*original_vkCmdBindDescriptorSets)(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
    DTRACE_PROBE(vkapi, vkCmdBindDescriptorSets_exit);
}

extern "C" PUBLIC
void vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) {
    DTRACE_PROBE(vkapi, vkCmdBindIndexBuffer_entry);
    (*original_vkCmdBindIndexBuffer)(commandBuffer, buffer, offset, indexType);
    DTRACE_PROBE(vkapi, vkCmdBindIndexBuffer_exit);
}

extern "C" PUBLIC
void vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets) {
    DTRACE_PROBE(vkapi, vkCmdBindVertexBuffers_entry);
    (*original_vkCmdBindVertexBuffers)(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
    DTRACE_PROBE(vkapi, vkCmdBindVertexBuffers_exit);
}

extern "C" PUBLIC
void vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    DTRACE_PROBE(vkapi, vkCmdDraw_entry);
    (*original_vkCmdDraw)(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    DTRACE_PROBE(vkapi, vkCmdDraw_exit);
}

extern "C" PUBLIC
void vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    DTRACE_PROBE(vkapi, vkCmdDrawIndexed_entry);
    (*original_vkCmdDrawIndexed)(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    DTRACE_PROBE(vkapi, vkCmdDrawIndexed_exit);
}

extern "C" PUBLIC
void vkCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride) {
    DTRACE_PROBE(vkapi, vkCmdDrawIndirect_entry);
    (*original_vkCmdDrawIndirect)(commandBuffer, buffer, offset, drawCount, stride);
    DTRACE_PROBE(vkapi, vkCmdDrawIndirect_exit);
}

extern "C" PUBLIC
void vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride) {
    DTRACE_PROBE(vkapi, vkCmdDrawIndexedIndirect_entry);
    (*original_vkCmdDrawIndexedIndirect)(commandBuffer, buffer, offset, drawCount, stride);
    DTRACE_PROBE(vkapi, vkCmdDrawIndexedIndirect_exit);
}

extern "C" PUBLIC
void vkCmdDispatch(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
    DTRACE_PROBE(vkapi, vkCmdDispatch_entry);
    (*original_vkCmdDispatch)(commandBuffer, groupCountX, groupCountY, groupCountZ);
    DTRACE_PROBE(vkapi, vkCmdDispatch_exit);
}

extern "C" PUBLIC
void vkCmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset) {
    DTRACE_PROBE(vkapi, vkCmdDispatchIndirect_entry);
    (*original_vkCmdDispatchIndirect)(commandBuffer, buffer, offset);
    DTRACE_PROBE(vkapi, vkCmdDispatchIndirect_exit);
}

extern "C" PUBLIC
void vkCmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions) {
    DTRACE_PROBE(vkapi, vkCmdCopyBuffer_entry);
    (*original_vkCmdCopyBuffer)(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
    DTRACE_PROBE(vkapi, vkCmdCopyBuffer_exit);
}

extern "C" PUBLIC
void vkCmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy* pRegions) {
    DTRACE_PROBE(vkapi, vkCmdCopyImage_entry);
    (*original_vkCmdCopyImage)(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
    DTRACE_PROBE(vkapi, vkCmdCopyImage_exit);
}

extern "C" PUBLIC
void vkCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter) {
    DTRACE_PROBE(vkapi, vkCmdBlitImage_entry);
    (*original_vkCmdBlitImage)(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
    DTRACE_PROBE(vkapi, vkCmdBlitImage_exit);
}

extern "C" PUBLIC
void vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions) {
    DTRACE_PROBE(vkapi, vkCmdCopyBufferToImage_entry);
    (*original_vkCmdCopyBufferToImage)(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
    DTRACE_PROBE(vkapi, vkCmdCopyBufferToImage_exit);
}

extern "C" PUBLIC
void vkCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy* pRegions) {
    DTRACE_PROBE(vkapi, vkCmdCopyImageToBuffer_entry);
    (*original_vkCmdCopyImageToBuffer)(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
    DTRACE_PROBE(vkapi, vkCmdCopyImageToBuffer_exit);
}

extern "C" PUBLIC
void vkCmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData) {
    DTRACE_PROBE(vkapi, vkCmdUpdateBuffer_entry);
    (*original_vkCmdUpdateBuffer)(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
    DTRACE_PROBE(vkapi, vkCmdUpdateBuffer_exit);
}

extern "C" PUBLIC
void vkCmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data) {
    DTRACE_PROBE(vkapi, vkCmdFillBuffer_entry);
    (*original_vkCmdFillBuffer)(commandBuffer, dstBuffer, dstOffset, size, data);
    DTRACE_PROBE(vkapi, vkCmdFillBuffer_exit);
}

extern "C" PUBLIC
void vkCmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint32_t rangeCount, const VkImageSubresourceRange* pRanges) {
    DTRACE_PROBE(vkapi, vkCmdClearColorImage_entry);
    (*original_vkCmdClearColorImage)(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
    DTRACE_PROBE(vkapi, vkCmdClearColorImage_exit);
}

extern "C" PUBLIC
void vkCmdClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange* pRanges) {
    DTRACE_PROBE(vkapi, vkCmdClearDepthStencilImage_entry);
    (*original_vkCmdClearDepthStencilImage)(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
    DTRACE_PROBE(vkapi, vkCmdClearDepthStencilImage_exit);
}

extern "C" PUBLIC
void vkCmdClearAttachments(VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkClearAttachment* pAttachments, uint32_t rectCount, const VkClearRect* pRects) {
    DTRACE_PROBE(vkapi, vkCmdClearAttachments_entry);
    (*original_vkCmdClearAttachments)(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
    DTRACE_PROBE(vkapi, vkCmdClearAttachments_exit);
}

extern "C" PUBLIC
void vkCmdResolveImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve* pRegions) {
    DTRACE_PROBE(vkapi, vkCmdResolveImage_entry);
    (*original_vkCmdResolveImage)(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
    DTRACE_PROBE(vkapi, vkCmdResolveImage_exit);
}

extern "C" PUBLIC
void vkCmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask) {
    DTRACE_PROBE(vkapi, vkCmdSetEvent_entry);
    (*original_vkCmdSetEvent)(commandBuffer, event, stageMask);
    DTRACE_PROBE(vkapi, vkCmdSetEvent_exit);
}

extern "C" PUBLIC
void vkCmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask) {
    DTRACE_PROBE(vkapi, vkCmdResetEvent_entry);
    (*original_vkCmdResetEvent)(commandBuffer, event, stageMask);
    DTRACE_PROBE(vkapi, vkCmdResetEvent_exit);
}

extern "C" PUBLIC
void vkCmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) {
    DTRACE_PROBE(vkapi, vkCmdWaitEvents_entry);
    (*original_vkCmdWaitEvents)(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    DTRACE_PROBE(vkapi, vkCmdWaitEvents_exit);
}

extern "C" PUBLIC
void vkCmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) {
    DTRACE_PROBE(vkapi, vkCmdPipelineBarrier_entry);
    (*original_vkCmdPipelineBarrier)(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    DTRACE_PROBE(vkapi, vkCmdPipelineBarrier_exit);
}

extern "C" PUBLIC
void vkCmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags) {
    DTRACE_PROBE(vkapi, vkCmdBeginQuery_entry);
    (*original_vkCmdBeginQuery)(commandBuffer, queryPool, query, flags);
    DTRACE_PROBE(vkapi, vkCmdBeginQuery_exit);
}

extern "C" PUBLIC
void vkCmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query) {
    DTRACE_PROBE(vkapi, vkCmdEndQuery_entry);
    (*original_vkCmdEndQuery)(commandBuffer, queryPool, query);
    DTRACE_PROBE(vkapi, vkCmdEndQuery_exit);
}

extern "C" PUBLIC
void vkCmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount) {
    DTRACE_PROBE(vkapi, vkCmdResetQueryPool_entry);
    (*original_vkCmdResetQueryPool)(commandBuffer, queryPool, firstQuery, queryCount);
    DTRACE_PROBE(vkapi, vkCmdResetQueryPool_exit);
}

extern "C" PUBLIC
void vkCmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query) {
    DTRACE_PROBE(vkapi, vkCmdWriteTimestamp_entry);
    (*original_vkCmdWriteTimestamp)(commandBuffer, pipelineStage, queryPool, query);
    DTRACE_PROBE(vkapi, vkCmdWriteTimestamp_exit);
}

extern "C" PUBLIC
void vkCmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags) {
    DTRACE_PROBE(vkapi, vkCmdCopyQueryPoolResults_entry);
    (*original_vkCmdCopyQueryPoolResults)(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
    DTRACE_PROBE(vkapi, vkCmdCopyQueryPoolResults_exit);
}

extern "C" PUBLIC
void vkCmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues) {
    DTRACE_PROBE(vkapi, vkCmdPushConstants_entry);
    (*original_vkCmdPushConstants)(commandBuffer, layout, stageFlags, offset, size, pValues);
    DTRACE_PROBE(vkapi, vkCmdPushConstants_exit);
}

extern "C" PUBLIC
void vkCmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents) {
    DTRACE_PROBE(vkapi, vkCmdBeginRenderPass_entry);
    (*original_vkCmdBeginRenderPass)(commandBuffer, pRenderPassBegin, contents);
    DTRACE_PROBE(vkapi, vkCmdBeginRenderPass_exit);
}

extern "C" PUBLIC
void vkCmdNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents contents) {
    DTRACE_PROBE(vkapi, vkCmdNextSubpass_entry);
    (*original_vkCmdNextSubpass)(commandBuffer, contents);
    DTRACE_PROBE(vkapi, vkCmdNextSubpass_exit);
}

extern "C" PUBLIC
void vkCmdEndRenderPass(VkCommandBuffer commandBuffer) {
    DTRACE_PROBE(vkapi, vkCmdEndRenderPass_entry);
    (*original_vkCmdEndRenderPass)(commandBuffer);
    DTRACE_PROBE(vkapi, vkCmdEndRenderPass_exit);
}

extern "C" PUBLIC
void vkCmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) {
    DTRACE_PROBE(vkapi, vkCmdExecuteCommands_entry);
    (*original_vkCmdExecuteCommands)(commandBuffer, commandBufferCount, pCommandBuffers);
    DTRACE_PROBE(vkapi, vkCmdExecuteCommands_exit);
}

extern "C" PUBLIC
VkResult vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) {
    DTRACE_PROBE(vkapi, vkQueuePresentKHR_entry);
    auto _result = (*original_vkQueuePresentKHR)(queue, pPresentInfo);
    DTRACE_PROBE(vkapi, vkQueuePresentKHR_exit);
    onSwapCompleted();
    return _result;
}

extern "C" PUBLIC
void vkGetDeviceQueue2(VkDevice device, const VkDeviceQueueInfo2* pQueueInfo, VkQueue* pQueue) {
    DTRACE_PROBE(vkapi, vkGetDeviceQueue2_entry);
    (*original_vkGetDeviceQueue2)(device, pQueueInfo, pQueue);
    DTRACE_PROBE(vkapi, vkGetDeviceQueue2_exit);
}

extern "C" PUBLIC
void *dlsym(void *handle, const char *symbol)
{
    auto ptr = original_dlsym(handle, symbol);
    if (strcmp("vkCreateInstance", symbol) == 0)
    {
        original_vkCreateInstance = reinterpret_cast<decltype(original_vkCreateInstance)>(ptr);
        return reinterpret_cast<void *>(vkCreateInstance);
    }
    if (strcmp("vkDestroyInstance", symbol) == 0)
    {
        original_vkDestroyInstance = reinterpret_cast<decltype(original_vkDestroyInstance)>(ptr);
        return reinterpret_cast<void *>(vkDestroyInstance);
    }
    if (strcmp("vkEnumeratePhysicalDevices", symbol) == 0)
    {
        original_vkEnumeratePhysicalDevices = reinterpret_cast<decltype(original_vkEnumeratePhysicalDevices)>(ptr);
        return reinterpret_cast<void *>(vkEnumeratePhysicalDevices);
    }
    if (strcmp("vkGetPhysicalDeviceProperties", symbol) == 0)
    {
        original_vkGetPhysicalDeviceProperties = reinterpret_cast<decltype(original_vkGetPhysicalDeviceProperties)>(ptr);
        return reinterpret_cast<void *>(vkGetPhysicalDeviceProperties);
    }
    if (strcmp("vkGetPhysicalDeviceQueueFamilyProperties", symbol) == 0)
    {
        original_vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<decltype(original_vkGetPhysicalDeviceQueueFamilyProperties)>(ptr);
        return reinterpret_cast<void *>(vkGetPhysicalDeviceQueueFamilyProperties);
    }
    if (strcmp("vkGetPhysicalDeviceMemoryProperties", symbol) == 0)
    {
        original_vkGetPhysicalDeviceMemoryProperties = reinterpret_cast<decltype(original_vkGetPhysicalDeviceMemoryProperties)>(ptr);
        return reinterpret_cast<void *>(vkGetPhysicalDeviceMemoryProperties);
    }
    if (strcmp("vkGetPhysicalDeviceFeatures", symbol) == 0)
    {
        original_vkGetPhysicalDeviceFeatures = reinterpret_cast<decltype(original_vkGetPhysicalDeviceFeatures)>(ptr);
        return reinterpret_cast<void *>(vkGetPhysicalDeviceFeatures);
    }
    if (strcmp("vkGetPhysicalDeviceFormatProperties", symbol) == 0)
    {
        original_vkGetPhysicalDeviceFormatProperties = reinterpret_cast<decltype(original_vkGetPhysicalDeviceFormatProperties)>(ptr);
        return reinterpret_cast<void *>(vkGetPhysicalDeviceFormatProperties);
    }
    if (strcmp("vkGetPhysicalDeviceImageFormatProperties", symbol) == 0)
    {
        original_vkGetPhysicalDeviceImageFormatProperties = reinterpret_cast<decltype(original_vkGetPhysicalDeviceImageFormatProperties)>(ptr);
        return reinterpret_cast<void *>(vkGetPhysicalDeviceImageFormatProperties);
    }
    if (strcmp("vkCreateDevice", symbol) == 0)
    {
        original_vkCreateDevice = reinterpret_cast<decltype(original_vkCreateDevice)>(ptr);
        return reinterpret_cast<void *>(vkCreateDevice);
    }
    if (strcmp("vkDestroyDevice", symbol) == 0)
    {
        original_vkDestroyDevice = reinterpret_cast<decltype(original_vkDestroyDevice)>(ptr);
        return reinterpret_cast<void *>(vkDestroyDevice);
    }
    if (strcmp("vkEnumerateInstanceVersion", symbol) == 0)
    {
        original_vkEnumerateInstanceVersion = reinterpret_cast<decltype(original_vkEnumerateInstanceVersion)>(ptr);
        return reinterpret_cast<void *>(vkEnumerateInstanceVersion);
    }
    if (strcmp("vkEnumerateInstanceLayerProperties", symbol) == 0)
    {
        original_vkEnumerateInstanceLayerProperties = reinterpret_cast<decltype(original_vkEnumerateInstanceLayerProperties)>(ptr);
        return reinterpret_cast<void *>(vkEnumerateInstanceLayerProperties);
    }
    if (strcmp("vkEnumerateInstanceExtensionProperties", symbol) == 0)
    {
        original_vkEnumerateInstanceExtensionProperties = reinterpret_cast<decltype(original_vkEnumerateInstanceExtensionProperties)>(ptr);
        return reinterpret_cast<void *>(vkEnumerateInstanceExtensionProperties);
    }
    if (strcmp("vkEnumerateDeviceLayerProperties", symbol) == 0)
    {
        original_vkEnumerateDeviceLayerProperties = reinterpret_cast<decltype(original_vkEnumerateDeviceLayerProperties)>(ptr);
        return reinterpret_cast<void *>(vkEnumerateDeviceLayerProperties);
    }
    if (strcmp("vkEnumerateDeviceExtensionProperties", symbol) == 0)
    {
        original_vkEnumerateDeviceExtensionProperties = reinterpret_cast<decltype(original_vkEnumerateDeviceExtensionProperties)>(ptr);
        return reinterpret_cast<void *>(vkEnumerateDeviceExtensionProperties);
    }
    if (strcmp("vkGetDeviceQueue", symbol) == 0)
    {
        original_vkGetDeviceQueue = reinterpret_cast<decltype(original_vkGetDeviceQueue)>(ptr);
        return reinterpret_cast<void *>(vkGetDeviceQueue);
    }
    if (strcmp("vkQueueSubmit", symbol) == 0)
    {
        original_vkQueueSubmit = reinterpret_cast<decltype(original_vkQueueSubmit)>(ptr);
        return reinterpret_cast<void *>(vkQueueSubmit);
    }
    if (strcmp("vkQueueWaitIdle", symbol) == 0)
    {
        original_vkQueueWaitIdle = reinterpret_cast<decltype(original_vkQueueWaitIdle)>(ptr);
        return reinterpret_cast<void *>(vkQueueWaitIdle);
    }
    if (strcmp("vkDeviceWaitIdle", symbol) == 0)
    {
        original_vkDeviceWaitIdle = reinterpret_cast<decltype(original_vkDeviceWaitIdle)>(ptr);
        return reinterpret_cast<void *>(vkDeviceWaitIdle);
    }
    if (strcmp("vkAllocateMemory", symbol) == 0)
    {
        original_vkAllocateMemory = reinterpret_cast<decltype(original_vkAllocateMemory)>(ptr);
        return reinterpret_cast<void *>(vkAllocateMemory);
    }
    if (strcmp("vkFreeMemory", symbol) == 0)
    {
        original_vkFreeMemory = reinterpret_cast<decltype(original_vkFreeMemory)>(ptr);
        return reinterpret_cast<void *>(vkFreeMemory);
    }
    if (strcmp("vkMapMemory", symbol) == 0)
    {
        original_vkMapMemory = reinterpret_cast<decltype(original_vkMapMemory)>(ptr);
        return reinterpret_cast<void *>(vkMapMemory);
    }
    if (strcmp("vkUnmapMemory", symbol) == 0)
    {
        original_vkUnmapMemory = reinterpret_cast<decltype(original_vkUnmapMemory)>(ptr);
        return reinterpret_cast<void *>(vkUnmapMemory);
    }
    if (strcmp("vkFlushMappedMemoryRanges", symbol) == 0)
    {
        original_vkFlushMappedMemoryRanges = reinterpret_cast<decltype(original_vkFlushMappedMemoryRanges)>(ptr);
        return reinterpret_cast<void *>(vkFlushMappedMemoryRanges);
    }
    if (strcmp("vkInvalidateMappedMemoryRanges", symbol) == 0)
    {
        original_vkInvalidateMappedMemoryRanges = reinterpret_cast<decltype(original_vkInvalidateMappedMemoryRanges)>(ptr);
        return reinterpret_cast<void *>(vkInvalidateMappedMemoryRanges);
    }
    if (strcmp("vkGetDeviceMemoryCommitment", symbol) == 0)
    {
        original_vkGetDeviceMemoryCommitment = reinterpret_cast<decltype(original_vkGetDeviceMemoryCommitment)>(ptr);
        return reinterpret_cast<void *>(vkGetDeviceMemoryCommitment);
    }
    if (strcmp("vkGetBufferMemoryRequirements", symbol) == 0)
    {
        original_vkGetBufferMemoryRequirements = reinterpret_cast<decltype(original_vkGetBufferMemoryRequirements)>(ptr);
        return reinterpret_cast<void *>(vkGetBufferMemoryRequirements);
    }
    if (strcmp("vkBindBufferMemory", symbol) == 0)
    {
        original_vkBindBufferMemory = reinterpret_cast<decltype(original_vkBindBufferMemory)>(ptr);
        return reinterpret_cast<void *>(vkBindBufferMemory);
    }
    if (strcmp("vkGetImageMemoryRequirements", symbol) == 0)
    {
        original_vkGetImageMemoryRequirements = reinterpret_cast<decltype(original_vkGetImageMemoryRequirements)>(ptr);
        return reinterpret_cast<void *>(vkGetImageMemoryRequirements);
    }
    if (strcmp("vkBindImageMemory", symbol) == 0)
    {
        original_vkBindImageMemory = reinterpret_cast<decltype(original_vkBindImageMemory)>(ptr);
        return reinterpret_cast<void *>(vkBindImageMemory);
    }
    if (strcmp("vkGetImageSparseMemoryRequirements", symbol) == 0)
    {
        original_vkGetImageSparseMemoryRequirements = reinterpret_cast<decltype(original_vkGetImageSparseMemoryRequirements)>(ptr);
        return reinterpret_cast<void *>(vkGetImageSparseMemoryRequirements);
    }
    if (strcmp("vkGetPhysicalDeviceSparseImageFormatProperties", symbol) == 0)
    {
        original_vkGetPhysicalDeviceSparseImageFormatProperties = reinterpret_cast<decltype(original_vkGetPhysicalDeviceSparseImageFormatProperties)>(ptr);
        return reinterpret_cast<void *>(vkGetPhysicalDeviceSparseImageFormatProperties);
    }
    if (strcmp("vkQueueBindSparse", symbol) == 0)
    {
        original_vkQueueBindSparse = reinterpret_cast<decltype(original_vkQueueBindSparse)>(ptr);
        return reinterpret_cast<void *>(vkQueueBindSparse);
    }
    if (strcmp("vkCreateFence", symbol) == 0)
    {
        original_vkCreateFence = reinterpret_cast<decltype(original_vkCreateFence)>(ptr);
        return reinterpret_cast<void *>(vkCreateFence);
    }
    if (strcmp("vkDestroyFence", symbol) == 0)
    {
        original_vkDestroyFence = reinterpret_cast<decltype(original_vkDestroyFence)>(ptr);
        return reinterpret_cast<void *>(vkDestroyFence);
    }
    if (strcmp("vkResetFences", symbol) == 0)
    {
        original_vkResetFences = reinterpret_cast<decltype(original_vkResetFences)>(ptr);
        return reinterpret_cast<void *>(vkResetFences);
    }
    if (strcmp("vkGetFenceStatus", symbol) == 0)
    {
        original_vkGetFenceStatus = reinterpret_cast<decltype(original_vkGetFenceStatus)>(ptr);
        return reinterpret_cast<void *>(vkGetFenceStatus);
    }
    if (strcmp("vkWaitForFences", symbol) == 0)
    {
        original_vkWaitForFences = reinterpret_cast<decltype(original_vkWaitForFences)>(ptr);
        return reinterpret_cast<void *>(vkWaitForFences);
    }
    if (strcmp("vkCreateSemaphore", symbol) == 0)
    {
        original_vkCreateSemaphore = reinterpret_cast<decltype(original_vkCreateSemaphore)>(ptr);
        return reinterpret_cast<void *>(vkCreateSemaphore);
    }
    if (strcmp("vkDestroySemaphore", symbol) == 0)
    {
        original_vkDestroySemaphore = reinterpret_cast<decltype(original_vkDestroySemaphore)>(ptr);
        return reinterpret_cast<void *>(vkDestroySemaphore);
    }
    if (strcmp("vkCreateEvent", symbol) == 0)
    {
        original_vkCreateEvent = reinterpret_cast<decltype(original_vkCreateEvent)>(ptr);
        return reinterpret_cast<void *>(vkCreateEvent);
    }
    if (strcmp("vkDestroyEvent", symbol) == 0)
    {
        original_vkDestroyEvent = reinterpret_cast<decltype(original_vkDestroyEvent)>(ptr);
        return reinterpret_cast<void *>(vkDestroyEvent);
    }
    if (strcmp("vkGetEventStatus", symbol) == 0)
    {
        original_vkGetEventStatus = reinterpret_cast<decltype(original_vkGetEventStatus)>(ptr);
        return reinterpret_cast<void *>(vkGetEventStatus);
    }
    if (strcmp("vkSetEvent", symbol) == 0)
    {
        original_vkSetEvent = reinterpret_cast<decltype(original_vkSetEvent)>(ptr);
        return reinterpret_cast<void *>(vkSetEvent);
    }
    if (strcmp("vkResetEvent", symbol) == 0)
    {
        original_vkResetEvent = reinterpret_cast<decltype(original_vkResetEvent)>(ptr);
        return reinterpret_cast<void *>(vkResetEvent);
    }
    if (strcmp("vkCreateQueryPool", symbol) == 0)
    {
        original_vkCreateQueryPool = reinterpret_cast<decltype(original_vkCreateQueryPool)>(ptr);
        return reinterpret_cast<void *>(vkCreateQueryPool);
    }
    if (strcmp("vkDestroyQueryPool", symbol) == 0)
    {
        original_vkDestroyQueryPool = reinterpret_cast<decltype(original_vkDestroyQueryPool)>(ptr);
        return reinterpret_cast<void *>(vkDestroyQueryPool);
    }
    if (strcmp("vkGetQueryPoolResults", symbol) == 0)
    {
        original_vkGetQueryPoolResults = reinterpret_cast<decltype(original_vkGetQueryPoolResults)>(ptr);
        return reinterpret_cast<void *>(vkGetQueryPoolResults);
    }
    if (strcmp("vkCreateBuffer", symbol) == 0)
    {
        original_vkCreateBuffer = reinterpret_cast<decltype(original_vkCreateBuffer)>(ptr);
        return reinterpret_cast<void *>(vkCreateBuffer);
    }
    if (strcmp("vkDestroyBuffer", symbol) == 0)
    {
        original_vkDestroyBuffer = reinterpret_cast<decltype(original_vkDestroyBuffer)>(ptr);
        return reinterpret_cast<void *>(vkDestroyBuffer);
    }
    if (strcmp("vkCreateBufferView", symbol) == 0)
    {
        original_vkCreateBufferView = reinterpret_cast<decltype(original_vkCreateBufferView)>(ptr);
        return reinterpret_cast<void *>(vkCreateBufferView);
    }
    if (strcmp("vkDestroyBufferView", symbol) == 0)
    {
        original_vkDestroyBufferView = reinterpret_cast<decltype(original_vkDestroyBufferView)>(ptr);
        return reinterpret_cast<void *>(vkDestroyBufferView);
    }
    if (strcmp("vkCreateImage", symbol) == 0)
    {
        original_vkCreateImage = reinterpret_cast<decltype(original_vkCreateImage)>(ptr);
        return reinterpret_cast<void *>(vkCreateImage);
    }
    if (strcmp("vkDestroyImage", symbol) == 0)
    {
        original_vkDestroyImage = reinterpret_cast<decltype(original_vkDestroyImage)>(ptr);
        return reinterpret_cast<void *>(vkDestroyImage);
    }
    if (strcmp("vkGetImageSubresourceLayout", symbol) == 0)
    {
        original_vkGetImageSubresourceLayout = reinterpret_cast<decltype(original_vkGetImageSubresourceLayout)>(ptr);
        return reinterpret_cast<void *>(vkGetImageSubresourceLayout);
    }
    if (strcmp("vkCreateImageView", symbol) == 0)
    {
        original_vkCreateImageView = reinterpret_cast<decltype(original_vkCreateImageView)>(ptr);
        return reinterpret_cast<void *>(vkCreateImageView);
    }
    if (strcmp("vkDestroyImageView", symbol) == 0)
    {
        original_vkDestroyImageView = reinterpret_cast<decltype(original_vkDestroyImageView)>(ptr);
        return reinterpret_cast<void *>(vkDestroyImageView);
    }
    if (strcmp("vkCreateShaderModule", symbol) == 0)
    {
        original_vkCreateShaderModule = reinterpret_cast<decltype(original_vkCreateShaderModule)>(ptr);
        return reinterpret_cast<void *>(vkCreateShaderModule);
    }
    if (strcmp("vkDestroyShaderModule", symbol) == 0)
    {
        original_vkDestroyShaderModule = reinterpret_cast<decltype(original_vkDestroyShaderModule)>(ptr);
        return reinterpret_cast<void *>(vkDestroyShaderModule);
    }
    if (strcmp("vkCreatePipelineCache", symbol) == 0)
    {
        original_vkCreatePipelineCache = reinterpret_cast<decltype(original_vkCreatePipelineCache)>(ptr);
        return reinterpret_cast<void *>(vkCreatePipelineCache);
    }
    if (strcmp("vkDestroyPipelineCache", symbol) == 0)
    {
        original_vkDestroyPipelineCache = reinterpret_cast<decltype(original_vkDestroyPipelineCache)>(ptr);
        return reinterpret_cast<void *>(vkDestroyPipelineCache);
    }
    if (strcmp("vkGetPipelineCacheData", symbol) == 0)
    {
        original_vkGetPipelineCacheData = reinterpret_cast<decltype(original_vkGetPipelineCacheData)>(ptr);
        return reinterpret_cast<void *>(vkGetPipelineCacheData);
    }
    if (strcmp("vkMergePipelineCaches", symbol) == 0)
    {
        original_vkMergePipelineCaches = reinterpret_cast<decltype(original_vkMergePipelineCaches)>(ptr);
        return reinterpret_cast<void *>(vkMergePipelineCaches);
    }
    if (strcmp("vkCreateGraphicsPipelines", symbol) == 0)
    {
        original_vkCreateGraphicsPipelines = reinterpret_cast<decltype(original_vkCreateGraphicsPipelines)>(ptr);
        return reinterpret_cast<void *>(vkCreateGraphicsPipelines);
    }
    if (strcmp("vkCreateComputePipelines", symbol) == 0)
    {
        original_vkCreateComputePipelines = reinterpret_cast<decltype(original_vkCreateComputePipelines)>(ptr);
        return reinterpret_cast<void *>(vkCreateComputePipelines);
    }
    if (strcmp("vkDestroyPipeline", symbol) == 0)
    {
        original_vkDestroyPipeline = reinterpret_cast<decltype(original_vkDestroyPipeline)>(ptr);
        return reinterpret_cast<void *>(vkDestroyPipeline);
    }
    if (strcmp("vkCreatePipelineLayout", symbol) == 0)
    {
        original_vkCreatePipelineLayout = reinterpret_cast<decltype(original_vkCreatePipelineLayout)>(ptr);
        return reinterpret_cast<void *>(vkCreatePipelineLayout);
    }
    if (strcmp("vkDestroyPipelineLayout", symbol) == 0)
    {
        original_vkDestroyPipelineLayout = reinterpret_cast<decltype(original_vkDestroyPipelineLayout)>(ptr);
        return reinterpret_cast<void *>(vkDestroyPipelineLayout);
    }
    if (strcmp("vkCreateSampler", symbol) == 0)
    {
        original_vkCreateSampler = reinterpret_cast<decltype(original_vkCreateSampler)>(ptr);
        return reinterpret_cast<void *>(vkCreateSampler);
    }
    if (strcmp("vkDestroySampler", symbol) == 0)
    {
        original_vkDestroySampler = reinterpret_cast<decltype(original_vkDestroySampler)>(ptr);
        return reinterpret_cast<void *>(vkDestroySampler);
    }
    if (strcmp("vkCreateDescriptorSetLayout", symbol) == 0)
    {
        original_vkCreateDescriptorSetLayout = reinterpret_cast<decltype(original_vkCreateDescriptorSetLayout)>(ptr);
        return reinterpret_cast<void *>(vkCreateDescriptorSetLayout);
    }
    if (strcmp("vkDestroyDescriptorSetLayout", symbol) == 0)
    {
        original_vkDestroyDescriptorSetLayout = reinterpret_cast<decltype(original_vkDestroyDescriptorSetLayout)>(ptr);
        return reinterpret_cast<void *>(vkDestroyDescriptorSetLayout);
    }
    if (strcmp("vkCreateDescriptorPool", symbol) == 0)
    {
        original_vkCreateDescriptorPool = reinterpret_cast<decltype(original_vkCreateDescriptorPool)>(ptr);
        return reinterpret_cast<void *>(vkCreateDescriptorPool);
    }
    if (strcmp("vkDestroyDescriptorPool", symbol) == 0)
    {
        original_vkDestroyDescriptorPool = reinterpret_cast<decltype(original_vkDestroyDescriptorPool)>(ptr);
        return reinterpret_cast<void *>(vkDestroyDescriptorPool);
    }
    if (strcmp("vkResetDescriptorPool", symbol) == 0)
    {
        original_vkResetDescriptorPool = reinterpret_cast<decltype(original_vkResetDescriptorPool)>(ptr);
        return reinterpret_cast<void *>(vkResetDescriptorPool);
    }
    if (strcmp("vkAllocateDescriptorSets", symbol) == 0)
    {
        original_vkAllocateDescriptorSets = reinterpret_cast<decltype(original_vkAllocateDescriptorSets)>(ptr);
        return reinterpret_cast<void *>(vkAllocateDescriptorSets);
    }
    if (strcmp("vkFreeDescriptorSets", symbol) == 0)
    {
        original_vkFreeDescriptorSets = reinterpret_cast<decltype(original_vkFreeDescriptorSets)>(ptr);
        return reinterpret_cast<void *>(vkFreeDescriptorSets);
    }
    if (strcmp("vkUpdateDescriptorSets", symbol) == 0)
    {
        original_vkUpdateDescriptorSets = reinterpret_cast<decltype(original_vkUpdateDescriptorSets)>(ptr);
        return reinterpret_cast<void *>(vkUpdateDescriptorSets);
    }
    if (strcmp("vkCreateFramebuffer", symbol) == 0)
    {
        original_vkCreateFramebuffer = reinterpret_cast<decltype(original_vkCreateFramebuffer)>(ptr);
        return reinterpret_cast<void *>(vkCreateFramebuffer);
    }
    if (strcmp("vkDestroyFramebuffer", symbol) == 0)
    {
        original_vkDestroyFramebuffer = reinterpret_cast<decltype(original_vkDestroyFramebuffer)>(ptr);
        return reinterpret_cast<void *>(vkDestroyFramebuffer);
    }
    if (strcmp("vkCreateRenderPass", symbol) == 0)
    {
        original_vkCreateRenderPass = reinterpret_cast<decltype(original_vkCreateRenderPass)>(ptr);
        return reinterpret_cast<void *>(vkCreateRenderPass);
    }
    if (strcmp("vkDestroyRenderPass", symbol) == 0)
    {
        original_vkDestroyRenderPass = reinterpret_cast<decltype(original_vkDestroyRenderPass)>(ptr);
        return reinterpret_cast<void *>(vkDestroyRenderPass);
    }
    if (strcmp("vkGetRenderAreaGranularity", symbol) == 0)
    {
        original_vkGetRenderAreaGranularity = reinterpret_cast<decltype(original_vkGetRenderAreaGranularity)>(ptr);
        return reinterpret_cast<void *>(vkGetRenderAreaGranularity);
    }
    if (strcmp("vkCreateCommandPool", symbol) == 0)
    {
        original_vkCreateCommandPool = reinterpret_cast<decltype(original_vkCreateCommandPool)>(ptr);
        return reinterpret_cast<void *>(vkCreateCommandPool);
    }
    if (strcmp("vkDestroyCommandPool", symbol) == 0)
    {
        original_vkDestroyCommandPool = reinterpret_cast<decltype(original_vkDestroyCommandPool)>(ptr);
        return reinterpret_cast<void *>(vkDestroyCommandPool);
    }
    if (strcmp("vkResetCommandPool", symbol) == 0)
    {
        original_vkResetCommandPool = reinterpret_cast<decltype(original_vkResetCommandPool)>(ptr);
        return reinterpret_cast<void *>(vkResetCommandPool);
    }
    if (strcmp("vkAllocateCommandBuffers", symbol) == 0)
    {
        original_vkAllocateCommandBuffers = reinterpret_cast<decltype(original_vkAllocateCommandBuffers)>(ptr);
        return reinterpret_cast<void *>(vkAllocateCommandBuffers);
    }
    if (strcmp("vkFreeCommandBuffers", symbol) == 0)
    {
        original_vkFreeCommandBuffers = reinterpret_cast<decltype(original_vkFreeCommandBuffers)>(ptr);
        return reinterpret_cast<void *>(vkFreeCommandBuffers);
    }
    if (strcmp("vkBeginCommandBuffer", symbol) == 0)
    {
        original_vkBeginCommandBuffer = reinterpret_cast<decltype(original_vkBeginCommandBuffer)>(ptr);
        return reinterpret_cast<void *>(vkBeginCommandBuffer);
    }
    if (strcmp("vkEndCommandBuffer", symbol) == 0)
    {
        original_vkEndCommandBuffer = reinterpret_cast<decltype(original_vkEndCommandBuffer)>(ptr);
        return reinterpret_cast<void *>(vkEndCommandBuffer);
    }
    if (strcmp("vkResetCommandBuffer", symbol) == 0)
    {
        original_vkResetCommandBuffer = reinterpret_cast<decltype(original_vkResetCommandBuffer)>(ptr);
        return reinterpret_cast<void *>(vkResetCommandBuffer);
    }
    if (strcmp("vkCmdBindPipeline", symbol) == 0)
    {
        original_vkCmdBindPipeline = reinterpret_cast<decltype(original_vkCmdBindPipeline)>(ptr);
        return reinterpret_cast<void *>(vkCmdBindPipeline);
    }
    if (strcmp("vkCmdSetViewport", symbol) == 0)
    {
        original_vkCmdSetViewport = reinterpret_cast<decltype(original_vkCmdSetViewport)>(ptr);
        return reinterpret_cast<void *>(vkCmdSetViewport);
    }
    if (strcmp("vkCmdSetScissor", symbol) == 0)
    {
        original_vkCmdSetScissor = reinterpret_cast<decltype(original_vkCmdSetScissor)>(ptr);
        return reinterpret_cast<void *>(vkCmdSetScissor);
    }
    if (strcmp("vkCmdSetLineWidth", symbol) == 0)
    {
        original_vkCmdSetLineWidth = reinterpret_cast<decltype(original_vkCmdSetLineWidth)>(ptr);
        return reinterpret_cast<void *>(vkCmdSetLineWidth);
    }
    if (strcmp("vkCmdSetDepthBias", symbol) == 0)
    {
        original_vkCmdSetDepthBias = reinterpret_cast<decltype(original_vkCmdSetDepthBias)>(ptr);
        return reinterpret_cast<void *>(vkCmdSetDepthBias);
    }
    if (strcmp("vkCmdSetBlendConstants", symbol) == 0)
    {
        original_vkCmdSetBlendConstants = reinterpret_cast<decltype(original_vkCmdSetBlendConstants)>(ptr);
        return reinterpret_cast<void *>(vkCmdSetBlendConstants);
    }
    if (strcmp("vkCmdSetDepthBounds", symbol) == 0)
    {
        original_vkCmdSetDepthBounds = reinterpret_cast<decltype(original_vkCmdSetDepthBounds)>(ptr);
        return reinterpret_cast<void *>(vkCmdSetDepthBounds);
    }
    if (strcmp("vkCmdSetStencilCompareMask", symbol) == 0)
    {
        original_vkCmdSetStencilCompareMask = reinterpret_cast<decltype(original_vkCmdSetStencilCompareMask)>(ptr);
        return reinterpret_cast<void *>(vkCmdSetStencilCompareMask);
    }
    if (strcmp("vkCmdSetStencilWriteMask", symbol) == 0)
    {
        original_vkCmdSetStencilWriteMask = reinterpret_cast<decltype(original_vkCmdSetStencilWriteMask)>(ptr);
        return reinterpret_cast<void *>(vkCmdSetStencilWriteMask);
    }
    if (strcmp("vkCmdSetStencilReference", symbol) == 0)
    {
        original_vkCmdSetStencilReference = reinterpret_cast<decltype(original_vkCmdSetStencilReference)>(ptr);
        return reinterpret_cast<void *>(vkCmdSetStencilReference);
    }
    if (strcmp("vkCmdBindDescriptorSets", symbol) == 0)
    {
        original_vkCmdBindDescriptorSets = reinterpret_cast<decltype(original_vkCmdBindDescriptorSets)>(ptr);
        return reinterpret_cast<void *>(vkCmdBindDescriptorSets);
    }
    if (strcmp("vkCmdBindIndexBuffer", symbol) == 0)
    {
        original_vkCmdBindIndexBuffer = reinterpret_cast<decltype(original_vkCmdBindIndexBuffer)>(ptr);
        return reinterpret_cast<void *>(vkCmdBindIndexBuffer);
    }
    if (strcmp("vkCmdBindVertexBuffers", symbol) == 0)
    {
        original_vkCmdBindVertexBuffers = reinterpret_cast<decltype(original_vkCmdBindVertexBuffers)>(ptr);
        return reinterpret_cast<void *>(vkCmdBindVertexBuffers);
    }
    if (strcmp("vkCmdDraw", symbol) == 0)
    {
        original_vkCmdDraw = reinterpret_cast<decltype(original_vkCmdDraw)>(ptr);
        return reinterpret_cast<void *>(vkCmdDraw);
    }
    if (strcmp("vkCmdDrawIndexed", symbol) == 0)
    {
        original_vkCmdDrawIndexed = reinterpret_cast<decltype(original_vkCmdDrawIndexed)>(ptr);
        return reinterpret_cast<void *>(vkCmdDrawIndexed);
    }
    if (strcmp("vkCmdDrawIndirect", symbol) == 0)
    {
        original_vkCmdDrawIndirect = reinterpret_cast<decltype(original_vkCmdDrawIndirect)>(ptr);
        return reinterpret_cast<void *>(vkCmdDrawIndirect);
    }
    if (strcmp("vkCmdDrawIndexedIndirect", symbol) == 0)
    {
        original_vkCmdDrawIndexedIndirect = reinterpret_cast<decltype(original_vkCmdDrawIndexedIndirect)>(ptr);
        return reinterpret_cast<void *>(vkCmdDrawIndexedIndirect);
    }
    if (strcmp("vkCmdDispatch", symbol) == 0)
    {
        original_vkCmdDispatch = reinterpret_cast<decltype(original_vkCmdDispatch)>(ptr);
        return reinterpret_cast<void *>(vkCmdDispatch);
    }
    if (strcmp("vkCmdDispatchIndirect", symbol) == 0)
    {
        original_vkCmdDispatchIndirect = reinterpret_cast<decltype(original_vkCmdDispatchIndirect)>(ptr);
        return reinterpret_cast<void *>(vkCmdDispatchIndirect);
    }
    if (strcmp("vkCmdCopyBuffer", symbol) == 0)
    {
        original_vkCmdCopyBuffer = reinterpret_cast<decltype(original_vkCmdCopyBuffer)>(ptr);
        return reinterpret_cast<void *>(vkCmdCopyBuffer);
    }
    if (strcmp("vkCmdCopyImage", symbol) == 0)
    {
        original_vkCmdCopyImage = reinterpret_cast<decltype(original_vkCmdCopyImage)>(ptr);
        return reinterpret_cast<void *>(vkCmdCopyImage);
    }
    if (strcmp("vkCmdBlitImage", symbol) == 0)
    {
        original_vkCmdBlitImage = reinterpret_cast<decltype(original_vkCmdBlitImage)>(ptr);
        return reinterpret_cast<void *>(vkCmdBlitImage);
    }
    if (strcmp("vkCmdCopyBufferToImage", symbol) == 0)
    {
        original_vkCmdCopyBufferToImage = reinterpret_cast<decltype(original_vkCmdCopyBufferToImage)>(ptr);
        return reinterpret_cast<void *>(vkCmdCopyBufferToImage);
    }
    if (strcmp("vkCmdCopyImageToBuffer", symbol) == 0)
    {
        original_vkCmdCopyImageToBuffer = reinterpret_cast<decltype(original_vkCmdCopyImageToBuffer)>(ptr);
        return reinterpret_cast<void *>(vkCmdCopyImageToBuffer);
    }
    if (strcmp("vkCmdUpdateBuffer", symbol) == 0)
    {
        original_vkCmdUpdateBuffer = reinterpret_cast<decltype(original_vkCmdUpdateBuffer)>(ptr);
        return reinterpret_cast<void *>(vkCmdUpdateBuffer);
    }
    if (strcmp("vkCmdFillBuffer", symbol) == 0)
    {
        original_vkCmdFillBuffer = reinterpret_cast<decltype(original_vkCmdFillBuffer)>(ptr);
        return reinterpret_cast<void *>(vkCmdFillBuffer);
    }
    if (strcmp("vkCmdClearColorImage", symbol) == 0)
    {
        original_vkCmdClearColorImage = reinterpret_cast<decltype(original_vkCmdClearColorImage)>(ptr);
        return reinterpret_cast<void *>(vkCmdClearColorImage);
    }
    if (strcmp("vkCmdClearDepthStencilImage", symbol) == 0)
    {
        original_vkCmdClearDepthStencilImage = reinterpret_cast<decltype(original_vkCmdClearDepthStencilImage)>(ptr);
        return reinterpret_cast<void *>(vkCmdClearDepthStencilImage);
    }
    if (strcmp("vkCmdClearAttachments", symbol) == 0)
    {
        original_vkCmdClearAttachments = reinterpret_cast<decltype(original_vkCmdClearAttachments)>(ptr);
        return reinterpret_cast<void *>(vkCmdClearAttachments);
    }
    if (strcmp("vkCmdResolveImage", symbol) == 0)
    {
        original_vkCmdResolveImage = reinterpret_cast<decltype(original_vkCmdResolveImage)>(ptr);
        return reinterpret_cast<void *>(vkCmdResolveImage);
    }
    if (strcmp("vkCmdSetEvent", symbol) == 0)
    {
        original_vkCmdSetEvent = reinterpret_cast<decltype(original_vkCmdSetEvent)>(ptr);
        return reinterpret_cast<void *>(vkCmdSetEvent);
    }
    if (strcmp("vkCmdResetEvent", symbol) == 0)
    {
        original_vkCmdResetEvent = reinterpret_cast<decltype(original_vkCmdResetEvent)>(ptr);
        return reinterpret_cast<void *>(vkCmdResetEvent);
    }
    if (strcmp("vkCmdWaitEvents", symbol) == 0)
    {
        original_vkCmdWaitEvents = reinterpret_cast<decltype(original_vkCmdWaitEvents)>(ptr);
        return reinterpret_cast<void *>(vkCmdWaitEvents);
    }
    if (strcmp("vkCmdPipelineBarrier", symbol) == 0)
    {
        original_vkCmdPipelineBarrier = reinterpret_cast<decltype(original_vkCmdPipelineBarrier)>(ptr);
        return reinterpret_cast<void *>(vkCmdPipelineBarrier);
    }
    if (strcmp("vkCmdBeginQuery", symbol) == 0)
    {
        original_vkCmdBeginQuery = reinterpret_cast<decltype(original_vkCmdBeginQuery)>(ptr);
        return reinterpret_cast<void *>(vkCmdBeginQuery);
    }
    if (strcmp("vkCmdEndQuery", symbol) == 0)
    {
        original_vkCmdEndQuery = reinterpret_cast<decltype(original_vkCmdEndQuery)>(ptr);
        return reinterpret_cast<void *>(vkCmdEndQuery);
    }
    if (strcmp("vkCmdResetQueryPool", symbol) == 0)
    {
        original_vkCmdResetQueryPool = reinterpret_cast<decltype(original_vkCmdResetQueryPool)>(ptr);
        return reinterpret_cast<void *>(vkCmdResetQueryPool);
    }
    if (strcmp("vkCmdWriteTimestamp", symbol) == 0)
    {
        original_vkCmdWriteTimestamp = reinterpret_cast<decltype(original_vkCmdWriteTimestamp)>(ptr);
        return reinterpret_cast<void *>(vkCmdWriteTimestamp);
    }
    if (strcmp("vkCmdCopyQueryPoolResults", symbol) == 0)
    {
        original_vkCmdCopyQueryPoolResults = reinterpret_cast<decltype(original_vkCmdCopyQueryPoolResults)>(ptr);
        return reinterpret_cast<void *>(vkCmdCopyQueryPoolResults);
    }
    if (strcmp("vkCmdPushConstants", symbol) == 0)
    {
        original_vkCmdPushConstants = reinterpret_cast<decltype(original_vkCmdPushConstants)>(ptr);
        return reinterpret_cast<void *>(vkCmdPushConstants);
    }
    if (strcmp("vkCmdBeginRenderPass", symbol) == 0)
    {
        original_vkCmdBeginRenderPass = reinterpret_cast<decltype(original_vkCmdBeginRenderPass)>(ptr);
        return reinterpret_cast<void *>(vkCmdBeginRenderPass);
    }
    if (strcmp("vkCmdNextSubpass", symbol) == 0)
    {
        original_vkCmdNextSubpass = reinterpret_cast<decltype(original_vkCmdNextSubpass)>(ptr);
        return reinterpret_cast<void *>(vkCmdNextSubpass);
    }
    if (strcmp("vkCmdEndRenderPass", symbol) == 0)
    {
        original_vkCmdEndRenderPass = reinterpret_cast<decltype(original_vkCmdEndRenderPass)>(ptr);
        return reinterpret_cast<void *>(vkCmdEndRenderPass);
    }
    if (strcmp("vkCmdExecuteCommands", symbol) == 0)
    {
        original_vkCmdExecuteCommands = reinterpret_cast<decltype(original_vkCmdExecuteCommands)>(ptr);
        return reinterpret_cast<void *>(vkCmdExecuteCommands);
    }
    if (strcmp("vkQueuePresentKHR", symbol) == 0)
    {
        original_vkQueuePresentKHR = reinterpret_cast<decltype(original_vkQueuePresentKHR)>(ptr);
        return reinterpret_cast<void *>(vkQueuePresentKHR);
    }
    if (strcmp("vkGetDeviceQueue2", symbol) == 0)
    {
        original_vkGetDeviceQueue2 = reinterpret_cast<decltype(original_vkGetDeviceQueue2)>(ptr);
        return reinterpret_cast<void *>(vkGetDeviceQueue2);
    }
    return ptr;
}
