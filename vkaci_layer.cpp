#include <vulkan/vulkan.h>
#include <vulkan/vk_layer.h>
#include <vulkan/utility/vk_dispatch_table.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <map>
#include <mutex>

#include "throttling_frame_joined.cpp"

std::mutex global_lock;

template<typename DispatchableType>
void *GetKey(DispatchableType inst)
{
  return *(void **)inst;
}

std::map<void *, VkuInstanceDispatchTable> instance_dispatch;
std::map<void *, VkuDeviceDispatchTable> device_dispatch;

#define PUBLIC __attribute__((visibility("default")))
#define GETPROCADDR(func) if(!strcmp(pName, "vk" #func)) return (PFN_vkVoidFunction)&VKACI_##func;


extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance)
{
  std::lock_guard<std::mutex> lg(global_lock);
  VkLayerInstanceCreateInfo *layerCreateInfo = (VkLayerInstanceCreateInfo *)pCreateInfo->pNext;
  while(layerCreateInfo && (layerCreateInfo->sType != VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO || layerCreateInfo->function != VK_LAYER_LINK_INFO))
  {
    layerCreateInfo = (VkLayerInstanceCreateInfo *)layerCreateInfo->pNext;
  }
  if(layerCreateInfo == NULL)
  {
    return VK_ERROR_INITIALIZATION_FAILED;
  }
  PFN_vkGetInstanceProcAddr gpa = layerCreateInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;
  layerCreateInfo->u.pLayerInfo = layerCreateInfo->u.pLayerInfo->pNext;
  PFN_vkCreateInstance createFunc = (PFN_vkCreateInstance)gpa(VK_NULL_HANDLE, "vkCreateInstance");

  VkResult ret = createFunc(pCreateInfo, pAllocator, pInstance);

  VkuInstanceDispatchTable dispatchTable;
  dispatchTable.GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)gpa(*pInstance, "vkGetInstanceProcAddr");
  dispatchTable.DestroyInstance = (PFN_vkDestroyInstance)gpa(*pInstance, "vkDestroyInstance");
  dispatchTable.EnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)gpa(*pInstance, "vkEnumeratePhysicalDevices");
  dispatchTable.GetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)gpa(*pInstance, "vkGetPhysicalDeviceProperties");
  dispatchTable.GetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)gpa(*pInstance, "vkGetPhysicalDeviceQueueFamilyProperties");
  dispatchTable.GetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)gpa(*pInstance, "vkGetPhysicalDeviceMemoryProperties");
  dispatchTable.GetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)gpa(*pInstance, "vkGetPhysicalDeviceFeatures");
  dispatchTable.GetPhysicalDeviceFormatProperties = (PFN_vkGetPhysicalDeviceFormatProperties)gpa(*pInstance, "vkGetPhysicalDeviceFormatProperties");
  dispatchTable.GetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties)gpa(*pInstance, "vkGetPhysicalDeviceImageFormatProperties");
  dispatchTable.EnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)gpa(*pInstance, "vkEnumerateInstanceVersion");
  dispatchTable.EnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)gpa(*pInstance, "vkEnumerateInstanceLayerProperties");
  dispatchTable.EnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)gpa(*pInstance, "vkEnumerateInstanceExtensionProperties");
  dispatchTable.EnumerateDeviceLayerProperties = (PFN_vkEnumerateDeviceLayerProperties)gpa(*pInstance, "vkEnumerateDeviceLayerProperties");
  dispatchTable.EnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)gpa(*pInstance, "vkEnumerateDeviceExtensionProperties");
  dispatchTable.GetPhysicalDeviceSparseImageFormatProperties = (PFN_vkGetPhysicalDeviceSparseImageFormatProperties)gpa(*pInstance, "vkGetPhysicalDeviceSparseImageFormatProperties");
  instance_dispatch[GetKey(*pInstance)] = dispatchTable;
  return VK_SUCCESS;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
  std::lock_guard<std::mutex> lg(global_lock);
  instance_dispatch.erase(GetKey(instance));
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice)
{
  std::lock_guard<std::mutex> lg(global_lock);
  VkLayerDeviceCreateInfo *layerCreateInfo = (VkLayerDeviceCreateInfo *)pCreateInfo->pNext;
  while(layerCreateInfo && (layerCreateInfo->sType != VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO || layerCreateInfo->function != VK_LAYER_LINK_INFO))
  {
    layerCreateInfo = (VkLayerDeviceCreateInfo *)layerCreateInfo->pNext;
  }
  if(layerCreateInfo == NULL)
  {
    return VK_ERROR_INITIALIZATION_FAILED;
  }
  PFN_vkGetInstanceProcAddr gipa = layerCreateInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;
  PFN_vkGetDeviceProcAddr gdpa = layerCreateInfo->u.pLayerInfo->pfnNextGetDeviceProcAddr;
  layerCreateInfo->u.pLayerInfo = layerCreateInfo->u.pLayerInfo->pNext;
  PFN_vkCreateDevice createFunc = (PFN_vkCreateDevice)gipa(VK_NULL_HANDLE, "vkCreateDevice");

  VkResult ret = createFunc(physicalDevice, pCreateInfo, pAllocator, pDevice);

  VkuDeviceDispatchTable dispatchTable;
  dispatchTable.GetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)gdpa(*pDevice, "vkGetDeviceProcAddr");
  dispatchTable.DestroyDevice = (PFN_vkDestroyDevice)gdpa(*pDevice, "vkDestroyDevice");
  dispatchTable.GetDeviceQueue = (PFN_vkGetDeviceQueue)gdpa(*pDevice, "vkGetDeviceQueue");
  dispatchTable.QueueSubmit = (PFN_vkQueueSubmit)gdpa(*pDevice, "vkQueueSubmit");
  dispatchTable.QueueWaitIdle = (PFN_vkQueueWaitIdle)gdpa(*pDevice, "vkQueueWaitIdle");
  dispatchTable.DeviceWaitIdle = (PFN_vkDeviceWaitIdle)gdpa(*pDevice, "vkDeviceWaitIdle");
  dispatchTable.AllocateMemory = (PFN_vkAllocateMemory)gdpa(*pDevice, "vkAllocateMemory");
  dispatchTable.FreeMemory = (PFN_vkFreeMemory)gdpa(*pDevice, "vkFreeMemory");
  dispatchTable.MapMemory = (PFN_vkMapMemory)gdpa(*pDevice, "vkMapMemory");
  dispatchTable.UnmapMemory = (PFN_vkUnmapMemory)gdpa(*pDevice, "vkUnmapMemory");
  dispatchTable.FlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)gdpa(*pDevice, "vkFlushMappedMemoryRanges");
  dispatchTable.InvalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges)gdpa(*pDevice, "vkInvalidateMappedMemoryRanges");
  dispatchTable.GetDeviceMemoryCommitment = (PFN_vkGetDeviceMemoryCommitment)gdpa(*pDevice, "vkGetDeviceMemoryCommitment");
  dispatchTable.GetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)gdpa(*pDevice, "vkGetBufferMemoryRequirements");
  dispatchTable.BindBufferMemory = (PFN_vkBindBufferMemory)gdpa(*pDevice, "vkBindBufferMemory");
  dispatchTable.GetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)gdpa(*pDevice, "vkGetImageMemoryRequirements");
  dispatchTable.BindImageMemory = (PFN_vkBindImageMemory)gdpa(*pDevice, "vkBindImageMemory");
  dispatchTable.GetImageSparseMemoryRequirements = (PFN_vkGetImageSparseMemoryRequirements)gdpa(*pDevice, "vkGetImageSparseMemoryRequirements");
  dispatchTable.QueueBindSparse = (PFN_vkQueueBindSparse)gdpa(*pDevice, "vkQueueBindSparse");
  dispatchTable.CreateFence = (PFN_vkCreateFence)gdpa(*pDevice, "vkCreateFence");
  dispatchTable.DestroyFence = (PFN_vkDestroyFence)gdpa(*pDevice, "vkDestroyFence");
  dispatchTable.ResetFences = (PFN_vkResetFences)gdpa(*pDevice, "vkResetFences");
  dispatchTable.GetFenceStatus = (PFN_vkGetFenceStatus)gdpa(*pDevice, "vkGetFenceStatus");
  dispatchTable.WaitForFences = (PFN_vkWaitForFences)gdpa(*pDevice, "vkWaitForFences");
  dispatchTable.CreateSemaphore = (PFN_vkCreateSemaphore)gdpa(*pDevice, "vkCreateSemaphore");
  dispatchTable.DestroySemaphore = (PFN_vkDestroySemaphore)gdpa(*pDevice, "vkDestroySemaphore");
  dispatchTable.CreateEvent = (PFN_vkCreateEvent)gdpa(*pDevice, "vkCreateEvent");
  dispatchTable.DestroyEvent = (PFN_vkDestroyEvent)gdpa(*pDevice, "vkDestroyEvent");
  dispatchTable.GetEventStatus = (PFN_vkGetEventStatus)gdpa(*pDevice, "vkGetEventStatus");
  dispatchTable.SetEvent = (PFN_vkSetEvent)gdpa(*pDevice, "vkSetEvent");
  dispatchTable.ResetEvent = (PFN_vkResetEvent)gdpa(*pDevice, "vkResetEvent");
  dispatchTable.CreateQueryPool = (PFN_vkCreateQueryPool)gdpa(*pDevice, "vkCreateQueryPool");
  dispatchTable.DestroyQueryPool = (PFN_vkDestroyQueryPool)gdpa(*pDevice, "vkDestroyQueryPool");
  dispatchTable.GetQueryPoolResults = (PFN_vkGetQueryPoolResults)gdpa(*pDevice, "vkGetQueryPoolResults");
  dispatchTable.CreateBuffer = (PFN_vkCreateBuffer)gdpa(*pDevice, "vkCreateBuffer");
  dispatchTable.DestroyBuffer = (PFN_vkDestroyBuffer)gdpa(*pDevice, "vkDestroyBuffer");
  dispatchTable.CreateBufferView = (PFN_vkCreateBufferView)gdpa(*pDevice, "vkCreateBufferView");
  dispatchTable.DestroyBufferView = (PFN_vkDestroyBufferView)gdpa(*pDevice, "vkDestroyBufferView");
  dispatchTable.CreateImage = (PFN_vkCreateImage)gdpa(*pDevice, "vkCreateImage");
  dispatchTable.DestroyImage = (PFN_vkDestroyImage)gdpa(*pDevice, "vkDestroyImage");
  dispatchTable.GetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout)gdpa(*pDevice, "vkGetImageSubresourceLayout");
  dispatchTable.CreateImageView = (PFN_vkCreateImageView)gdpa(*pDevice, "vkCreateImageView");
  dispatchTable.DestroyImageView = (PFN_vkDestroyImageView)gdpa(*pDevice, "vkDestroyImageView");
  dispatchTable.CreateShaderModule = (PFN_vkCreateShaderModule)gdpa(*pDevice, "vkCreateShaderModule");
  dispatchTable.DestroyShaderModule = (PFN_vkDestroyShaderModule)gdpa(*pDevice, "vkDestroyShaderModule");
  dispatchTable.CreatePipelineCache = (PFN_vkCreatePipelineCache)gdpa(*pDevice, "vkCreatePipelineCache");
  dispatchTable.DestroyPipelineCache = (PFN_vkDestroyPipelineCache)gdpa(*pDevice, "vkDestroyPipelineCache");
  dispatchTable.GetPipelineCacheData = (PFN_vkGetPipelineCacheData)gdpa(*pDevice, "vkGetPipelineCacheData");
  dispatchTable.MergePipelineCaches = (PFN_vkMergePipelineCaches)gdpa(*pDevice, "vkMergePipelineCaches");
  dispatchTable.CreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)gdpa(*pDevice, "vkCreateGraphicsPipelines");
  dispatchTable.CreateComputePipelines = (PFN_vkCreateComputePipelines)gdpa(*pDevice, "vkCreateComputePipelines");
  dispatchTable.DestroyPipeline = (PFN_vkDestroyPipeline)gdpa(*pDevice, "vkDestroyPipeline");
  dispatchTable.CreatePipelineLayout = (PFN_vkCreatePipelineLayout)gdpa(*pDevice, "vkCreatePipelineLayout");
  dispatchTable.DestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)gdpa(*pDevice, "vkDestroyPipelineLayout");
  dispatchTable.CreateSampler = (PFN_vkCreateSampler)gdpa(*pDevice, "vkCreateSampler");
  dispatchTable.DestroySampler = (PFN_vkDestroySampler)gdpa(*pDevice, "vkDestroySampler");
  dispatchTable.CreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)gdpa(*pDevice, "vkCreateDescriptorSetLayout");
  dispatchTable.DestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)gdpa(*pDevice, "vkDestroyDescriptorSetLayout");
  dispatchTable.CreateDescriptorPool = (PFN_vkCreateDescriptorPool)gdpa(*pDevice, "vkCreateDescriptorPool");
  dispatchTable.DestroyDescriptorPool = (PFN_vkDestroyDescriptorPool)gdpa(*pDevice, "vkDestroyDescriptorPool");
  dispatchTable.ResetDescriptorPool = (PFN_vkResetDescriptorPool)gdpa(*pDevice, "vkResetDescriptorPool");
  dispatchTable.AllocateDescriptorSets = (PFN_vkAllocateDescriptorSets)gdpa(*pDevice, "vkAllocateDescriptorSets");
  dispatchTable.FreeDescriptorSets = (PFN_vkFreeDescriptorSets)gdpa(*pDevice, "vkFreeDescriptorSets");
  dispatchTable.UpdateDescriptorSets = (PFN_vkUpdateDescriptorSets)gdpa(*pDevice, "vkUpdateDescriptorSets");
  dispatchTable.CreateFramebuffer = (PFN_vkCreateFramebuffer)gdpa(*pDevice, "vkCreateFramebuffer");
  dispatchTable.DestroyFramebuffer = (PFN_vkDestroyFramebuffer)gdpa(*pDevice, "vkDestroyFramebuffer");
  dispatchTable.CreateRenderPass = (PFN_vkCreateRenderPass)gdpa(*pDevice, "vkCreateRenderPass");
  dispatchTable.DestroyRenderPass = (PFN_vkDestroyRenderPass)gdpa(*pDevice, "vkDestroyRenderPass");
  dispatchTable.GetRenderAreaGranularity = (PFN_vkGetRenderAreaGranularity)gdpa(*pDevice, "vkGetRenderAreaGranularity");
  dispatchTable.CreateCommandPool = (PFN_vkCreateCommandPool)gdpa(*pDevice, "vkCreateCommandPool");
  dispatchTable.DestroyCommandPool = (PFN_vkDestroyCommandPool)gdpa(*pDevice, "vkDestroyCommandPool");
  dispatchTable.ResetCommandPool = (PFN_vkResetCommandPool)gdpa(*pDevice, "vkResetCommandPool");
  dispatchTable.AllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)gdpa(*pDevice, "vkAllocateCommandBuffers");
  dispatchTable.FreeCommandBuffers = (PFN_vkFreeCommandBuffers)gdpa(*pDevice, "vkFreeCommandBuffers");
  dispatchTable.BeginCommandBuffer = (PFN_vkBeginCommandBuffer)gdpa(*pDevice, "vkBeginCommandBuffer");
  dispatchTable.EndCommandBuffer = (PFN_vkEndCommandBuffer)gdpa(*pDevice, "vkEndCommandBuffer");
  dispatchTable.ResetCommandBuffer = (PFN_vkResetCommandBuffer)gdpa(*pDevice, "vkResetCommandBuffer");
  dispatchTable.CmdBindPipeline = (PFN_vkCmdBindPipeline)gdpa(*pDevice, "vkCmdBindPipeline");
  dispatchTable.CmdSetViewport = (PFN_vkCmdSetViewport)gdpa(*pDevice, "vkCmdSetViewport");
  dispatchTable.CmdSetScissor = (PFN_vkCmdSetScissor)gdpa(*pDevice, "vkCmdSetScissor");
  dispatchTable.CmdSetLineWidth = (PFN_vkCmdSetLineWidth)gdpa(*pDevice, "vkCmdSetLineWidth");
  dispatchTable.CmdSetDepthBias = (PFN_vkCmdSetDepthBias)gdpa(*pDevice, "vkCmdSetDepthBias");
  dispatchTable.CmdSetBlendConstants = (PFN_vkCmdSetBlendConstants)gdpa(*pDevice, "vkCmdSetBlendConstants");
  dispatchTable.CmdSetDepthBounds = (PFN_vkCmdSetDepthBounds)gdpa(*pDevice, "vkCmdSetDepthBounds");
  dispatchTable.CmdSetStencilCompareMask = (PFN_vkCmdSetStencilCompareMask)gdpa(*pDevice, "vkCmdSetStencilCompareMask");
  dispatchTable.CmdSetStencilWriteMask = (PFN_vkCmdSetStencilWriteMask)gdpa(*pDevice, "vkCmdSetStencilWriteMask");
  dispatchTable.CmdSetStencilReference = (PFN_vkCmdSetStencilReference)gdpa(*pDevice, "vkCmdSetStencilReference");
  dispatchTable.CmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)gdpa(*pDevice, "vkCmdBindDescriptorSets");
  dispatchTable.CmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)gdpa(*pDevice, "vkCmdBindIndexBuffer");
  dispatchTable.CmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)gdpa(*pDevice, "vkCmdBindVertexBuffers");
  dispatchTable.CmdDraw = (PFN_vkCmdDraw)gdpa(*pDevice, "vkCmdDraw");
  dispatchTable.CmdDrawIndexed = (PFN_vkCmdDrawIndexed)gdpa(*pDevice, "vkCmdDrawIndexed");
  dispatchTable.CmdDrawIndirect = (PFN_vkCmdDrawIndirect)gdpa(*pDevice, "vkCmdDrawIndirect");
  dispatchTable.CmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect)gdpa(*pDevice, "vkCmdDrawIndexedIndirect");
  dispatchTable.CmdDispatch = (PFN_vkCmdDispatch)gdpa(*pDevice, "vkCmdDispatch");
  dispatchTable.CmdDispatchIndirect = (PFN_vkCmdDispatchIndirect)gdpa(*pDevice, "vkCmdDispatchIndirect");
  dispatchTable.CmdCopyBuffer = (PFN_vkCmdCopyBuffer)gdpa(*pDevice, "vkCmdCopyBuffer");
  dispatchTable.CmdCopyImage = (PFN_vkCmdCopyImage)gdpa(*pDevice, "vkCmdCopyImage");
  dispatchTable.CmdBlitImage = (PFN_vkCmdBlitImage)gdpa(*pDevice, "vkCmdBlitImage");
  dispatchTable.CmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)gdpa(*pDevice, "vkCmdCopyBufferToImage");
  dispatchTable.CmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer)gdpa(*pDevice, "vkCmdCopyImageToBuffer");
  dispatchTable.CmdUpdateBuffer = (PFN_vkCmdUpdateBuffer)gdpa(*pDevice, "vkCmdUpdateBuffer");
  dispatchTable.CmdFillBuffer = (PFN_vkCmdFillBuffer)gdpa(*pDevice, "vkCmdFillBuffer");
  dispatchTable.CmdClearColorImage = (PFN_vkCmdClearColorImage)gdpa(*pDevice, "vkCmdClearColorImage");
  dispatchTable.CmdClearDepthStencilImage = (PFN_vkCmdClearDepthStencilImage)gdpa(*pDevice, "vkCmdClearDepthStencilImage");
  dispatchTable.CmdClearAttachments = (PFN_vkCmdClearAttachments)gdpa(*pDevice, "vkCmdClearAttachments");
  dispatchTable.CmdResolveImage = (PFN_vkCmdResolveImage)gdpa(*pDevice, "vkCmdResolveImage");
  dispatchTable.CmdSetEvent = (PFN_vkCmdSetEvent)gdpa(*pDevice, "vkCmdSetEvent");
  dispatchTable.CmdResetEvent = (PFN_vkCmdResetEvent)gdpa(*pDevice, "vkCmdResetEvent");
  dispatchTable.CmdWaitEvents = (PFN_vkCmdWaitEvents)gdpa(*pDevice, "vkCmdWaitEvents");
  dispatchTable.CmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)gdpa(*pDevice, "vkCmdPipelineBarrier");
  dispatchTable.CmdBeginQuery = (PFN_vkCmdBeginQuery)gdpa(*pDevice, "vkCmdBeginQuery");
  dispatchTable.CmdEndQuery = (PFN_vkCmdEndQuery)gdpa(*pDevice, "vkCmdEndQuery");
  dispatchTable.CmdResetQueryPool = (PFN_vkCmdResetQueryPool)gdpa(*pDevice, "vkCmdResetQueryPool");
  dispatchTable.CmdWriteTimestamp = (PFN_vkCmdWriteTimestamp)gdpa(*pDevice, "vkCmdWriteTimestamp");
  dispatchTable.CmdCopyQueryPoolResults = (PFN_vkCmdCopyQueryPoolResults)gdpa(*pDevice, "vkCmdCopyQueryPoolResults");
  dispatchTable.CmdPushConstants = (PFN_vkCmdPushConstants)gdpa(*pDevice, "vkCmdPushConstants");
  dispatchTable.CmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)gdpa(*pDevice, "vkCmdBeginRenderPass");
  dispatchTable.CmdNextSubpass = (PFN_vkCmdNextSubpass)gdpa(*pDevice, "vkCmdNextSubpass");
  dispatchTable.CmdEndRenderPass = (PFN_vkCmdEndRenderPass)gdpa(*pDevice, "vkCmdEndRenderPass");
  dispatchTable.CmdExecuteCommands = (PFN_vkCmdExecuteCommands)gdpa(*pDevice, "vkCmdExecuteCommands");
  dispatchTable.QueuePresentKHR = (PFN_vkQueuePresentKHR)gdpa(*pDevice, "vkQueuePresentKHR");
  dispatchTable.GetDeviceQueue2 = (PFN_vkGetDeviceQueue2)gdpa(*pDevice, "vkGetDeviceQueue2");
  device_dispatch[GetKey(*pDevice)] = dispatchTable;
  return VK_SUCCESS;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator)
{
  std::lock_guard<std::mutex> lg(global_lock);
  device_dispatch.erase(GetKey(device));
}
extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_EnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkEnumeratePhysicalDevices_entry\n");
  VkResult _result = instance_dispatch[GetKey(instance)].EnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);
  //printf("vkEnumeratePhysicalDevices_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetPhysicalDeviceProperties_entry\n");
  instance_dispatch[GetKey(physicalDevice)].GetPhysicalDeviceProperties(physicalDevice, pProperties);
  //printf("vkGetPhysicalDeviceProperties_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_GetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetPhysicalDeviceQueueFamilyProperties_entry\n");
  instance_dispatch[GetKey(physicalDevice)].GetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
  //printf("vkGetPhysicalDeviceQueueFamilyProperties_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_GetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetPhysicalDeviceMemoryProperties_entry\n");
  instance_dispatch[GetKey(physicalDevice)].GetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
  //printf("vkGetPhysicalDeviceMemoryProperties_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_GetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetPhysicalDeviceFeatures_entry\n");
  instance_dispatch[GetKey(physicalDevice)].GetPhysicalDeviceFeatures(physicalDevice, pFeatures);
  //printf("vkGetPhysicalDeviceFeatures_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_GetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties* pFormatProperties) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetPhysicalDeviceFormatProperties_entry\n");
  instance_dispatch[GetKey(physicalDevice)].GetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
  //printf("vkGetPhysicalDeviceFormatProperties_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_GetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetPhysicalDeviceImageFormatProperties_entry\n");
  VkResult _result = instance_dispatch[GetKey(physicalDevice)].GetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
  //printf("vkGetPhysicalDeviceImageFormatProperties_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_EnumerateInstanceVersion(uint32_t* pApiVersion) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkEnumerateInstanceVersion_entry\n");
  VkResult _result = instance_dispatch[GetKey(pApiVersion)].EnumerateInstanceVersion(pApiVersion);
  //printf("vkEnumerateInstanceVersion_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_EnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkEnumerateInstanceLayerProperties_entry\n");
  VkResult _result = instance_dispatch[GetKey(pPropertyCount)].EnumerateInstanceLayerProperties(pPropertyCount, pProperties);
  //printf("vkEnumerateInstanceLayerProperties_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_EnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkEnumerateInstanceExtensionProperties_entry\n");
  VkResult _result = instance_dispatch[GetKey(pLayerName)].EnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
  //printf("vkEnumerateInstanceExtensionProperties_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_EnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t* pPropertyCount, VkLayerProperties* pProperties) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkEnumerateDeviceLayerProperties_entry\n");
  VkResult _result = instance_dispatch[GetKey(physicalDevice)].EnumerateDeviceLayerProperties(physicalDevice, pPropertyCount, pProperties);
  //printf("vkEnumerateDeviceLayerProperties_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_EnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkEnumerateDeviceExtensionProperties_entry\n");
  VkResult _result = instance_dispatch[GetKey(physicalDevice)].EnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pPropertyCount, pProperties);
  //printf("vkEnumerateDeviceExtensionProperties_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_GetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetDeviceQueue_entry\n");
  device_dispatch[GetKey(device)].GetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
  //printf("vkGetDeviceQueue_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_QueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkQueueSubmit_entry\n");
  VkResult _result = device_dispatch[GetKey(queue)].QueueSubmit(queue, submitCount, pSubmits, fence);
  //printf("vkQueueSubmit_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_QueueWaitIdle(VkQueue queue) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkQueueWaitIdle_entry\n");
  VkResult _result = device_dispatch[GetKey(queue)].QueueWaitIdle(queue);
  //printf("vkQueueWaitIdle_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_DeviceWaitIdle(VkDevice device) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDeviceWaitIdle_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].DeviceWaitIdle(device);
  //printf("vkDeviceWaitIdle_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_AllocateMemory(VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkAllocateMemory_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].AllocateMemory(device, pAllocateInfo, pAllocator, pMemory);
  //printf("vkAllocateMemory_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_FreeMemory(VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkFreeMemory_entry\n");
  device_dispatch[GetKey(device)].FreeMemory(device, memory, pAllocator);
  //printf("vkFreeMemory_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_MapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkMapMemory_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].MapMemory(device, memory, offset, size, flags, ppData);
  //printf("vkMapMemory_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_UnmapMemory(VkDevice device, VkDeviceMemory memory) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkUnmapMemory_entry\n");
  device_dispatch[GetKey(device)].UnmapMemory(device, memory);
  //printf("vkUnmapMemory_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_FlushMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkFlushMappedMemoryRanges_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].FlushMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
  //printf("vkFlushMappedMemoryRanges_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_InvalidateMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkInvalidateMappedMemoryRanges_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].InvalidateMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
  //printf("vkInvalidateMappedMemoryRanges_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_GetDeviceMemoryCommitment(VkDevice device, VkDeviceMemory memory, VkDeviceSize* pCommittedMemoryInBytes) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetDeviceMemoryCommitment_entry\n");
  device_dispatch[GetKey(device)].GetDeviceMemoryCommitment(device, memory, pCommittedMemoryInBytes);
  //printf("vkGetDeviceMemoryCommitment_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_GetBufferMemoryRequirements(VkDevice device, VkBuffer buffer, VkMemoryRequirements* pMemoryRequirements) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetBufferMemoryRequirements_entry\n");
  device_dispatch[GetKey(device)].GetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
  //printf("vkGetBufferMemoryRequirements_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_BindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkBindBufferMemory_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].BindBufferMemory(device, buffer, memory, memoryOffset);
  //printf("vkBindBufferMemory_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_GetImageMemoryRequirements(VkDevice device, VkImage image, VkMemoryRequirements* pMemoryRequirements) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetImageMemoryRequirements_entry\n");
  device_dispatch[GetKey(device)].GetImageMemoryRequirements(device, image, pMemoryRequirements);
  //printf("vkGetImageMemoryRequirements_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_BindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkBindImageMemory_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].BindImageMemory(device, image, memory, memoryOffset);
  //printf("vkBindImageMemory_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_GetImageSparseMemoryRequirements(VkDevice device, VkImage image, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements* pSparseMemoryRequirements) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetImageSparseMemoryRequirements_entry\n");
  device_dispatch[GetKey(device)].GetImageSparseMemoryRequirements(device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
  //printf("vkGetImageSparseMemoryRequirements_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_GetPhysicalDeviceSparseImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling, uint32_t* pPropertyCount, VkSparseImageFormatProperties* pProperties) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetPhysicalDeviceSparseImageFormatProperties_entry\n");
  instance_dispatch[GetKey(physicalDevice)].GetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
  //printf("vkGetPhysicalDeviceSparseImageFormatProperties_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_QueueBindSparse(VkQueue queue, uint32_t bindInfoCount, const VkBindSparseInfo* pBindInfo, VkFence fence) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkQueueBindSparse_entry\n");
  VkResult _result = device_dispatch[GetKey(queue)].QueueBindSparse(queue, bindInfoCount, pBindInfo, fence);
  //printf("vkQueueBindSparse_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateFence(VkDevice device, const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateFence_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateFence(device, pCreateInfo, pAllocator, pFence);
  //printf("vkCreateFence_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyFence(VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroyFence_entry\n");
  device_dispatch[GetKey(device)].DestroyFence(device, fence, pAllocator);
  //printf("vkDestroyFence_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_ResetFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkResetFences_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].ResetFences(device, fenceCount, pFences);
  //printf("vkResetFences_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_GetFenceStatus(VkDevice device, VkFence fence) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetFenceStatus_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].GetFenceStatus(device, fence);
  //printf("vkGetFenceStatus_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_WaitForFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkWaitForFences_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].WaitForFences(device, fenceCount, pFences, waitAll, timeout);
  //printf("vkWaitForFences_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateSemaphore_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore);
  //printf("vkCreateSemaphore_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroySemaphore(VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroySemaphore_entry\n");
  device_dispatch[GetKey(device)].DestroySemaphore(device, semaphore, pAllocator);
  //printf("vkDestroySemaphore_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateEvent(VkDevice device, const VkEventCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkEvent* pEvent) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateEvent_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateEvent(device, pCreateInfo, pAllocator, pEvent);
  //printf("vkCreateEvent_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyEvent(VkDevice device, VkEvent event, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroyEvent_entry\n");
  device_dispatch[GetKey(device)].DestroyEvent(device, event, pAllocator);
  //printf("vkDestroyEvent_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_GetEventStatus(VkDevice device, VkEvent event) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetEventStatus_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].GetEventStatus(device, event);
  //printf("vkGetEventStatus_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_SetEvent(VkDevice device, VkEvent event) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkSetEvent_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].SetEvent(device, event);
  //printf("vkSetEvent_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_ResetEvent(VkDevice device, VkEvent event) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkResetEvent_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].ResetEvent(device, event);
  //printf("vkResetEvent_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateQueryPool(VkDevice device, const VkQueryPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkQueryPool* pQueryPool) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateQueryPool_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateQueryPool(device, pCreateInfo, pAllocator, pQueryPool);
  //printf("vkCreateQueryPool_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyQueryPool(VkDevice device, VkQueryPool queryPool, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroyQueryPool_entry\n");
  device_dispatch[GetKey(device)].DestroyQueryPool(device, queryPool, pAllocator);
  //printf("vkDestroyQueryPool_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_GetQueryPoolResults(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, size_t dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetQueryPoolResults_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].GetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
  //printf("vkGetQueryPoolResults_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateBuffer(VkDevice device, const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateBuffer_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
  //printf("vkCreateBuffer_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyBuffer(VkDevice device, VkBuffer buffer, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroyBuffer_entry\n");
  device_dispatch[GetKey(device)].DestroyBuffer(device, buffer, pAllocator);
  //printf("vkDestroyBuffer_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateBufferView(VkDevice device, const VkBufferViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBufferView* pView) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateBufferView_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateBufferView(device, pCreateInfo, pAllocator, pView);
  //printf("vkCreateBufferView_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyBufferView(VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroyBufferView_entry\n");
  device_dispatch[GetKey(device)].DestroyBufferView(device, bufferView, pAllocator);
  //printf("vkDestroyBufferView_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateImage(VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateImage_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateImage(device, pCreateInfo, pAllocator, pImage);
  //printf("vkCreateImage_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroyImage_entry\n");
  device_dispatch[GetKey(device)].DestroyImage(device, image, pAllocator);
  //printf("vkDestroyImage_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_GetImageSubresourceLayout(VkDevice device, VkImage image, const VkImageSubresource* pSubresource, VkSubresourceLayout* pLayout) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetImageSubresourceLayout_entry\n");
  device_dispatch[GetKey(device)].GetImageSubresourceLayout(device, image, pSubresource, pLayout);
  //printf("vkGetImageSubresourceLayout_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateImageView(VkDevice device, const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pView) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateImageView_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateImageView(device, pCreateInfo, pAllocator, pView);
  //printf("vkCreateImageView_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyImageView(VkDevice device, VkImageView imageView, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroyImageView_entry\n");
  device_dispatch[GetKey(device)].DestroyImageView(device, imageView, pAllocator);
  //printf("vkDestroyImageView_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateShaderModule_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule);
  //printf("vkCreateShaderModule_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroyShaderModule_entry\n");
  device_dispatch[GetKey(device)].DestroyShaderModule(device, shaderModule, pAllocator);
  //printf("vkDestroyShaderModule_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineCache* pPipelineCache) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreatePipelineCache_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreatePipelineCache(device, pCreateInfo, pAllocator, pPipelineCache);
  //printf("vkCreatePipelineCache_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroyPipelineCache_entry\n");
  device_dispatch[GetKey(device)].DestroyPipelineCache(device, pipelineCache, pAllocator);
  //printf("vkDestroyPipelineCache_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_GetPipelineCacheData(VkDevice device, VkPipelineCache pipelineCache, size_t* pDataSize, void* pData) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetPipelineCacheData_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].GetPipelineCacheData(device, pipelineCache, pDataSize, pData);
  //printf("vkGetPipelineCacheData_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_MergePipelineCaches(VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount, const VkPipelineCache* pSrcCaches) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkMergePipelineCaches_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].MergePipelineCaches(device, dstCache, srcCacheCount, pSrcCaches);
  //printf("vkMergePipelineCaches_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateGraphicsPipelines_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
  //printf("vkCreateGraphicsPipelines_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateComputePipelines_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
  //printf("vkCreateComputePipelines_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyPipeline(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroyPipeline_entry\n");
  device_dispatch[GetKey(device)].DestroyPipeline(device, pipeline, pAllocator);
  //printf("vkDestroyPipeline_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreatePipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineLayout* pPipelineLayout) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreatePipelineLayout_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout);
  //printf("vkCreatePipelineLayout_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroyPipelineLayout_entry\n");
  device_dispatch[GetKey(device)].DestroyPipelineLayout(device, pipelineLayout, pAllocator);
  //printf("vkDestroyPipelineLayout_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateSampler(VkDevice device, const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSampler* pSampler) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateSampler_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateSampler(device, pCreateInfo, pAllocator, pSampler);
  //printf("vkCreateSampler_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroySampler(VkDevice device, VkSampler sampler, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroySampler_entry\n");
  device_dispatch[GetKey(device)].DestroySampler(device, sampler, pAllocator);
  //printf("vkDestroySampler_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateDescriptorSetLayout_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout);
  //printf("vkCreateDescriptorSetLayout_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroyDescriptorSetLayout_entry\n");
  device_dispatch[GetKey(device)].DestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
  //printf("vkDestroyDescriptorSetLayout_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorPool* pDescriptorPool) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateDescriptorPool_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);
  //printf("vkCreateDescriptorPool_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroyDescriptorPool_entry\n");
  device_dispatch[GetKey(device)].DestroyDescriptorPool(device, descriptorPool, pAllocator);
  //printf("vkDestroyDescriptorPool_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_ResetDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkResetDescriptorPool_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].ResetDescriptorPool(device, descriptorPool, flags);
  //printf("vkResetDescriptorPool_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_AllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkAllocateDescriptorSets_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].AllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);
  //printf("vkAllocateDescriptorSets_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_FreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkFreeDescriptorSets_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].FreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);
  //printf("vkFreeDescriptorSets_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_UpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkUpdateDescriptorSets_entry\n");
  device_dispatch[GetKey(device)].UpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
  //printf("vkUpdateDescriptorSets_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateFramebuffer(VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateFramebuffer_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer);
  //printf("vkCreateFramebuffer_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroyFramebuffer_entry\n");
  device_dispatch[GetKey(device)].DestroyFramebuffer(device, framebuffer, pAllocator);
  //printf("vkDestroyFramebuffer_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateRenderPass(VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateRenderPass_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass);
  //printf("vkCreateRenderPass_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyRenderPass(VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroyRenderPass_entry\n");
  device_dispatch[GetKey(device)].DestroyRenderPass(device, renderPass, pAllocator);
  //printf("vkDestroyRenderPass_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_GetRenderAreaGranularity(VkDevice device, VkRenderPass renderPass, VkExtent2D* pGranularity) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetRenderAreaGranularity_entry\n");
  device_dispatch[GetKey(device)].GetRenderAreaGranularity(device, renderPass, pGranularity);
  //printf("vkGetRenderAreaGranularity_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_CreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCreateCommandPool_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].CreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);
  //printf("vkCreateCommandPool_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_DestroyCommandPool(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkDestroyCommandPool_entry\n");
  device_dispatch[GetKey(device)].DestroyCommandPool(device, commandPool, pAllocator);
  //printf("vkDestroyCommandPool_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_ResetCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkResetCommandPool_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].ResetCommandPool(device, commandPool, flags);
  //printf("vkResetCommandPool_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_AllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkAllocateCommandBuffers_entry\n");
  VkResult _result = device_dispatch[GetKey(device)].AllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);
  //printf("vkAllocateCommandBuffers_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_FreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkFreeCommandBuffers_entry\n");
  device_dispatch[GetKey(device)].FreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
  //printf("vkFreeCommandBuffers_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_BeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkBeginCommandBuffer_entry\n");
  VkResult _result = device_dispatch[GetKey(commandBuffer)].BeginCommandBuffer(commandBuffer, pBeginInfo);
  //printf("vkBeginCommandBuffer_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_EndCommandBuffer(VkCommandBuffer commandBuffer) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkEndCommandBuffer_entry\n");
  VkResult _result = device_dispatch[GetKey(commandBuffer)].EndCommandBuffer(commandBuffer);
  //printf("vkEndCommandBuffer_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_ResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkResetCommandBuffer_entry\n");
  VkResult _result = device_dispatch[GetKey(commandBuffer)].ResetCommandBuffer(commandBuffer, flags);
  //printf("vkResetCommandBuffer_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdBindPipeline_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
  //printf("vkCmdBindPipeline_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdSetViewport_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
  //printf("vkCmdSetViewport_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdSetScissor_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
  //printf("vkCmdSetScissor_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdSetLineWidth_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdSetLineWidth(commandBuffer, lineWidth);
  //printf("vkCmdSetLineWidth_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdSetDepthBias(VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdSetDepthBias_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
  //printf("vkCmdSetDepthBias_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdSetBlendConstants(VkCommandBuffer commandBuffer, const float blendConstants[4]) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdSetBlendConstants_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdSetBlendConstants(commandBuffer, blendConstants);
  //printf("vkCmdSetBlendConstants_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdSetDepthBounds(VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdSetDepthBounds_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
  //printf("vkCmdSetDepthBounds_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdSetStencilCompareMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t compareMask) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdSetStencilCompareMask_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
  //printf("vkCmdSetStencilCompareMask_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdSetStencilWriteMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t writeMask) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdSetStencilWriteMask_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
  //printf("vkCmdSetStencilWriteMask_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdSetStencilReference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t reference) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdSetStencilReference_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdSetStencilReference(commandBuffer, faceMask, reference);
  //printf("vkCmdSetStencilReference_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdBindDescriptorSets_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
  //printf("vkCmdBindDescriptorSets_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdBindIndexBuffer_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
  //printf("vkCmdBindIndexBuffer_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdBindVertexBuffers_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
  //printf("vkCmdBindVertexBuffers_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdDraw_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
  //printf("vkCmdDraw_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdDrawIndexed_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
  //printf("vkCmdDrawIndexed_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdDrawIndirect_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
  //printf("vkCmdDrawIndirect_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdDrawIndexedIndirect_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
  //printf("vkCmdDrawIndexedIndirect_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdDispatch(VkCommandBuffer commandBuffer, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdDispatch_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
  //printf("vkCmdDispatch_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdDispatchIndirect_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdDispatchIndirect(commandBuffer, buffer, offset);
  //printf("vkCmdDispatchIndirect_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdCopyBuffer_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
  //printf("vkCmdCopyBuffer_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy* pRegions) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdCopyImage_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
  //printf("vkCmdCopyImage_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdBlitImage_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
  //printf("vkCmdBlitImage_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdCopyBufferToImage_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
  //printf("vkCmdCopyBufferToImage_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy* pRegions) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdCopyImageToBuffer_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
  //printf("vkCmdCopyImageToBuffer_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdUpdateBuffer_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
  //printf("vkCmdUpdateBuffer_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdFillBuffer_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
  //printf("vkCmdFillBuffer_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint32_t rangeCount, const VkImageSubresourceRange* pRanges) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdClearColorImage_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
  //printf("vkCmdClearColorImage_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange* pRanges) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdClearDepthStencilImage_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
  //printf("vkCmdClearDepthStencilImage_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdClearAttachments(VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkClearAttachment* pAttachments, uint32_t rectCount, const VkClearRect* pRects) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdClearAttachments_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
  //printf("vkCmdClearAttachments_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdResolveImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve* pRegions) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdResolveImage_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
  //printf("vkCmdResolveImage_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdSetEvent_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdSetEvent(commandBuffer, event, stageMask);
  //printf("vkCmdSetEvent_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdResetEvent_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdResetEvent(commandBuffer, event, stageMask);
  //printf("vkCmdResetEvent_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdWaitEvents_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
  //printf("vkCmdWaitEvents_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdPipelineBarrier_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
  //printf("vkCmdPipelineBarrier_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdBeginQuery_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdBeginQuery(commandBuffer, queryPool, query, flags);
  //printf("vkCmdBeginQuery_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdEndQuery_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdEndQuery(commandBuffer, queryPool, query);
  //printf("vkCmdEndQuery_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdResetQueryPool_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
  //printf("vkCmdResetQueryPool_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdWriteTimestamp_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
  //printf("vkCmdWriteTimestamp_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdCopyQueryPoolResults_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
  //printf("vkCmdCopyQueryPoolResults_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdPushConstants_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
  //printf("vkCmdPushConstants_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdBeginRenderPass_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
  //printf("vkCmdBeginRenderPass_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents contents) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdNextSubpass_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdNextSubpass(commandBuffer, contents);
  //printf("vkCmdNextSubpass_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdEndRenderPass(VkCommandBuffer commandBuffer) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdEndRenderPass_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdEndRenderPass(commandBuffer);
  //printf("vkCmdEndRenderPass_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_CmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkCmdExecuteCommands_entry\n");
  device_dispatch[GetKey(commandBuffer)].CmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
  //printf("vkCmdExecuteCommands_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR VkResult VKAPI_CALL VKACI_QueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) {
  std::lock_guard<std::mutex> lg(global_lock);
  
    onSwapCompleted();
    //printf("vkQueuePresentKHR_entry\n");
  VkResult _result = device_dispatch[GetKey(queue)].QueuePresentKHR(queue, pPresentInfo);
  //printf("vkQueuePresentKHR_exit\n");
  return _result;
}

