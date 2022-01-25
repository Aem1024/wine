# Automatically generated from Vulkan vk.xml; DO NOT EDIT!
#
# This file is generated from Vulkan vk.xml file covered
# by the following copyright and permission notice:
#
# Copyright 2015-2022 The Khronos Group Inc.
#
# SPDX-License-Identifier: Apache-2.0 OR MIT
#

@ stdcall -private vk_icdGetInstanceProcAddr(ptr str)
@ stdcall -private vk_icdGetPhysicalDeviceProcAddr(ptr str)
@ stdcall -private vk_icdNegotiateLoaderICDInterfaceVersion(ptr)
@ stdcall vkAcquireNextImage2KHR(ptr ptr ptr)
@ stdcall vkAcquireNextImageKHR(ptr int64 int64 int64 int64 ptr)
@ stdcall vkAllocateCommandBuffers(ptr ptr ptr)
@ stdcall vkAllocateDescriptorSets(ptr ptr ptr)
@ stdcall vkAllocateMemory(ptr ptr ptr ptr)
@ stdcall vkBeginCommandBuffer(ptr ptr)
@ stdcall vkBindBufferMemory(ptr int64 int64 int64)
@ stdcall vkBindBufferMemory2(ptr long ptr)
@ stdcall vkBindImageMemory(ptr int64 int64 int64)
@ stdcall vkBindImageMemory2(ptr long ptr)
@ stdcall vkCmdBeginQuery(ptr int64 long long)
@ stdcall vkCmdBeginRenderPass(ptr ptr long)
@ stdcall vkCmdBeginRenderPass2(ptr ptr ptr)
@ stub vkCmdBeginRendering
@ stdcall vkCmdBindDescriptorSets(ptr long int64 long long ptr long ptr)
@ stdcall vkCmdBindIndexBuffer(ptr int64 int64 long)
@ stdcall vkCmdBindPipeline(ptr long int64)
@ stdcall vkCmdBindVertexBuffers(ptr long long ptr ptr)
@ stub vkCmdBindVertexBuffers2
@ stdcall vkCmdBlitImage(ptr int64 long int64 long long ptr long)
@ stub vkCmdBlitImage2
@ stdcall vkCmdClearAttachments(ptr long ptr long ptr)
@ stdcall vkCmdClearColorImage(ptr int64 long ptr long ptr)
@ stdcall vkCmdClearDepthStencilImage(ptr int64 long ptr long ptr)
@ stdcall vkCmdCopyBuffer(ptr int64 int64 long ptr)
@ stub vkCmdCopyBuffer2
@ stdcall vkCmdCopyBufferToImage(ptr int64 int64 long long ptr)
@ stub vkCmdCopyBufferToImage2
@ stdcall vkCmdCopyImage(ptr int64 long int64 long long ptr)
@ stub vkCmdCopyImage2
@ stdcall vkCmdCopyImageToBuffer(ptr int64 long int64 long ptr)
@ stub vkCmdCopyImageToBuffer2
@ stdcall vkCmdCopyQueryPoolResults(ptr int64 long long int64 int64 int64 long)
@ stdcall vkCmdDispatch(ptr long long long)
@ stdcall vkCmdDispatchBase(ptr long long long long long long)
@ stdcall vkCmdDispatchIndirect(ptr int64 int64)
@ stdcall vkCmdDraw(ptr long long long long)
@ stdcall vkCmdDrawIndexed(ptr long long long long long)
@ stdcall vkCmdDrawIndexedIndirect(ptr int64 int64 long long)
@ stdcall vkCmdDrawIndexedIndirectCount(ptr int64 int64 int64 int64 long long)
@ stdcall vkCmdDrawIndirect(ptr int64 int64 long long)
@ stdcall vkCmdDrawIndirectCount(ptr int64 int64 int64 int64 long long)
@ stdcall vkCmdEndQuery(ptr int64 long)
@ stdcall vkCmdEndRenderPass(ptr)
@ stdcall vkCmdEndRenderPass2(ptr ptr)
@ stub vkCmdEndRendering
@ stdcall vkCmdExecuteCommands(ptr long ptr)
@ stdcall vkCmdFillBuffer(ptr int64 int64 int64 long)
@ stdcall vkCmdNextSubpass(ptr long)
@ stdcall vkCmdNextSubpass2(ptr ptr ptr)
@ stdcall vkCmdPipelineBarrier(ptr long long long long ptr long ptr long ptr)
@ stub vkCmdPipelineBarrier2
@ stdcall vkCmdPushConstants(ptr int64 long long long ptr)
@ stdcall vkCmdResetEvent(ptr int64 long)
@ stub vkCmdResetEvent2
@ stdcall vkCmdResetQueryPool(ptr int64 long long)
@ stdcall vkCmdResolveImage(ptr int64 long int64 long long ptr)
@ stub vkCmdResolveImage2
@ stdcall vkCmdSetBlendConstants(ptr ptr)
@ stub vkCmdSetCullMode
@ stdcall vkCmdSetDepthBias(ptr float float float)
@ stub vkCmdSetDepthBiasEnable
@ stdcall vkCmdSetDepthBounds(ptr float float)
@ stub vkCmdSetDepthBoundsTestEnable
@ stub vkCmdSetDepthCompareOp
@ stub vkCmdSetDepthTestEnable
@ stub vkCmdSetDepthWriteEnable
@ stdcall vkCmdSetDeviceMask(ptr long)
@ stdcall vkCmdSetEvent(ptr int64 long)
@ stub vkCmdSetEvent2
@ stub vkCmdSetFrontFace
@ stdcall vkCmdSetLineWidth(ptr float)
@ stub vkCmdSetPrimitiveRestartEnable
@ stub vkCmdSetPrimitiveTopology
@ stub vkCmdSetRasterizerDiscardEnable
@ stdcall vkCmdSetScissor(ptr long long ptr)
@ stub vkCmdSetScissorWithCount
@ stdcall vkCmdSetStencilCompareMask(ptr long long)
@ stub vkCmdSetStencilOp
@ stdcall vkCmdSetStencilReference(ptr long long)
@ stub vkCmdSetStencilTestEnable
@ stdcall vkCmdSetStencilWriteMask(ptr long long)
@ stdcall vkCmdSetViewport(ptr long long ptr)
@ stub vkCmdSetViewportWithCount
@ stdcall vkCmdUpdateBuffer(ptr int64 int64 int64 ptr)
@ stdcall vkCmdWaitEvents(ptr long ptr long long long ptr long ptr long ptr)
@ stub vkCmdWaitEvents2
@ stdcall vkCmdWriteTimestamp(ptr long int64 long)
@ stub vkCmdWriteTimestamp2
@ stdcall vkCreateBuffer(ptr ptr ptr ptr)
@ stdcall vkCreateBufferView(ptr ptr ptr ptr)
@ stdcall vkCreateCommandPool(ptr ptr ptr ptr)
@ stdcall vkCreateComputePipelines(ptr int64 long ptr ptr ptr)
@ stdcall vkCreateDescriptorPool(ptr ptr ptr ptr)
@ stdcall vkCreateDescriptorSetLayout(ptr ptr ptr ptr)
@ stdcall vkCreateDescriptorUpdateTemplate(ptr ptr ptr ptr)
@ stdcall vkCreateDevice(ptr ptr ptr ptr)
@ stub vkCreateDisplayModeKHR
@ stub vkCreateDisplayPlaneSurfaceKHR
@ stdcall vkCreateEvent(ptr ptr ptr ptr)
@ stdcall vkCreateFence(ptr ptr ptr ptr)
@ stdcall vkCreateFramebuffer(ptr ptr ptr ptr)
@ stdcall vkCreateGraphicsPipelines(ptr int64 long ptr ptr ptr)
@ stdcall vkCreateImage(ptr ptr ptr ptr)
@ stdcall vkCreateImageView(ptr ptr ptr ptr)
@ stdcall vkCreateInstance(ptr ptr ptr)
@ stdcall vkCreatePipelineCache(ptr ptr ptr ptr)
@ stdcall vkCreatePipelineLayout(ptr ptr ptr ptr)
@ stub vkCreatePrivateDataSlot
@ stdcall vkCreateQueryPool(ptr ptr ptr ptr)
@ stdcall vkCreateRenderPass(ptr ptr ptr ptr)
@ stdcall vkCreateRenderPass2(ptr ptr ptr ptr)
@ stdcall vkCreateSampler(ptr ptr ptr ptr)
@ stdcall vkCreateSamplerYcbcrConversion(ptr ptr ptr ptr)
@ stdcall vkCreateSemaphore(ptr ptr ptr ptr)
@ stdcall vkCreateShaderModule(ptr ptr ptr ptr)
@ stub vkCreateSharedSwapchainsKHR
@ stdcall vkCreateSwapchainKHR(ptr ptr ptr ptr)
@ stdcall vkCreateWin32SurfaceKHR(ptr ptr ptr ptr)
@ stdcall vkDestroyBuffer(ptr int64 ptr)
@ stdcall vkDestroyBufferView(ptr int64 ptr)
@ stdcall vkDestroyCommandPool(ptr int64 ptr)
@ stdcall vkDestroyDescriptorPool(ptr int64 ptr)
@ stdcall vkDestroyDescriptorSetLayout(ptr int64 ptr)
@ stdcall vkDestroyDescriptorUpdateTemplate(ptr int64 ptr)
@ stdcall vkDestroyDevice(ptr ptr)
@ stdcall vkDestroyEvent(ptr int64 ptr)
@ stdcall vkDestroyFence(ptr int64 ptr)
@ stdcall vkDestroyFramebuffer(ptr int64 ptr)
@ stdcall vkDestroyImage(ptr int64 ptr)
@ stdcall vkDestroyImageView(ptr int64 ptr)
@ stdcall vkDestroyInstance(ptr ptr)
@ stdcall vkDestroyPipeline(ptr int64 ptr)
@ stdcall vkDestroyPipelineCache(ptr int64 ptr)
@ stdcall vkDestroyPipelineLayout(ptr int64 ptr)
@ stub vkDestroyPrivateDataSlot
@ stdcall vkDestroyQueryPool(ptr int64 ptr)
@ stdcall vkDestroyRenderPass(ptr int64 ptr)
@ stdcall vkDestroySampler(ptr int64 ptr)
@ stdcall vkDestroySamplerYcbcrConversion(ptr int64 ptr)
@ stdcall vkDestroySemaphore(ptr int64 ptr)
@ stdcall vkDestroyShaderModule(ptr int64 ptr)
@ stdcall vkDestroySurfaceKHR(ptr int64 ptr)
@ stdcall vkDestroySwapchainKHR(ptr int64 ptr)
@ stdcall vkDeviceWaitIdle(ptr)
@ stdcall vkEndCommandBuffer(ptr)
@ stdcall vkEnumerateDeviceExtensionProperties(ptr str ptr ptr)
@ stdcall vkEnumerateDeviceLayerProperties(ptr ptr ptr)
@ stdcall vkEnumerateInstanceExtensionProperties(str ptr ptr)
@ stdcall vkEnumerateInstanceLayerProperties(ptr ptr)
@ stdcall vkEnumerateInstanceVersion(ptr)
@ stdcall vkEnumeratePhysicalDeviceGroups(ptr ptr ptr)
@ stdcall vkEnumeratePhysicalDevices(ptr ptr ptr)
@ stdcall vkFlushMappedMemoryRanges(ptr long ptr)
@ stdcall vkFreeCommandBuffers(ptr int64 long ptr)
@ stdcall vkFreeDescriptorSets(ptr int64 long ptr)
@ stdcall vkFreeMemory(ptr int64 ptr)
@ stdcall vkGetBufferDeviceAddress(ptr ptr)
@ stdcall vkGetBufferMemoryRequirements(ptr int64 ptr)
@ stdcall vkGetBufferMemoryRequirements2(ptr ptr ptr)
@ stdcall vkGetBufferOpaqueCaptureAddress(ptr ptr)
@ stdcall vkGetDescriptorSetLayoutSupport(ptr ptr ptr)
@ stub vkGetDeviceBufferMemoryRequirements
@ stdcall vkGetDeviceGroupPeerMemoryFeatures(ptr long long long ptr)
@ stdcall vkGetDeviceGroupPresentCapabilitiesKHR(ptr ptr)
@ stdcall vkGetDeviceGroupSurfacePresentModesKHR(ptr int64 ptr)
@ stub vkGetDeviceImageMemoryRequirements
@ stub vkGetDeviceImageSparseMemoryRequirements
@ stdcall vkGetDeviceMemoryCommitment(ptr int64 ptr)
@ stdcall vkGetDeviceMemoryOpaqueCaptureAddress(ptr ptr)
@ stdcall vkGetDeviceProcAddr(ptr str)
@ stdcall vkGetDeviceQueue(ptr long long ptr)
@ stdcall vkGetDeviceQueue2(ptr ptr ptr)
@ stub vkGetDisplayModePropertiesKHR
@ stub vkGetDisplayPlaneCapabilitiesKHR
@ stub vkGetDisplayPlaneSupportedDisplaysKHR
@ stdcall vkGetEventStatus(ptr int64)
@ stdcall vkGetFenceStatus(ptr int64)
@ stdcall vkGetImageMemoryRequirements(ptr int64 ptr)
@ stdcall vkGetImageMemoryRequirements2(ptr ptr ptr)
@ stdcall vkGetImageSparseMemoryRequirements(ptr int64 ptr ptr)
@ stdcall vkGetImageSparseMemoryRequirements2(ptr ptr ptr ptr)
@ stdcall vkGetImageSubresourceLayout(ptr int64 ptr ptr)
@ stdcall vkGetInstanceProcAddr(ptr str)
@ stub vkGetPhysicalDeviceDisplayPlanePropertiesKHR
@ stub vkGetPhysicalDeviceDisplayPropertiesKHR
@ stdcall vkGetPhysicalDeviceExternalBufferProperties(ptr ptr ptr)
@ stdcall vkGetPhysicalDeviceExternalFenceProperties(ptr ptr ptr)
@ stdcall vkGetPhysicalDeviceExternalSemaphoreProperties(ptr ptr ptr)
@ stdcall vkGetPhysicalDeviceFeatures(ptr ptr)
@ stdcall vkGetPhysicalDeviceFeatures2(ptr ptr)
@ stdcall vkGetPhysicalDeviceFormatProperties(ptr long ptr)
@ stdcall vkGetPhysicalDeviceFormatProperties2(ptr long ptr)
@ stdcall vkGetPhysicalDeviceImageFormatProperties(ptr long long long long long ptr)
@ stdcall vkGetPhysicalDeviceImageFormatProperties2(ptr ptr ptr)
@ stdcall vkGetPhysicalDeviceMemoryProperties(ptr ptr)
@ stdcall vkGetPhysicalDeviceMemoryProperties2(ptr ptr)
@ stdcall vkGetPhysicalDevicePresentRectanglesKHR(ptr int64 ptr ptr)
@ stdcall vkGetPhysicalDeviceProperties(ptr ptr)
@ stdcall vkGetPhysicalDeviceProperties2(ptr ptr)
@ stdcall vkGetPhysicalDeviceQueueFamilyProperties(ptr ptr ptr)
@ stdcall vkGetPhysicalDeviceQueueFamilyProperties2(ptr ptr ptr)
@ stdcall vkGetPhysicalDeviceSparseImageFormatProperties(ptr long long long long long ptr ptr)
@ stdcall vkGetPhysicalDeviceSparseImageFormatProperties2(ptr ptr ptr ptr)
@ stdcall vkGetPhysicalDeviceSurfaceCapabilities2KHR(ptr ptr ptr)
@ stdcall vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ptr int64 ptr)
@ stdcall vkGetPhysicalDeviceSurfaceFormats2KHR(ptr ptr ptr ptr)
@ stdcall vkGetPhysicalDeviceSurfaceFormatsKHR(ptr int64 ptr ptr)
@ stdcall vkGetPhysicalDeviceSurfacePresentModesKHR(ptr int64 ptr ptr)
@ stdcall vkGetPhysicalDeviceSurfaceSupportKHR(ptr long int64 ptr)
@ stub vkGetPhysicalDeviceToolProperties
@ stdcall vkGetPhysicalDeviceWin32PresentationSupportKHR(ptr long)
@ stdcall vkGetPipelineCacheData(ptr int64 ptr ptr)
@ stub vkGetPrivateData
@ stdcall vkGetQueryPoolResults(ptr int64 long long long ptr int64 long)
@ stdcall vkGetRenderAreaGranularity(ptr int64 ptr)
@ stdcall vkGetSemaphoreCounterValue(ptr int64 ptr)
@ stdcall vkGetSwapchainImagesKHR(ptr int64 ptr ptr)
@ stdcall vkInvalidateMappedMemoryRanges(ptr long ptr)
@ stdcall vkMapMemory(ptr int64 int64 int64 long ptr)
@ stdcall vkMergePipelineCaches(ptr int64 long ptr)
@ stdcall vkQueueBindSparse(ptr long ptr int64)
@ stdcall vkQueuePresentKHR(ptr ptr)
@ stdcall vkQueueSubmit(ptr long ptr int64)
@ stub vkQueueSubmit2
@ stdcall vkQueueWaitIdle(ptr)
@ stdcall vkResetCommandBuffer(ptr long)
@ stdcall vkResetCommandPool(ptr int64 long)
@ stdcall vkResetDescriptorPool(ptr int64 long)
@ stdcall vkResetEvent(ptr int64)
@ stdcall vkResetFences(ptr long ptr)
@ stdcall vkResetQueryPool(ptr int64 long long)
@ stdcall vkSetEvent(ptr int64)
@ stub vkSetPrivateData
@ stdcall vkSignalSemaphore(ptr ptr)
@ stdcall vkTrimCommandPool(ptr int64 long)
@ stdcall vkUnmapMemory(ptr int64)
@ stdcall vkUpdateDescriptorSetWithTemplate(ptr int64 int64 ptr)
@ stdcall vkUpdateDescriptorSets(ptr long ptr long ptr)
@ stdcall vkWaitForFences(ptr long ptr long int64)
@ stdcall vkWaitSemaphores(ptr ptr int64)
@ stdcall -private DllRegisterServer()
@ stdcall -private DllUnregisterServer()
