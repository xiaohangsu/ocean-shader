varying vec3 varyingNormal;
varying vec3 varyingPosition;
varying vec2 varyingTexCoord;

uniform sampler2D diffuseTexture;
uniform vec3 uColor;

uniform float time;

struct Light {
	vec3 lightPosition;
	vec3 lightColor;
	vec3 specularLightColor;
};

uniform Light lights[2];

float attenuate(float dist, float a, float b) {
	return 1.0 / (1.0 + a * dist + b * dist * dist);
}

vec2 calculateWrap(vec2 texture) {
	float x = texture.x / 10.0;
	float y = texture.y / 10.0;
    texture.x += (sin(y * 1.0 + time) + sin(y * 2.3 + time) + sin(y * 3.3 + time)) / 3.0;
	texture.y += (sin(x * 2.3 + time) + sin(x * 3.5 + time) + sin(x * 1.3 + time)) / 3.0;
    return texture;
}




void main() {
	vec3 diffuseColor = vec3(0.0, 0.0, 0.0);
	vec3 specularColor = vec3(0.0, 0.0, 0.0);

	for (int i = 0; i < 2; i++) {
		vec3 lightDirection = -normalize(varyingPosition - lights[i].lightPosition);
		
		float diffuse = max(0.1, dot(varyingNormal, lightDirection));
		
		float attenuation = attenuate(distance(varyingPosition,
			lights[i].lightPosition) / 5.0, 2.7, 5.0);
		
		diffuseColor += lights[i].lightColor * diffuse;
		
		vec3 v = normalize(-varyingPosition);
		vec3 h = normalize(v + lightDirection);

		float specular = pow(max(0.1, dot(h, varyingNormal)), 64.0);

		specularColor += lights[i].specularLightColor * specular;
	}

	vec2 textureCoord = fract(calculateWrap(varyingPosition.xy) / 10.0);
	vec3 intensity = ((uColor + texture2D(diffuseTexture, textureCoord).xyz) * diffuseColor) + specularColor;
    
	gl_FragColor = vec4(intensity.xyz, 1.0);
}
