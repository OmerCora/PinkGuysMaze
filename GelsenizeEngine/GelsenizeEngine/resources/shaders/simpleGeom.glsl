#version 400 core

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 18) out;


uniform float explodeTime;

//uniform mat4 shadowMatrices[6];
//uniform bool shadowPass;

in vec4 diffuseColour[];
in vec3 fragPos[];
in vec3 vertMV[];
in vec3 vertWorld[];
in vec3 vertNorm[];
in vec3 viewVector[];
in vec3 vertTangent[];
in vec3 vertBitangent[];
in vec3 text_STU[];
in vec3 sb_texcoords[];
in vec2 offscreen_texcoords[];
in vec4 directionalShadow_FragPosLightSpace[];

// Going to the fragment shader (from fragment shader inputs)
out vec4 f_diffuseColour;	
out vec3 f_fragPos;
out vec3 f_vertMV;		
out vec3 f_vertWorld;
out vec3 f_vertNorm;
out vec3 f_viewVector;
out vec3 f_vertTangent;	
out vec3 f_vertBitangent;
out vec3 f_text_STU;
out vec3 f_sb_texcoords;
out vec2 f_offscreen_texcoords;
out vec4 f_directionalShadow_FragPosLightSpace;

out vec4 f_geom_fragPos;


float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 GetNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   a.x += rand(vec2(a.y, a.z));
   a.y += rand(vec2(a.x, a.z));
   a.z += rand(vec2(a.x, a.y));
   b.x += rand(vec2(b.y, b.z));
   b.y += rand(vec2(b.x, b.z));
   b.z += rand(vec2(b.x, b.y));
   return normalize(cross(a, b));
} 

vec4 explode(vec4 position, vec3 normal, vec3 worldPoint)
{
    float magnitude = 20.0f;
    //vec3 direction = normal * ((sin(explodeTime) + 1.0f) / 2.0f) * magnitude; 
	vec3 direction = normal * (explodeTime) * magnitude;
	//vec3 direction = (worldPoint - position.xyz) * magnitude * explodeTime;
    return position + vec4(direction, 0.0f);
} 

void main(void)
{
	bool shadowPass = false;
	if (shadowPass)
	{
		for(int face = 0; face < 6; ++face)
		{
			gl_Layer = face; // built-in variable that specifies to which face we render.
			for(int i = 0; i < 3; ++i) // for each triangle's vertices
			{
			    f_geom_fragPos = gl_in[i].gl_Position;
			    gl_Position = f_geom_fragPos;//shadowMatrices[face] * f_geom_fragPos;
		      EmitVertex();
			}    
			EndPrimitive();
		}
		return;
	}


	if(explodeTime == 0.0f)
	{
		int i;
		for ( i = 0; i < 3; i++ )
		{	// Do nothing... 
			// Pass everything to the fragment shdaer
			f_diffuseColour = diffuseColour[i];
			f_fragPos = fragPos[i];
			f_vertMV = vertMV[i];			
			f_vertWorld = vertWorld[i];
			f_viewVector = viewVector[i];
			f_vertNorm = vertNorm[i];
			f_vertTangent = vertTangent[i];		
			f_vertBitangent = vertBitangent[i];		
			f_text_STU = text_STU[i];	
			f_sb_texcoords = sb_texcoords[i];
			f_offscreen_texcoords = offscreen_texcoords[i];
			f_directionalShadow_FragPosLightSpace = directionalShadow_FragPosLightSpace[i];
			gl_Position = gl_in[i].gl_Position;
			EmitVertex();
		}
		EndPrimitive();	
	}
	else
	{
	   vec3 normal = GetNormal();

		    f_diffuseColour = diffuseColour[0];
			f_fragPos = fragPos[0];
			f_vertMV = vertMV[0];			
			f_vertWorld = vertWorld[0];
			f_viewVector = viewVector[0];
			f_vertNorm = vertNorm[0];
			f_vertTangent = vertTangent[0];		
			f_vertBitangent = vertBitangent[0];		
			f_text_STU = text_STU[0];	
			f_sb_texcoords = sb_texcoords[0];
			f_offscreen_texcoords = offscreen_texcoords[0];
			f_directionalShadow_FragPosLightSpace = directionalShadow_FragPosLightSpace[0];
	   gl_Position = explode(gl_in[0].gl_Position, normal, vertWorld[0]);	
	   EmitVertex();

		    f_diffuseColour = diffuseColour[1];
			f_fragPos = fragPos[1];
			f_vertMV = vertMV[1];			
			f_vertWorld = vertWorld[1];
			f_viewVector = viewVector[1];
			f_vertNorm = vertNorm[1];
			f_vertTangent = vertTangent[1];		
			f_vertBitangent = vertBitangent[1];		
			f_text_STU = text_STU[1];	
			f_sb_texcoords = sb_texcoords[1];
			f_offscreen_texcoords = offscreen_texcoords[1];
			f_directionalShadow_FragPosLightSpace = directionalShadow_FragPosLightSpace[1];
	   gl_Position = explode(gl_in[1].gl_Position, normal, vertWorld[1]);
	   EmitVertex();

		    f_diffuseColour = diffuseColour[2];
			f_fragPos = fragPos[2];
			f_vertMV = vertMV[2];			
			f_vertWorld = vertWorld[2];
			f_viewVector = viewVector[2];
			f_vertNorm = vertNorm[2];
			f_vertTangent = vertTangent[2];		
			f_vertBitangent = vertBitangent[2];		
			f_text_STU = text_STU[2];	
			f_sb_texcoords = sb_texcoords[2];
			f_offscreen_texcoords = offscreen_texcoords[2];
			f_directionalShadow_FragPosLightSpace = directionalShadow_FragPosLightSpace[2];
	   gl_Position = explode(gl_in[2].gl_Position, normal, vertWorld[2]);		
	   EmitVertex();
	   EndPrimitive();
	}

		
}
