#version 400

// FRAGMENT shader

uniform bool bUseDebugDiffuseNoLighting;

const int NUMBEROFLIGHTS = 10;

struct sLightDesc
{
	bool enabled;
	vec4 position;		// In the world
	vec4 ambient;		// 0,0,0 (no ambient)
	vec4 diffuse;		// 1,1,1 (white colour light)
	vec4 specular;		// w is specular power
	vec4 attenuation;	// x = const, y = linear, z = quad, w = specular ratio
	vec4 direction;		// For directional and spot (ignored by others)
	vec4 typeAndParams;	// To be used soon. Specify type and attributes of type
	// x = type: 0 = point, 1 = spot, 2 = directional, 3 =hemispherical 
						// y = for Spot: cos cutoff start (start of penumbra, edge of bright spot)
						// z = for Spot: cos cutoff end (end of penumbra, beyond this it's dark)
};

uniform sLightDesc theLights[NUMBEROFLIGHTS];

// For spec:
uniform vec3 eyeLocation;

// At least 80 texture units.
// Something like 16 to 32 on high end cards PER SHADER AT THE SAME TIME

uniform bool bUseLighting;

// In from the vertex shader
in vec4 diffuseColour;	
in vec3 vertMV;			// view vector
in vec3 vertWorld;
in vec3 vertNorm;
//in vec3 viewVector;
in vec3 lightVector[NUMBEROFLIGHTS];
in vec3 lightHalfVector[NUMBEROFLIGHTS];
in vec3 text_STU;	// ADDED, texture coordinates (also called UV)

in vec3 sb_texcoords;
in vec3 lightDirection[NUMBEROFLIGHTS];
in float attenuation[NUMBEROFLIGHTS];

// out to the framebuffer
out vec4 out_Colour;

vec3 calcLightContribution( vec3 diffuseObjectColour, vec3 vecView, int lightIndex )
{
	if ( !theLights[lightIndex].enabled )
	{
		//no contributuion
		return vec3(0.0f, 0.0f, 0.0f);
	}

	float diffuseVal = max(0.0f, dot(vertNorm.xyz, lightDirection[lightIndex]));
	float specularVal = max(0.0f, dot(vertNorm.xyz, lightHalfVector[lightIndex]));

	if (diffuseVal == 0.0f)
		specularVal = 0.0f;
	else
		specularVal = pow(specularVal,  theLights[lightIndex].specular.w) * 1.0f;

	vec3 diffuseVec = theLights[lightIndex].diffuse.xyz * diffuseVal;
	vec3 specularVec = theLights[lightIndex].specular.xyz * specularVal;
	
	diffuseVec *= attenuation[lightIndex];
	specularVec *= attenuation[lightIndex];		   	
	
	if (  int(theLights[lightIndex].typeAndParams.x) == 2)
	{
		//directional
		vec3 result = clamp((diffuseObjectColour*theLights[lightIndex].ambient.xyz) * (diffuseVec + specularVec), 0.0f, 1.0f);
		return result;
	}
	else
	{
		vec3 result = clamp(diffuseObjectColour * (diffuseVec + specularVec + theLights[lightIndex].ambient.xyz), 0.0f, 1.0f);
		return result;
	}
	
}

void main()
{
	// Check to see if we are going to do the lighthing calculation
	if (bUseDebugDiffuseNoLighting == true)
	{	// Bypass lighting entirely and set to uniform "debug" diffuse
		out_Colour.xyz = diffuseColour.xyz;
		out_Colour.a = 1.0f;
		return;
	}

	vec3 objectDiffuseColour = diffuseColour.xyz;

	float alphaOut = diffuseColour.a;

	if ( ! bUseLighting )
	{	// Don't perform the lighting calculation, just use existing diffuse
		out_Colour = vec4(objectDiffuseColour, alphaOut);	
	}
	else
	{	// Perform lighting calculation
		vec3 vecView = normalize(vertMV);
		
		// Do lighting calculation per light
		vec3 lightContrib = vec3(0.0f, 0.0f, 0.0f);
		for ( int index = 0; index != NUMBEROFLIGHTS; index++ )
		{
			lightContrib += calcLightContribution( objectDiffuseColour, vecView, index );
		}
		
		out_Colour = vec4(lightContrib, alphaOut);	
	}//if ( bUseLighting )

	out_Colour += vec4(0.05f,0.05f,0.05f,0.0f);
	//out_Colour.a = 1.0f;
}