#include <exception>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <optional>
#include <iostream>
#include <vector>
#include <set>


class Triangle{

private:

VkInstance instance;
VkPhysicalDevice physdev=VK_NULL_HANDLE;
GLFWwindow* window;
VkDevice device;//logical device
VkSurfaceKHR surface;
VkQueue graphicsQueue;
VkQueue presentQueue;
VkSwapchainKHR swapChain;
std::vector<VkImage> swapChainImages;
std::vector<VkImageView> swapChainImageViews;
VkFormat swapChainImageFormat;
VkExtent2D swapChainExtent;

/* Every operation in Vulkan needs to be submitted to a specific queue family capable of doing specific operations, for example like drawing */
struct QueueFamilyIndices{

std::optional<uint32_t> graphicsFamily; //data structure that holds no value until it is filled up, we can check if it is filled with has_value
std::optional<uint32_t> presentFamily; //graphics queue family may not be able to present to a surface so we need to search for one capable of it
bool isComplete(){

return graphicsFamily.has_value() && presentFamily.has_value();

    }
};

QueueFamilyIndices findQueueFamily(VkPhysicalDevice device){

QueueFamilyIndices indices;
uint32_t queueCount=0;
vkGetPhysicalDeviceQueueFamilyProperties(device,&queueCount,nullptr);
std::vector<VkQueueFamilyProperties> familyProperties (queueCount);
vkGetPhysicalDeviceQueueFamilyProperties(device,&queueCount,familyProperties.data());
int i=0;

for (const auto& queuefamily : familyProperties){

if (queuefamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) //we are checking if our device supports graphics operations
        indices.graphicsFamily=i;

VkBool32 isPresent=false;
vkGetPhysicalDeviceSurfaceSupportKHR(device,i,surface,&isPresent);

if(isPresent)
        indices.presentFamily=i;

if (indices.isComplete())
        break;


i++;


    }
    return indices;
}


bool checkDeviceExtensionSupport(VkPhysicalDevice device){

uint32_t extensionCount=0;
vkEnumerateDeviceExtensionProperties(device,nullptr,&extensionCount,nullptr);
std::vector<VkExtensionProperties> availableExtensions(extensionCount);
vkEnumerateDeviceExtensionProperties(device,nullptr,&extensionCount,availableExtensions.data());
bool isExtensionAvailable=false;
for (const auto& extension : availableExtensions){

if (true)
        isExtensionAvailable=true;


}

return isExtensionAvailable;

}

typedef struct SwapChainSupportDetails{

VkSurfaceCapabilitiesKHR capabilities;
std::vector<VkSurfaceFormatKHR> formats;
std::vector<VkPresentModeKHR> presentModes;


}SwapChainSupportDetails;

SwapChainSupportDetails querySwapChainCapabilites(VkPhysicalDevice device){

SwapChainSupportDetails details;
vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,surface,&details.capabilities);
uint32_t formatCount=0;
vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&formatCount,nullptr);
if(formatCount != 0){

details.formats.resize(formatCount);
vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&formatCount,details.formats.data());

}

uint32_t presentModesCount=0;
vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&presentModesCount,nullptr);
if(formatCount != 0){

details.presentModes.resize(presentModesCount);
vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&presentModesCount,details.presentModes.data());

}
return details;


}

bool isDeviceSuitable(VkPhysicalDevice device){

QueueFamilyIndices indices=findQueueFamily(device);
bool extensionSupported=checkDeviceExtensionSupport(device);
bool swapChainAdequate=false;
if (extensionSupported){//check if it supports VK_KHR_swapchain and if it queries information about the type of swapchain it can create

SwapChainSupportDetails swapChainsupport=querySwapChainCapabilites(device);
swapChainAdequate = !swapChainsupport.formats.empty() && !swapChainsupport.presentModes.empty();
}
return indices.isComplete() && extensionSupported && swapChainAdequate;

}


//before creating a swapchain we need to choose formant, colourspace, avaliablemode and extent
VkSurfaceFormatKHR chooseSwapchainFormat(const std::vector<VkSurfaceFormatKHR> availableformats){

for (const auto& availableformat : availableformats){

if (availableformat.format==VK_FORMAT_R8G8B8A8_SRGB && availableformat.colorSpace==VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        return availableformat;

}

return availableformats[0];

}

VkPresentModeKHR chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR> availablemodes){

bool modePresent=false;
for (const auto& mode : availablemodes){

if (mode == VK_PRESENT_MODE_FIFO_KHR)
        modePresent=true;
}

if (modePresent)
	return VK_PRESENT_MODE_FIFO_KHR;

}


