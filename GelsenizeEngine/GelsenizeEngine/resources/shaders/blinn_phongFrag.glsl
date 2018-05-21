#version 400

// out to the framebuffer
layout (location = 0) out vec4 out_Colour;
layout (location = 1) out vec4 BrightColor;

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
	vec4 typeAndParams;	// Specify type and attributes of type
	// x = type: 0 = point, 1 = spot, 2 = directional, 3 =hemispherical
						// y = for Spot: cos cutoff start (start of penumbra, edge of bright spot)
						// z = for Spot: cos cutoff end (end of penumbra, beyond this it's dark)
};

uniform sLightDesc theLights[NUMBEROFLIGHTS];

uniform float alpha;

// For spec:
uniform vec3 eyeLocation;

// Texture "sampler"
// note that this ISN'T a 'texture', they are in 'texture units'
uniform sampler2D texSamp2D_00;			// Location 12
uniform sampler2D texSamp2D_01;
uniform sampler2D texSamp2D_02;
uniform sampler2D texSamp2D_03;
uniform sampler2D texSamp2D_04;
uniform sampler2D texSamp2D_05;

uniform sampler2D materialDiffuse;
uniform sampler2D materialSpecular;

uniform vec4 textureExtra; //z is time //w = 0, no animation, 1 = flow
uniform samplerCube myCubemapSkyBoxSampler;
uniform bool bIsCubeMapObject;
uniform int usedTexNum;

// Fullscreen 2nd pass texture sampler
uniform sampler2D offscreenColorTexture;
uniform bool bIsSecondRenderPass;		// true on 2nd pass
uniform int secondPassMode;

// At least 80 texture units.
// Something like 16 to 32 on high end cards PER SHADER AT THE SAME TIME

uniform bool bUseTextureAsDiffuse;
uniform bool bUseLighting;
uniform bool bIsDiscardTextureBlack;
//uniform bool bIsImposterBlackIsAlpha;
uniform bool clothPass;

//cel shading
uniform int backFaceDrawPassMode;
uniform vec3 backFaceColor;

//directional shadow
uniform bool directionalShadowPass;
uniform sampler2D directionalShadowSampler;
uniform vec3 diretionalLight_position;

//bloom
uniform sampler2D offscreenBloomTexture;
uniform vec3 glowColor;
uniform float bloomWeights[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);
uniform float exposure;
uniform bool isHorizontal;
uniform bool isGlowing;
uniform bool hasBloom;

uniform bool reflectsSkybox;

// In from the vertex shader
in vec4 f_diffuseColour;
in vec3 f_fragPos;
in vec3 f_vertMV;			// view vector
in vec3 f_vertWorld;
in vec3 f_vertNorm;
in vec3 f_vertTangent;
in vec3 f_vertBitangent;
in vec3 f_viewVector;
in vec3 f_text_STU;	// ADDED, texture coordinates (also called UV)
in vec3 f_sb_texcoords;
in vec2 f_offscreen_texcoords;
in vec4 f_directionalShadow_FragPosLightSpace;



float DirectionalShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(directionalShadowSampler, projCoords.xy).r; 

    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // Calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(f_vertNorm);
    vec3 lightDir = normalize(diretionalLight_position - f_fragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	bias += 0.01;

    // Check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(directionalShadowSampler, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(directionalShadowSampler, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}


// Calculates the color when using a directional light.
vec3 CalcDirLight(sLightDesc light, vec3 normal, vec3 viewDir, vec3 objectDiffuse)
{
	if ( !light.enabled )
	{
		//no contributuion
		return vec3(0.0f, 0.0f, 0.0f);
	}
	
    vec3 lightDir = normalize(-light.direction.xyz);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16); //material.shininess = 0.0f
    // Combine results
		vec3 ambient = light.ambient.xyz * objectDiffuse;//vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse.xyz * diff * objectDiffuse;//vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular.xyz * spec * vec3(1.0f,1.0f,1.0f);//vec3(texture(material.specular, TexCoords));

	if ( int(light.typeAndParams.w) == 1)
	{
		float shadow = true ? DirectionalShadowCalculation(f_directionalShadow_FragPosLightSpace) : 0.0;                      
		shadow = min(shadow, 0.75); // reduce shadow strength a little: allow some diffuse/specular light in shadowed regions
		vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
		return lighting;
	}
	else
	{
		return (ambient + diffuse + specular);
	}

    
}

