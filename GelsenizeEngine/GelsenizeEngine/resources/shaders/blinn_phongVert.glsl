#version 400

// VAO layout (vertex attribute layout)
in vec4 vColour;
in vec3 vPosition;
in vec3 vNormal;
in vec3 vTexSTU;
in vec3 vTangent; //ADDED
in vec3 vBitangent; //ADDED (binormal)
in vec4 vBoneIDs_x4;		// IS OK. Note these are only used in the vertex shader
in vec4 vBoneWeights_x4;	// IS OK. Note these are only used in the vertex shader

uniform mat4 matModel;   // "per object"
uniform mat4 matView;	 // "per scene" or "per frame"
uniform mat4 matProj;	 // "Projection" (only if size or aspect changes)

// For skinned mesh
const int MAXNUMBEROFBONES = 100;
uniform mat4 bones[MAXNUMBEROFBONES];

// Pass the acutal number you are using to control how often this loops
uniform int numberOfBonesUsed;
uniform bool bIsASkinnedMesh;	// True to do the skinned mesh
uniform vec3 slideDistance;

uniform vec4 objectColour;
uniform int isWireframe;

//new cel shading
uniform int backFaceDrawPassMode;
uniform float backFaceOffset;

const int NUMBEROFLIGHTS = 10;

struct sLightDesc
{
	bool enabled;
	vec4 position;		// In the world
	vec4 ambient;		// 0,0,0 (no ambient)
	vec4 diffuse;		// 1,1,1 (white colour light)
	vec4 specular;
	vec4 attenuation;	// x = const, y = linear, z = quad, w = not used
	vec4 direction;		// For directional and spot (ignored by others)
	vec4 typeAndParams;	// Specify type and attributes of type
	// x = type: 0 = point, 1 = spot, 2 = directional, 3 =hemispherical
						// y = for Spot: cos cutoff start (start of penumbra, edge of bright spot)
						// z = for Spot: cos cutoff end (end of penumbra, beyond this it's dark)
};

uniform sLightDesc theLights[NUMBEROFLIGHTS];

uniform bool clothPass;

// For spec:
uniform vec3 eyeLocation;

uniform bool bIsSecondRenderPass;		// true on 2nd pass
uniform int secondPassMode;

//directional shadow
uniform mat4 directionalLight_spaceMatrix;
uniform bool directionalShadowPass;

out vec4 diffuseColour;
out vec3 fragPos;
out vec3 vertMV;
out vec3 vertWorld;
out vec3 vertNorm;
out vec3 vertTangent;
out vec3 vertBitangent;
out vec3 viewVector;
out vec3 text_STU;	// ADDED, texture coordinates (also called UV)
out vec3 sb_texcoords;
out vec2 offscreen_texcoords;
out vec4 directionalShadow_FragPosLightSpace;