//extent is the resolution of the imagws in the swapchain and it is usually equal to the window size. All availables extents are>
VkExtent2D chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilites){

return capabilites.currentExtent;

}

void initWindow(){
 
glfwInit(); //start glfw 
glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  //we tell it to not load opengl
glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //we tell it to not be resizable
window=glfwCreateWindow(800,600,"Vulkan",nullptr,nullptr); //create a window

}

void createInstance(){

VkApplicationInfo appinfo{}; //struct that contains information for the application which we need to fill up
appinfo.sType=VK_STRUCTURE_TYPE_APPLICATION_INFO;
appinfo.pApplicationName="Hello Triangle";
appinfo.applicationVersion= VK_MAKE_VERSION(1,0,0);
appinfo.pEngineName="No Engine";
appinfo.apiVersion=VK_API_VERSION_1_0;

VkInstanceCreateInfo createinfo{}; //struct that contains info for instance creation like extension and validation layers
createinfo.sType=VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
createinfo.pApplicationInfo=&appinfo;

uint32_t glfwExtensionCount=0; //use glfw to query required extensions
const char** glfwExtensions;
glfwExtensions=glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

createinfo.enabledExtensionCount=glfwExtensionCount;
createinfo.ppEnabledExtensionNames=glfwExtensions;
createinfo.enabledLayerCount=0;

VkResult result = vkCreateInstance(&createinfo,nullptr,&instance);

if (result != VK_SUCCESS)
	throw std::runtime_error("Failed to create instance");
}



void pickPhysicalDevice(){

uint32_t deviceCount=0;
vkEnumeratePhysicalDevices(instance,&deviceCount,nullptr); //we first get the device count
if (deviceCount == 0)
	throw std::runtime_error("Couldn't find any device");
std::vector<VkPhysicalDevice> devices(deviceCount);
vkEnumeratePhysicalDevices(instance,&deviceCount,devices.data()); //then we put it in a std::vector
for (const auto& dev : devices){

if (isDeviceSuitable(dev)){
	physdev=dev;
	break;
}
}

if (physdev == VK_NULL_HANDLE)
	throw std::runtime_error("Couldn't find a suitable physical device");


}


void createLogicalDevice(){

QueueFamilyIndices indices=findQueueFamily(physdev);

std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;//create an array of queue infos
std::set<uint32_t> uniqueQueueFamilies={indices.graphicsFamily.value(),indices.presentFamily.value()}; //create a set for the graphics families we need
float queuePriorities=1.0f;

for ( uint32_t queueFamily : uniqueQueueFamilies){

VkDeviceQueueCreateInfo queueCreateInfo{}; //specificy a struct that contains information about the numbers of queue and the queue family we want to use
queueCreateInfo.sType=VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
queueCreateInfo.queueFamilyIndex=queueFamily;//specificy which queue family we wants to use
queueCreateInfo.queueCount=1;//and the number of queues we want to create, only small number allowed
queueCreateInfo.pQueuePriorities=&queuePriorities; //priority value, float from 0 to 1
queueCreateInfo.flags=0;
queueCreateInfos.push_back(queueCreateInfo); //add the queue created for the family to the vector containing all the queues info

}

VkPhysicalDeviceFeatures deviceFeatures {}; //initialized to VK_FALSE as we don't need any special device feature for now
VkDeviceCreateInfo createInfo {}; //struct that contains information about the logical device. Similiar to the instance creation struct
createInfo.sType=VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
createInfo.queueCreateInfoCount=static_cast<uint32_t>(queueCreateInfos.size());
createInfo.pQueueCreateInfos=queueCreateInfos.data();
createInfo.pEnabledFeatures=&deviceFeatures; 
const std::vector<const char*> deviceExtensions={VK_KHR_SWAPCHAIN_EXTENSION_NAME};
createInfo.enabledExtensionCount=static_cast<uint32_t>(deviceExtensions.size());
createInfo.ppEnabledExtensionNames=deviceExtensions.data();
createInfo.enabledLayerCount=0; 

if (vkCreateDevice(physdev,&createInfo,nullptr,&device) != VK_SUCCESS)
	std::cout<<"Creation of logical device failed";

vkGetDeviceQueue(device,indices.graphicsFamily.value(),0,&graphicsQueue);
vkGetDeviceQueue(device,indices.presentFamily.value(),0,&presentQueue);//we bind the present family queue and graphics family queue to the vkQueue class members


}

