# Procedural City Generation
This project contains the implementations of several procedural generation methods used to generate a virtual city. All algorithms are written in C++ for use in Unreal Engine.

The city generation is split into 3 parts: terrain generation, road network generation and buildings generation. For each part there are 2 diffent methods.

Terrain generation:
* Perlin noise
* Diamond-square

Road network generation:
* Voronoi based roads
* Custom road expansion algorithm

Building generation:
* Plot extrusion
* Shrinking layer stacking


# Examples of generated cities:
![DS_VOR_LAYER](Screenshots/DS_VOR_LAYER.png)
Terrain: diamond-square  
Road network: voronoi  
Buildings: layer stacking  
</br>

![FLAT_GRID_EXTR](Screenshots/FLAT_GRID_EXTR.png)
Terrain: flat  
Road network: voronoi (grid)  
Buildings: extrusion  
</br>

![FLAT_VOR_LAYER](Screenshots/FLAT_VOR_LAYER.png)
Terrain: flat  
Road network: voronoi  
Buildings: layer stacking  
</br>

![PER_CUST_EXTR](Screenshots/PER_CUST_EXTR.png)
Terrain: perlin  
Road network: custom  
Buildings: extrusion  