// VERTEX shader
void main()
{
	vec4 vertPosition = vec4( vPosition, 1.0f );
	vec4 vertNormal = vec4(vNormal, 0.0f);

	sb_texcoords = vertPosition.xyz;

	mat4 matMVP = matProj * matView * matModel;		// m = p * v * m;
	mat4 matMV = matView * matModel;	 // model-view matrix

	if (directionalShadowPass)
	{
		if (bIsASkinnedMesh)
		{
			mat4 BoneTransform = bones[ int(vBoneIDs_x4[0]) ] * vBoneWeights_x4[0];
			BoneTransform +=     bones[ int(vBoneIDs_x4[1]) ] * vBoneWeights_x4[1];
			BoneTransform +=     bones[ int(vBoneIDs_x4[2]) ] * vBoneWeights_x4[2];
			BoneTransform +=     bones[ int(vBoneIDs_x4[3]) ] * vBoneWeights_x4[3];

			// - vec4(slideDistance, 0.0f)
			vertPosition = BoneTransform * vertPosition;
			vertPosition.y += 4.87;
			gl_Position = directionalLight_spaceMatrix * matModel * vertPosition;
		}
		else
		{
			gl_Position = directionalLight_spaceMatrix * matModel * vec4(vPosition, 1.0f);
		}
		return;
	}

    // Copy the texture coordinates to the fragment shader
	text_STU = vTexSTU;

	fragPos = vec3(matModel * vec4(vPosition, 1.0f));
	directionalShadow_FragPosLightSpace = directionalLight_spaceMatrix * vec4(fragPos, 1.0f);

	//second pass render
	if ( bIsSecondRenderPass )
	{
		if (secondPassMode == 0 || secondPassMode == 1 || secondPassMode == 2)
		{
			if (vPosition.x == 1.0f && vPosition.y == 1.0f)
			{
				offscreen_texcoords = vec2(1.0f, 1.0f);
			}
			else if (vPosition.x == -1.0f && vPosition.y == 1.0f)
			{
				offscreen_texcoords = vec2(0.0f, 1.0f);
			}
			else if (vPosition.x == 1.0f && vPosition.y == -1.0f)
			{
				offscreen_texcoords = vec2(1.0f, 0.0f);
			}
			else if (vPosition.x == -1.0f && vPosition.y == -1.0f)
			{
				offscreen_texcoords = vec2(0.0f, 0.0f);
			}

			gl_Position = vertPosition;

			return;
		}
	}

	

	//apply bone weigths for skinned meshes
	if (bIsASkinnedMesh)
	{
		//vertPosition -= vec4(slideDistance, 0.0f);

		mat4 BoneTransform = bones[ int(vBoneIDs_x4[0]) ] * vBoneWeights_x4[0];
		BoneTransform +=     bones[ int(vBoneIDs_x4[1]) ] * vBoneWeights_x4[1];
		BoneTransform +=     bones[ int(vBoneIDs_x4[2]) ] * vBoneWeights_x4[2];
		BoneTransform +=     bones[ int(vBoneIDs_x4[3]) ] * vBoneWeights_x4[3];

		// - vec4(slideDistance, 0.0f)
		vertPosition = BoneTransform * (vertPosition);
		vertNormal = BoneTransform * vertNormal;

		gl_Position = matMVP * vertPosition;

		vertMV = vec3(matMV * vertPosition).xyz;

		mat4 matNormal = inverse( transpose(matModel) );
		vertNorm = mat3(matNormal) * normalize(vertNormal.xyz);

		vertTangent = mat3(matNormal) * normalize(vTangent);
		vertBitangent = mat3(matNormal) * normalize(vBitangent);

		vertWorld = vec3(matModel * vertPosition);
	}
	else
	{

		vertMV = vec3(matMV * vertPosition).xyz;

		// The "normal" matrix. The normal without scaling or tranlation
		// (So this vector is extending from the origin, in the direction
		//  of the vertex normal)
		// Since the vertex normal only needs rotation, we only
		//  need to use a 3x3 matrix (the "top-left" portion of the 4x4 matrix)
		mat4 matNormal = inverse( transpose(matModel) );
		vertNorm = mat3(matNormal) * normalize(vertNormal.xyz);
		vertNorm = normalize(vertNorm);
		vertTangent = mat3(matNormal) * normalize(vTangent);
		vertBitangent = mat3(matNormal) * normalize(vBitangent);

		// Position of the vertex in the world
		// (i.e. without View or Projection matrix
		vec4 asdf = (matModel * vec4(vPosition, 1.0f));
		vertWorld = asdf.xyz;

		//final vertex position
		gl_Position = matMVP * vertPosition;
	}


	viewVector = normalize(eyeLocation - vertWorld);


	//cel shading check
	if (backFaceDrawPassMode == 1)
	{
		vec4 tPos = vec4(vertPosition.xyz + normalize(vertNorm) * backFaceOffset, 1.0f);
		gl_Position = matMVP * tPos;
		return;
	}
	else if (backFaceDrawPassMode == 2)
	{
		vec4 tPos = vec4(vertPosition.xyz + normalize(vertNorm) * backFaceOffset, 1.0f);
		gl_Position = matMVP * tPos;
		return;
	}
	else if (backFaceDrawPassMode == 3)
	{
		//old regular drawing
	}


	if ( clothPass )
	{
		diffuseColour = vec4(vColour.rgb, 1.0f);
	}

	if ( isWireframe == 1 || true )
	{
		diffuseColour = objectColour;
	}
	else
	{
		diffuseColour = objectColour; //vec4(vColour, 1.0f);
	}
}
