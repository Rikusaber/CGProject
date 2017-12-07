Rain, snow, and sleet simulation 
by Igor Carvalho, Ben Gruber, and Enoch Huang
Using OpenGL and C++.

This was done by creating a particle system, a matrix to hold different particles. When the simulation starts, particles will spawn at random locations on the grid, and fall to the ground. While each particle is falling, it has a varied lifetime, size, and velocity. This results in more realistic results of rain, snow, and slate. Each type of particle also has different weights so they fall at different velocities.

When a particle hits the ground, the particle ends its life, and turns the area it hit the ground a different color. If rain hits the ground, the area hit will turn a light blue, and if snow or slate hits the ground the area turns white. Furthermore, the area at which rain hits the ground will decrease in height, to simulate the rain droplets pushing into the ground. Similarly, when the snow/slate hits the ground, it raises the height of the ground, simulating snow accumulation.

Theoretically, if you ran the simulation for a long time, snow should create snowy mountains and rain should create deep lakes.

We have also added various ways for the user to interact with and modify the simulation. For example, the user can modify the density of the particles, the speed at which the particle falls, as well as adding a wind component to the simulation. The user can change the direction of the wind, which affects how the particles fall. The user can also pan around the scene, and zoom in and out in order to look around at what is happening in the simulation.

