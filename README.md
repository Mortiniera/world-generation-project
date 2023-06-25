![second_screen](https://github.com/Mortiniera/world-generation-project/assets/17401882/59441b62-a7d8-4b84-a8ea-fd50ba9b281a)# Computer-

## World Generation - Graphics Software Project

## Authors : Thevoz Melik, Wagner Patrik, Mortiniera Thevie

Main Features :

- Feature 1 : Terrain Generation ( generate a heightmap for a terrain on the GPU, and then render it)

- Feature 2 : Texturing, Sky modelling, Water modeling, Reflection/Refraction

- Feature 3 : Basic camera control (Fly through mode, First-person-shooting mode, Bezier Curves to control camera paths)

  Extra Features :
- Water reflection : We flipped the camera along the horizontal plane to
get a mirrored texture of the terrain and the sky that we use to texture
the water by mixing it with a phong shading.
- Wave like animation : We use our perlin noise implementation to
simulate waves that move according to the time.
- Infinite map : We build a big grid, each time we arrive to a limit
close to the borders of this grid, we regenerate a new grid at this
position. In our first implementation, we had an array with multiple
grids to give the impression of an infinite size.
- Day/Sunset/Night : We can switch between three different day times by
using the keyboard.
- Texture mixing : We mix our different textures in proportions depending
on the height/normals to get a smoother transition between the layers and
a more natural general feel.

Some Screenshots (Video demo can be found in the foloder Reports)
![first_screen](https://github.com/Mortiniera/world-generation-project/assets/17401882/dc3a3777-9312-408c-92a3-b87a459162c4)
![third_screen](https://github.com/Mortiniera/world-generation-project/assets/17401882/cb180e73-0599-4ee3-bc46-a090fd7ba914)




Work split
- Patrik took care of the Bezier algorithms implementation, and
everything related to the camera and infinite map.
- Melik and Thevie took care of the texturing (including the sky box) and
the water (reflection) implementation
- The rest of the work (Perlin, ...) and most of the debugging was done
all together.


External code references :
- Perlin noise was implemented by us based on the algorithm presented in
the lecture slides, and not using the available source code. However we used a
piece of code from this page :
http://stackoverflow.com/questions/4200224/random-noise-functions-forglsl as a random generator for our Perlin.
- Ridged multifractal algorithm was found in these websites :
https://engineering.purdue.edu/~ebertd/texture/1stEdition/musgrave/musgra
ve.c
https://github.com/jdupuy/fractalTerrain/blob/master/terrain.glsl
- Bezier curve implementation from lectures


## Setup : 
  - OpenGL/C++
