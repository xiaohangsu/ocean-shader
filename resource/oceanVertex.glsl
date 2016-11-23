attribute vec4 position;
attribute vec4 normal;
attribute vec2 texCoord;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

uniform float time2;

varying vec3 varyingNormal;
varying vec3 varyingPosition;
varying vec2 varyingTexCoord;

vec3 computeNormal( vec3 pos, 
                    vec3 tangent, 
                    vec3 binormal)
{
	mat3 J;
	
	
	// A matrix is an array of column vectors so J[2] is 
	// the third column of J.
	
	J[0][0] = 1.0;
	J[0][1] = (0.1 * cos(pos.x * 0.1 + pos.z * 0.5 + time2 * 1.0) + 0.23 * cos(pos.x * 0.23 + pos.z * 0.41 + time2 * 1.5) + 0.33 * cos(pos.x * 0.33 + pos.z * 0.09 + time2 * 0.4));
	J[0][2] = 0.0;
	
	J[1][0] = 0.0;
	J[1][1] = 1.0;
	J[1][2] = 0.0;

	J[2][0] = 0.0;
	J[2][1] = (0.5 * cos(pos.x * 0.1 + pos.z * 0.5 + time2 * 1.0) + 0.41 * cos(pos.x * 0.23 + pos.z * 0.41 + time2 * 1.5) + 0.09 * cos(pos.x * 0.33 + pos.z * 0.09 + time2 * 0.4));
	J[2][2] = 1.0;
	
	vec3 u = J * tangent;
	vec3 v = J * binormal;
	
	vec3 n = cross(v, u);
	return normalize(n);
}

float calculateSurface(float x, float y) {

    return (sin(x * 0.1 + y * 0.5 + time2 * 1.0) + sin(x * 0.23 + y * 0.41 + time2 * 1.5) + sin(x * 0.33 + y * 0.09 + time2 * 0.4));
}



void main() {
	varyingNormal = normalize((normalMatrix * normal).xyz);
	varyingTexCoord = texCoord;
	
	vec4 p = position;
	p.y += calculateSurface(p.x, p.z) - calculateSurface(0.0, -30.0);

	vec4 displacedPosition = p;

	p = modelViewMatrix * p;
	
	varyingPosition = p.xyz;
	gl_Position = projectionMatrix * p;

	// 2 - Compute the displaced normal
	//
	
	// if the engine does not provide the tangent vector you 
	// can compute it with the following piece of of code:
	//
	vec3 tangent; 
	vec3 binormal; 
	
	vec3 c1 = cross(normal.xyz, vec3(0.0, 0.0, 1.0)); 
	vec3 c2 = cross(normal.xyz, vec3(0.0, 1.0, 0.0));
	
	if(length(c1)>length(c2))
	{
		tangent = c1;	
	}
	else
	{
		tangent = c2;	
	}
	
	tangent = normalize(tangent);
	
	binormal = cross(normal.xyz, tangent); 
	binormal = normalize(binormal);

	vec3 displacedNormal = computeNormal( displacedPosition.xyz, 
	                                 tangent.xyz, 
	                                 binormal);
   	
   	varyingNormal = normalize(normalMatrix * vec4(displacedNormal.xyz, 1.0)).xyz;
}
