#version 330 core

in vec3 iPosWorld;
in vec3 iNormalWorld;
in vec2 iTexCoord;


// Material properties.
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float Ns;
// Light data.
uniform vec3 ambientLight;

uniform vec3 dirLightDir;
uniform vec3 dirLightRadiance;

uniform vec3 pointLightPos;
uniform vec3 pointLightIntensity;

uniform vec3 spotLightPos;
uniform vec3 spotLightDir;
uniform vec3 spotLightIntensity;
uniform float spotLightCutoff;
uniform float spotLightTotalWidthD;
// Camera data.
uniform vec3 cameraPos;

uniform float haveMapKd;
uniform sampler2D mapKd;


out vec4 FragColor;


vec3 Diffuse(vec3 Kd, vec3 I, vec3 N, vec3 lightDir)
{
    return Kd * I * max(0, dot(N, lightDir));
}

vec3 Specular(vec3 ks, vec3 I, vec3 N, vec3 lightDir)
{
    vec3 V = normalize(cameraPos - iPosWorld);
    vec3 R = reflect(-lightDir, N);
    return ks * I * pow(max(0, dot(V, R)), Ns);
	//return vec3(1.0,1.0,1.0);
}

void main()
{
	if(texture(mapKd, iTexCoord).rgb==vec3(0.0,0.0,0.0)){
		vec3 Kd = texture(mapKd, iTexCoord).rgb;
    }
    
    // Compute the normal vector.
    vec3 Normal = normalize(iNormalWorld);
    vec3 Position = iPosWorld;
    // -------------------------------------------------------------
    // Ambient light.
    vec3 ambient = Ka * ambientLight;
    // -------------------------------------------------------------
    // Directional light.
    vec3 LightDir = -dirLightDir;
    // Diffuse.
    vec3 diffuse = Diffuse(Kd, dirLightRadiance, Normal, LightDir);
    // Specular.
    vec3 specular = Specular(Ks,dirLightRadiance, Normal, LightDir);
    vec3 dirLight = diffuse + specular;
    // -------------------------------------------------------------
    // Point light.
    vec3 LightPos = pointLightPos;
    LightDir = normalize(LightPos - Position);
    float distSurfaceToLight = distance(LightPos, Position);
    float attenuation = 1.0f / (distSurfaceToLight * distSurfaceToLight);
    vec3 radiance = pointLightIntensity * attenuation;
    // Diffuse.
    diffuse = Diffuse(Kd, radiance, Normal, LightDir);
    // Specular.
    specular = Specular(Ks,dirLightRadiance, Normal, LightDir);
    vec3 pointLight = diffuse + specular;
    // -------------------------------------------------------------
    // radiance = pointLightIntensity * attenuation * spot;
	
	//spotlight
    LightPos = spotLightPos;
    LightDir = normalize(LightPos - Position);
    distSurfaceToLight = distance(LightPos, Position);
    attenuation = 1.0f / (distSurfaceToLight * distSurfaceToLight);
    float cosAngle = dot(LightDir, -spotLightDir);
    float degree = acos(cosAngle) * 180 / 3.1415;
    float attenuationAngle = 1.0f;
    if (degree < spotLightCutoff)
    {
        attenuationAngle = 1.0f;
    }
    else if (degree > spotLightTotalWidthD)
    {
        attenuationAngle = 0.0f;
    }
    else
    {
        attenuationAngle = (spotLightCutoff + spotLightTotalWidthD - degree) / spotLightTotalWidthD;
    }
    radiance = spotLightIntensity * attenuation * attenuationAngle;
	
    //LightDir = normalize(LightPos - Position);
    //float theta = dot(LightDir, normalize(-spotLightDir));
	//float epsilon = spotLightCutoff * 3.1415 / 180;
	//float epsilon2 = spotLightTotalWidthD * 3.1415 / 180;
	//float spot = 0.0;
	//if (theta > cos(epsilon))
	//{
		//spot = 1.0;
	//}
    //else if (theta < cos(epsilon2))
	//{
		//spot = 0.0;
	//}
	//else
	//{
		//spot = (cos(epsilon) - theta) / (cos(epsilon) - cos(epsilon2));
	//}
	//radiance = spotLightIntensity * attenuation * spot;
    // Diffuse.
    diffuse = Diffuse(Kd, radiance, Normal, LightDir);
    // Specular.
    specular = Specular(Ks, radiance, Normal, LightDir);
    vec3 spotLight = diffuse + specular;
    
    // FragColor = vec4(iPosWorld, 1.0);
	vec3 texColor = texture2D(mapKd,iTexCoord).rgb;
	FragColor = vec4(texColor * (ambient + dirLight + pointLight + spotLight), 1.0);
	//FragColor = vec4(ambient + dirLight + pointLight + spotLight, 1.0);
}