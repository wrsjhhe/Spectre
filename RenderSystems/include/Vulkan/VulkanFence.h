#pragma once

BEGIN_NAMESPACE_SPECTRE

class VulkanFence : public Noncopyable
{
public:
	static std::shared_ptr<VulkanFence> CreateFence(const VulkanDevice& vulkanDevice);
public:
	~VulkanFence();

	VkFence& GetVkFence() { return m_VkFence; }

	void Destory();
private:
	VulkanFence(const VulkanDevice& vulkanDevice);

private:
	const VulkanDevice& m_Device;
	VkFence             m_VkFence = VK_NULL_HANDLE;
};

END_NAMESPACE_SPECTRE