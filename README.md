# Ray Tracer
This is a Ray Tracer built using the libraries: Eigen (vectors) and OpenCV (image display).

The ray tracer can render a Cornell box, spheres and cubes.

It has different types of materials which allow the Blinn-Phong reflection model, light emission and refraction. 

It uses anti-aliasing and area lights to prevent jagged edges and allows soft shadows. We can use multiple light sources in the scene.

It allows creating a checkerboard texture.

It uses instances to rotate any primitive around the x, y, z axes.

It uses a BVH acceleration data structure to speed up the renders.


Final Render
![alt text](https://github.com/humaid123/GraphicsProjects/blob/main/renders/HIGH%20QUALITY%20final%20render.png)

The render above shows the many features that were implemented. Notice the reflection on the metal sphere, the refraction on the glass sphere, the 3D-rotated cubes and the soft shadows near the matte red ball (consequence of the light ball and a far away light). We also note that the roof is a mirror but that will be more apparent in the second photo.

Render from inside the room
![alt text](https://github.com/humaid123/GraphicsProjects/blob/main/renders/HIGH%20QUALITY%20final%20render%20inside%20the%20room.png)

In this render, we get to clearly see thta reflection in the metallic ball as well as the refraction and total internal reflection in the glass ball. We also get to see how one of the cubes has faces with a checkerboard pattern and other faces with just a red color. Also note how the roof is a mirror.