// Calculates the color when using a point light.
vec3 CalcPointLight(sLightDesc light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 objectDiffuse)
{
		if ( !light.enabled )
		{
			//no contributuion
			return vec3(0.0f, 0.0f, 0.0f);
		}

    vec3 lightDir = normalize(light.position.xyz - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16); //material.shininess = 0.0f
    // Attenuation
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0f / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance));
    // Combine results
    vec3 ambient = light.ambient.xyz * objectDiffuse;//vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse.xyz * diff * objectDiffuse;//vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular.xyz * spec * vec3(1.0f,1.0f,1.0f);//vec3(texture(material.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

	if ( int(light.typeAndParams.w) == 1)
	{
		float shadow = true ? DirectionalShadowCalculation(f_directionalShadow_FragPosLightSpace) : 0.0;                      
		shadow = min(shadow, 0.75); // reduce shadow strength a little: allow some diffuse/specular light in shadowed regions
		vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));
		return lighting;
	}
	else
	{
		return (ambient + diffuse + specular);
	}
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight(sLightDesc light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 objectDiffuse)
{
		if ( !light.enabled )
		{
			//no contributuion
			return vec3(0.0f, 0.0f, 0.0f);
		}

    vec3 lightDir = normalize(light.position.xyz - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16); //material.shininess = 0.0f
    // Attenuation
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0f / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance));
    // Spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction.xyz));
    float epsilon = light.typeAndParams.y - light.typeAndParams.z;
    float intensity = clamp((theta - light.typeAndParams.z) / epsilon, 0.0, 1.0);
    // Combine results
		vec3 ambient = light.ambient.xyz * objectDiffuse;//vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse.xyz * diff * objectDiffuse;//vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular.xyz * spec * vec3(1.0f,1.0f,1.0f);//vec3(texture(material.specular, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}



//// Function prototypes
//vec3 CalcDirLight(sLightDesc light, vec3 normal, vec3 viewDir, vec3 objectDiffuse);
//vec3 CalcPointLight(sLightDesc light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 objectDiffuse);
//vec3 CalcSpotLight(sLightDesc light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 objectDiffuse);