extern "C" PUBLIC
VKAPI_ATTR void VKAPI_CALL VKACI_GetDeviceQueue2(VkDevice device, const VkDeviceQueueInfo2* pQueueInfo, VkQueue* pQueue) {
  std::lock_guard<std::mutex> lg(global_lock);
  //printf("vkGetDeviceQueue2_entry\n");
  device_dispatch[GetKey(device)].GetDeviceQueue2(device, pQueueInfo, pQueue);
  //printf("vkGetDeviceQueue2_exit\n");
}

extern "C" PUBLIC
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL VKACI_GetDeviceProcAddr(VkDevice device, const char* pName)
{
  GETPROCADDR(GetDeviceProcAddr);
  GETPROCADDR(DestroyDevice);
  GETPROCADDR(GetDeviceQueue);
  GETPROCADDR(QueueSubmit);
  GETPROCADDR(QueueWaitIdle);
  GETPROCADDR(DeviceWaitIdle);
  GETPROCADDR(AllocateMemory);
  GETPROCADDR(FreeMemory);
  GETPROCADDR(MapMemory);
  GETPROCADDR(UnmapMemory);
  GETPROCADDR(FlushMappedMemoryRanges);
  GETPROCADDR(InvalidateMappedMemoryRanges);
  GETPROCADDR(GetDeviceMemoryCommitment);
  GETPROCADDR(GetBufferMemoryRequirements);
  GETPROCADDR(BindBufferMemory);
  GETPROCADDR(GetImageMemoryRequirements);
  GETPROCADDR(BindImageMemory);
  GETPROCADDR(GetImageSparseMemoryRequirements);
  GETPROCADDR(QueueBindSparse);
  GETPROCADDR(CreateFence);
  GETPROCADDR(DestroyFence);
  GETPROCADDR(ResetFences);
  GETPROCADDR(GetFenceStatus);
  GETPROCADDR(WaitForFences);
  GETPROCADDR(CreateSemaphore);
  GETPROCADDR(DestroySemaphore);
  GETPROCADDR(CreateEvent);
  GETPROCADDR(DestroyEvent);
  GETPROCADDR(GetEventStatus);
  GETPROCADDR(SetEvent);
  GETPROCADDR(ResetEvent);
  GETPROCADDR(CreateQueryPool);
  GETPROCADDR(DestroyQueryPool);
  GETPROCADDR(GetQueryPoolResults);
  GETPROCADDR(CreateBuffer);
  GETPROCADDR(DestroyBuffer);
  GETPROCADDR(CreateBufferView);
  GETPROCADDR(DestroyBufferView);
  GETPROCADDR(CreateImage);
  GETPROCADDR(DestroyImage);
  GETPROCADDR(GetImageSubresourceLayout);
  GETPROCADDR(CreateImageView);
  GETPROCADDR(DestroyImageView);
  GETPROCADDR(CreateShaderModule);
  GETPROCADDR(DestroyShaderModule);
  GETPROCADDR(CreatePipelineCache);
  GETPROCADDR(DestroyPipelineCache);
  GETPROCADDR(GetPipelineCacheData);
  GETPROCADDR(MergePipelineCaches);
  GETPROCADDR(CreateGraphicsPipelines);
  GETPROCADDR(CreateComputePipelines);
  GETPROCADDR(DestroyPipeline);
  GETPROCADDR(CreatePipelineLayout);
  GETPROCADDR(DestroyPipelineLayout);
  GETPROCADDR(CreateSampler);
  GETPROCADDR(DestroySampler);
  GETPROCADDR(CreateDescriptorSetLayout);
  GETPROCADDR(DestroyDescriptorSetLayout);
  GETPROCADDR(CreateDescriptorPool);
  GETPROCADDR(DestroyDescriptorPool);
  GETPROCADDR(ResetDescriptorPool);
  GETPROCADDR(AllocateDescriptorSets);
  GETPROCADDR(FreeDescriptorSets);
  GETPROCADDR(UpdateDescriptorSets);
  GETPROCADDR(CreateFramebuffer);
  GETPROCADDR(DestroyFramebuffer);
  GETPROCADDR(CreateRenderPass);
  GETPROCADDR(DestroyRenderPass);
  GETPROCADDR(GetRenderAreaGranularity);
  GETPROCADDR(CreateCommandPool);
  GETPROCADDR(DestroyCommandPool);
  GETPROCADDR(ResetCommandPool);
  GETPROCADDR(AllocateCommandBuffers);
  GETPROCADDR(FreeCommandBuffers);
  GETPROCADDR(BeginCommandBuffer);
  GETPROCADDR(EndCommandBuffer);
  GETPROCADDR(ResetCommandBuffer);
  GETPROCADDR(CmdBindPipeline);
  GETPROCADDR(CmdSetViewport);
  GETPROCADDR(CmdSetScissor);
  GETPROCADDR(CmdSetLineWidth);
  GETPROCADDR(CmdSetDepthBias);
  GETPROCADDR(CmdSetBlendConstants);
  GETPROCADDR(CmdSetDepthBounds);
  GETPROCADDR(CmdSetStencilCompareMask);
  GETPROCADDR(CmdSetStencilWriteMask);
  GETPROCADDR(CmdSetStencilReference);
  GETPROCADDR(CmdBindDescriptorSets);
  GETPROCADDR(CmdBindIndexBuffer);
  GETPROCADDR(CmdBindVertexBuffers);
  GETPROCADDR(CmdDraw);
  GETPROCADDR(CmdDrawIndexed);
  GETPROCADDR(CmdDrawIndirect);
  GETPROCADDR(CmdDrawIndexedIndirect);
  GETPROCADDR(CmdDispatch);
  GETPROCADDR(CmdDispatchIndirect);
  GETPROCADDR(CmdCopyBuffer);
  GETPROCADDR(CmdCopyImage);
  GETPROCADDR(CmdBlitImage);
  GETPROCADDR(CmdCopyBufferToImage);
  GETPROCADDR(CmdCopyImageToBuffer);
  GETPROCADDR(CmdUpdateBuffer);
  GETPROCADDR(CmdFillBuffer);
  GETPROCADDR(CmdClearColorImage);
  GETPROCADDR(CmdClearDepthStencilImage);
  GETPROCADDR(CmdClearAttachments);
  GETPROCADDR(CmdResolveImage);
  GETPROCADDR(CmdSetEvent);
  GETPROCADDR(CmdResetEvent);
  GETPROCADDR(CmdWaitEvents);
  GETPROCADDR(CmdPipelineBarrier);
  GETPROCADDR(CmdBeginQuery);
  GETPROCADDR(CmdEndQuery);
  GETPROCADDR(CmdResetQueryPool);
  GETPROCADDR(CmdWriteTimestamp);
  GETPROCADDR(CmdCopyQueryPoolResults);
  GETPROCADDR(CmdPushConstants);
  GETPROCADDR(CmdBeginRenderPass);
  GETPROCADDR(CmdNextSubpass);
  GETPROCADDR(CmdEndRenderPass);
  GETPROCADDR(CmdExecuteCommands);
  GETPROCADDR(QueuePresentKHR);
  GETPROCADDR(GetDeviceQueue2);
  return device_dispatch[GetKey(device)].GetDeviceProcAddr(device, pName);
}

