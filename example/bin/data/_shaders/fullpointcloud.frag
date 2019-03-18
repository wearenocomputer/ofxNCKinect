#version 120

uniform sampler2DRect texture;

void main() {
	vec2 st = gl_TexCoord[0].st; 
	vec4 mycolor = texture2DRect(texture,st);
	gl_FragColor = mycolor;
}