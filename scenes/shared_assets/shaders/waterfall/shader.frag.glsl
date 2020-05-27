#version 330 core

in struct fragment_data
{
    vec4 position;
    vec4 normal;
    vec4 color;
    vec2 texture_uv;
} fragment;

uniform float time;
uniform sampler2D falaise;
uniform sampler2D texture_noise;



out vec4 FragColor;


// https://www.shadertoy.com/view/XdS3RW
vec3 screen_blend(vec3 s, vec3 d) {
	return s+d - s*d;
}

float get_mask(vec2 uv) {
    //uv.x *= iResolution.x / iResolution.y; (jouer sur le flottant)
    uv.x += sign(uv.x)*uv.y*.2;
    uv.x = abs(uv.x);
	return clamp(1.-(smoothstep(0.2, .6, uv.x)), 0., 1.);
}

// https://catlikecoding.com/unity/tutorials/flow/texture-distortion/
vec2 flow(vec2 uv, vec2 flowmap, float phase, float t, out float weight) {
	float progress = fract(t + phase);
	vec2 displacement = flowmap * progress;
    weight = 1. - abs(progress*2.-1.);
	return uv + displacement;
}

// https://thebookofshaders.com/13/
float fbm(sampler2D sampler, vec2 p) {
	// Initial values
	float value = 0.;
	float amplitude = .5;
	float frequency = 1.;
	// Loop of octaves
	for (int i = 0; i < 5; i++) {
        value += amplitude * (texture(sampler, p*frequency).r);
		frequency *= 2.;
		amplitude *= .5;
	}
	return value;
}


void main()
{
    vec2 uv = vec2(fragment.texture_uv.y, fragment.texture_uv.x);
    uv.x = uv.x*2.-1.5;// jouer sur la translation ici pour décaler la facade
    
    vec2 flowuv = uv*.03 + vec2(0., time*0.01);
    float noise = fbm(texture_noise, flowuv);
    vec2 flowmap = vec2(0., smoothstep(0.2, 1.,noise)) * .006;
	float weightA, weightB;
    float t = time * .8;
    vec2 uvA = flow(flowuv, flowmap, 0.0, t, weightA);
    vec2 uvB = flow(flowuv, flowmap, 0.5, t, weightB);
    float flowA = fbm(texture_noise, uvA) * weightA;
    float flowB = fbm(texture_noise, uvB) * weightB;
    float flow = (flowA + flowB);
    
    float waterfall_mask = get_mask(uv);
    float spray_mask = 1.-length(vec2(uv.x*.8, pow(uv.y, .5))*1.7);
    uv.y += .5;
    vec2 radial_uv = 0.01*vec2(atan(uv.x, uv.y), length(uv)) + vec2(0., -time*0.002);
    float spray = fbm(texture_noise, radial_uv);

    vec3 background = texture(falaise, uv).rgb;
    
    vec3 blue = vec3(0.6, .6, .9);
    vec3 waterfall = ((1.-flow)*blue + smoothstep(0., 1.,flow)) * waterfall_mask;
    vec3 col = mix(background, waterfall, waterfall_mask);
    //col += vec3(spray_mask*spray*(1.-waterfall_mask));
    spray_mask = clamp(spray_mask, 0., 1.);
    //col = screen_blend(vec3(spray*spray_mask*2.5), col);

    FragColor = vec4(col,1.0);
}