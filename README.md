This chapter is describing the different approach of the 
rendering lightning 

Instead of passing information to the lightnign calculations throuth the 
vertex shader 

We will use the gBuffer which is the frame buffer where the input variables that are neccessary for the 
lightning calculations are stored as the sepparate textures (position, normals and color) 

When drawing the final output of the pipline we will calculate the light by sampling those texture 
and use those sampled values as the input to the lightning calcualtions

With this approach we can render lot of lightning 