void main()
{
	if(directionalShadowPass)
	{
		return;
	}

	float alphaOut = alpha;
	
	if ( bIsSecondRenderPass )
	{
		if (secondPassMode == 0) //default quad render
		{
			//float results = pow(texture(offscreenColorTexture, f_offscreen_texcoords).r,256); //draw depth buffer with this;

			vec3 screenTextureColor = texture(offscreenColorTexture, f_offscreen_texcoords).rgb;
			out_Colour = vec4(screenTextureColor, 1.0f);
		}
		else if (secondPassMode == 1) //quad render with bloom-blur
		{
			vec2 tex_offset = 1.0 / textureSize(offscreenColorTexture, 0); // gets size of single texel
			vec3 result = texture(offscreenColorTexture, f_offscreen_texcoords).rgb * bloomWeights[0];
			if(isHorizontal)
			{
				for(int i = 1; i < 5; ++i)
				{
					result += texture(offscreenColorTexture, f_offscreen_texcoords + vec2(tex_offset.x * i, 0.0)).rgb * bloomWeights[i];
					result += texture(offscreenColorTexture, f_offscreen_texcoords - vec2(tex_offset.x * i, 0.0)).rgb * bloomWeights[i];
				}
			}
			else
			{
				for(int i = 1; i < 5; ++i)
				{
					result += texture(offscreenColorTexture, f_offscreen_texcoords + vec2(0.0, tex_offset.y * i)).rgb * bloomWeights[i];
					result += texture(offscreenColorTexture, f_offscreen_texcoords - vec2(0.0, tex_offset.y * i)).rgb * bloomWeights[i];
				}
			}
			out_Colour = vec4(result, 1.0);
		}
		else if (secondPassMode == 2) //hdr color rendering + bloom blend
		{
			const float gamma = 1.8;
			vec3 hdrColor = texture(offscreenColorTexture, f_offscreen_texcoords).rgb;      
			vec3 bloomColor = texture(offscreenBloomTexture, f_offscreen_texcoords).rgb;

			if(hasBloom) 
				hdrColor += bloomColor; // additive blending

			// tone mapping
			vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

			// also gamma correct while we're at it       
			result = pow(result, vec3(1.0 / gamma));

			out_Colour = vec4(result, 1.0f);
		}
		else
		{
			out_Colour = vec4( clamp(f_offscreen_texcoords.x, 0.0f, 1.0f), 0.0f, 0.0f, 1.0f );
		}
		return;
	}

	if (alpha == 0.0f)
	{
		discard;
	}

	if(clothPass)
	{
		out_Colour = f_diffuseColour;
		return;
	}

	if(backFaceDrawPassMode == 1)
	{
		out_Colour = vec4(backFaceColor, 1.0f);
		return;
	}

	// Check to see if we are going to do the lighthing calculation
	if (bUseDebugDiffuseNoLighting == true)
	{	// Bypass lighting entirely and set to uniform "debug" diffuse
		out_Colour.xyz = f_diffuseColour.xyz;
		out_Colour.a = 1.0f;
		return;
	}

	// For the cube map
	if ( bIsCubeMapObject == true )
	{
		// Cube maps require 3 texture coordinates, specifically
		//  a "ray" cast towards the sphere or cube.
		// We could calculate this, or simply use the normal
		//  from the object we're using as a "cube", which
		//  in our case is actually a sphere.
		vec4 errorWasHere = texture( myCubemapSkyBoxSampler , f_sb_texcoords);
		out_Colour.x = errorWasHere.x;
		out_Colour.y = errorWasHere.y;
		out_Colour.z = errorWasHere.z;
		out_Colour.a = 1.0f;
		return;
	}

	
	vec3 brightnessThreshold = vec3(0.2126, 0.2, 0.2022);
	

	vec3 objectDiffuseColour = f_diffuseColour.xyz;
	

	// are we using textures?
	if ( bUseTextureAsDiffuse == true )
	{	// Replace the diffuse colour with what's in the texture
		vec3 modText_STU = f_text_STU;

		if (textureExtra.w >= 1.0)
		{
			modText_STU.y += textureExtra.z * textureExtra.y;
		}

		vec3 diffuse0 = texture( texSamp2D_00, modText_STU.xy ).rgb; //diffuse map
		vec3 diffuse1 = texture( texSamp2D_01, modText_STU.xy ).rgb; //normal  map

		vec3 diffuse2 = texture( texSamp2D_02, modText_STU.xy ).rgb; //zart map
		vec3 diffuse3 = texture( texSamp2D_03, modText_STU.xy ).rgb; //alpha mask

		vec3 diffuse4 = texture( texSamp2D_04, modText_STU.xy ).rgb;
		vec3 diffuse5 = texture( texSamp2D_05, modText_STU.xy ).rgb;

		//diffuse1 = vec3(1.0f, 0.0f, 0.0f);
		if (usedTexNum == 0)
		{
			objectDiffuseColour = diffuse0;
		}
		else if (usedTexNum == 1)
		{
			objectDiffuseColour = mix(diffuse0, diffuse1, 0.5f);
		}
		else if (usedTexNum == 2)
		{
			objectDiffuseColour = mix(diffuse0, diffuse1, 0.5f);
			objectDiffuseColour = mix(objectDiffuseColour, diffuse2, 0.5f);
		}
		else if (usedTexNum == 3)
		{
			objectDiffuseColour = diffuse3;
		}
		else if (usedTexNum == 4)
		{
			objectDiffuseColour = diffuse4;
		}
		else if (usedTexNum == 5)
		{
			objectDiffuseColour = diffuse5;
		}

		if (bIsDiscardTextureBlack)
		{
			float blabla = f_vertTangent.y + f_vertBitangent.x;
			float diffuseGray = (objectDiffuseColour.r + objectDiffuseColour.g + objectDiffuseColour.b) / 3.0f + blabla;
			if ( diffuseGray <= 0.01f )
			{
				discard;
			}
		}

		// here take the transparency directly from the image
		//alphaOut = normalize(diffuse1.r + diffuse1.g + diffuse1.b );
	}

	if (isGlowing)
	{
		out_Colour = vec4(glowColor, alphaOut);
		float brightness = dot(out_Colour.rgb, brightnessThreshold * 0.01f);
		if(brightness > 1.0)
			BrightColor = vec4(out_Colour.rgb, alphaOut);
		else
			BrightColor = vec4(0.0f, 0.0f, 0.0f, alphaOut);
		BrightColor = vec4(out_Colour.rgb, alphaOut);
		return;
	}

	if ( !bUseLighting )
	{
		// Don't perform the lighting calculation, just use existing diffuse
		out_Colour = vec4(objectDiffuseColour, alphaOut);
	}
	else
	{

		if (true || backFaceDrawPassMode == 4)
		{
			// Perform lighting calculation
			vec3 vecView = normalize(f_vertMV);
			vec3 viewDir = f_viewVector;
			vec3 norm    = normalize(f_vertNorm);
			// Do lighting calculation per light
			vec3 lightContrib = vec3(0.0f, 0.0f, 0.0f);
			for ( int index = 0; index != NUMBEROFLIGHTS; index++ )
			{
				sLightDesc light = theLights[index];

				if (light.typeAndParams.x == 0)
						lightContrib += CalcPointLight(light, norm, f_fragPos, viewDir, objectDiffuseColour);
				else if (light.typeAndParams.x == 1)
						lightContrib += CalcSpotLight(light, norm, f_fragPos, viewDir, objectDiffuseColour);
				else if (light.typeAndParams.x == 2)
						lightContrib += CalcDirLight(light, norm, viewDir, objectDiffuseColour);

			}

			float brightness = dot(lightContrib, brightnessThreshold);
			if(brightness > 1.0)
				BrightColor = vec4(lightContrib, alphaOut);
			else
				BrightColor = vec4(0.0f, 0.0f, 0.0f, alphaOut);
			out_Colour = vec4(lightContrib, alphaOut);

			if (reflectsSkybox)
			{
				vec3 I = normalize(f_vertWorld - eyeLocation);
				vec3 R = reflect(I, normalize(f_vertNorm));
				vec4 reflectedColor = texture(myCubemapSkyBoxSampler, R);

				out_Colour += reflectedColor*0.3f;
			}

			
		}
		else if (backFaceDrawPassMode == 3)
		{

			// Do lighting calculation per light
			//float lightContrib = 0.0f;
			//for ( int index = 0; index != NUMBEROFLIGHTS; index++ )
			//{
				//lightContrib += calcLightIntensity(index);
			//}


			//int numberOfShades = 60;
			// quantize intensity for cel shading
			//float shadeIntensity = ceil(lightContrib * numberOfShades)/numberOfShades;

			// use base color
			//out_Colour.xyz = objectDiffuseColour * shadeIntensity ;
			// or use color from texture
			//out_Colour.xyz = $(COLOR_FROM_TEXTURE) * shadeIntensity ;
			// or use mixed colors
			//out_Colour.xyz = objectDiffuseColour * $(COLOR_FROM_TEXTURE) * shadeIntensity;

			out_Colour.a = alphaOut;
		}
	}
	//out_Colour;
}
