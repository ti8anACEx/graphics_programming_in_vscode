#version 330

// 'in' data are varying for every other vertex
in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) mat4 fullTransformMatrix; // loc=2 is the beginning location, but mat4 means 16 matrics or like 4 vec4s, so this whole DS will take locations 2,3,4,5

// 'uniform' data are the data that stay constant for every vertex
// uniform vec3 dominatingColor;
// uniform float yFlip;

out vec3 theColor;

void main()
{
   gl_Position = fullTransformMatrix * vec4(position, 1.0);
   // gl_Position = vec4(position, 1.0);
   // gl_Position.y *= yFlip;
   // theColor = vertexColor;
   // theColor = dominatingColor; 
   theColor = vertexColor;
};