extern "C" PUBLIC
VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL VKACI_GetInstanceProcAddr(VkInstance instance, const char* pName)
{
  GETPROCADDR(GetInstanceProcAddr);
  GETPROCADDR(GetDeviceProcAddr);
  GETPROCADDR(CreateInstance);
  GETPROCADDR(DestroyInstance);
  GETPROCADDR(CreateDevice);
  GETPROCADDR(EnumeratePhysicalDevices);
  GETPROCADDR(GetPhysicalDeviceProperties);
  GETPROCADDR(GetPhysicalDeviceQueueFamilyProperties);
  GETPROCADDR(GetPhysicalDeviceMemoryProperties);
  GETPROCADDR(GetPhysicalDeviceFeatures);
  GETPROCADDR(GetPhysicalDeviceFormatProperties);
  GETPROCADDR(GetPhysicalDeviceImageFormatProperties);
  GETPROCADDR(EnumerateInstanceVersion);
  GETPROCADDR(EnumerateInstanceLayerProperties);
  GETPROCADDR(EnumerateInstanceExtensionProperties);
  GETPROCADDR(EnumerateDeviceLayerProperties);
  GETPROCADDR(EnumerateDeviceExtensionProperties);
  GETPROCADDR(GetDeviceQueue);
  GETPROCADDR(QueueSubmit);
  GETPROCADDR(QueueWaitIdle);
  GETPROCADDR(DeviceWaitIdle);
  GETPROCADDR(AllocateMemory);
  GETPROCADDR(FreeMemory);
  GETPROCADDR(MapMemory);
  GETPROCADDR(UnmapMemory);
  GETPROCADDR(FlushMappedMemoryRanges);
  GETPROCADDR(InvalidateMappedMemoryRanges);
  GETPROCADDR(GetDeviceMemoryCommitment);
  GETPROCADDR(GetBufferMemoryRequirements);
  GETPROCADDR(BindBufferMemory);
  GETPROCADDR(GetImageMemoryRequirements);
  GETPROCADDR(BindImageMemory);
  GETPROCADDR(GetImageSparseMemoryRequirements);
  GETPROCADDR(GetPhysicalDeviceSparseImageFormatProperties);
  GETPROCADDR(QueueBindSparse);
  GETPROCADDR(CreateFence);
  GETPROCADDR(DestroyFence);
  GETPROCADDR(ResetFences);
  GETPROCADDR(GetFenceStatus);
  GETPROCADDR(WaitForFences);
  GETPROCADDR(CreateSemaphore);
  GETPROCADDR(DestroySemaphore);
  GETPROCADDR(CreateEvent);
  GETPROCADDR(DestroyEvent);
  GETPROCADDR(GetEventStatus);
  GETPROCADDR(SetEvent);
  GETPROCADDR(ResetEvent);
  GETPROCADDR(CreateQueryPool);
  GETPROCADDR(DestroyQueryPool);
  GETPROCADDR(GetQueryPoolResults);
  GETPROCADDR(CreateBuffer);
  GETPROCADDR(DestroyBuffer);
  GETPROCADDR(CreateBufferView);
  GETPROCADDR(DestroyBufferView);
  GETPROCADDR(CreateImage);
  GETPROCADDR(DestroyImage);
  GETPROCADDR(GetImageSubresourceLayout);
  GETPROCADDR(CreateImageView);
  GETPROCADDR(DestroyImageView);
  GETPROCADDR(CreateShaderModule);
  GETPROCADDR(DestroyShaderModule);
  GETPROCADDR(CreatePipelineCache);
  GETPROCADDR(DestroyPipelineCache);
  GETPROCADDR(GetPipelineCacheData);
  GETPROCADDR(MergePipelineCaches);
  GETPROCADDR(CreateGraphicsPipelines);
  GETPROCADDR(CreateComputePipelines);
  GETPROCADDR(DestroyPipeline);
  GETPROCADDR(CreatePipelineLayout);
  GETPROCADDR(DestroyPipelineLayout);
  GETPROCADDR(CreateSampler);
  GETPROCADDR(DestroySampler);
  GETPROCADDR(CreateDescriptorSetLayout);
  GETPROCADDR(DestroyDescriptorSetLayout);
  GETPROCADDR(CreateDescriptorPool);
  GETPROCADDR(DestroyDescriptorPool);
  GETPROCADDR(ResetDescriptorPool);
  GETPROCADDR(AllocateDescriptorSets);
  GETPROCADDR(FreeDescriptorSets);
  GETPROCADDR(UpdateDescriptorSets);
  GETPROCADDR(CreateFramebuffer);
  GETPROCADDR(DestroyFramebuffer);
  GETPROCADDR(CreateRenderPass);
  GETPROCADDR(DestroyRenderPass);
  GETPROCADDR(GetRenderAreaGranularity);
  GETPROCADDR(CreateCommandPool);
  GETPROCADDR(DestroyCommandPool);
  GETPROCADDR(ResetCommandPool);
  GETPROCADDR(AllocateCommandBuffers);
  GETPROCADDR(FreeCommandBuffers);
  GETPROCADDR(BeginCommandBuffer);
  GETPROCADDR(EndCommandBuffer);
  GETPROCADDR(ResetCommandBuffer);
  GETPROCADDR(CmdBindPipeline);
  GETPROCADDR(CmdSetViewport);
  GETPROCADDR(CmdSetScissor);
  GETPROCADDR(CmdSetLineWidth);
  GETPROCADDR(CmdSetDepthBias);
  GETPROCADDR(CmdSetBlendConstants);
  GETPROCADDR(CmdSetDepthBounds);
  GETPROCADDR(CmdSetStencilCompareMask);
  GETPROCADDR(CmdSetStencilWriteMask);
  GETPROCADDR(CmdSetStencilReference);
  GETPROCADDR(CmdBindDescriptorSets);
  GETPROCADDR(CmdBindIndexBuffer);
  GETPROCADDR(CmdBindVertexBuffers);
  GETPROCADDR(CmdDraw);
  GETPROCADDR(CmdDrawIndexed);
  GETPROCADDR(CmdDrawIndirect);
  GETPROCADDR(CmdDrawIndexedIndirect);
  GETPROCADDR(CmdDispatch);
  GETPROCADDR(CmdDispatchIndirect);
  GETPROCADDR(CmdCopyBuffer);
  GETPROCADDR(CmdCopyImage);
  GETPROCADDR(CmdBlitImage);
  GETPROCADDR(CmdCopyBufferToImage);
  GETPROCADDR(CmdCopyImageToBuffer);
  GETPROCADDR(CmdUpdateBuffer);
  GETPROCADDR(CmdFillBuffer);
  GETPROCADDR(CmdClearColorImage);
  GETPROCADDR(CmdClearDepthStencilImage);
  GETPROCADDR(CmdClearAttachments);
  GETPROCADDR(CmdResolveImage);
  GETPROCADDR(CmdSetEvent);
  GETPROCADDR(CmdResetEvent);
  GETPROCADDR(CmdWaitEvents);
  GETPROCADDR(CmdPipelineBarrier);
  GETPROCADDR(CmdBeginQuery);
  GETPROCADDR(CmdEndQuery);
  GETPROCADDR(CmdResetQueryPool);
  GETPROCADDR(CmdWriteTimestamp);
  GETPROCADDR(CmdCopyQueryPoolResults);
  GETPROCADDR(CmdPushConstants);
  GETPROCADDR(CmdBeginRenderPass);
  GETPROCADDR(CmdNextSubpass);
  GETPROCADDR(CmdEndRenderPass);
  GETPROCADDR(CmdExecuteCommands);
  GETPROCADDR(QueuePresentKHR);
  GETPROCADDR(GetDeviceQueue2);
  return instance_dispatch[GetKey(instance)].GetInstanceProcAddr(instance, pName);
}

