uniform sampler2D tex;
uniform sampler3D volume_tex;
uniform float stepSize;
uniform float emissivity;
uniform float scale;
uniform float epsilon;
uniform int drawBack;
uniform int drawRays;
uniform int drawFront;
uniform int rayDepth;

void main()
{

//	if (drawBack == 1) {
//		gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
//		return;
//	}

	// GPU Ray tracing algorithm
	// Described in Marques, Santos, Leskovky & Paloc. GPU Ray Casting
	// Alpha blending equation is used
	// as described in the SIGGRAPH course on advanced
	// illumination techniques for GPU volume raycasting

	// find the right place to lookup in the backside buffer
    vec2 texc = ((gl_TexCoord[2].xy/gl_TexCoord[2].w) + 1.0) / 2.0;
	
	// the start position of the ray is stored in the texturecoordinate
    vec4 start = gl_TexCoord[0];
    vec4 back_position = texture2D(tex, texc);

    vec4 background = vec4(0.0,0.0,0.0,0.0);

	// Subtract buffers
    vec3 dir = back_position.xyz - start.xyz;

	if (drawRays == 1) {
		gl_FragColor = vec4(dir.x, dir.y, dir.z, 1.0);
		return;
	}

	if (drawFront == 1) {
		gl_FragColor = vec4(start);
		return;
	}

	if (drawBack == 1) {
		gl_FragColor = vec4(back_position.x, back_position.y, back_position.z, 1.0);
		return;
	}

	// Length of ray
    float maxRayLength = length(dir.xyz); 

	// Step offset in the ray
    vec3 stepVector = normalize(dir) * stepSize;

    vec3 rayPosition = start.xyz;
	
	// Ray color accumulator
    vec3 colorAcc = vec3(0.0,0.0,0.0);

    float alphaAcc = 0.0;
    float lengthAcc = 0.0;

	// Volume color
    vec4 voxelSample; 
    vec3 voxelColor; 
	float voxelAlpha;

	int i = 0;
	
	while(i < rayDepth && lengthAcc < maxRayLength && alphaAcc < 1.0) {

		voxelSample = texture3D(volume_tex, rayPosition);
		
		voxelColor = voxelSample.xyz;
		voxelAlpha = voxelSample.a;
		
		rayPosition += stepVector;
	
		lengthAcc += stepSize;
		i++;

//		if (voxelAlpha >= epsilon) {
		if (voxelAlpha > epsilon - 0.1 && voxelAlpha < epsilon + 0.1) {
			// The 4 * emissivity factor was computed by hand to get good luminosity
			colorAcc += (1.0 - alphaAcc) * voxelColor * scale * emissivity;
			alphaAcc += (1.0 - alphaAcc) * emissivity;
		}
    }
 	gl_FragColor =  vec4(colorAcc.x, colorAcc.y, colorAcc.z, alphaAcc);

}