void createSurface(){

if((glfwCreateWindowSurface(instance,window,nullptr,&surface) != VK_SUCCESS))
	throw std::runtime_error("Failed to create a window surface"); //a surface is what allows Vulkan to present to a window manager, a surface is created through a WSI extension


}

void createSwapChain(){

SwapChainSupportDetails swapChainSupport=querySwapChainCapabilites(physdev);
VkSurfaceFormatKHR  surfaceFormat=chooseSwapchainFormat(swapChainSupport.formats);
VkPresentModeKHR presentMode=chooseSwapchainPresentMode(swapChainSupport.presentModes);
VkExtent2D extent=chooseSwapchainExtent(swapChainSupport.capabilities);
uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}
VkSwapchainCreateInfoKHR createInfo{};
createInfo.sType=VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
createInfo.surface = surface;
createInfo.minImageCount = imageCount;
createInfo.imageFormat=surfaceFormat.format;
createInfo.imageColorSpace=surfaceFormat.colorSpace;
createInfo.imageExtent=extent;
createInfo.imageArrayLayers=1;
createInfo.imageUsage=VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
QueueFamilyIndices indices=findQueueFamily(physdev);
uint32_t queueFamilyIndices[] {indices.graphicsFamily.value(),indices.presentFamily.value()};

if(indices.graphicsFamily != indices.presentFamily){

createInfo.imageSharingMode=VK_SHARING_MODE_CONCURRENT;
createInfo.queueFamilyIndexCount=2;
createInfo.pQueueFamilyIndices=queueFamilyIndices;

}

else
	createInfo.imageSharingMode=VK_SHARING_MODE_EXCLUSIVE;
	
createInfo.preTransform=swapChainSupport.capabilities.currentTransform;
createInfo.compositeAlpha=VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
createInfo.presentMode=presentMode;
createInfo.clipped=VK_TRUE;
createInfo.oldSwapchain=VK_NULL_HANDLE;

if (vkCreateSwapchainKHR(device,&createInfo,nullptr,&swapChain))
	std::cout<<"Failed to create swapchain";
	
vkGetSwapchainImagesKHR(device,swapChain,&imageCount, nullptr);
swapChainImages.resize(imageCount);
vkGetSwapchainImagesKHR(device,swapChain,&imageCount,swapChainImages.data());
swapChainImageFormat=surfaceFormat.format;
swapChainExtent=extent;

}

void createImageView(){

swapChainImageViews.resize(swapChainImages.size());
for (size_t i=0; i<swapChainImages.size(); i++){
	VkImageViewCreateInfo createInfo{};
	createInfo.sType=VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image=swapChainImages[i];
	createInfo.viewType=VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format=swapChainImageFormat;
	createInfo.components.r=VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g=VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b=VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a=VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask=VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel=0;
	createInfo.subresourceRange.levelCount=1;
	createInfo.subresourceRange.baseArrayLayer=0;
	createInfo.subresourceRange.layerCount=1;
	if (vkCreateImageView(device,&createInfo,nullptr,&swapChainImageViews[i]) != VK_SUCCESS)
		std::cout<<"Couldn't create image view";
		
		}
	
	
}



void initVulkan(){

createInstance();
createSurface();
pickPhysicalDevice();
createLogicalDevice();
createSwapChain();
createImageView();

}


void mainLoop(){

while(!glfwWindowShouldClose(window)){

glfwPollEvents(); //tells to keep polling events until the X button is pressed

}

}

void cleanUp(){

 for (auto imageView : swapChainImageViews) {
	vkDestroyImageView(device, imageView, nullptr);
}
vkDestroySwapchainKHR(device,swapChain,nullptr);
vkDestroyDevice(device,nullptr);
vkDestroySurfaceKHR(instance,surface,nullptr);
vkDestroyInstance(instance,nullptr);

glfwDestroyWindow(window); //destroy the window
glfwTerminate(); //stops glfw


}

public:
		void run(){
			
			initWindow();
			initVulkan();
			mainLoop();
			cleanUp();
			
			
		}	



};

int main() {
    
	Triangle T;
	try{
	T.run();
	}
	catch(const std::exception& e ){
		 std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
	}
    return EXIT_SUCCESS;
}

