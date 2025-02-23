# LW-Vulkan
This will be my test environment to learn rendering in Vulkan

#My Goals for this Project
I am making a 3D Renderer using the Vulkan API. My goal is to learn more about Graphics Programming and then Vulkan API with the final result being able to render multiple 3D objects to screen and be able to
move them in a 3D space, manipulate their size and also have a Fly Camera to look around the "scene". I will also be implementing an IMGUI inteface to assist with the
manipulation of these objects. 

#Why I chose Vulkan
As modern graphic card architectures matured, they offered a lot more progrommable functionality. This functionality had to somehow be integrated with the Existing APIs. 
This resulted in less than ideal abstractions and a lot of guess work and the graphics driver side to map out the programmer's intern into the modern graphics architecture.
That is why there are so many driver updates for improving the performance in games by sometimes a very large margine, due to the complexity of these drivers, application developers
also need to deal with inconsistencies between vendors, like the syntax that is accepted for "shaders". Also there has been a massive spike in Mobile Development and the graphics
architecture on mobile is significantly different due to their energy consumption and space requirements.

Vulkan solves these problems by being designed from "scratch" for modern graphics architectures. It reduces the drivers overhead by allowing programmers to cearly specify their intent
using a more verbose API, and allows multiple threads to create and submit commands in parallel. It also reduces inconsistencies in shader compilation by switching to a 
standardized byte code format witha  single compiler. Lastly, it acknowledges the general purpose processing capabilities of modern graphics cards by unifying the graphics
and computre functionality into a single API.

