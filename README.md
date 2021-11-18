# GraphicsProjects

## Ray tracing project
For as1 we built a ray tracing from scratch using Eigen as our vector library and OpenCV as our image builder. All the other components and maths equations were coded within 4 weeks. Here are a few of my renders...

Final Render
![alt text](https://github.com/humaid123/GraphicsProjects/blob/main/renders/HIGH%20QUALITY%20final%20render.png)

Render from inside the room
![alt text](https://github.com/humaid123/GraphicsProjects/blob/main/renders/HIGH%20QUALITY%20final%20render%20inside%20the%20room.png)

Details:
The ray tracer can render a Cornell box, spheres and cubes.

It has different types of materials which allow the Blinn-Phong reflection model, light emission and refraction. 

It uses anti-aliasing and area lights to prevent jagged edges and allows soft shadows.

It uses instances to rotate any primitive around the x, y, z axes.

It uses a BVH acceleration data structure to speed up the renders.